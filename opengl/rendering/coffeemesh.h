#ifndef COFFEEMESH
#define COFFEEMESH

#include "coffeevertex.h"
#include <QList>
class CoffeeMesh : public QObject{
    CoffeeMesh(){

    }
    CoffeeMesh(QPointer<CoffeeMesh> mesh){
        this->vertices = mesh->vertices;
    }
    QList<QPointer<CoffeeVertex> > copy(){
        QList<QPointer<CoffeeVertex> > copy;
        for(QPointer<CoffeeVertex> vert : vertices){
            copy.append(new CoffeeVertex(vert));
        }
        return copy;
    }
    GLfloat* getData(){
        GLfloat data[vertices.size()*CoffeeVertex::VERTEX_STRIDE];
        int size = 0;
        for(QPointer<CoffeeVertex> vert : vertices){
            data[size+0] = vert->position.x;
            data[size+1] = vert->position.y;
            data[size+2] = vert->position.z;
            data[size+3] = vert->texCoord.x;
            data[size+4] = vert->texCoord.y;
            data[size+5] = vert->normal.x;
            data[size+6] = vert->normal.y;
            data[size+7] = vert->normal.z;
            data[size+8] = vert->tangent.x;
            data[size+9] = vert->tangent.y;
            data[size+10] = vert->tangent.z;
            size+=CoffeeVertex::VERTEX_STRIDE;
        }
        return data;
    }

    QList<QPointer<CoffeeVertex> > vertices;
};

#endif // COFFEEMESH

