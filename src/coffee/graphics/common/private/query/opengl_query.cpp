#include <coffee/graphics/apis/gleam/levels/all_levels.h>
#include <coffee/graphics/apis/gleam/levels/gl_loader.h>
#include <coffee/graphics/common/query/gpu_query.h>
#include <peripherals/stl/type_safety.h>

#define GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX 0x9047
#define GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX 0x9048
#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX 0x9049

extern "C" GpuQueryFunction* GetGpuQuery();

bool HasExtension(C_UNUSED(Coffee::std::string const& ext_name))
{
    using namespace Coffee;
    using namespace CGL;

#if GL_VERSION_VERIFY(0x300, GL_VERSION_NONE)
    i32 num = 0;
    gl::vlow::IntegerGetv(GL_NUM_EXTENSIONS, &num);
    for(auto i : Range<i32>(num))
    {
        if(gl::vlow::GetStringi(GL_EXTENSIONS, C_FCAST<u32>(i)) == ext_name)
            return true;
    }
#endif

    return false;
}

Coffee::GpuInfo::GpuQueryInterface GetQuery()
{
    using namespace Coffee;
    using namespace Coffee::CGL;

    using MemStat = GpuInfo::MemStatus;

    using GL = gl::vlow;

    if(!GL::GetString(GL_VERSION))
        return {};

    return {[]() {
                auto device = Debug::Renderer();

                return SWVersionInfo(device.firmware, 1, 0);
            },
            []() { return C_FCAST<u32>(1); },
            [](u32) { return Debug::Renderer(); },
            [](u32) {
                MemStat out = {};

                if(HasExtension("GL_NVX_gpu_memory_info"))
                {
                    i32 total, dedic, freem;

                    gl::vlow::IntegerGetv(
                        GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &total);
                    gl::vlow::IntegerGetv(
                        GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &freem);
                    gl::vlow::IntegerGetv(
                        GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &dedic);

                    out.free  = C_FCAST<u32>(freem);
                    out.total = C_FCAST<u32>(total);
                    out.used  = C_FCAST<u32>(total - freem);
                }
                return out;
            },
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            nullptr};
}

GpuQueryFunction* GetGpuQuery()
{
    auto interface = GetQuery();

    if(!interface.GetDriver)
        return nullptr;

    return new GpuQueryFunction{GetQuery};
}
