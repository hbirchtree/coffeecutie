#ifndef COFFEE
#define COFFEE

namespace Coffee{
    class CObject;

    namespace CInput{
//        using CInputHandlerFunction = void (*void)(void*,uint32_t);
        typedef std::function<void(void*,uint32_t)> CInputHandlerFunction;

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
        typedef std::function<void()> QueueFunction;

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
