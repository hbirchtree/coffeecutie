#include <coffee/CCore>
#include <coffee/CGraphics>

#include <coffee/graphics_apis/opengl/include/glfunctions.h>

//#include <ext/pcl-shim/include/cpcl.h>

using namespace Coffee;
using namespace CDisplay;
using namespace CResources;
using namespace CGraphicsData;

class CDRenderer : public Coffee::CDisplay::CGLBindingRenderer
{
public:
    CDRenderer()
        : CGLBindingRenderer(0)
    {
        m_msg_filter = coffee_graphics_debug_filter_ignore_notifications;
    }

    void run()
    {
        CResource color_file("cframe.raw");
        CResource depth_file("dframe.raw");
        coffee_file_pull(color_file);
        coffee_file_pull(depth_file);

        CRGBA* color_data = (CRGBA*)color_file.data;
        scalar* depth_data = (scalar*)depth_file.data;

        constexpr byte_t vshader_src[] = {
            "#version 330\n"
            "layout(location = 0) in vec2 position;"
            "uniform mat4 transform;"
            "uniform sampler2D dtext;"
            "out gl_PerVertex {"
            "   vec4 gl_Position;"
            "};"
            "out VData {"
            "   vec2 vtex;"
            "} vdata;"
            "void main(){"
            "   vdata.vtex = position;"
            "   float d = texture(dtext,position).x;"
            "   gl_Position = transform * vec4(vec3(position,0.0),1.0);"
            "}"
        };

        constexpr byte_t fshader_src[] = {
            "#version 330\n"
            "layout(location = 0) out vec4 Out_color;"
            "uniform sampler2D dftext;"
            "in VData {"
            "   vec2 vtex;"
            "} vdata;"
            "void main(){"
            "   vec4 smp = texture(dftext,vdata.vtex);"
            "   Out_color = vec4(1.0);"
            "}"
        };

        CSimplePipeline pl;
        pl.create(vshader_src,fshader_src);
        coffee_graphics_alloc(pl.data_ref());

        CBuffer grid_buf;

        coffee_graphics_alloc(grid_buf);

        std::vector<CVec2> tgrid;

        tgrid.resize(1920*1080);

        for(szptr x=0;x<1920;x++)
            for(szptr y=0;y<1080;y++)
            {
                tgrid[y*1080+x].x() = x;
                tgrid[y*1080+x].y() = y;
            }

        coffee_graphics_buffer_store(
                    grid_buf,tgrid.data(),tgrid.size()*sizeof(tgrid[0]),
                CBufferUsage::StaticDraw);

        CVertexArrayObject vao;
        CVertexDescription va_desc;

        coffee_graphics_alloc(vao);
        coffee_graphics_bind(vao);

        va_desc.addAttribute<scalar,2,CDataType::Scalar>(0);

        va_desc.getBinding(0)->binding = 0;
        va_desc.getBinding(0)->buffer = &grid_buf;

        va_desc.applyAttributes(vao);
        va_desc.bindAttributes(vao);

        CTextureSize texsize = {};
        texsize.w = 1920;
        texsize.h = 1080;
        CByteData texdata;
        texdata.data = (byte_t*)depth_data;
        texdata.size = depth_file.size;
        CBufferedTexture<1> texture;
        texture.createTexture(texsize,CTexIntFormat::RGBA8,CTexType::Tex2D,
                              1,texdata,CTexFormat::RED);
        coffee_graphics_tex_load(texture.sampler(),texture.texture());

        CGCamera cam;
        cam.aspect = 1.6f;
        cam.fieldOfView = 60.f;
        cam.position = CVec3(0,0,0);
        CTransform obj;
        obj.position = CVec3(0,0,-3);
        obj.scale = CVec3(1);

        CMat4 xform = coffee_graphics_gen_perspective(cam)
                * coffee_graphics_gen_transform(cam)
                * coffee_graphics_gen_transform(obj);

        CUniform matuni;
        matuni.object_name = "transform";
        coffee_graphics_uniform_get(pl.vert,matuni);
        glProgramUniformMatrix4fv(pl.vert.handle,matuni.index,1,GL_FALSE,(scalar*)&xform);

        CUniform disptex;
        disptex.object_name = "dtext";
        coffee_graphics_uniform_get(pl.vert,disptex);
        coffee_graphics_tex_make_resident(texture.sampler());
        coffee_graphics_uniform_set_texhandle(pl.vert,disptex,texture.sampler().bhandle);

        CUniform difftex;
        difftex.object_name = "dftext";
        coffee_graphics_uniform_get(pl.frag,difftex);
        coffee_graphics_uniform_set_texhandle(pl.vert,difftex,texture.sampler().bhandle);
        coffee_graphics_uniform_set_texhandle(pl.frag,difftex,texture.sampler().bhandle);

        this->showWindow();
        while(!closeFlag())
        {
            coffee_graphics_clear(CClearFlag::Color);

            coffee_graphics_draw_arrays(CPrimitiveMode::LineStrip,0,1920*1080);

            this->pollEvents();
            this->swapBuffers();
        }

        //Write code here
    }
    void eventHandleD(const CDisplay::CDEvent &e, c_cptr data)
    {
        CSDL2Renderer::eventHandleD(e,data);
    }
    void eventHandleI(const CIEvent &e, c_cptr data)
    {
        CSDL2Renderer::eventHandleI(e,data);
    }
};

int32 coffee_main(int32 argc, byte_t** argv)
{
    CDRendererBase *renderer = new CDRenderer();
    renderer->init(coffee_get_default_visual());
    renderer->run();
    renderer->cleanup();
    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
