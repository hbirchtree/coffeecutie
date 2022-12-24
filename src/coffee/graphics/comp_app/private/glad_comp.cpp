#include <coffee/glad/glad_comp.h>

#include <coffee/comp_app/gl_config.h>
#include <coffee/components/components.h>

#include <glad/glad.h>

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
    auto loader = e.service<comp_app::AppLoader>()
                      ->config<comp_app::GraphicsBindingConfig>()
                      .loader;

    if(!loader)
    {
        ec = "No dynamic loader provided";
        ec = comp_app::AppError::BindingFailed;
        return;
    }

    if(gladLoadGLES2Loader(loader) == 0)
    {
        ec = "Failed to load function pointers";
        ec = comp_app::AppError::BindingFailed;
    }
#else
    static_assert(false, "no binding set up");
#endif
}

} // namespace glad
