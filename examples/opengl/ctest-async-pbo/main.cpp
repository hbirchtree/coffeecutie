#include <coffee/CCore>
#include <coffee/CGraphics>
#include <coffee/graphics_apis/COpenGL>
#include <coffee/CImage>

using namespace Coffee;
using namespace CDisplay;
using namespace CGraphicsData;
using namespace CGraphicsWrappers;

void framefun(uint32 t, const void*)
{
    cDebug("Framerate: {0}",t);
}

class CDHudRenderer : public Coffee::CDisplay::CGLBindingRenderer
{
public:
    CDHudRenderer()
        : CGLBindingRenderer(0)
    {
        m_msg_filter = coffee_graphics_debug_filter_ignore_notifications;
    }

    void run()
    {
        CFunctional::CFrameCounter counter(framefun);
        counter.interval = 1000000;
        CElapsedTimerMicro* clock = AllocTimerMicro();
        clock->start();

        const CVec3 vertexdata[] = {
            CVec3(-1.f, 1.f, 0.f),
            CVec3( 1.f, 1.f, 0.f),
            CVec3(-1.f,-1.f, 0.f),
            CVec3( 1.f,-1.f, 0.f),
        };

        const CVec2 texdata[] = {
            CVec2(0.f, 0.f), //1
            CVec2(1.f, 0.f), //2
            CVec2(0.f, 1.f), //3
            CVec2(1.f, 1.f), //4
        };

        const uint32 indexdata[] = {
            0, 1, 2, 1, 2, 3,
        };

        constexpr cstring vshader_src = {
            "#version 330\n"
            "layout(location = 0) in vec3 position;"
            "layout(location = 1) in vec2 texcoord;"
            "layout(location = 2) in mat4 transform;"
            "out gl_PerVertex {"
            "   vec4 gl_Position;"
            "};"
            "out VData {"
            "   vec2 vtex;"
            "} vdata;"
            "void main(){"
            "   vdata.vtex = texcoord;"
            "   gl_Position = transform * vec4(position,1.0);"
            "}"
        };

        constexpr cstring fshader_src = {
            "#version 330\n"
            "layout(location = 0) out vec4 Out_color;"
            "uniform sampler2D diffsamp;"
            "in VData {"
            "   vec2 vtex;"
            "} vdata;"
            "void main(){"
            "   vec4 smp = texture(diffsamp,vdata.vtex);"
            "   Out_color = smp;"
            "}"
        };

        //Creating a shader pipeline
        CSimplePipeline basePipeline;
        basePipeline.create(vshader_src,fshader_src);
        coffee_graphics_bind(basePipeline.data_ref());

        CBuffer vertices;
        CBuffer texcoords;
        CBuffer indices;
        CNBuffer<3> transforms;

        {
            texcoords.type = CBufferType::Array;
            vertices.type = CBufferType::Array;
            indices.type = CBufferType::Index;
            coffee_graphics_alloc(vertices);
            coffee_graphics_alloc(texcoords);
            coffee_graphics_alloc(indices);
            coffee_graphics_alloc(transforms.size,transforms.data,CBufferType::Array);

            coffee_graphics_buffer_store(vertices,
                                         vertexdata,
                                         sizeof(vertexdata),
                                         CBufferUsage::StaticDraw);
            coffee_graphics_buffer_store(texcoords,
                                         texdata,
                                         sizeof(texdata),
                                         CBufferUsage::StaticDraw);
            coffee_graphics_buffer_store(indices,
                                         indexdata,
                                         sizeof(indexdata),
                                         CBufferUsage::StaticDraw);
        }

        CMat4 rtf,wtf,rt;

        //Vertex specification
        CVertexArrayObject vao;
        CVertexDescription vao_desc;

        coffee_graphics_alloc(vao);
        coffee_graphics_bind(vao);

        vao_desc.addAttribute<scalar,3,CDataType::Scalar>(0,vertexdata);
        vao_desc.addAttribute<scalar,2,CDataType::Scalar>(1,texdata);

        vao_desc.getBinding(0)->binding = 0;
        vao_desc.getBinding(0)->buffer = &vertices;
        vao_desc.getBinding(1)->binding = 1;
        vao_desc.getBinding(1)->buffer = &texcoords;

        vao_desc.addAttributeMatrix(2,&rt);

        for(size_t i=0;i<4;i++)
        {
            vao_desc.getBinding(2+i)->binding = 2+i;
            vao_desc.getBinding(2+i)->divisor = 1;
            vao_desc.getBinding(2+i)->buffer = &transforms.current();
        }
        coffee_graphics_vao_attribute_index_buffer(vao,indices);

        vao_desc.applyAttributes(vao);
        vao_desc.bindAttributes(vao);

        //Creating transforms
        CTransform root;
        root.position = CVec3(0,0,0);
        root.scale = CVec3(0.5);

        CGCamera camera;
        camera.aspect = 1.6f;
        camera.position = CVec3(0,0,-3);

        rtf = GenTransform(root);
        wtf = GenPerspective(camera)
                * GenTransform(camera);

        CNode worldNode;
        worldNode.transform = &wtf;

        CNode rootNode;
        rootNode.parent = &worldNode;
        rootNode.transform = &rtf;

        rt = AccumulateTransform(&rootNode);

        for(uint32 i=0;i<transforms.size;i++)
        {
            coffee_graphics_activate(transforms.current());
            coffee_graphics_buffer_store_immutable(
                        transforms.current(),&rt,sizeof(CMat4),
                        CBufferStorage::Coherent|
                        CBufferStorage::Persistent|
                        CBufferStorage::WriteBit);
            coffee_graphics_buffer_map(
                        transforms.current(),
                        CBufferConstants::PersistentAccessFlags());
            transforms.advance();
        }

        //Creating texture

        CUniform texuni;
        {
            texuni.object_name = "diffsamp";

            coffee_graphics_uniform_get(basePipeline.frag,texuni);
        }

        CTextureSize texsize = {};
        texsize.width = texsize.height = 1024;
        CByteData initTexData;
        initTexData.size = coffee_graphics_tex_get_size(texsize,CTexFormat::RGBA);
        initTexData.data = (byte_t*)Alloc(initTexData.size);
        CByteData texstorage_2;
        texstorage_2.size = initTexData.size;
        texstorage_2.data = (byte_t*)Alloc(texstorage_2.size);

        {
            CRGBA* d1 = (CRGBA*)initTexData.data;
            CRGBA* d2 = (CRGBA*)texstorage_2.data;

            for(int64 i=0;i<texsize.width*texsize.height;i++)
            {
                d1[i].r = 255;
                d2[i].r = 127;

                d1[i].a = 255;
                d2[i].a = 255;
            }
        }

        CBufferedTexture<2> texture;
        texture.createTexture(texsize,CTexIntFormat::RGBA8,CTexType::Tex2D,
                              1,initTexData,CTexFormat::RGBA);
        coffee_graphics_tex_load(texture.sampler(),texture.texture());

        //Enable some states
        coffee_graphics_blend(true);
        coffee_graphics_depth(true);

        //Create a drawcall
        CGLDrawCall drawcall;
        drawcall.count = sizeof(indexdata)/sizeof(uint32);
        drawcall.instanceCount = 1;

        //Set uniform, a texture handle
        coffee_graphics_tex_load_safe(texture.sampler(),texture.texture());
        coffee_graphics_uniform_set_texhandle_safe(basePipeline.frag,texuni,
                                                   texture.sampler().unit);
//        coffee_graphics_uniform_set_texhandle(basePipeline.frag,texuni,
//                                              texture.sampler().bhandle);

        CTextureData texture_data;
        texture_data.datatype = CDataType::UByte;
        texture_data.format = CTexFormat::RGBA;
        texture_data.size = texture.texture().size;

        bool flop = false;

        this->showWindow();
        while(!closeFlag())
        {
            coffee_graphics_clear(CClearFlag::Color|CClearFlag::Depth);

            {
                camera.position.x() = CMath::fmod(this->contextTime(),3.0)-1.5;

                wtf = GenPerspective(camera)
                        * GenTransform(camera);
                rt = AccumulateTransform(&rootNode);

                CMemCpy(transforms.current().data,&rt,sizeof(rt));

                for(int i=0;i<4;i++)
                {
                    CVertexBufferBinding* bind = vao_desc.getBinding(2+i);
                    coffee_graphics_vao_attribute_bind_buffer(
                                vao,
                                *bind,
                                transforms.current());
                }

            }

            {
                if(!texstore_operations)
                    texture.uploadData(CTexFormat::RGBA,0);

                CByteData* dSrc;
                if(flop)
                    dSrc = &initTexData;
                else
                    dSrc = &texstorage_2;
                flop = !flop;

                if(!texstore_operations)
                    texture.prefillBuffer(*dSrc);
                else
                {
                    texture_data.data = dSrc->data;
                    coffee_graphics_tex_store_2d(texture.texture(),
                                                 texture_data,
                                                 0);
                }
            }

            counter.update(clock->elapsed());
            coffee_graphics_draw_indexed(CPrimitiveMode::Triangles,drawcall);
            texture.advance();

            this->swapBuffers();
            this->pollEvents();
        }

        CFree(initTexData.data);
        CFree(texstorage_2.data);

        coffee_graphics_free(vao);
        coffee_graphics_free(vertices);
        coffee_graphics_free(texcoords);
        coffee_graphics_free(transforms.size,transforms.data);
        coffee_graphics_free(indices);

        FreeTimer(clock);
    }

