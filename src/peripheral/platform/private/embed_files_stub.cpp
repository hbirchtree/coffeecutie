#include <platforms/embed/file.h>

namespace platform::file::embed {

const bool embeds_enabled = false;

semantic::Span<const data_descriptor_t> files_listing()
{
    return {};
}

semantic::Span<const libc_types::u8> files_data()
{
    return {};
}

} // namespace platform::file::embed
