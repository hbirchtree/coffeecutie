#include "coffeemessagedisplay.h"
#include "ui_coffeemessagedisplay.h"

CoffeeMessageDisplay::CoffeeMessageDisplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CoffeeMessageDisplay)
{
    ui->setupUi(this);
}

CoffeeMessageDisplay::~CoffeeMessageDisplay()
{
    delete ui;
}

QTextEdit *CoffeeMessageDisplay::text()
{
    return ui->log;
}
