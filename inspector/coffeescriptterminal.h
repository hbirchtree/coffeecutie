#ifndef COFFEESCRIPTTERMINAL_H
#define COFFEESCRIPTTERMINAL_H

#include <QWidget>
#include "engine/scripting/coffeescriptengine.h"

namespace Ui {
class CoffeeScriptTerminal;
}

class CoffeeScriptTerminal : public QWidget
{
    Q_OBJECT

public:
    CoffeeScriptTerminal(QWidget *parent, QList<QObject *> rootObject, CoffeeScriptEngine* engine);
    ~CoffeeScriptTerminal();

private slots:
    void on_scriptInput_returnPressed();

    void on_pushButton_2_clicked();

private:
    void appendLog(const QString &command, const QString &output);

    CoffeeScriptEngine* engine;
    Ui::CoffeeScriptTerminal *ui;
};

#endif // COFFEESCRIPTTERMINAL_H
