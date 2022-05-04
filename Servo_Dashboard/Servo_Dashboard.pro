QT += core gui serialport charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

SOURCES += \
    crc32.cpp \
    file.cpp \
    graph.cpp \
    main.cpp \
    mainwindow.cpp \
    parser.cpp \
    ui.cpp \
    usb.cpp

HEADERS += \
    mainwindow.h \
    parser.h \
    usb.h

FORMS += \
    mainwindow.ui

unix: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    default.qrc
