#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QFileDialog>
#include <QSlider>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include <QMessageBox>
#include <QThread>
#include <QCommandLineParser>
#include <iostream>
#include <png.h>
#include <jpeglib.h>

// Worker class to handle image conversion in a separate thread
class ConverterWorker : public QThread {
    Q_OBJECT
public:
    ConverterWorker(const QString& input, const QString& output, int quality)
        : inputFile(input), outputFile(output), quality(quality) {}

    void run() override {
        convert_png_to_hippo(inputFile.toStdString().c_str(), outputFile.toStdString().c_str());
        emit conversionFinished();
    }

signals:
    void conversionFinished();

private:
    QString inputFile;
    QString outputFile;
    int quality;

    void compress_image(const unsigned char* rgba_data, int width, int height, const char* output_path, int quality) {
        struct jpeg_compress_struct cinfo;
        struct jpeg_error_mgr jerr;

        cinfo.err = jpeg_std_error(&jerr);
        jpeg_create_compress(&cinfo);

        // Open the output file
        FILE* outfile = fopen(output_path, "wb");
        if (!outfile) return;

        jpeg_stdio_dest(&cinfo, outfile);
        cinfo.image_width = width;
        cinfo.image_height = height;
        cinfo.input_components = 3; // RGB
        cinfo.in_color_space = JCS_RGB; // Convert to RGB
        jpeg_set_defaults(&cinfo);
        jpeg_set_quality(&cinfo, quality, TRUE);
        jpeg_start_compress(&cinfo, TRUE);

        // Write RGB data
        while (cinfo.next_scanline < cinfo.image_height) {
            JSAMPROW row_pointer[1];
            row_pointer[0] = const_cast<JSAMPROW>(rgba_data + (cinfo.next_scanline * width * 4)); // RGBA
            for (int i = 0; i < width; ++i) {
                row_pointer[0][i * 3] = row_pointer[0][i * 4];     // R
                row_pointer[0][i * 3 + 1] = row_pointer[0][i * 4 + 1]; // G
                row_pointer[0][i * 3 + 2] = row_pointer[0][i * 4 + 2]; // B
            }
            jpeg_write_scanlines(&cinfo, row_pointer, 1);
        }

        jpeg_finish_compress(&cinfo);
        fclose(outfile);
    }

    void convert_png_to_hippo(const char* png_file, const char* hippo_file) {
        FILE* fp = fopen(png_file, "rb");
        if (!fp) return;

        png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        png_infop info_ptr = png_create_info_struct(png_ptr);
        png_init_io(png_ptr, fp);
        
        png_set_sig_bytes(png_ptr, 8);
        png_read_info(png_ptr, info_ptr);

        int width = png_get_image_width(png_ptr, info_ptr);
        int height = png_get_image_height(png_ptr, info_ptr);
        png_byte color_type = png_get_color_type(png_ptr, info_ptr);

        if (color_type != PNG_COLOR_TYPE_RGBA) {
            fclose(fp);
            return;
        }

        png_read_update_info(png_ptr, info_ptr);
        unsigned char* row = new unsigned char[width * 4];
        unsigned char* rgba_data = new unsigned char[width * height * 4];

        for (int y = 0; y < height; y++) {
            png_read_row(png_ptr, row, nullptr);
            std::copy(row, row + width * 4, rgba_data + (y * width * 4));
        }

        fclose(fp);
        delete[] row;

        // Compress and save as .hippo
        compress_image(rgba_data, width, height, hippo_file, quality);
        delete[] rgba_data;
    }
};

// Main window class for the GUI
class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow() {
        QVBoxLayout* layout = new QVBoxLayout(this);
        
        selectButton = new QPushButton("Select PNG File", this);
        layout->addWidget(selectButton);

        qualitySlider = new QSlider(Qt::Horizontal, this);
        qualitySlider->setRange(1, 100);
        qualitySlider->setValue(75);
        layout->addWidget(new QLabel("Quality:"));
        layout->addWidget(qualitySlider);

        progressBar = new QProgressBar(this);
        layout->addWidget(progressBar);
        
        convertButton = new QPushButton("Convert to .hippo", this);
        layout->addWidget(convertButton);

        connect(selectButton, &QPushButton::clicked, this, &MainWindow::selectFile);
        connect(convertButton, &QPushButton::clicked, this, &MainWindow::convertFile);
    }

private slots:
    void selectFile() {
        QString fileName = QFileDialog::getOpenFileName(this, "Open PNG File", "", "PNG Files (*.png)");
        if (!fileName.isEmpty()) {
            inputFile = fileName;
        }
    }

    void convertFile() {
        if (inputFile.isEmpty()) {
            QMessageBox::warning(this, "Warning", "Please select a PNG file first.");
            return;
        }

        QString outputFile = QFileDialog::getSaveFileName(this, "Save .hippo File", "", "HIPPO Files (*.hippo)");
        if (outputFile.isEmpty()) return;

        progressBar->setValue(0);
        int quality = qualitySlider->value();

        ConverterWorker* worker = new ConverterWorker(inputFile, outputFile, quality);
        connect(worker, &ConverterWorker::conversionFinished, this, &MainWindow::onConversionFinished);
        worker->start();
    }

    void onConversionFinished() {
        progressBar->setValue(100);
        QMessageBox::information(this, "Success", "File conversion completed!");
        progressBar->setValue(0);
    }

private:
    QPushButton* selectButton;
    QPushButton* convertButton;
    QSlider* qualitySlider;
    QProgressBar* progressBar;
    QString inputFile;
};

// Function to handle command-line conversion
void handleCommandLineConversion(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("PNG to HIPPO Converter");
    parser.addHelpOption();
    parser.addVersionOption();

    // Define command-line options
    QCommandLineOption inputOption("input", "Input PNG file", "file");
    QCommandLineOption outputOption("output", "Output HIPPO file", "file");
    QCommandLineOption qualityOption("quality", "Quality (1-100)", "value", "75");

    parser.addOption(inputOption);
    parser.addOption(outputOption);
    parser.addOption(qualityOption);
    
    parser.process(app);

    QString inputFile = parser.value(inputOption);
    QString outputFile = parser.value(outputOption);
    int quality = parser.value(qualityOption).toInt();

    // Validate inputs
    if (inputFile.isEmpty() || outputFile.isEmpty() || quality < 1 || quality > 100) {
        std::cerr << "Invalid parameters. Use --help for usage." << std::endl;
        return;
    }

    ConverterWorker worker(inputFile, outputFile, quality);
    worker.run(); // Run the conversion in the main thread for CLI
    std::cout << "Conversion completed!" << std::endl;
}

// Main function to launch GUI or handle CLI
int main(int argc, char* argv[]) {
    // Check if any command line arguments are provided
    if (argc > 1 && QString(argv[1]) == "--convert") {
        handleCommandLineConversion(argc, argv);
        return 0;
    }

    // Launch GUI application
    QApplication app(argc, argv);
    MainWindow window;
    window.setWindowTitle("PNG to HIPPO Converter");
    window.resize(400, 200);
    window.show();
    return app.exec();
}

#include "main.moc"
