#ifndef COFFEE
#define COFFEE

namespace Coffee{
    class CObject;

    namespace CInput{
        using CInputHandlerFunction = void (void*)(void* data,uint32_t size);

        struct CIEvent;
        struct CIKeyEvent;
        struct CIMouseEvent;
        struct CIScrollEvent;
        struct CIDropEvent;

        struct CIJoyInfo;
        struct CIJoyState;

        class CIEventParser;
    }

    namespace CDisplay{
        class CDRendererBase;
        class CDRenderer;
    }

    namespace CFunctional{
        using QueueFunction = void (void*)();

        class CRQueueRunner;
    }

    namespace CFiles{
        class CFResource;
        class CFAssetStorage;
        class CFObjectImport;
    }

    namespace CScripting{
        class CSScriptEngine;
        class CSObject;
    }
}

#endif // COFFEE
