#include "coffeescriptterminal.h"
#include "ui_coffeescriptterminal.h"

#include <QFile>
#include <QFileDialog>
#include <QTreeWidget>
#include <QSplitter>

CoffeeScriptTerminal::CoffeeScriptTerminal(QWidget *parent, CoffeeScriptEngine *engine) :
    QWidget(parent),
    ui(new Ui::CoffeeScriptTerminal)
{
    this->engine = engine;

    ui->setupUi(this);

    QSplitter* dbgSplit = new QSplitter(this);
    dbgSplit->setOrientation(Qt::Vertical);

    m_editor = new CoffeeCodeEditor(dbgSplit);
    QTreeWidget* backtree = new QTreeWidget(dbgSplit);
    backtree->setHeaderHidden(true);
    ui->gridLayout_4->addWidget(dbgSplit);
    dbgSplit->addWidget(m_editor);
    dbgSplit->addWidget(backtree);

    connect(engine->getAgent(),&CoffeeScriptEngineAgent::exceptionReport,
            [=](CoffeeScriptException ex){

        backtree->clear();

        QList<QTreeWidgetItem*> m_items;

        QTreeWidgetItem* p = new QTreeWidgetItem();
        p->setText(0,ex.self.toString());
        backtree->addTopLevelItem(p);
        m_items.append(p);

        for(int i=0;i<ex.backtrace.size();i++){
            QTreeWidgetItem* e = new QTreeWidgetItem;
            e->setText(0,ex.backtrace.at(i));
            m_items.last()->addChild(e);
            m_items.append(e);
        }
        m_editor->setText(engine->getAgent()->getProgram(ex.scriptReference));

        //Error marking of line
        bool okCheck = false;
        int line = ex.backtrace.first().split(":").last().toInt(&okCheck);
        if(okCheck)
            m_editor->addErrorLine(line);
        else
            qDebug() << "Failed to locate error line";
    });
}

CoffeeScriptTerminal::~CoffeeScriptTerminal()
{
    delete ui;
}

void CoffeeScriptTerminal::clearLog()
{
    ui->scriptLog->clear();
}

void CoffeeScriptTerminal::execFile(const QString &file)
{
    bool ret;
    QScriptValue e = engine->execFile(engine->getEngine(),file,&ret);

    appendLog(QString("File load: %1").arg(file),e.toString());
}

void CoffeeScriptTerminal::on_scriptInput_returnPressed()
{
    QString cmd = ui->scriptInput->text();
    QString res = engine->getEngine()->evaluate(cmd).toString();
    appendLog(cmd,res);
}

void CoffeeScriptTerminal::appendLog(const QString &command, const QString &output)
{
    QString cmd(command);
    QString out(output);
    ui->scriptLog->append(QString("<em><strong>%1</strong></em><br>"
                                  "<small>%2</small><br>")
                          .arg(cmd.replace("\n","<br>"))
                          .arg(out.replace("\n","<br>")));
}
