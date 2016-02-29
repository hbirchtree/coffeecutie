#include "profilingviewer.h"
#include "ui_profilingviewer.h"

#include "profilemodel.h"

#include <QApplication>
#include <QTreeWidget>
#include <QTreeView>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>

const constexpr cstring BaseTitle = "Espresso";

ProfilingViewer::ProfilingViewer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ProfilingViewer)
{
    ui->setupUi(this);

    this->setWindowTitle(tr(BaseTitle));
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
	ci->setText(EvTimeIndex,QString::number(ts));
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
}

void ProfilingViewer::loadProfileLog(const char *fname, QTreeWidget* tree)
{
    /* Reset widgets and their states */
    tree->clear();
    this->setWindowTitle(BaseTitle);

    if(!fname||CStrCmp(fname,""))
        return;

    CResources::CResource rsc(fname,true);

    if(!CResources::FileExists(rsc))
    {
	QMessageBox::warning(
		    this,tr("File error"),
		    tr("Could not locate file: %1")
		    .arg(rsc.resource()));
	return;
    }

    CResources::FilePull(rsc,true);

    XML::Document* doc = XML::XMLRead(rsc);

    if(!doc)
    {
	QMessageBox::warning(this,tr("Error parsing XML"),
			     tr("Please verify file contents of %1")
			     .arg(rsc.resource()));
	return;
    }

    CResources::FileFree(rsc);

    QSet<QString> thread_id;
    QMap<QString,QTreeWidgetItem*> thread_items;

    XML::Element* xdat = doc->FirstChildElement("profile");

    if(!xdat)
    {
        QMessageBox::warning(this,tr("Error parsing XML"),
                             tr("Failed to locate profiling document root: %1")
                             .arg(rsc.resource()));
        return;
    }

    /* Grab datapoints */
    {
        XML::Element* datap = xdat->FirstChildElement("datapoints");

	if(datap)
	{
	    QString title = QString("%1 - %2")
		    .arg(BaseTitle)
		    .arg(datap->Attribute("start2"));
	    this->setWindowTitle(title);

	    quint64 base = QString(datap->Attribute("start1")).toULongLong();
	    base = 0;

	    buildProfileTree(tree,nullptr,thread_id,thread_items,datap,base);

	    //        ui->profileView->setModel(GenerateProfileTree(datap,this));
	}
    }

    /* Set thread names if found */
    {
        XML::Element* threadlist = doc->FirstChildElement("threads");
        if(threadlist)
        {
            QMap<QString,QString> threadmap;
            XML::Element* tid = threadlist->FirstChildElement("thread");
            while(tid)
            {
                threadmap.insert(tid->Attribute("id"),tid->Attribute("name"));
                tid = tid->NextSiblingElement("thread");
            }
            for(int i=0;i<tree->topLevelItemCount();i++)
            {
                QTreeWidgetItem* it = tree->topLevelItem(i);
                if(threadmap.contains(it->text(0)))
                {
                    it->setText(0,threadmap[it->text(0)]);
                }
            }
        }
    }

    delete doc;
}

void ProfilingViewer::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void ProfilingViewer::on_actionOpen_file_triggered()
{
    QString l = QFileDialog::getOpenFileName(
		this,tr("Choose a captured profile"),
		QString(),"*.xml");
    fileDialogOpenFile(l);
}

void ProfilingViewer::fileDialogOpenFile(QString const& fn)
{
    loadProfileLog(fn.toStdString().c_str(),ui->profileTree);
}

void ProfilingViewer::on_actionClose_file_triggered()
{
    loadProfileLog(nullptr,ui->profileTree);
}
