#include "mainwindowtester.h"
#include "ui_mainwindowtester.h"

MainWindowTester::MainWindowTester(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindowTester)
{
    ui->setupUi(this);
}

MainWindowTester::~MainWindowTester()
{
    delete ui;
}

void MainWindowTester::addWidget(QWidget *w)
{
    ui->mdiArea->addSubWindow(w);
}
