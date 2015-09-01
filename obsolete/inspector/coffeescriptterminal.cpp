#include "coffeescriptterminal.h"
#include "ui_coffeescriptterminal.h"

#include <QSplitter>
#include <QLineEdit>

CoffeeScriptTerminal::CoffeeScriptTerminal(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CoffeeScriptTerminal)
{
    ui->setupUi(this);


}

CoffeeScriptTerminal::~CoffeeScriptTerminal()
{
    delete ui;
}

QLineEdit *CoffeeScriptTerminal::getScriptInput()
{
    return ui->scriptInput;
}

void CoffeeScriptTerminal::clearLog()
{
    ui->scriptLog->clear();
}

void CoffeeScriptTerminal::execFile(const QString &file)
{
    requestExecFile(file);
}

void CoffeeScriptTerminal::on_scriptInput_returnPressed()
{
    requestExecCmd(ui->scriptInput->text());
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

void CoffeeScriptTerminal::receiveOutput(QString program, QString file, QString result)
{
    QString cmd;
    if(file.isEmpty()){
        cmd = program;
    }else{
        cmd = file;
    }
    appendLog(cmd,result);
}
