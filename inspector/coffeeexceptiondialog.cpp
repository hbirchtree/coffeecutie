#include "coffeeexceptiondialog.h"
#include "ui_coffeeexceptiondialog.h"

CoffeeExceptionDialog::CoffeeExceptionDialog(QWidget *parent, CoffeeScriptException ex) :
    QDialog(parent),
    ui(new Ui::CoffeeExceptionDialog)
{
    ui->setupUi(this);

    ui->label->setText(ex.self.toString());

    for(int i=0;i<ex.backtrace.size();i++){
        QTreeWidgetItem* e = new QTreeWidgetItem;
        e->setText(0,ex.backtrace.at(i));
        if(m_items.size()>0)
            m_items.last()->addChild(e);
        else
            ui->treeWidget->addTopLevelItem(e);
        m_items.append(e);
    }
}

CoffeeExceptionDialog::~CoffeeExceptionDialog()
{
    delete ui;
}
