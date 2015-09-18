#include "cmultidrawgraph.h"

namespace Coffee {
namespace CRendering {

CMultiDrawGraph::CMultiDrawGraph()
{
    drawcalls.create();
    drawcalls.bufferType = GL_DRAW_INDIRECT_BUFFER;
    drawcalls.flags = GL_DYNAMIC_STORAGE_BIT;
//    desc->vao.create();

//    desc->vao.bind();

//    int idx = 0;
//    int i,j;
//    for(CVertexAttribute& attr : desc->attributes){
//        CBuffer buffer;
//        buffer.create();
//        buffer.bind();
//        if(attr.flags&CVertexAttribute::MatrixType){
//            for(i=0;i<attr.rows;i++)
//                desc->vao.addAttributeDivided(
//                            attr.location,attr.type,
//                            attr.normalized,
//                            attr.size,attr.stride,
//                            attr.divisor,attr.offset);
//        }else{
//            desc->vao.addAttribute(&attr);
//        }
//        idx++;
    //    }
}

void CMultiDrawGraph::addDrawCalls(CGLDrawCall *dc,szptr numCalls)
{
    //"Resize" the buffer
    //You should not add drawcalls during operation, use existing ones
    //Those will use faster methods
    GLuint old = drawcalls.handle;
    drawcalls.create();
    drawcalls.store(drawcalls.size+sizeof(CGLDrawCall)*numCalls,nullptr);
    drawcalls.subCopy(old,0,0,drawcalls.size);
    glDeleteBuffers(1,&old);

    drawcalls.subStore(drawcalls.size,sizeof(CGLDrawCall)*numCalls,dc);

    drawcalls.size += sizeof(CGLDrawCall)*numCalls;
    numPrimitives++;
}

void CMultiDrawGraph::render()
{
    drawcalls.bind();
    glMultiDrawElementsIndirect(GL_TRIANGLES,GL_UNSIGNED_INT,
                                0,numPrimitives,
                                sizeof(CGLDrawCall));
}

} // namespace CRendering
} // namespace Coffee

