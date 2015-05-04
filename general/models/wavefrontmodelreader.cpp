#include "wavefrontmodelreader.h"

WavefrontModelReader::WavefrontModelReader(QObject *parent) : QObject(parent)
{
}
WavefrontModelReader::~WavefrontModelReader()
{

}


QHash<QString,QPointer<ModelReaderInterface::ModelContainer> > WavefrontModelReader::parseModel(QString file){
    QStringList data = FileHandler::getStringListFromFile(file);
    if(data.isEmpty())
        return QHash<QString,QPointer<ModelContainer> >();
    QListIterator<QString> it(data);
    int vertex_c = 0;
    int normal_c = 0;
    int texcrd_c = 0;
    QString itv;
    while(it.hasNext()){
        itv = it.next();
        if(itv.startsWith("mtllib ")){
            parseMtlFile(QFileInfo(file).absoluteDir().absolutePath()+QDir::separator()+itv.mid(7));
            continue;
        }else if(itv.startsWith("o ")&&it.hasNext()){
            QPointer<ModelContainer> mdl = new ModelContainer();
            mdl->mdlName = itv.mid(2);
            while(it.hasNext()){
                itv = it.next();
                if(itv.startsWith("usemtl "))
                    mdl->mtlName = itv.mid(7);
                if(itv.startsWith("v ")){
                    mdl->vertices.append(parseStrVec3f(itv.mid(2),' '));
                    vertex_c++;
                }
                if(itv.startsWith("vn ")){
                    mdl->vnormals.append(parseStrVec3f(itv.mid(3),' '));
                    normal_c++;
                }
                if(itv.startsWith("vt ")){
                    glm::vec2 tc = parseStrVec2f(itv.mid(3),' ');
                    mdl->txcoords.append(glm::vec2(tc.x,1-tc.y));
                    texcrd_c++;
                }
                if(itv.startsWith("f "))
                    mdl->faces.append(itv.mid(2).split(' '));
                if(it.hasNext()&&it.peekNext().startsWith("o "))
                    break;
            }
            mdl->model = new CoffeeMesh(this->parent());
            for(QStringList face : mdl->faces) //We will always assume that the object has all the vertices it needs
                for(QString vert : face){
                    QPointer<CoffeeVertex> vertex = new CoffeeVertex(this->parent()); //We use the reader's parent as our parent
                    QStringList pts = vert.split('/');
                    for(int i=0;i<3;i++)
                        switch(i){
                        case 0:{
                            int vrt = pts.at(0).toInt()-1-vertex_c+mdl->vertices.size();
                            if(vrt<0)
                                break;
                            vertex->position = mdl->vertices.at(vrt);
                            break;
                        }
                        case 1:{
                            if(pts.size()<2)
                                break;
                            int vrt = pts.at(1).toInt()-1-texcrd_c+mdl->txcoords.size();
                            if(vrt<0)
                                break;
                            vertex->texCoord = mdl->txcoords.at(vrt);
                            break;
                        }
                        case 2:{
                            if(pts.size()<3)
                                break;
                            int vrt = pts.at(2).toInt()-1-normal_c+mdl->vnormals.size();
                            if(vrt<0)
                                break;
                            vertex->normal = mdl->vnormals.at(vrt);
                            vertex->hasNormal = true;
                            break;
                        }
                        }
                    mdl->model->addVertex(vertex);
                }
            models.insert(mdl->mdlName,mdl);
        }
    }
    for(QPointer<ModelContainer> mdl : models.values()){
        mdl->model->raw_vertices = mdl->vertices;
        mdl->model->raw_texcoords = mdl->txcoords;
        if(!mdl->mtlName.isEmpty()&&materials.contains(mdl->mtlName)){
            mdl->material = materials.value(mdl->mtlName);
        }else
            mdl->material = new CoffeeMaterial(mdl->parent());
        VAOHelper::genTangents(mdl->model);
    }
    return models;
}

QHash<QString,QPointer<CoffeeMaterial> > WavefrontModelReader::getMaterials(){
    return materials;
}

