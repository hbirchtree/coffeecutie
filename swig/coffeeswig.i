%module Coffee
%{

#include "../coffee.h"
#include "../coffee/cinput.h"
#include "../coffee/cdisplay.h"
#include "../coffee/cobject.h"
#include "../coffee/display/cdrendererbase.h"
#include "../coffee_impl/context/cdrenderer.h"
#include "../coffee_impl/display/csdl2renderer.h"

%}

%include "../coffee.h"
%include "../coffee_types.h"
%include "../coffee/cdisplay.h"
%include "../coffee/cobject.h"

using namespace Coffee;
using namespace Coffee::CDisplay;

%include "../coffee/cinput.h"
%include "../coffee_impl/context/cdrenderer.h"
%include "../coffee_impl/context/cglbindingrenderer.h"
%include "../coffee_impl/display/csdl2renderer.h"
%include "../coffee_impl/functional/cqueuerunner.h"
%include "../coffee/display/cdrendererbase.h"

%ignore Coffee::CGraphicsWrappers::CGLState;
%ignore Coffee::CGraphicsWrappers::CGLReport;

%ignore Coffee::CFunctional::CThreading::thread_id;

%ignore Coffee::CDisplay::CDQueueRendererBase::contextThread;
%ignore Coffee::CDisplay::CDRendererBase::contextThread;
%ignore Coffee::CDisplay::CGLBindingRenderer::contextThread;
%ignore Coffee::CDisplay::CSDL2Renderer::contextThread;
%ignore Coffee::CDisplay::CDRenderer::contextThread;

%ignore Coffee::CDisplay::CDQueueRendererBase::CDQueueRendererBase(CObject*);
%ignore Coffee::CDisplay::CDRendererBase::CDRendererBase(CObject*);
%ignore Coffee::CDisplay::CGLBindingRenderer::CGLBindingRenderer(CObject*);
%ignore Coffee::CDisplay::CSDL2Renderer::CSDL2Renderer(CObject*);
