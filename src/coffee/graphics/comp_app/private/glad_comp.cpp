#include <coffee/glad/glad_comp.h>

#include <coffee/comp_app/gl_config.h>
#include <coffee/components/components.h>

#include <glad/glad.h>

namespace glad {

void Binding::load(entity_container& e, comp_app::app_error& ec)
{
    [[maybe_unused]] auto loader = e.service<comp_app::AppLoader>()
                      ->config<comp_app::GraphicsBindingConfig>()
                      .loader;
#if defined(GLADCOMP_COMPILE_CORE)
    (void)e;
    if(!gladLoadGL())
    {
        ec = comp_app::AppError::BindingFailed;
    } else
        return;
#if defined(GLADCOMP_USE_GETPROC)
    if(!gladLoadGLLoader(loader))
    {
        ec = comp_app::AppError::BindingFailed;
    } else
    {
        ec = comp_app::AppError::None;
        return;
    }
#endif
#elif defined(GLADCOMP_COMPILE_ES)

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
