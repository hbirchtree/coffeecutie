#include <platforms/embed/file.h>

namespace platform {
namespace file {
namespace embed {

bool embeds_enabled = false;

bool file_lookup(file_reference_t, semantic::Bytes&)
{
    Throw(undefined_behavior(
        "calling into embed::file_lookup without it being present"));
}

} // namespace embed
} // namespace file
} // namespace platform
