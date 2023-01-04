#Qt
QT += core gui serialport charts
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#C++
CONFIG += c++11

#Files
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    crc32.cpp \
    file.cpp \
    graph.cpp \
    parser.cpp \
    usb.cpp

HEADERS += \
    mainwindow.h \
    crc32.h \
    file.h \
    graph.h \
    parser.h \
    usb.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    default.qrc
