#include <cpcl.h>

//TODO: Create PCL binding

namespace CoffeeExt{
namespace CPCL{

//pcl::PointCloud<pcl::PointXYZRGB> *CPCLImplementation::gen_pointcloud(
//        const CVec3 *points, const CVec3 *cPoints, const szptr &numPoints, const CSize &cloudSize)
//{
//    pcl::PointCloud<pcl::PointXYZRGB>* pcloud = new pcl::PointCloud<pcl::PointXYZRGB>;

//    pcloud->width = cloudSize.w;
//    pcloud->height = cloudSize.h;

//    pcloud->points.resize(numPoints);

//    for(szptr i=0;i<numPoints;i++)
//    {
//        pcloud->points[i].x = points[i].x();
//        pcloud->points[i].y = points[i].y();
//        pcloud->points[i].z = points[i].z();

//        pcloud->points[i].r = cPoints[i].x();
//        pcloud->points[i].g = cPoints[i].y();
//        pcloud->points[i].b = cPoints[i].z();
//    }

//    return pcloud;
//}

//pcl::PointCloud<pcl::PointXYZRGB> *CPCLImplementation::merge_clouds(
//        const pcl::PointCloud<pcl::PointXYZRGB> *c1, const pcl::PointCloud<pcl::PointXYZRGB> *c2)
//{

//}

//void CPCLImplementation::io_save_pcdfile(const pcl::PointCloud<pcl::PointXYZRGB>& cloud,
//                                         cstring fname)
//{
//    pcl::io::savePCDFileASCII(fname,cloud);
//}

//pcl::PointCloud<pcl::PointXYZRGB>* CPCLImplementation::io_load_pcdfile(cstring fname)
//{
//    pcl::PointCloud<pcl::PointXYZRGB>* cloud = new pcl::PointCloud<pcl::PointXYZRGB>;
//    pcl::io::loadPCDFile(fname,*cloud);

//    return cloud;
//}

}
}
