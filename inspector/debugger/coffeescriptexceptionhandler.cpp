#include "coffeescriptexceptionhandler.h"
#include <QTreeWidget>
#include "engine/scripting/coffeescriptengine.h"
#include "coffeecodeeditor.h"

CoffeeScriptExceptionHandler::CoffeeScriptExceptionHandler(QObject *parent) : QObject(parent)
{

}

void CoffeeScriptExceptionHandler::receiveScriptException(CoffeeScriptException ex)
{
    m_backtree->clear();

    QList<QTreeWidgetItem*> m_items;

    QTreeWidgetItem* p = new QTreeWidgetItem();
    p->setText(0,ex.self.toString());
    m_backtree->addTopLevelItem(p);
    m_items.append(p);

    for(int i=0;i<ex.backtrace.size();i++){
        QTreeWidgetItem* e = new QTreeWidgetItem;
        e->setText(0,ex.backtrace.at(i));
        m_items.last()->addChild(e);
        m_items.append(e);
    }
    m_editor->setText(ex.program);

    //Error marking of line
    bool okCheck = false;
    int line = ex.backtrace.first().split(":").last().toInt(&okCheck);
    if(okCheck)
        m_editor->addErrorLine(line);
    else
        qDebug() << "Failed to locate error line";
}
