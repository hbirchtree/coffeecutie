#ifndef COFFEESCRIPTTERMINAL_H
#define COFFEESCRIPTTERMINAL_H

#include <QWidget>

namespace Ui {
class CoffeeScriptTerminal;
}

class CoffeeScriptTerminal : public QWidget
{
    Q_OBJECT

public:
    explicit CoffeeScriptTerminal(QWidget *parent = 0);
    ~CoffeeScriptTerminal();

private:
    Ui::CoffeeScriptTerminal *ui;
};

#endif // COFFEESCRIPTTERMINAL_H
