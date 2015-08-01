#ifndef COFFEEGAMEEDITOR_H
#define COFFEEGAMEEDITOR_H

#include <QMainWindow>
#include <QGridLayout>

namespace Ui {
class CoffeeGameEditor;
}

class CoffeeGameEditor : public QMainWindow
{
    Q_OBJECT
public:
    CoffeeGameEditor(QWidget* parent,
                     QWidget* info,
                     QWidget* inspector,
                     QWidget* scripting);
    ~CoffeeGameEditor();

signals:
    void requestShutdown();

private slots:
    void on_actionQuit_triggered();

private:
    Ui::CoffeeGameEditor *ui;
};

#endif // COFFEEGAMEEDITOR_H
