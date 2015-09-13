#include "cmultidrawgraph.h"

namespace Coffee {
namespace CRendering {

CMultiDrawGraph::CMultiDrawGraph(CMultiDrawDescriptor *desc)
{
    desc->vao.create();

    desc->vao.bind();

    int idx = 0;
    int i,j;
    for(CVertexAttribute& attr : desc->attributes){
        CBuffer buffer;
        buffer.create();
        buffer.bind();
        if(attr.flags&CVertexAttribute::MatrixType){
            for(i=0;i<attr.rows;i++)
                desc->vao.addAttributeDivided(
                            attr.location,attr.type,
                            attr.normalized,
                            attr.size,attr.stride,
                            attr.divisor,attr.offset);
        }else{
            desc->vao.addAttribute(&attr);
        }
        idx++;
    }
}

void CMultiDrawGraph::addMesh(CResourceTypes::CAssimp::CAssimpMesh *mesh)
{

}

} // namespace CRendering
} // namespace Coffee

