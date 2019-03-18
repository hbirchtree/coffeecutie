#include <coffee/components/components.h>

namespace Coffee {
namespace Components {
namespace detail {

static void assign_reachables(
    Vector<bool>& matrix,
    size_t        size,
    node_id       source_node,
    node_id       current_node)
{
    for(auto i : Range<>(size))
        if(matrix[size * current_node + i] && !matrix[size * source_node + i])
        {
            matrix[size * source_node + i] = true;
            assign_reachables(matrix, size, source_node, i);
        }
}

void expand_neighbor_matrix(
    Vector<bool>& matrix, size_t size, node_id current_node)
{
    for(auto i : Range<>(size))
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

} // namespace Components
} // namespace Coffee
