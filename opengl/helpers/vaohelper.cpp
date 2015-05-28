#include "vaohelper.h"

//void VAOHelper::genVAO(RenderableObject *object, int vertLocation, int vertTexCoordLocation, int vertNormalLocation, int vertTangentLocation){
//    GLuint vbo = 0;
//    GLuint vao = 0;
//    GLenum drawMode = GL_STATIC_DRAW;
////    if(object->isStreamDraw())
////        drawMode = GL_STREAM_DRAW;
//    glGenBuffers(1,&vbo);
//    glBindBuffer(GL_ARRAY_BUFFER,vbo);
//    NumberBuffer<GLfloat>* data = object->getVertexData();
//    glBufferData(GL_ARRAY_BUFFER,data->getSize()*sizeof(GLfloat),data->get(),drawMode);

//    glGenVertexArrays(1,&vao);
//    glBindVertexArray(vao);

//    glEnableVertexAttribArray(vertLocation);
//    glVertexAttribPointer(vertLocation,3,GL_FLOAT,GL_FALSE,CoffeeVertex::VERTEX_STRIDE,(GLvoid*)(sizeof(GLfloat)*0));

//    glEnableVertexAttribArray(vertTexCoordLocation);
//    glVertexAttribPointer(vertTexCoordLocation,2,GL_FLOAT,GL_TRUE,CoffeeVertex::VERTEX_STRIDE,(GLvoid*)(sizeof(GLfloat)*3));

//    glEnableVertexAttribArray(vertNormalLocation);
//    glVertexAttribPointer(vertNormalLocation,3,GL_FLOAT,GL_TRUE,CoffeeVertex::VERTEX_STRIDE,(GLvoid*)(sizeof(GLfloat)*(3+2)));

//    glEnableVertexAttribArray(vertTangentLocation);
//    glVertexAttribPointer(vertTangentLocation,3,GL_FLOAT,GL_FALSE,CoffeeVertex::VERTEX_STRIDE,(GLvoid*)(sizeof(GLfloat)*(3+2+3)));

//    glBindBuffer(GL_ARRAY_BUFFER,0);
//    glBindVertexArray(0);

//    object->setVaoHandle(vao);
//    object->setVboHandle(vbo);
//}

void VAOHelper::genTangents(QList<QPointer<CoffeeVertex>> &vertices){
    int pointer = 0;
    while(pointer<vertices.size()){
        QPointer<CoffeeVertex> v0 = vertices.at(pointer+0);
        QPointer<CoffeeVertex> v1 = vertices.at(pointer+1);
        QPointer<CoffeeVertex> v2 = vertices.at(pointer+2);

        glm::vec3 edge1 = v1->position - v0->position;
        glm::vec3 edge2 = v2->position - v0->position;

        float deltaU1 = v1->texCoord.x - v0->texCoord.x;
        float deltaV1 = v1->texCoord.y - v0->texCoord.y;
        float deltaU2 = v2->texCoord.x - v0->texCoord.x;
        float deltaV2 = v2->texCoord.y - v0->texCoord.y;

        float f = 1.0/(deltaU1*deltaV2-deltaU2*deltaV1);

        glm::vec3 t,bt,n;

        t.x = f * (deltaV2 * edge1.x - deltaV1 * edge2.x);
        t.y = f * (deltaV2 * edge1.y - deltaV1 * edge2.y);
        t.z = f * (deltaV2 * edge1.z - deltaV1 * edge2.z);

        bt.x = f * (-deltaU2 * edge1.x - deltaU1 * edge2.x);
        bt.y = f * (-deltaU2 * edge1.y - deltaU1 * edge2.y);
        bt.z = f * (-deltaU2 * edge1.z - deltaU1 * edge2.z);

        n = glm::normalize(glm::cross(t,bt));

        if(!v0->hasNormal)
            v0->normal = n;
        if(!v1->hasNormal)
            v1->normal = n;
        if(!v2->hasNormal)
            v2->normal = n;

        v0->tangent += t;
        v0->bitangent += bt;
        v1->tangent += t;
        v1->bitangent += bt;
        v2->tangent += t;
        v2->bitangent += bt;

        pointer+=3;
    }
}

void VAOHelper::modifyVbo(GLint vboId, QPointer<CoffeeMesh> mesh){
    Q_UNUSED(vboId);
    Q_UNUSED(mesh);
}

void VAOHelper::generateIndices(QPointer<CoffeeMesh> mesh)
{
    QList<glm::vec3> positions;
    QList<glm::vec3> normals;
    QList<glm::vec2> texcoords;
    QList<glm::vec3> tangents;

    QList<int> p_i;
    QList<int> p_n;
    QList<int> p_tx;
    QList<int> p_t;

    for(QPointer<CoffeeVertex> vert : mesh->getVertices()){
        if(!positions.contains(vert->position)){
            positions.append(vert->position);
            p_i.append(positions.indexOf(vert->position));
        }else{
            p_i.append(positions.indexOf(vert->position));
        }
        if(!normals.contains(vert->normal)){
            normals.append(vert->normal);
            p_n.append(normals.indexOf(vert->normal));
        }else{
            p_n.append(normals.indexOf(vert->normal));
        }
        if(!texcoords.contains(vert->texCoord)){
            texcoords.append(vert->texCoord);
            p_tx.append(texcoords.indexOf(vert->texCoord));
        }else{
            p_tx.append(texcoords.indexOf(vert->texCoord));
        }
        if(!positions.contains(vert->position)){
            tangents.append(vert->tangent);
            p_t.append(tangents.indexOf(vert->tangent));
        }else{
            p_t.append(tangents.indexOf(vert->tangent));
        }
    }
    CoffeeMesh* m;
}
