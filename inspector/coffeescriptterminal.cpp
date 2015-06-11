#include "coffeescriptterminal.h"
#include "ui_coffeescriptterminal.h"

#include "engine/physics/genericphysicsinterface.h"

CoffeeScriptTerminal::CoffeeScriptTerminal(QWidget *parent, QList<QObject*> rootObject) :
    QWidget(parent),
    ui(new Ui::CoffeeScriptTerminal)
{
    for(QObject* o : rootObject){
        QScriptValue v = engine.newQObject(o);
        engine.globalObject().setProperty(o->objectName(),v);
    }

    qRegisterMetaType<GenericPhysicsInterface::PhysicsProperty>("GenericPhysicsInterface::PhysicsProperty");
    qRegisterMetaType<QEvent::Type>("QEvent::Type");

    QScriptValue MetaObj = engine.newQMetaObject(&staticMetaObject);
    QScriptValue QtMeta = engine.newQMetaObject(&staticQtMetaObject);

    engine.globalObject().setProperty("Qt",QtMeta);
    engine.globalObject().setProperty("MOC",MetaObj);

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

    ui->scriptLog->appendHtml(QString("<b>%1</b>").arg(cmd));
    ui->scriptLog->appendPlainText(res);
    ui->scriptLog->appendPlainText("------------");
}
