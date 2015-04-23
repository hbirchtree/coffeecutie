#include "coffeescene.h"

CoffeeScene::CoffeeScene(QObject *parent) : QObject(parent)
{

}

CoffeeScene::~CoffeeScene()
{

}

QList<QPointer<RenderableObject> > CoffeeScene::getRenderables(){
    return renderables;
}

void CoffeeScene::renderAll(){

}
