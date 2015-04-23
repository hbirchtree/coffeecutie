#ifndef VAOHELPER_H
#define VAOHELPER_H

#include <QObject>
#include "opengl/rendering/renderableobject.h"
#include "opengl/rendering/coffeemesh.h"

class VAOHelper
{
public:
    static void genVAO(QPointer<RenderableObject> object,int vertLocation,int vertTexCoordLocation,int vertNormalLocation,int vertTangentLocation){
        GLint vbo,vao;
        GLint drawMode = GL_STATIC_DRAW;
        if(object->isStreamDraw())
            drawMode = GL_STREAM_DRAW;
        glGenBuffers(1,&vbo);
        glBindBuffer(GL_ARRAY_BUFFER,vbo);
        glBufferData(GL_ARRAY_BUFFER,object->getVertexData(),drawMode);
        glBindBuffer(GL_ARRAY_BUFFER,0);

        glGenVertexArrays(1,&vao);
        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER,vbo);

        glEnableVertexAttribArray(vertLocation);
        glVertexAttribPointer(vertLocation,3,GL_FLOAT,false,CoffeeVertex::VERTEX_STRIDE,sizeof(GLfloat)*3);

        glEnableVertexAttribArray(vertTexCoordLocation);
        glVertexAttribPointer(vertTexCoordLocation,2,GL_FLOAT,false,CoffeeVertex::VERTEX_STRIDE,sizeof(GLfloat)*(3+2));

        glEnableVertexAttribArray(vertNormalLocation);
        glVertexAttribPointer(vertNormalLocation,3,GL_FLOAT,false,CoffeeVertex::VERTEX_STRIDE,sizeof(GLfloat)*(3+2+3));

        glEnableVertexAttribArray(vertTangentLocation);
        glVertexAttribPointer(vertTangentLocation,3,GL_FLOAT,false,CoffeeVertex::VERTEX_STRIDE,sizeof(GLfloat)*(3+2+3+3));

        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindVertexArray(0);

        object->setVaoHandle(vao);
        object->setVboHandle(vbo);
    }
    static void genTangents(QPointer<CoffeeMesh> mesh){
        int pointer = 0;
        while(pointer<mesh->vertices.size()){
            QPointer<CoffeeVertex> v0 = mesh->vertices.at(pointer+0);
            QPointer<CoffeeVertex> v1 = mesh->vertices.at(pointer+1);
            QPointer<CoffeeVertex> v2 = mesh->vertices.at(pointer+2);

            glm::vec3 edge1 = v1.position - v0.position;
            glm::vec3 edge2 = v2.position - v0.position;

            float deltaU1 = v1.texCoord.x - v0.texCoord.x;
            float deltaV1 = v1.texCoord.y - v0.texCoord.y;
            float deltaU2 = v2.texCoord.x - v0.texCoord.x;
            float deltaV2 = v2.texCoord.y - v0.texCoord.y;

            float f = 1.0/(deltaU1*deltaV2-deltaU2*deltaV1);

            glm::vec3 t,bt;


            t.x = f * (deltaV2 * edge1.x - deltaV1 * edge2.x);
            t.y = f * (deltaV2 * edge1.y - deltaV1 * edge2.y);
            t.z = f * (deltaV2 * edge1.z - deltaV1 * edge2.z);

            bt.x = f * (-deltaU2 * edge1.x - deltaU1 * edge2.x);
            bt.y = f * (-deltaU2 * edge1.y - deltaU1 * edge2.y);
            bt.z = f * (-deltaU2 * edge1.z - deltaU1 * edge2.z);

            v0.tangent += t;
            v0.bitangent += bt;

            pointer+=3;
        }
    }
    static void modifyVbo(GLint vboId,QPointer<CoffeeMesh> mesh){

    }
};

#endif // VAOHELPER_H
