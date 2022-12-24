#pragma once

//#include "cgraphics_api.h"

/*!
 * Extensions for fixed-function pipeline APIs
 * This is intended for compatibility with OpenGL 1.x-like APIs,
 *  and should not be used if you are aiming for low overhead.
 */
namespace Coffee {

struct Graphics_Extensions : RHI::GraphicsAPI
{

enum VertAttrType {
    VA_Pos = 0x1000,  /*!< Position */
    VA_Col = 0x2000,  /*!< Color */
    VA_Tex = 0x4000,  /*!< Texture coordinate */
    VA_Nrm = 0x8000,  /*!< Normal*/
    VA_NBT = 0x10000, /*!< Normal/bitangent/tangent */
    
    VA_IndexMask = 0x0FFF, /*!< Indexing mask, if attribute supports multiple values */
};

/*!
 * \brief Reference to the global, fixed-function pipeline object.
 * Uniforms, shader inputs and outputs are all defined by
 *  the hardware/driver of the graphics API and exposed through
 *  this pipeline.
 * \returns
 */
STATICINLINE Pipeline& DefaultPipeline();

};

}
