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
    if(!engineRoot){
        QTreeWidgetItem* it = new QTreeWidgetItem();
        it->setText(0,"Engine root was deleted!");
        it->setText(1,"No data to display");
        ui->inspectorWidget->addTopLevelItem(it);
        return;
    }

    if(objectsMapping.contains(engineRoot)){
        updateTreeWidgetItem(engineRoot,nullptr);
    }else{
        updateTreeWidgetItem(engineRoot,nullptr);
        ui->inspectorWidget->addTopLevelItem(objectsMapping.value(engineRoot));
    }
}

void CoffeeInspector::updateTreeWidgetItem(QObject *object,QTreeWidgetItem* parent)
{
    QTreeWidgetItem* root;
    if(objectsMapping.contains(object))
        root = objectsMapping.value(object);
    else if(object->children().size()>0||object->metaObject()->propertyCount()>1){
        root = new QTreeWidgetItem();
        root->setText(0,object->objectName());
        root->setText(1,object->metaObject()->className());
        objectsMapping.insert(object,root);
        if(parent)
            parent->addChild(root);
    }else
        return;
    QTreeWidgetItem* childTree = nullptr;
    if(childTrees.contains(object)){
        childTree = childTrees.value(object);
    }
    if(childTree){
        for(QObject* o : object->children()){
            //check if the child tree contains the object and its item
            //if it does, update it
            //if it does not, create it
            updateTreeWidgetItem(o,childTree); //all hail recursion!
        }
    }else if(object->children().size()>0){
        //no child tree? create it
        QTreeWidgetItem* target = new QTreeWidgetItem();
        target->setText(0,"[Children]");
        for(QObject* o : object->children())
            updateTreeWidgetItem(o,target);
        if(target->childCount()>0){
            childTrees.insert(object,target);
            root->addChild(target);
        }else
            delete target;
    }
    updateProperties(object);
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

void CoffeeInspector::updateProperties(QObject *object)
{
    QTreeWidgetItem* root = objectsMapping.value(object);

    QHash<int,QTreeWidgetItem*> properties = propertyMapping.value(object);

    for(int i=0;i<object->metaObject()->propertyCount();i++){
        if(i==0) //we skip on the object name
            continue;
        QMetaProperty prop = object->metaObject()->property(i);
        if(!prop.isReadable()) //unreadable? no go
            continue;
        QVariant value = prop.read(object);

        if(properties.contains(i)){
            QTreeWidgetItem* it = properties.value(i);
            updateProperty(it,value);
        }else{
            QTreeWidgetItem* it = new QTreeWidgetItem();
            it->setText(0,prop.name());

            updateProperty(it,value);

            root->addChild(it);
            properties.insert(i,it);
        }
    }
    propertyMapping.insert(object,properties);
}

void CoffeeInspector::updateProperty(QTreeWidgetItem *it, QVariant value)
{
    if(value.canConvert(QVariant::String)){
        it->setText(1,value.toString());
    }else if(value.type()==QVariant::List){
        it->setText(1,"List-type");
        clearChildren(it);
        for(int i=0;i<value.toList().size();i++){
            QVariant v = value.toList().at(i);
            QTreeWidgetItem* entry = new QTreeWidgetItem();
            entry->setText(0,QString::number(i));
            updateProperty(entry,v);
            it->addChild(entry);
        }
    }else if(value.type()==QVariant::Map){
        it->setText(1,"Hash-type");
        clearChildren(it);
        for(QString key : value.toMap().keys()){
            QTreeWidgetItem* entry = new QTreeWidgetItem();
            entry->setText(0,key);
            updateProperty(entry,value.toMap().value(key));
            it->addChild(entry);
        }
    }else if(value.type()==QVariant::Hash){
        it->setText(1,"Hash-type");
        clearChildren(it);
        for(QString key : value.toHash().keys()){
            QTreeWidgetItem* entry = new QTreeWidgetItem();
            entry->setText(0,key);
            updateProperty(entry,value.toHash().value(key));
            it->addChild(entry);
        }
    }else if(value.userType()==QMetaType::QSize){
        it->setText(1,QString("QSize(%1x%2)").arg(value.toSize().width()).arg(value.toSize().height()));
    }else if(value.userType()==QMetaType::QRect){
        it->setText(1,QString("QRect(%1x%2+%3+%4)").arg(value.toRect().width()).arg(value.toRect().height()).arg(value.toRect().x()).arg(value.toRect().y()));
    }else if(value.userType()==QMetaType::QSize){
        it->setText(1,QString("QSizeF(%1x%2)").arg(value.toSizeF().width()).arg(value.toSizeF().height()));
    }else if(value.userType()==QMetaType::QRect){
        it->setText(1,QString("QRectF(%1x%2+%3+%4)").arg(value.toRectF().width()).arg(value.toRectF().height()).arg(value.toRectF().x()).arg(value.toRectF().y()));
    }else{
        it->setText(1,value.typeName());
    }
}

void CoffeeInspector::clearChildren(QTreeWidgetItem *it)
{
    //The size changes, we cannot remove items repeatedly.
    QList<QTreeWidgetItem*> children;
    for(int i=0;i<it->childCount();i++)
        children.append(it->child(i));
    for(QTreeWidgetItem* c : children){
        it->removeChild(c);
        delete c;
    }
}
