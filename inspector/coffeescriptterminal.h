#ifndef COFFEESCRIPTTERMINAL_H
#define COFFEESCRIPTTERMINAL_H

#include <QWidget>
#include <QScriptEngine>

namespace Ui {
class CoffeeScriptTerminal;
}

class CoffeeScriptTerminal : public QWidget
{
    Q_OBJECT

public:
    CoffeeScriptTerminal(QWidget *parent, QList<QObject *> rootObject);
    ~CoffeeScriptTerminal();

private slots:
    void on_scriptInput_returnPressed();

private:
    QScriptEngine engine;
    Ui::CoffeeScriptTerminal *ui;
};

#endif // COFFEESCRIPTTERMINAL_H
