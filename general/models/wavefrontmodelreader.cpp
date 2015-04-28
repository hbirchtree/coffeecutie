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
                    mdl->txcoords.append(parseStrVec2f(itv.mid(3),' '));
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
                            vertex->position = mdl->vertices.at(vrt);
                            break;
                        }
                        case 1:{
                            if(pts.size()<2)
                                break;
                            int vrt = pts.at(1).toInt()-1-texcrd_c+mdl->txcoords.size();
                            vertex->texCoord = mdl->txcoords.at(vrt);
                            break;
                        }
                        case 2:{
                            if(pts.size()<3)
                                break;
                            int vrt = pts.at(2).toInt()-1-normal_c+mdl->vnormals.size();
                            vertex->normal = mdl->vnormals.at(vrt);
                            break;
                        }
                        }
                    mdl->model->addVertex(vertex);
                }
            models.insert(mdl->mdlName,mdl);
        }
    }
    for(QPointer<ModelContainer> mdl : models.values()){
        if(!mdl->mtlName.isEmpty()&&materials.contains(mdl->mtlName)){
            mdl->material = materials.value(mdl->mtlName);
        }else
            mdl->material = new CoffeeMaterial(mdl->parent());
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
    if(ls.size()!=3)
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
            while(it.hasNext()){
                line = it.next();
                if(line.startsWith("Ns ")){
                    mtl->shininessObject()->setValue(line.mid(3).toFloat());
                }else if(line.startsWith("Ks ")){
                    mtl->setSpecularColor(parseStrVec3f(line.mid(3),' '));
                }else if(line.startsWith("d ")){
                    mtl->transparencyObject()->setValue(line.mid(2).toFloat());
                }else if(line.startsWith("map_Kd ")){
                    mtl->setTexture(CoffeeTexture::Texture_Diffusion,new CoffeeTexture(mtl->parent(),filePrefix+QDir::separator()+line.mid(7)));
                }else if(line.startsWith("map_Ks ")){
                    mtl->setTexture(CoffeeTexture::Texture_Specular,new CoffeeTexture(mtl->parent(),filePrefix+QDir::separator()+line.mid(7)));
                }else if(line.startsWith("map_Ns ")){
                    mtl->setTexture(CoffeeTexture::Texture_Highlight,new CoffeeTexture(mtl->parent(),filePrefix+QDir::separator()+line.mid(7)));
                }else if(line.startsWith("map_d ")){
                    mtl->setTexture(CoffeeTexture::Texture_Transparency,new CoffeeTexture(mtl->parent(),filePrefix+QDir::separator()+line.mid(6)));
                }else if(line.startsWith("map_Bump ")){
                    mtl->setTexture(CoffeeTexture::Texture_Bumpmap,new CoffeeTexture(mtl->parent(),filePrefix+QDir::separator()+line.mid(9)));
                }else if(line.startsWith("bump ")){
                    mtl->setTexture(CoffeeTexture::Texture_Bumpmap,new CoffeeTexture(mtl->parent(),filePrefix+QDir::separator()+line.mid(5)));
                }
                if(it.hasNext()&&it.peekNext().startsWith("newmtl "))
                    break;
            }
            materials.insert(name,mtl);
        }
    }
}
