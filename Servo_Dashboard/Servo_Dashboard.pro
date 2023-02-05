#Qt
QT += core gui serialport charts
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#C++
CONFIG += c++11

#Files
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    System/file.cpp \
    System/graph.cpp \
    System/parser.cpp \
    System/usb.cpp \
    System/Drivers/crc32.cpp

HEADERS += \
    mainwindow.h \
    System/file.h \
    System/graph.h \
    System/parser.h \
    System/usb.h \
    System/Drivers/crc32.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    default.qrc
