#include <coffee/blam/volta/cblam_scenario_loader.h>

#include <coffee/core/CDebug>
#include <coffee/blam/volta/cblam_map.h>
#include <coffee/blam/volta/blam_stl.h>

namespace Coffee{
namespace Blam{

const scenario *scn_get(tag_index_view& tags)
{
    static const auto pred = [](index_item_t const* v)
    {
        return cmp_tag_class(v->tagclass_e[0], tag_class_t::scnr);
    };

    auto base_it = std::find_if(tags.begin(), tags.end(),
                                pred);

    if(base_it == tags.end())
        return nullptr;

    auto base = *base_it;

    cDebug("Scenario name: {0},offset={1}",
           index_item_get_string(base,tags.file(),tags.tags()),
           base->offset-tags.tags()->index_magic);

    return C_CAST<const scenario*>(
                blam_mptr(
                    tags.file(),
                    tags.tags()->index_magic,
                    base->offset)
                );
}

}
}
