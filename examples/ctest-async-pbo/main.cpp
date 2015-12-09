#include <coffee/CCore>
#include <coffee/core/CGraphics>
#include <coffee/core/plat/application_start.h>

using namespace Coffee;
using namespace CDisplay;

class CDRenderer : public Coffee::CDisplay::CGLBindingRenderer
{
public:
    CDRenderer()
        : CGLBindingRenderer(0)
    {
    }

    void run()
    {
        CTexture testTex;
        CNBuffer<2> buffers;

        testTex.textureType = CTexType::Tex2D;
        testTex.levels = 1;
        testTex.format = CTexIntFormat::RED8;

        CTextureSize size;
        size.w = 1024;
        size.h = 1024;

        CTextureData testdata;
        testdata.size = size;

        cstring_w imgdata = (cstring_w)c_calloc(1,1024*1024*sizeof(uint32));

        coffee_graphics_alloc(2,CBufferType::PixelUnpack,buffers.data);
        coffee_graphics_alloc(&testTex);

        for(size_t i=0;i<2;i++)
        {
            coffee_graphics_activate(&buffers.current());
            coffee_graphics_buffer_store_immutable(
                        &buffers.current(),
                        imgdata,1024*1024*sizeof(uint32),
                        CBufferStorage::Coherent|CBufferStorage::Persistent|
                        CBufferStorage::WriteBit|CBufferStorage::Dynamic);
            coffee_graphics_buffer_map(&buffers.current(),
                                       CBufferAccess::Coherent|CBufferAccess::Persistent|
                                       CBufferAccess::WriteBit);
            buffers.advance();
        }

        coffee_graphics_activate(&testTex);
        coffee_graphics_tex_define(&testTex);

        CTextureRegion reg = {};
        reg.w = size.w;
        reg.h = size.h;

        CElapsedTimerMicro* mtmr = coffee_fun_alloc_timer_micro();
        this->showWindow();
        while(!closeFlag())
        {
            mtmr->start();
            coffee_graphics_clear(CClearFlag::Color);

            coffee_graphics_tex_pbo_upload(
                        &testTex,&buffers.current(),
                        CTexFormat::RGBA,reg,CDataType::UByte,1);
            coffee_graphics_memory_barrier(CMemoryBarrier::PixelBuffer);

            {
                CRGBA* texture = (CRGBA*)buffers.next().data;
                for(size_t i=0;i<size.w;i++)
                    for(size_t j=0;j<size.h;j++)
                        texture[j*size.h + i].r++;
            }

            buffers.advance();
            this->pollEvents();
            this->swapBuffers();
            cDebug("Frametime: %lld",mtmr->elapsed());
        }

        coffee_graphics_free(&testTex);
        coffee_graphics_free(2,buffers.data);
    }
    void eventWindowsHandle(const CDisplay::CDEvent *e)
    {
        if(e->type==CDEvent::State)
        {
            const CDStateEvent* sev = (const CDStateEvent*)&e[1];
            if(sev->type==CDStateEvent::Closed)
                this->closeWindow();
        }
    }
    void eventInputHandle(const CIEvent *e)
    {
        CSDL2Renderer::eventInputHandle(e);
        if(e->type==CIEvent::Keyboard)
        {
            const CIKeyEvent* kev = (const CIKeyEvent*)&e[1];
            if(kev->key == CK_Escape)
                this->closeWindow();
        }
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
