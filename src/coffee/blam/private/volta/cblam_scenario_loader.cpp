#include <coffee/blam/volta/cblam_scenario_loader.h>

#include <coffee/blam/volta/blam_stl.h>
#include <coffee/blam/volta/cblam_map.h>
#include <coffee/strings/libc_types.h>

#include <coffee/core/CDebug>

namespace blam {
namespace scn {

const scenario* get(tag_index_view& tags)
{
    static const auto pred = [](index_item_t const* v) {
        return v->matches(tag_class_t::scnr);
    };

    auto base_it = std::find_if(tags.begin(), tags.end(), pred);

    if(base_it == tags.end())
        return nullptr;

    auto base = *base_it;

    return magic_ptr<scenario const*>(
        tags.file(), tags.tags()->index_magic, base->offset);
}

} // namespace scn
} // namespace blam
