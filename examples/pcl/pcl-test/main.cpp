#include <coffee/CCore>
#include <coffee/CGraphics>
#include <coffee/CImage>

#include <coffee_ext/pcl_shim/cpcl.h>
//#include <coffee_ext/kinect_shim/cnect.h>

using namespace Coffee;
using namespace CoffeeExt;

//using CNect = Freenect::FreenectImplementation;
using CPCLI = CPCL::CPCLImplementation;

struct ColorVec3
{
    CVec3 p;
    CRGBA c;
};

int32 coffee_main(int32, cstring_w*)
{
    CResources::CResource depth("cloudframe.raw");
    CResources::FileMap(depth,ResourceAccess::ReadOnly|ResourceAccess::Streaming);

    CSize depth_size(512,424);
    CSize color_size(512,424);

    const ColorVec3* depth_comp_data = (ColorVec3*)depth.data;

    depth_comp_data += depth_size.area()*5;

    CVec3* depth_data = (CVec3*)Alloc(depth_size.area()*sizeof(CVec3));
    CRGBA* color_data = (CRGBA*)Alloc(color_size.area()*sizeof(CRGBA));

    if(!depth_comp_data)
        return 1;

    for(uint32 i=0;i<color_size.area();i++)
    {
        color_data[i] = depth_comp_data[i].c;
        depth_data[i] = depth_comp_data[i].p;
    }

    CPCL::PointCloud<CPCL::PointXYZRGB>::Ptr pcl =
            CPCLI::GenPointCloud(depth_data,
                                 color_data,
                                 depth_size.area());

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

    while(!viewer.wasStopped());

    CPCL::CPCLImplementation::SavePCDFile(*pcl,"depthdata.pcd");

    return 0;
}

COFFEE_APPLICATION_MAIN(coffee_main)
