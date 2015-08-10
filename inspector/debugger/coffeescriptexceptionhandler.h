#ifndef COFFEESCRIPTEXCEPTIONHANDLER_H
#define COFFEESCRIPTEXCEPTIONHANDLER_H

#include <QObject>
#include "engine/scripting/coffeescriptengineagent.h"

class CoffeeCodeEditor;
class QTreeWidget;

class CoffeeScriptExceptionHandler : public QObject
{
    Q_OBJECT
public:
    CoffeeScriptExceptionHandler(QObject *parent);

    CoffeeCodeEditor* m_editor = nullptr;
    QTreeWidget* m_backtree = nullptr;

public slots:
    void receiveScriptException(CoffeeScriptException ex);
};

#endif // COFFEESCRIPTEXCEPTIONHANDLER_H