glm::vec3 WavefrontModelReader::parseStrVec3f(QString src,QChar sep){
    QStringList ls = src.split(sep);
    if(ls.size()!=3)
        return glm::vec3();
    else
        return glm::vec3(ls.at(0).toFloat(),ls.at(1).toFloat(),ls.at(2).toFloat());
}
glm::vec2 WavefrontModelReader::parseStrVec2f(QString src,QChar sep){
    QStringList ls = src.split(sep);
    if(ls.size()!=2)
        return glm::vec2();
    else
        return glm::vec2(ls.at(0).toFloat(),ls.at(1).toFloat());
}
QList<int> WavefrontModelReader::parseStrVec(QString src,QChar sep){
    QStringList ls = src.split(sep);
    if(ls.size()!=3)
        return QList<int>();
    else
        return QList<int>() << ls.at(0).toFloat() << ls.at(1).toFloat() << ls.at(2).toFloat();
}


void WavefrontModelReader::parseMtlFile(QString file){
    QStringList data = FileHandler::getStringListFromFile(file);

    QString filePrefix = QFileInfo(file).absoluteDir().absolutePath();

    QListIterator<QString> it(data);
    QString line;
    while(it.hasNext()){
        line = it.next();
        if(line.startsWith("newmtl ")){
            QPointer<CoffeeMaterial> mtl = new CoffeeMaterial(this->parent());
            QString name = line.mid(7);
            QString arg;
            QString filename;
            while(it.hasNext()){
                line = it.next();
                arg = line.mid(line.indexOf(' ')+1);
                if(line.startsWith("Ns ")){
                    mtl->shininess()->setValue(arg.toFloat());
                }else if(line.startsWith("Ks ")){
                    mtl->setSpecularColor(parseStrVec3f(arg,' '));
                }else if(line.startsWith("d ")){
                    mtl->transparency()->setValue(arg.toFloat());
                }else if(line.startsWith("map_Kd ")){
                    if(!arg.startsWith(QDir::separator()))
                        arg.prepend(filePrefix+QDir::separator());
                    mtl->setTexture(CoffeeTexture::Texture_Diffusion,
                                    QSharedPointer<CoffeeTexture>(new CoffeeTexture(mtl->parent(),arg)));
                }else if(line.startsWith("map_Ks ")){
                    if(!arg.startsWith(QDir::separator()))
                        arg.prepend(filePrefix+QDir::separator());
                    mtl->setTexture(CoffeeTexture::Texture_Specular,
                                    QSharedPointer<CoffeeTexture>(new CoffeeTexture(mtl->parent(),arg)));
                }else if(line.startsWith("map_Ns ")){
                    if(!arg.startsWith(QDir::separator()))
                        arg.prepend(filePrefix+QDir::separator());
                    mtl->setTexture(CoffeeTexture::Texture_Highlight,
                                    QSharedPointer<CoffeeTexture>(new CoffeeTexture(mtl->parent(),arg)));
                }else if(line.startsWith("map_d ")){
                    if(!arg.startsWith(QDir::separator()))
                        arg.prepend(filePrefix+QDir::separator());
                    mtl->setTexture(CoffeeTexture::Texture_Transparency,
                                    QSharedPointer<CoffeeTexture>(new CoffeeTexture(mtl->parent(),arg)));
                }else if(line.startsWith("map_Bump ")){
                    if(!arg.startsWith(QDir::separator()))
                        arg.prepend(filePrefix+QDir::separator());
                    mtl->setTexture(CoffeeTexture::Texture_Bumpmap,
                                    QSharedPointer<CoffeeTexture>(new CoffeeTexture(mtl->parent(),arg)));
                }else if(line.startsWith("bump ")){
                    if(!arg.startsWith(QDir::separator()))
                        arg.prepend(filePrefix+QDir::separator());
                    mtl->setTexture(CoffeeTexture::Texture_Bumpmap,
                                    QSharedPointer<CoffeeTexture>(new CoffeeTexture(mtl->parent(),arg)));
                }
                if(it.hasNext()&&it.peekNext().startsWith("newmtl "))
                    break;
            }
            materials.insert(name,mtl);
        }
    }
}
