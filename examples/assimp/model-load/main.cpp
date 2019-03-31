#include <coffee/core/CApplication>

#include <coffee/assimp/assimp_data.h>
#include <coffee/assimp/assimp_deserializer.h>
#include <coffee/assimp/assimp_iterators.h>
#include <coffee/assimp/assimp_material_iterators.h>
#include <coffee/assimp/cassimpimporters.h>
#include <coffee/graphics/common/SMesh>
#include <coffee/strings/libc_types.h>
#include <coffee/strings/vector_types.h>

#include <coffee/core/CDebug>

using namespace Coffee;

i32 coffee_main(i32, cstring_w*)
{
    auto sceneFile = Resource("metro.fbx", RSCA::None);
    if(FileMap(sceneFile, RSCA::ReadOnly))
    {
        using Node = ASSIMP::Node;

        ASSIMP::AssimpPtr scene;
        if(!ASSIMP::LoadScene(scene, C_OCAST<Bytes>(sceneFile), "FBX"))
            return -1;

        Vector<ASSIMP::ObjectDesc> objects;
        ASSIMP::GetSceneObjects(scene, objects);

        for(ASSIMP::ObjectDesc const& obj : objects)
            cDebug("Object: {0}.{1}", C_CAST<u32>(obj.type), obj.objectName);

        Node*            root_node;
        ASSIMP::NodeList nodes;
        ASSIMP::GetSceneRoot(scene, &root_node, nodes);

        Queue<Node*> m_node_hierarchy;
        m_node_hierarchy.push(root_node);
        while(m_node_hierarchy.size() > 0)
        {
            auto& node = *m_node_hierarchy.front();
            cDebug(
                "Node: {3} <- {0}:{1},{2}",
                node.objectName(),
                node.transform,
                node.mesh,
                nullptr);
            Mesh output_mesh(true);
            ASSIMP::GetMeshData(scene, node.mesh, output_mesh);
            cDebug(
                "Mesh: {0}, {1}, {2}",
                output_mesh.numAttributes(),
                output_mesh.indices(sizeof(u32)),
                output_mesh.vertices(Mesh::Position, sizeof(Vecf3)));
            cDebug("Children: {0}", node.children().size());
            for(auto child : node.children())
                m_node_hierarchy.push(C_CAST<Node*>(child));
            m_node_hierarchy.pop();
        }
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
