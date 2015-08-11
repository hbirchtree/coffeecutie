#ifndef COFFEESCRIPTEXCEPTIONHANDLER_H
#define COFFEESCRIPTEXCEPTIONHANDLER_H

#include <QObject>
#include "engine/scripting/coffeescriptengineagent.h"

class CoffeeScriptEngine;
class CoffeeDebugView;
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
    void setDebugView(CoffeeDebugView* m);
    void attachAgent(CoffeeScriptEngine* e);

    void receiveScriptException(CoffeeScriptException ex);
};

#endif // COFFEESCRIPTEXCEPTIONHANDLER_H
