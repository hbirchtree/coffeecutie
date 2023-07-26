#include <coffee/glad/glad_comp.h>

#include <coffee/comp_app/gl_config.h>
#include <coffee/comp_app/subsystems.h>
#include <coffee/components/components.h>

#if defined(GLADCOMP_COMPILE_CORE)
#include <glad/gl.h>
#else
#include <glad/gles2.h>
#endif

namespace glad {

void Binding::load(entity_container& e, comp_app::app_error& ec)
{
    auto loader = reinterpret_cast<GLADloadfunc>(
        e.service<comp_app::AppLoader>()
            ->config<comp_app::GraphicsBindingConfig>()
            .loader);
#if defined(GLADCOMP_COMPILE_CORE)
    if(!gladLoadGL(loader))
    {
        ec = comp_app::AppError::BindingFailed;
    } else
    {
        ec = comp_app::AppError::None;
        return;
    }
#elif defined(GLADCOMP_COMPILE_ES)

    if(!loader)
    {
        ec = "No dynamic loader provided";
        ec = comp_app::AppError::BindingFailed;
        return;
    }

    if(gladLoadGLES2(loader) == 0)
    {
        ec = "Failed to load function pointers";
        ec = comp_app::AppError::BindingFailed;
    }
#else
    static_assert(false, "no binding set up");
#endif
}

} // namespace glad
