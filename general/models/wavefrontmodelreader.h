#ifndef WAVEFRONTMODELREADER
#define WAVEFRONTMODELREADER

#include <QListIterator>
#include "general/filehandler.h"
#include "opengl/rendering/coffeemesh.h"
#include "opengl/rendering/coffeevertex.h"
#include "opengl/components/coffeematerial.h"
class WavefrontModelReader{
    class ModelContainer {
        QPointer<CoffeeMesh> model;
        QPointer<CoffeeMaterial> material;
    };

public:
    QList<ModelContainer> parseModel(QString file){
        QStringList data = FileHandler::getStringListFromFile(file);
        QListIterator<QString> it(data);
        int vertex_c = 0;
        int normal_c = 0;
        int texcrd_c = 0;
        QString itv;
        while(it.hasNext()){
            itv = it.next();
            if(itv.startsWith("mtllib ")){
                parseMtlFile(QFileInfo(file).absoluteDir().absolutePath()+itv.mid(7));
                continue;
            }else if(itv.startsWith("o ")&&it.hasNext()){

            }
        }
    }
    void parseMtlFile(QString file){
        QStringList data = FileHandler::getStringListFromFile(file);

        QListIterator<QString> it(data);
        QString line;
        while(it.hasNext()){
            line = it.next();
            if(line.startsWith("newmtl ")){
                CoffeeMaterial mtl;
                QString name = line.mid(7);
                while(it.hasNext()){
                    line = it.next();
                    if(line.startsWith("Ns ")){
                        mtl.shininess()->setValue(line.mid(3).toFloat());
                    }else if(line.startsWith("Ks ")){
                        mtl.setSpecularColor(parseStrVecf(line.mid(3),' '));
                    }else if(line.startsWith("d ")){
                        mtl.transparency()->setValue(line.mid(2).toFloat());
                    }else if(line.startsWith("map_Kd ")){
                        mtl.setDiffuseTexture(line.mid(7));
                    }else if(line.startsWith("map_Ks ")){
                        mtl.setSpecularTexture(line.mid(7));
                    }else if(line.startsWith("map_Ns ")){
                        mtl.setHighlightTexture(line.mid(7));
                    }else if(line.startsWith("map_d ")){
                        mtl.setTransparencyTexture(line.mid(6));
                    }else if(line.startsWith("map_Bump ")){
                        mtl.setBumpTexture(line.mid(9));
                    }else if(line.startsWith("bump ")){
                        mtl.setDiffuseTexture(line.mid(5));
                    }
                    if(it.peekNext().startsWith("newmtl "))
                        break;
                }
                materials.insert(name,mtl);
            }
        }
    }
    static glm::vec3 parseStrVecf(QString src,QChar sep){
        QStringList ls = src.split(sep);
        if(ls.size()!=3)
            return glm::vec3();
        else
            return glm::vec3(ls.at(0).toFloat(),ls.at(1).toFloat(),ls.at(2).toFloat());
    }
    static QList<int> parseStrVec(QString src,QChar sep){
        QStringList ls = src.split(sep);
        if(ls.size()!=3)
            return glm::vec3();
        else
            return glm::vec3(ls.at(0).toFloat(),ls.at(1).toFloat(),ls.at(2).toFloat());
    }

private:
    QHash<QString,CoffeeMaterial> materials;
    QHash<QString,CoffeeMesh> models;
};

#endif // WAVEFRONTMODELREADER

