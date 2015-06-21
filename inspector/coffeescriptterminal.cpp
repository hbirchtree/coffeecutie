#include "coffeescriptterminal.h"
#include "ui_coffeescriptterminal.h"

#include <QFile>
#include <QFileDialog>

CoffeeScriptTerminal::CoffeeScriptTerminal(QWidget *parent, CoffeeScriptEngine *engine) :
    QWidget(parent),
    ui(new Ui::CoffeeScriptTerminal)
{
    this->engine = engine;

    ui->setupUi(this);
}

CoffeeScriptTerminal::~CoffeeScriptTerminal()
{
    delete ui;
}

void CoffeeScriptTerminal::on_scriptInput_returnPressed()
{
    QString cmd = ui->scriptInput->text();
    QString res = engine->getEngine()->evaluate(cmd).toString();
    appendLog(cmd,res);
}

void CoffeeScriptTerminal::on_pushButton_2_clicked()
{
    QFileDialog scriptSelect(this);
    QString file = scriptSelect.getOpenFileName(this,tr("Open a QtScript file"),QString(),"*.qts");

    bool ret;
    QString res = "Failed to load file: Either it does not exist, is not a file or cannot be opened for reading.";
    //if execFile() fails, it does not change *logOut
    //failure is the case where the script cannot be opened

    engine->execFile(file,&ret,&res);

    appendLog(QString("FileLoad: %1").arg(file),res);
}

void CoffeeScriptTerminal::appendLog(const QString &command, const QString &output)
{
    ui->scriptLog->append(QString("<em><strong>%1</strong></em><br>"
                                  "<small>%2</small><br>").arg(command).arg(output));
}
