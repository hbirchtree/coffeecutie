#include "coffeescriptterminal.h"
#include "ui_coffeescriptterminal.h"

#include "engine/physics/genericphysicsinterface.h"

#include <QFile>
#include <QFileDialog>

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
    QString cmd = ui->scriptInput->text();
    QString res = engine.evaluate(cmd).toString();
    appendLog(cmd,res);
}

void CoffeeScriptTerminal::on_pushButton_2_clicked()
{
    QFileDialog scriptSelect(this);
    QString file = scriptSelect.getOpenFileName(this,tr("Open a QtScript file"),QString(),"*.qts");
    QFileInfo fileTest(file);
    QFile script(file);
    if(!file.isEmpty()&&fileTest.exists()&&fileTest.isFile()&&script.open(QIODevice::ReadOnly)){
        QString src = script.readAll();
        QString res = engine.evaluate(src).toString();
        appendLog(QString("FileLoad: %1").arg(file),res);
    }else{
        appendLog(QString("FileLoad: %1").arg(file),tr("Failed to load file: Either it does not exist, is not a file or cannot be opened for reading."));
    }
}

void CoffeeScriptTerminal::appendLog(const QString &command, const QString &output)
{
    ui->scriptLog->append(QString("<em><strong>%1</strong></em><br>"
                                  "<small>%2</small><br>").arg(command).arg(output));
}
