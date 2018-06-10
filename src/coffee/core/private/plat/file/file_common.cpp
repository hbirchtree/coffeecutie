#include <coffee/core/plat/file/file_def.h>

#include <coffee/core/plat/plat_environment.h>

namespace Coffee{
namespace CResources{

const char* file_error_category::name() const noexcept
{
    return "FileFun";
}

std::string file_error_category::message(int error_code) const
{
    FileError ec = C_CAST<FileError>(error_code);

    switch(ec)
    {
    default:
        return "";
    }
}

}
}
