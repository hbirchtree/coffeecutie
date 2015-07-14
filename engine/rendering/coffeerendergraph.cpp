#include "coffeerendergraph.h"

/* Goal:
 *  - Set up shaders, objects, buffers, uniforms and textures for rendering
 *  - Put shaders in uniform buffers such that we may update uniforms only when they change
 *  - Only set uniforms which exist by checking the shader. If it fails once, never try again.
 *  - Avoid storing too much information about state
 */

CoffeeRenderGraph::CoffeeRenderGraph(QObject *parent) : QObject(parent)
{

}
