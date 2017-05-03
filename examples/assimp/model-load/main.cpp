#include <coffee/core/CApplication>
#include <coffee/core/CDebug>

#include <coffee/assimp/cassimpimporters.h>
#include <coffee/assimp/assimp_data.h>
#include <coffee/graphics/common/SMesh>

using namespace Coffee;

i32 coffee_main(i32, cstring_w*)
{
    CResources::FileResourcePrefix("sample_data/metro-demo/");

    auto sceneFile = CResources::Resource("metro.fbx", ResourceAccess::None);
    if(CResources::FileMap(sceneFile, ResourceAccess::ReadOnly))
    {
        using Node = ASSIMP::Node;

        auto scene = ASSIMP::LoadScene(&sceneFile, "FBX");

        Vector<ASSIMP::ObjectDesc> objects;
        ASSIMP::GetSceneObjects(scene, objects);

        for(ASSIMP::ObjectDesc const& obj : objects)
            cDebug("Object: {0}.{1}", C_CAST<u32>(obj.type), obj.objectName);

        Node* root_node;
        ASSIMP::NodeList nodes;
        ASSIMP::GetSceneRoot(scene, &root_node, nodes);

        Queue<Node*> m_node_hierarchy;
        m_node_hierarchy.push(root_node);
        while(m_node_hierarchy.size() > 0)
        {
            auto& node = *m_node_hierarchy.front();
            cDebug("Node: {3} <- {0}:{1},{2}", node.objectName(), node.transform, node.mesh,
                   (node.parent()) ? node.parent()->objectName() : nullptr);
            Mesh output_mesh(true);
            ASSIMP::GetMeshData(scene, node, output_mesh);
            cDebug("Mesh: {0}, {1}, {2}",
                   output_mesh.numAttributes(),
                   output_mesh.indices(sizeof(u32)),
                   output_mesh.vertices(Mesh::Position, sizeof(Vecf3)));
            cDebug("Children: {0}", node.children().size());
            for(CObject* child : node.children())
                m_node_hierarchy.push(C_CAST<Node*>(child));
            m_node_hierarchy.pop();
        }
    }

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)