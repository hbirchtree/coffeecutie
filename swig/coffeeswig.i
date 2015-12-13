%module Coffee
%{

#include "coffeeswig.h"

%}

%include "coffeeswig.h"

%ignore Coffee::CFunctional::CThreading::thread_id;

%ignore Coffee::CDisplay::CDQueueRendererBase::contextThread;
%ignore Coffee::CDisplay::CDRendererBase::contextThread;
%ignore Coffee::CDisplay::CGLBindingRenderer::contextThread;
%ignore Coffee::CDisplay::CSDL2Renderer::contextThread;
%ignore Coffee::CDisplay::CDRenderer::contextThread;

%ignore Coffee::_cbasic_static_map;
%ignore Coffee::_cbasic_raii_container;

%ignore Coffee::CGraphicsWrappers::CImportedTexture::CImportedTexture;