    void eventHandleD(const CDisplay::CDEvent &e, c_cptr data)
    {
        CSDL2Renderer::eventHandleD(e,data);
    }
    void eventHandleI(const CIEvent &e, c_cptr data)
    {
        CSDL2Renderer::eventHandleI(e,data);
        switch(e.type)
        {
        case CIEvent::Keyboard:
        {
            const CIKeyEvent* kev = (const CIKeyEvent*)data;
            if(kev->key == CK_Escape)
                this->closeWindow();
            else if(kev->key == CK_Space && (kev->mod&CIKeyEvent::PressedModifier))
            {
                cDebug("Switching operation mode");
                texstore_operations = !texstore_operations;
            }
            break;
        }
        default:break;
        }
    }
private:
    bool texstore_operations = false;
    CQuat t;
};

int32 coffee_main(int32, cstring_w*)
{
    CResources::FileResourcePrefix("sample_data/");

    CDRendererBase *renderer = new CDHudRenderer();
    CDProperties props = GetDefaultVisual();
    props.gl.flags =
            props.gl.flags|
//            CGLContextProperties::GLFeatureLevelProfile|
            GLProperties::GLDebug;
    renderer->init(props);
    renderer->run();
    renderer->cleanup();

    for(const std::pair<CString,CString>& ft : coffee_glbinding_get_graphics_feature_level())
        cBasicPrint("{1} : {0}",ft.first.c_str(),ft.second.c_str());

    delete renderer;

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)

