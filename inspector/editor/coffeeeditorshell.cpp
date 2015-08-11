#include "coffeeeditorshell.h"

#include "inspector/debugger/coffeedebugview.h"
#include "inspector/coffeescriptterminal.h"
#include "inspector/coffeerendererinspector.h"
#include "inspector/coffeeinspector.h"

#include "inspector/coffeeobjectexplorer.h"

#include <QWindow>
#include <QMainWindow>
#include <QSplitter>
#include <QTabWidget>

CoffeeEditorShell::CoffeeEditorShell(QObject *parent) : CoffeeBaseScriptEnvironment(parent)
{
    defineObjectConstructor<QWidget>(m_engine,"WindowWidget",
                                     [=](QScriptContext* ctxt, QScriptEngine* eng){
        QWidget* parent = qobject_cast<QWidget*>(ctxt->argument(0).toQObject());
        QWindow* window = qobject_cast<QWindow*>(ctxt->argument(1).toQObject());

        if(window){
            return eng->newQObject(QWidget::createWindowContainer(window,parent));
        }else{
            return ctxt->throwError("Invalid window argument!");
        }
    });

    qRegisterMetaType<CoffeeObjectExplorer*>("CoffeeObjectExplorer*");

    defineWidgetScriptType<CoffeeInspector>(m_engine);
    defineWidgetScriptType<CoffeeRendererInspector>(m_engine);
    defineWidgetScriptType<CoffeeScriptTerminal>(m_engine);
    defineWidgetScriptType<CoffeeDebugView>(m_engine);

    defineObjectConstructor<CoffeeObjectExplorer>(m_engine,"CoffeeObjectExplorer",
                                                  [=](QScriptContext* ctxt, QScriptEngine* eng){
        QWidget* parent = qobject_cast<QWidget*>(ctxt->argument(0).toQObject());
        QVariantList obj = ctxt->argument(1).toVariant().toList();

        QObjectList m;
        for(QVariant v : obj){
            QObject* o = qvariant_cast<QObject*>(v);
            if(o)
                m.append(o);
        }
        return eng->newQObject(new CoffeeObjectExplorer(parent,m));
    });

    defineWidgetScriptType<QTabWidget>(m_engine);
    defineWidgetScriptType<QSplitter>(m_engine);

    defineQMetaObjectByValue(m_engine,"Qt",&staticQtMetaObject);
}
