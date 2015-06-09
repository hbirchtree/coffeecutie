#include "coffeescriptterminal.h"
#include "ui_coffeescriptterminal.h"

CoffeeScriptTerminal::CoffeeScriptTerminal(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CoffeeScriptTerminal)
{
    ui->setupUi(this);
}

CoffeeScriptTerminal::~CoffeeScriptTerminal()
{
    delete ui;
}
