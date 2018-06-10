#include <coffee/core/CApplication>
#include <coffee/core/types/vector_types.h>
#include <coffee/core/types/cmatrix_functions.h>
#include <coffee/core/CDebug>
#include <coffee/components/components.h>

using namespace Coffee;
using namespace Components;

enum ObjectFlags
{
    OF_Test_1 = 0x1,
    OF_Test_2 = 0x2,
};

struct MatrixContainer : ComponentContainer<Matf4>
{
    Map<u64, szptr> mapping;
    Vector<Matf4> matrices;

    virtual ~MatrixContainer() override
    {
    }

    virtual void register_entity(u64 id) override
    {
        mapping[id] = matrices.size();
        matrices.resize(matrices.size() + 1);
    }
    virtual void unregister_entity(u64 id) override
    {
    }
    virtual Matf4 *get(u64 id) override
    {
        return &matrices.at(mapping[id]);
    }
};

void entity_process(EntityContainer& c)
{
    ProfContext _("Component");
    auto& m = c.get<Matf4>();

    m = CGraphicsData::GenPerspective(90.f, 1.f, {0.1f, 100.f});
    m = m * (Matf4() * 1.f);

    //cDebug("{0}", m * Vecf4(0.f, 0, 0.f, 1.f));
}

i32 coffee_main(i32, cstring_w*)
{
    EntityContainer entities;

    EntityRecipe rec1 = {
        {
            typeid (Matf4).hash_code()
        },
        entity_process,
        Chrono::milliseconds(10),
        OF_Test_1
    };

    EntityRecipe rec2 = {
        {
            typeid (Matf4).hash_code()
        },
        entity_process,
        Chrono::milliseconds(10),
        OF_Test_2
    };

    Profiler::PushContext("Register component");
    MatrixContainer matrix_store;

    entities.register_component<Matf4>(matrix_store);
    Profiler::PopContext();

    Profiler::PushContext("Create 200 entities");
    for(auto i : Range<>(100))
        entities.create_entity(rec1);

    for(auto i : Range<>(100))
        entities.create_entity(rec2);
    Profiler::PopContext();

    Profiler::PushContext("Updating entities");
    for(auto& o : entities.select(OF_Test_1))
        o.interval = Chrono::milliseconds(20);

    //for(auto& o : entities.select(0))
    //    cDebug("Object: {0} {1}", o.id, o.interval.count());

    entities.exec();
    Profiler::PopContext();

    Profiler::PushContext("Control");
    CurrentThread::sleep_for(Chrono::milliseconds(1));
    Profiler::PopContext();

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
