#ifndef COFFEE_EXT_PCL_SHIM_PCL_H
#define COFFEE_EXT_PCL_SHIM_PCL_H

#include <coffee/CCore>
#include <coffee/graphics/apis/cgraphicsdata.h>

/* Base headers */
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
#include <pcl/point_representation.h>

/* File export */
#include <pcl/io/pcd_io.h>
#include <pcl/io/vtk_io.h>

/* Just for testing */
#include <pcl/visualization/cloud_viewer.h>

namespace CoffeeExt{
namespace CPCL{

using namespace pcl;
using namespace Coffee;

struct CPCLImplementation
{

    static PointCloud<PointXYZRGB>::Ptr GenPointCloud(const CVec3* points,
                                                      const CRGBA *cPoints,
                                                      const szptr& numPoints);

    /*!
     * \brief Extract an XYZ cloud from an XYZRGB one, used for mesh approximation (incompatibility, really)
     * \param colorcloud
     * \return
     */
    static PointCloud<PointXYZ>::Ptr ExtractXYZCloud(PointCloud<PointXYZRGB>::Ptr colorcloud);

    /*!
     * \brief Remove statistical outliers from the cloud
     * \param cloud
     */
    static void DenoiseCloud(PointCloud<PointXYZ>::Ptr cloud);

    static PointCloud<PointXYZRGB>::Ptr MergeCloudsICP(
            PointCloud<PointXYZRGB>::Ptr c1,
            PointCloud<PointXYZRGB>::Ptr c2);

    /*!
     * \brief Merge two point clouds using normal distribution transform
     * \param c1 Combinable cloud, not modified
     * \param c2 Target cloud which is aligned with, accumulates
     * \return Pointer to c2 on success
     */
    static PointCloud<PointXYZRGB>::Ptr MergeCloudsNormals(
            PointCloud<PointXYZRGB>::Ptr c1,
            PointCloud<PointXYZRGB>::Ptr c2,
            CGraphicsData::CTransform const& transform);

    static PolygonMesh* CreatePolygonMesh(const PointCloud<PointXYZ>::Ptr &cloud);

    static void SavePCDFile(const PointCloud<PointXYZRGB>& cloud,
                                cstring fname);
    static PointCloud<PointXYZRGB>::Ptr LoadPCDFile(cstring fname);
};

}
}

#endif
