#include <coffee/glad/glad_comp.h>

#if defined(GLADCOMP_COMPILE_CORE)
#include <glad/glad.h>
#elif defined(GLADCOMP_COMPILE_ES)
#include <glad_es/glad.h>
#endif

namespace glad {

void Binding::load(entity_container& e, comp_app::app_error& ec)
{
#if defined(GLADCOMP_COMPILE_CORE)
    (void)e;

    if(!gladLoadGL())
    {
        ec = comp_app::AppError::BindingFailed;
    }
#elif defined(GLADCOMP_COMPILE_ES)
    auto loader = e.service<comp_app::GraphicsBindingConfig>()->loader;
    if(!gladLoadGLES2Loader(loader))
    {
        ec = comp_app::AppError::BindingFailed;
    }
#else
    static_assert(false, "no binding set up");
#endif
}

}
