#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow){
    ///Load fonts
    QFontDatabase::addApplicationFont(":/Resources/Latin Modern Mono/lmmono10-italic.otf");
    QFontDatabase::addApplicationFont(":/Resources/Latin Modern Mono/lmmono10-regular.otf");
    QFontDatabase::addApplicationFont(":/Resources/Latin Modern Mono/lmmonocaps10-oblique.otf");
    QFontDatabase::addApplicationFont(":/Resources/Latin Modern Mono/lmmonocaps10-regular.otf");
    QFontDatabase::addApplicationFont(":/Resources/Latin Modern Mono/lmmonolt10-bold.otf");
    QFontDatabase::addApplicationFont(":/Resources/Latin Modern Mono/lmmonolt10-boldoblique.otf");
    QFontDatabase::addApplicationFont(":/Resources/Latin Modern Mono/lmmonolt10-oblique.otf");
    QFontDatabase::addApplicationFont(":/Resources/Latin Modern Mono/lmmonolt10-regular.otf");
    QFontDatabase::addApplicationFont(":/Resources/Latin Modern Mono/lmmonoltcond10-oblique.otf");
    QFontDatabase::addApplicationFont(":/Resources/Latin Modern Mono/lmmonoltcond10-regular.otf");
    QFontDatabase::addApplicationFont(":/Resources/Latin Modern Mono/lmmonoprop10-oblique.otf");
    QFontDatabase::addApplicationFont(":/Resources/Latin Modern Mono/lmmonoprop10-regular.otf");
    QFontDatabase::addApplicationFont(":/Resources/Latin Modern Mono/lmmonoproplt10-bold.otf");
    QFontDatabase::addApplicationFont(":/Resources/Latin Modern Mono/lmmonoproplt10-boldoblique.otf");
    QFontDatabase::addApplicationFont(":/Resources/Latin Modern Mono/lmmonoproplt10-oblique.otf");
    QFontDatabase::addApplicationFont(":/Resources/Latin Modern Mono/lmmonoproplt10-regular.otf");
    QFontDatabase::addApplicationFont(":/Resources/Latin Modern Mono/lmmonoslant10-regular.otf");


    this->setFixedSize(1041, 691);
    ui->setupUi(this);
    Graph_Init();
    UI_Init();
    USB_Init();
}

MainWindow::~MainWindow(){
    delete ui;
}
