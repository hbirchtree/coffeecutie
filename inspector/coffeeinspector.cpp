#include "coffeeinspector.h"
#include "ui_coffeeinspector.h"

CoffeeInspector::CoffeeInspector(QWidget *parent, QObject *engineRoot, CoffeeRenderer *renderer) :
    QWidget(parent),
    ui(new Ui::CoffeeInspector)
{
    ui->setupUi(this);
    this->engineRoot = engineRoot;
    this->renderer = renderer;

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
        QTreeWidgetItem* it = new QTreeWidgetItem();
        it->setText(0,"Engine root was deleted!");
        it->setText(1,"No data to display");
        ui->inspectorWidget->addTopLevelItem(it);
        return;
    }

    ui->inspectorWidget->addTopLevelItem(generateItem(engineRoot));
}

void CoffeeInspector::populateTreeWidgetItem(QObjectList source, QTreeWidgetItem* target){
    for(QObject* o : source){
        if(o->metaObject()->propertyCount()<=1&&o->children().size()==0)
            continue;
        target->addChild(generateItem(o));
    }
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
    QList<QTreeWidgetItem*> children = getProperties(o);
    if(children.size()>0)
        it->addChildren(children);
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
        else if(object->metaObject()->property(i).isReadable()&&object->metaObject()->property(i).type()==QVariant::List){
            QVariantList vars = object->metaObject()->property(i).read(object).toList();
            for(int i=0;i<vars.size();i++){
                QTreeWidgetItem* it = new QTreeWidgetItem();
                it->setText(0,QString::number(i));
                it->setText(1,vars.at(i).toString());
                prop->addChild(it);
            }
        }else
            prop->setText(1,object->metaObject()->property(i).typeName());
        items.append(prop);
    }
    return items;
}

void CoffeeInspector::on_updateBtn_clicked()
{
    updateInformation();
}

void CoffeeInspector::on_rendererBtn_clicked()
{
    if(!rendererInspector){
        rendererInspector = new CoffeeRendererInspector(0,renderer);
    }
    rendererInspector->show();
}
