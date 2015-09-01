#ifndef COFFEEMESSAGEDISPLAY_H
#define COFFEEMESSAGEDISPLAY_H

#include <QWidget>

class QTextEdit;

namespace Ui {
class CoffeeMessageDisplay;
}

class CoffeeMessageDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit CoffeeMessageDisplay(QWidget *parent = 0);
    ~CoffeeMessageDisplay();

    QTextEdit* text();

private:
    Ui::CoffeeMessageDisplay *ui;
};

#endif // COFFEEMESSAGEDISPLAY_H
