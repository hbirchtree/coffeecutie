#ifndef COFFEESCRIPTTERMINAL_H
#define COFFEESCRIPTTERMINAL_H

#include <QWidget>
#include "engine/scripting/coffeescriptengine.h"
#include "debugger/coffeescriptexceptionhandler.h"

class QLineEdit;
class QTreeWidget;

namespace Ui {
class CoffeeScriptTerminal;
}

class CoffeeScriptTerminal : public QWidget
{
    Q_OBJECT

public:
    CoffeeScriptTerminal(QWidget *parent);
    ~CoffeeScriptTerminal();

    QLineEdit* getScriptInput();

signals:
    void requestExecFile(const QString& filename);
    void requestExecCmd(const QString& cmd);

public slots:
    void clearLog();
    void appendLog(const QString &command, const QString &output);

private slots:
    void execFile(const QString& file);
    void on_scriptInput_returnPressed();

private:
    Ui::CoffeeScriptTerminal *ui;
};

#endif // COFFEESCRIPTTERMINAL_H
