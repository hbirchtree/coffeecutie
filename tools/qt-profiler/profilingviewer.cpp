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

struct _tmp_val
{
    quint64 ts;
    QString name;
};

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

    XML::Element* xdat = doc->FirstChildElement("profile");
    XML::Element* tmp = xdat->FirstChildElement();
    while(tmp)
    {
        cstring tid = tmp->Attribute("thread");
        if(tid)
        {
            QString trd = QString::fromLocal8Bit(tid).trimmed();
            if(!thread_id.contains(trd))
            {
                /* Create thread item */
                QTreeWidgetItem* m = new QTreeWidgetItem;
                m->setText(0,trd);
                tree->insertTopLevelItem(0,m);

                thread_id.insert(trd);
            }
        }

        tmp = tmp->NextSiblingElement();
    }
}

void ProfilingViewer::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void ProfilingViewer::on_actionOpen_file_triggered()
{

}
