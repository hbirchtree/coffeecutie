#include "coffeeinspector.h"
#include "ui_coffeeinspector.h"

CoffeeInspector::CoffeeInspector(QWidget *parent, QObject *engineRoot) :
    QWidget(parent),
    ui(new Ui::CoffeeInspector)
{
    ui->setupUi(this);
    this->engineRoot = engineRoot;

    QStringList labels;
    labels << "Object name" << "Type/Data";
    ui->inspectorWidget->setHeaderLabels(labels);

    updateInformation();
}

CoffeeInspector::~CoffeeInspector()
{
    delete ui;
}

void CoffeeInspector::updateInformation()
{
    ui->inspectorWidget->clear();
    if(!engineRoot){
        qDebug() << "Failed to load object root!";
        return;
    }

    ui->inspectorWidget->addTopLevelItem(generateItem(engineRoot));
}

void CoffeeInspector::populateTreeWidgetItem(QObjectList source, QTreeWidgetItem* target){
    for(QObject* o : source)
        target->addChild(generateItem(o));
}

QTreeWidgetItem *CoffeeInspector::generateItem(QObject *o)
{
    QTreeWidgetItem* it = new QTreeWidgetItem();
    it->setText(0,o->objectName());
    it->setText(1,o->metaObject()->className());
    if(o->children().size()>0){
        QTreeWidgetItem* childItem = new QTreeWidgetItem();
        childItem->setText(0,"[Children]");
        it->addChild(childItem);
        populateTreeWidgetItem(o->children(),childItem);
    }
    it->addChildren(getProperties(o));
    return it;
}

QList<QTreeWidgetItem*> CoffeeInspector::getProperties(QObject *object)
{
    QList<QTreeWidgetItem*> items;
    for(int i=0;i<object->metaObject()->propertyCount();i++){
        QTreeWidgetItem* prop = new QTreeWidgetItem();
        prop->setText(0,object->metaObject()->property(i).name());
        if(object->metaObject()->property(i).isReadable()&&
                object->metaObject()->property(i).read(object).canConvert(QMetaType::QString))
            prop->setText(1,object->metaObject()->property(i).read(object).toString());
        else
            prop->setText(1,object->metaObject()->property(i).typeName());
        items.append(prop);
    }
    return items;
}

void CoffeeInspector::on_updateBtn_clicked()
{
    updateInformation();
}
