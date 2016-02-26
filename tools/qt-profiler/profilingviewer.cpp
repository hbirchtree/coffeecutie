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

    setupTreeWidget();
}

ProfilingViewer::~ProfilingViewer()
{
    delete ui;
}

quint64 buildProfileTree(QTreeWidget* troot,
                         QTreeWidgetItem* rt,
                         QSet<QString>& threads,
                         QMap<QString,QTreeWidgetItem*>& thread_items,
                         XML::Element* el)
{
    quint64 cost = 0;

    XML::Element* c = el->FirstChildElement();
    while(c)
    {
        QString thread_id = c->Attribute("thread");
        if(!threads.contains(thread_id))
        {
            threads.insert(thread_id);
            thread_items.insert(thread_id,new QTreeWidgetItem);
            thread_items[thread_id]->setText(0,thread_id);
            troot->addTopLevelItem(thread_items[thread_id]);
        }

        QTreeWidgetItem* p;
        if(rt)
            p = rt;
        else
            p = thread_items[thread_id];

        QTreeWidgetItem* ci = new QTreeWidgetItem;
        ci->setText(1,c->Attribute("label"));
        ci->setText(2,c->Attribute("ts"));
        p->addChild(ci);

        if(CStrCmp(c->Name(),"context"))
            cost += buildProfileTree(troot,ci,threads,thread_items,c);
        c = c->NextSiblingElement();
    }

    if(rt)
    {
        rt->setText(2,QString::number(cost));
        return cost;
    }else
        return 0;
}

void ProfilingViewer::setupTreeWidget()
{
    QTreeWidget* tree = ui->profileTree;

    QStringList headers;

    headers << tr("TID");
    headers << tr("Label");
    headers << tr("Time (µs)");
    headers << tr("Time (%)");
    headers << tr("");

    tree->setHeaderLabels(headers);

    CResources::CResource rsc("GLeamBaseTest-profile.xml");
    CResources::FilePull(rsc);

    XML::Document* doc = XML::XMLRead(rsc);

    CResources::FileFree(rsc);

    QSet<QString> thread_id;
    QMap<QString,QTreeWidgetItem*> thread_items;

    XML::Element* xdat = doc->FirstChildElement("profile");

    buildProfileTree(tree,nullptr,thread_id,thread_items,xdat);
}

void ProfilingViewer::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void ProfilingViewer::on_actionOpen_file_triggered()
{

}
