#include "profilingviewer.h"
#include "ui_profilingviewer.h"

#include <coffee/core/CXmlParser>
#include <coffee/core/plat/environment/sysinfo_def.h>

using namespace Coffee;

#include <QApplication>
#include <QFile>
#include <QTreeWidget>
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QDebug>

QVector<QTreeWidgetItem*> treeitems;

ProfilingViewer::ProfilingViewer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ProfilingViewer)
{
    ui->setupUi(this);

    setupTreeWidget(ui->profileTree);
}

ProfilingViewer::~ProfilingViewer()
{
    delete ui;
}

const constexpr int TIDIndex = 0;
const constexpr int LabelIndex = 1;
const constexpr int EvTimeIndex = 2;
const constexpr int TimeSecIndex = 3;
const constexpr int TimePrcIndex = 4;
const constexpr int BarItemIndex = 5;

const constexpr cstring token_thread = "thread";
const constexpr cstring token_label = "label";
const constexpr cstring token_timestamp = "ts";

const constexpr cstring token_context = "context";
const constexpr cstring token_datapoint = "dpoint";

quint64 buildProfileTree(QTreeWidget* troot,
                         QTreeWidgetItem* rt,
                         QSet<QString>& threads,
                         QMap<QString,QTreeWidgetItem*>& thread_items,
                         XML::Element* el,
                         quint64 base = 0)
{
    quint64 cost = 0;

    XML::Element* c = el->FirstChildElement();
    while(c)
    {
        QString thread_id = c->Attribute(token_thread);
        if(!threads.contains(thread_id))
        {
            threads.insert(thread_id);
            thread_items.insert(thread_id,new QTreeWidgetItem);
            thread_items[thread_id]->setText(TIDIndex,thread_id);
            troot->addTopLevelItem(thread_items[thread_id]);
        }

        QTreeWidgetItem* p;
        if(rt)
            p = rt;
        else
            p = thread_items[thread_id];

        quint64 ts = QString(c->Attribute(token_timestamp)).toULongLong();

        QTreeWidgetItem* ci = new QTreeWidgetItem;
        ci->setText(LabelIndex,c->Attribute(token_label));
        ci->setText(TimeSecIndex,c->Attribute(token_timestamp));
//        ci->setText(EvTimeIndex,QString::number(base+ts));
        p->addChild(ci);

        if(CStrCmp(c->Name(),token_context))
            cost += buildProfileTree(troot,ci,threads,thread_items,c,base+ts);
        else
            cost += ts;

        c = c->NextSiblingElement();
    }

    if(rt)
    {
        rt->setText(TimeSecIndex,QString::number(cost));
        return cost;
    }else
        return 0;
}

void ProfilingViewer::setupTreeWidget(QTreeWidget* tree)
{
    QStringList headers;

    headers << tr("TID");
    headers << tr("Label");
    headers << tr("Event time");
    headers << tr("Cost (µs)");
    headers << tr("Cost (%)");
    headers << tr("");

    tree->setHeaderLabels(headers);

    CResources::CResource rsc("GLeamBaseTest-profile.xml");
    CResources::FilePull(rsc);

    XML::Document* doc = XML::XMLRead(rsc);

    CResources::FileFree(rsc);

    QSet<QString> thread_id;
    QMap<QString,QTreeWidgetItem*> thread_items;

    XML::Element* xdat = doc->FirstChildElement("profile");

    quint64 base = QString(xdat->Attribute("start1")).toULongLong();
    base = 0;

    buildProfileTree(tree,nullptr,thread_id,thread_items,xdat,base);
}

void ProfilingViewer::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void ProfilingViewer::on_actionOpen_file_triggered()
{

}
