#ifndef COFFEE_EXT_PCL_SHIM_PCL_H
#define COFFEE_EXT_PCL_SHIM_PCL_H

#include <coffee/CCore>

#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/point_representation.h>

#include <pcl/io/pcd_io.h>
#include <pcl/io/vtk_io.h>

#include <pcl/filters/statistical_outlier_removal.h>

#include <pcl/features/normal_3d.h>

#include <pcl/surface/gp3.h>

#include <pcl/visualization/cloud_viewer.h>

namespace CoffeeExt{
namespace CPCL{

using namespace pcl;
using namespace Coffee;

struct CPCLImplementation
{
    static PointCloud<PointXYZRGB>* GenPointCloud(const CVec3* points,
                                                  const CRGBA *cPoints,
                                                  const szptr& numPoints);

    static PointCloud<PointXYZ>::Ptr ExtractXYZCloud(const PointCloud<PointXYZRGB>* colorcloud);

    static void DenoiseCloud(PointCloud<PointXYZ>::Ptr cloud);

    static PointCloud<PointXYZRGB> *MergeClouds(
            const PointCloud<PointXYZRGB>* c1,
            const PointCloud<PointXYZRGB>* c2);

    static PolygonMesh* CreatePolygonMesh(const PointCloud<PointXYZ>::Ptr &cloud);

    static void SavePCDFile(const PointCloud<PointXYZRGB>& cloud,
                                cstring fname);
    static PointCloud<PointXYZRGB>* LoadPCDFile(cstring fname);
};

}
}

#endif
