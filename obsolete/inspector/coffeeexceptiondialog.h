#ifndef COFFEEEXCEPTIONDIALOG_H
#define COFFEEEXCEPTIONDIALOG_H

#include <QDialog>
#include <QTreeWidgetItem>
#include "engine/scripting/coffeescriptengine.h"

namespace Ui {
class CoffeeExceptionDialog;
}

class CoffeeExceptionDialog : public QDialog
{
    Q_OBJECT

public:
    CoffeeExceptionDialog(QWidget *parent = 0,CoffeeScriptException ex = CoffeeScriptException());
    ~CoffeeExceptionDialog();

private:
    QList<QTreeWidgetItem*> m_items;
    Ui::CoffeeExceptionDialog *ui;
};

#endif // COFFEEEXCEPTIONDIALOG_H
