#include <coffee/components/components.h>
#include <peripherals/stl/range.h>

namespace compo {
namespace detail {

static void assign_reachables(
    std::vector<bool>& matrix,
    size_t             size,
    node_id            source_node,
    node_id            current_node)
{
    for(auto i : stl_types::Range<>(size))
        if(matrix[size * current_node + i] && !matrix[size * source_node + i])
        {
            matrix[size * source_node + i] = true;
            assign_reachables(matrix, size, source_node, i);
        }
}

void expand_neighbor_matrix(
    std::vector<bool>& matrix, size_t size, node_id current_node)
{
    for(auto i : stl_types::Range<>(size))
        if(i != current_node && matrix[size * current_node + i])
            assign_reachables(matrix, size, current_node, i);
}

} // namespace detail

ComponentContainerBase::~ComponentContainerBase()
{
}

EntityVisitorBase::~EntityVisitorBase()
{
}

SubsystemBase::~SubsystemBase()
{
}

} // namespace compo
