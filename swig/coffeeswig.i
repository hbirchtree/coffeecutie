%module Coffee
%{

#include "coffeeswig.h"

%}

%include "coffeeswig.h"

using namespace Coffee;
using namespace Coffee::CFunctional;
using namespace Coffee::CDisplay;
using namespace Coffee::CResources;

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

