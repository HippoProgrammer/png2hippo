QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = png_to_hippo_converter
TEMPLATE = app

SOURCES += main.cpp

# Link to the required libraries
LIBS += -lpng -ljpeg

# Include paths for the libraries
INCLUDEPATH += /path/to/libpng/include \
               /path/to/libjpeg/include

# Library paths
LIBS += -L/path/to/libpng/lib \
        -L/path/to/libjpeg/lib
