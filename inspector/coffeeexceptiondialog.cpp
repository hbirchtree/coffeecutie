#include "coffeeexceptiondialog.h"
#include "ui_coffeeexceptiondialog.h"

CoffeeExceptionDialog::CoffeeExceptionDialog(QWidget *parent, CoffeeScriptException ex) :
    QDialog(parent),
    ui(new Ui::CoffeeExceptionDialog)
{
    ui->setupUi(this);

    ui->label->setText(ex.self.toString());
}

CoffeeExceptionDialog::~CoffeeExceptionDialog()
{
    delete ui;
}
