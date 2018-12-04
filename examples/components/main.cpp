#include <coffee/components/components.h>
#include <coffee/core/CApplication>
#include <coffee/core/CProfiling>
#include <coffee/core/types/vector_types.h>
#include <peripherals/typing/vectors/matrix_functions.h>

#include <coffee/core/CDebug>

using namespace Coffee;
using namespace Components;

namespace SceneGraph {

using namespace typing::vectors::scene;
}

enum ObjectFlags
{
    OF_Test_1 = 0x1,
    OF_Test_2 = 0x2,
};

using TagMatf4 = TagType<Matf4, i32>;

struct MatrixContainer : ComponentContainer<TagMatf4>
{
    Map<u64, szptr> mapping;
    Vector<Matf4>   matrices;

    virtual ~MatrixContainer() override
    {
    }

    virtual void register_entity(u64 id) override
    {
        mapping[id] = matrices.size();
        matrices.resize(matrices.size() + 1);
    }
    virtual void unregister_entity(u64) override
    {
    }
    virtual Matf4* get(u64 id) override
    {
        return &matrices.at(mapping[id]);
    }

    virtual void prealloc(szptr count) override
    {
        matrices.reserve(matrices.size() + count);
    }

    virtual bool contains_entity(u64 id) const override
    {
        return mapping.find(id) != mapping.end();
    }
};

void entity_process(ContainerProxy& c)
{
    ProfContext _("Component");
    auto&       m = c.get<TagMatf4>();

    m = SceneGraph::GenPerspective(90.f, 1.f, {0.1f, 100.f});
    m = m * (Matf4() * 1.f);
}

i32 coffee_main(i32, cstring_w*)
{
    EntityContainer entities;

    EntityRecipe rec1 = {{typeid(TagMatf4).hash_code()},
                         entity_process,
                         Chrono::milliseconds(10),
                         OF_Test_1};

    EntityRecipe rec2 = {{typeid(TagMatf4).hash_code()},
                         entity_process,
                         Chrono::milliseconds(10),
                         OF_Test_2};

    Profiler::PushContext("Register component");
    MatrixContainer matrix_store;

    entities.register_component(matrix_store);
    Profiler::PopContext();

    Profiler::PushContext("Create 200 entities");

    entities.prealloc(rec1, 100);
    for(C_UNUSED(auto i) : Range<>(100))
        entities.create_entity(rec1);

    entities.prealloc(rec2, 500000);
    for(C_UNUSED(auto i) : Range<>(500000))
        entities.create_entity(rec2);
    Profiler::PopContext();

    Profiler::PushContext("Updating entities");
    for(auto& o : entities.select(OF_Test_1))
        o.interval = Chrono::milliseconds(20);

    //    for(auto& o : entities.select(0))
    //        cBasicPrint("Object: {0} {1}", o.id, o.interval.count());

    for(C_UNUSED(auto i) : Range<>(10))
        entities.exec();

    Profiler::PopContext();

    //    Profiler::PushContext("Control");
    //    CurrentThread::sleep_for(Chrono::milliseconds(1));
    //    Profiler::PopContext();

    C_UNUSED(ComponentContainerBase & t1) = matrix_store;

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
