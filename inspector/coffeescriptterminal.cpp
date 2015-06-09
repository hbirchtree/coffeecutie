#include "coffeescriptterminal.h"
#include "ui_coffeescriptterminal.h"

CoffeeScriptTerminal::CoffeeScriptTerminal(QWidget *parent, QList<QObject*> rootObject) :
    QWidget(parent),
    ui(new Ui::CoffeeScriptTerminal)
{
    for(QObject* o : rootObject){
        QScriptValue v = engine.newQObject(o);
        engine.globalObject().setProperty(o->objectName(),v);
    }

    ui->setupUi(this);
}

CoffeeScriptTerminal::~CoffeeScriptTerminal()
{
    delete ui;
}

void CoffeeScriptTerminal::on_scriptInput_returnPressed()
{
    QString cmd = ui->scriptInput->text()+"\n";
    QString res = engine.evaluate(cmd).toString();

    ui->scriptLog->appendPlainText(cmd);
    ui->scriptLog->appendPlainText(res);
}
