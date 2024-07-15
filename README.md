# PNG to HIPPO Converter

## Overview

The PNG to HIPPO Converter is a C++ application that converts PNG images into a custom HIPPO format. It provides both a graphical user interface (GUI) and a command-line interface (CLI) for ease of use. The program supports adjustable compression quality and retains RGBA channels.

## Features

- **GUI for easy file selection and conversion**
- **Command-line options for automated conversion**
- **Adjustable quality settings (1-100)**

## Dependencies

This program requires the following libraries:

- **Qt**: A cross-platform application framework.
- **libpng**: A library for handling PNG images.
- **libjpeg**: A library for handling JPEG images.

### Install Dependencies

#### Windows

1. **Install Qt**:
   - Download and install the [Qt Installer](https://www.qt.io/download).
   - During installation, select the components you need, including the Qt libraries.

2. **Install libpng and libjpeg**:
   - You can use a package manager like [vcpkg](https://github.com/microsoft/vcpkg):
     ```bash
     git clone https://github.com/microsoft/vcpkg.git
     cd vcpkg
     ./bootstrap-vcpkg.bat
     .\vcpkg integrate install
     .\vcpkg install libpng libjpeg
     ```

#### macOS

1. **Install Qt**:
   - Download and install [Qt](https://www.qt.io/download).

2. **Install Homebrew** (if not already installed):
   ```bash
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```

3. **Install libpng and libjpeg**:
   ```bash
   brew install libpng libjpeg
   ```

#### Linux

1. **Install Qt**:
   - On Ubuntu, you can install Qt via:
   ```bash
   sudo apt install qt5-default
   ```

2. **Install libpng and libjpeg**:
   ```bash
   sudo apt install libpng-dev libjpeg-dev
   ```

## Editing `converter.pro`

Open the `converter.pro` file in a text editor and adjust the following paths if necessary:

- Update `INCLUDEPATH` to point to the directories where `libpng` and `libjpeg` headers are located.
- Update `LIBS` to point to the directories where the `libpng` and `libjpeg` libraries are installed.

### Example:
```pro
INCLUDEPATH += /usr/local/include/libpng \
               /usr/local/include/libjpeg

LIBS += -L/usr/local/lib/libpng \
        -L/usr/local/lib/libjpeg
```

## Installing the Program

1. **Clone the repository**:
   ```bash
   git clone https://github.com/yourusername/png_to_hippo_converter.git
   cd png_to_hippo_converter
   ```

2. **Build the Project**:
   - Run the following commands:
   ```bash
   qmake converter.pro
   make
   ```

3. **Run the Program**:
   - Execute the binary generated from the build process:
   ```bash
   ./png_to_hippo_converter
   ```

## How It Works

1. **Graphical User Interface**:
   - Click "Select PNG File" to choose an input PNG image.
   - Adjust the quality slider (1-100) to set compression quality.
   - Click "Convert to .hippo" to save the converted file.

2. **Command-Line Interface**:
   - Use the command:
   ```bash
   ./png_to_hippo_converter --convert --input <input_file.png> --output <output_file.hippo> --quality <1-100>
   ```
   - Example:
   ```bash
   ./png_to_hippo_converter --convert --input image.png --output image.hippo --quality 75
   ```

3. **Conversion Process**:
   - The program reads the PNG file, decompresses it, and compresses it into the HIPPO format using the specified quality level.
   - A progress bar indicates the conversion status in the GUI.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
```
