#include <coffee/CCore>
#include <coffee/CGraphics>
#include <coffee/CImage>

#include <coffee/graphics_apis/opengl/include/glfunctions.h>

#include <ext/pcl-shim/include/cpcl.h>
#include <ext/kinect-shim/include/cnect.h>

using namespace Coffee;
using namespace CoffeeExt;

using CNect = Freenect::FreenectImplementation;
using CPCLI = CPCL::CPCLImplementation;

int32 coffee_main(int32, cstring_w*)
{
    CNect::FreenectContext* c = nullptr;
    try{
        c = CNect::Alloc();
    }catch(std::runtime_error ex){
        cDebug("Failed to initialize Freenect: {0}",ex.what());
    }
    if(c)
        CNect::Free(c);

    CResources::CResource depth("dframe.raw");
    CResources::FileMap(depth);

    CResources::CResource color("cframe.raw");
    CResources::FileMap(color);

    CSize depth_size(512,424);

    const scalar* depth_data = (const scalar*)depth.data;
    const CRGBA* color_data = (const CRGBA*)color.data;

    if(!depth_data || !color_data)
        return 1;

    {
        /* Just dump a texture */
        CStbImageLib::CStbImageConst texturestb;
        texturestb.bpp = 4;
        texturestb.data = (const ubyte_t*)color.data;
        texturestb.size = CSize(1920,1080);
        CResources::CResource texturedump("image.png");
        CStbImageLib::SavePNG(&texturedump,&texturestb);
        CResources::FileCommit(texturedump);
        CResources::FileFree(texturedump);
    }

    std::vector<CVec3> posdata;

    {
        CGraphicsData::CGCamera camera;
        camera.fieldOfView = 90.f;
        camera.aspect = (scalar)depth_size.w/(scalar)depth_size.h;
        camera.zVals.near = 0.f;
        camera.zVals.far = 1.f;

        CMat4 project = CGraphicsData::GenPerspective(camera);
        CMat4 inverse_project = Coffee::inverse(project);

        CVec3 pos_mul(-camera.aspect,
                      1.f,
                      5.f);

        for(int32 y=0;y<depth_size.h;y++)
            for(int32 x=0;x<depth_size.w;x++)
            {
                scalar depth_val = depth_data[y*depth_size.w+x];
                if(depth_val <= 0.0)
                    continue;

                CVec3 vec;
                vec.x() = (scalar)x/(scalar)depth_size.w;
                vec.y() = (scalar)(depth_size.h/2-y)/(scalar)depth_size.h;
                vec.z() = depth_val;

                vec = vec*pos_mul;

                CVec4 tmp;
                tmp.x() = vec.x();
                tmp.y() = vec.y();
                tmp.z() = vec.z();
                tmp.w() = 1.0;

                tmp = project*tmp;
                vec.x() = tmp.x();
                vec.y() = tmp.y();
                vec.z() = tmp.z();

                //Not optimal, but avoids adding empty points
                posdata.push_back(vec);
            }
    }

    CPCL::PointCloud<CPCL::PointXYZRGB>* pcl =
            CPCLI::GenPointCloud(posdata.data(),
                                 color_data,
                                 posdata.size());

    CElapsedTimerD timer;
    timer.start();

    CPCL::PointCloud<CPCL::PointXYZ>::Ptr pcl_xyz = CPCLI::ExtractXYZCloud(pcl);
    CPCLI::DenoiseCloud(pcl_xyz);
    CPCL::PolygonMesh* mesh = CPCLI::CreatePolygonMesh(pcl_xyz);
    cDebug("Mesh processing time: {0}",timer.elapsed());

    CPCL::io::saveVTKFile("mesh.vtk",*mesh);

    CPCL::PointCloud<CPCL::PointXYZRGB>::Ptr pcl_ptr(pcl);

    CPCL::visualization::CloudViewer viewer("CloudView");
    viewer.showCloud(pcl_ptr);
    while(!viewer.wasStopped())
        continue;

    CPCL::CPCLImplementation::SavePCDFile(*pcl,"depthdata.pcd");

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
