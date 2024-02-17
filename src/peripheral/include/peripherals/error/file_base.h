#pragma once

#include <peripherals/stl/error_code.h>
#include <peripherals/stl/types.h>

namespace platform {
namespace file {

using namespace ::stl_types;

enum class FileError
{
    NotFound = 1,
    MappingFailed,
    PermissionError,
    InvalidAccess,
    InvalidHandle,
    ReadFailed,
    WriteFailed,

    SystemError,
};

struct file_error_category : error_category
{
    virtual const char* name() const noexcept;
    virtual std::string message(int) const;
};

template<typename NestedError>
using base_file_error =
    nested_domain_error_code<FileError, file_error_category, NestedError>;

template<typename NestedError>
struct FileBase
{
    typedef base_file_error<NestedError> file_error;
};
} // namespace file
} // namespace platform
