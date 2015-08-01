#ifndef COFFEESCRIPTTERMINAL_H
#define COFFEESCRIPTTERMINAL_H

#include <QWidget>
#include "coffeeexceptiondialog.h"
#include "engine/scripting/coffeescriptengine.h"
#include "debugger/coffeecodeeditor.h"

namespace Ui {
class CoffeeScriptTerminal;
}

class CoffeeScriptTerminal : public QWidget
{
    Q_OBJECT

public:
    CoffeeScriptTerminal(QWidget *parent, CoffeeScriptEngine* engine);
    ~CoffeeScriptTerminal();

private slots:
    void clearLog();
    void execFile(const QString& file);

    void on_scriptInput_returnPressed();

private:
    void appendLog(const QString &command, const QString &output);

    CoffeeCodeEditor* m_editor;
    CoffeeScriptEngine* engine;
    Ui::CoffeeScriptTerminal *ui;
};

#endif // COFFEESCRIPTTERMINAL_H
