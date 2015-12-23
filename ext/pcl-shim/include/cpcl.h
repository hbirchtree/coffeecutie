#ifndef COFFEE_EXT_PCL_SHIM_PCL_H
#define COFFEE_EXT_PCL_SHIM_PCL_H

#include <coffee/CCore>

/*! TODO: These ugly includes! */
//#include <pcl/point_types.h>
//#include <pcl/point_cloud.h>
//#include <pcl/point_representation.h>

//#include <pcl/io/pcd_io.h>

//#include <pcl/filters/voxel_grid.h>
//#include <pcl/filters/filter.h>

//#include <pcl/features/normal_3d.h>

//#include <pcl/registration/icp.h>
//#include <pcl/registration/icp_nl.h>
//#include <pcl/registration/transforms.h>

//#include <pcl/visualization/pcl_visualizer.h>

namespace CoffeeExt{
namespace CPCL{

//struct CPCLImplementation
//{
//    static pcl::PointCloud<pcl::PointXYZRGB>* gen_pointcloud(
//            const CVec3* points,
//            const CVec3* cPoints,
//            const szptr& numPoints,
//            const CSize& cloudSize);

//    static pcl::PointCloud<pcl::PointXYZRGB> *merge_clouds(
//            const pcl::PointCloud<pcl::PointXYZRGB>* c1,
//            const pcl::PointCloud<pcl::PointXYZRGB>* c2);

//    static void io_save_pcdfile(const pcl::PointCloud<pcl::PointXYZRGB>& cloud,
//                                cstring fname);
//    static pcl::PointCloud<pcl::PointXYZRGB>* io_load_pcdfile(cstring fname);
//};

}
}

#endif
