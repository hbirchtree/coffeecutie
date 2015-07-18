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

    connect(engine,&CoffeeScriptEngine::uncaughtException,[=](CoffeeScriptException ex){
        CoffeeExceptionDialog* dialog = new CoffeeExceptionDialog(this,ex);
        connect(dialog,&CoffeeExceptionDialog::finished,[=](int res){
            Q_UNUSED(res)
            dialog->deleteLater();
        });
        dialog->show();
    });
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
    //if execFile() fails, it does not change *logOut
    //failure is the case where the script cannot be opened

    QScriptValue e = engine->execFile(engine->getEngine(),file,&ret);

    appendLog(QString("FileLoad: %1").arg(file),e.toString());
}

void CoffeeScriptTerminal::appendLog(const QString &command, const QString &output)
{
    QString cmd(command);
    QString out(output);
    ui->scriptLog->append(QString("<em><strong>%1</strong></em><br>"
                                  "<small>%2</small><br>").arg(cmd.replace("\n","<br>")).arg(out.replace("\n","<br>")));
}
