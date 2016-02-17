#include <coffee_ext/pcl_shim/cpcl.h>

#include <pcl/filters/filter.h>

/* Noise removal */
#include <pcl/filters/statistical_outlier_removal.h>

/* Mesh creation */
#include <pcl/features/normal_3d.h>
#include <pcl/surface/gp3.h>

/* Registration of point clouds (merging) */
#include <pcl/registration/ndt.h>
#include <pcl/filters/approximate_voxel_grid.h>

namespace CoffeeExt{
namespace CPCL{

PointCloud<PointXYZRGB>::Ptr CPCLImplementation::GenPointCloud(
        const CVec3 *points, const CRGBA *cPoints,
        const szptr &numPoints)
{
    PointCloud<PointXYZRGB>::Ptr pcloud =
            PointCloud<PointXYZRGB>::Ptr(new PointCloud<PointXYZRGB>);

    pcloud->height = 1;

    pcloud->points.resize(numPoints);

    szptr pi = 0;

    for(szptr i=0;i<numPoints;i++)
    {
        const CVec3& v = points[i];
        if(CMath::isnan(v.x())||CMath::isnan(v.y())||CMath::isnan(v.z()))
            continue;

        pcloud->points[pi].x = points[i].x();
        pcloud->points[pi].y = points[i].y();
        pcloud->points[pi].z = points[i].z();

        if(cPoints)
        {
            pcloud->points[pi].r = cPoints[i].r;
            pcloud->points[pi].g = cPoints[i].g;
            pcloud->points[pi].b = cPoints[i].b;
            pcloud->points[pi].a = 1.0;
        }
        pi++;
    }

    pcloud->width = pi;

    pcloud->points.resize(pi);

    return pcloud;
}

PointCloud<PointXYZ>::Ptr CPCLImplementation::ExtractXYZCloud(
        PointCloud<PointXYZRGB>::Ptr colorcloud)
{
    PointCloud<PointXYZ>::Ptr out = PointCloud<PointXYZ>::Ptr(new PointCloud<PointXYZ>);

    out->points.resize(colorcloud->points.size());
    out->height = colorcloud->height;
    out->width = colorcloud->width;

    for(size_t i=0;i<colorcloud->size();i++)
    {
        out->points[i].x = colorcloud->points[i].x;
        out->points[i].y = colorcloud->points[i].y;
        out->points[i].z = colorcloud->points[i].z;
    }

    return out;
}

void CPCLImplementation::DenoiseCloud(PointCloud<PointXYZ>::Ptr cloud)
{
    PointCloud<PointXYZ> cloud_filter_tmp;

    PointCloud<PointXYZ>::Ptr cloud_ptr(cloud);
    StatisticalOutlierRemoval<PointXYZ> filter;

    filter.setInputCloud(cloud_ptr);
    filter.setMeanK(50);
    filter.setStddevMulThresh(1.0);
    filter.filter(cloud_filter_tmp);

    cloud->points.clear();
    cloud->points.insert(cloud->points.begin(),
                         cloud_filter_tmp.points.begin(),
                         cloud_filter_tmp.points.end());
    cloud->width = cloud->points.size();
}

PointCloud<PointXYZRGB>::Ptr CPCLImplementation::MergeCloudsICP(
        PointCloud<PointXYZRGB>::Ptr c1,
        PointCloud<PointXYZRGB>::Ptr c2)
{

}

PointCloud<PointXYZRGB>::Ptr CPCLImplementation::MergeCloudsNormals(
        PointCloud<PointXYZRGB>::Ptr c1,
        PointCloud<PointXYZRGB>::Ptr c2,
        CGraphicsData::CTransform const& transform)
{
    NormalDistributionsTransform<PointXYZRGB,PointXYZRGB> ndt;

    ndt.setTransformationEpsilon(0.01);
    ndt.setStepSize(0.1);
    ndt.setResolution(1.0);

    ndt.setMaximumIterations(35);
    ndt.setInputSource(c1);
    ndt.setInputTarget(c2);

    Eigen::AngleAxisf rot(transform.rotation.w(),Eigen::Vector3f::UnitZ());
    Eigen::Translation3f transl(transform.position.x(),
                                transform.position.y(),
                                transform.position.z());
    Eigen::Matrix4f guess = (transl * rot).matrix();

    PointCloud<PointXYZRGB>::Ptr out = PointCloud<PointXYZRGB>::Ptr(new PointCloud<PointXYZRGB>);

    ndt.align(*out,guess);

    cDebug("Has converged: {0}",ndt.hasConverged());
    cDebug("Fitness: {0}",ndt.getFitnessScore());

    transformPointCloud(*c2,*out,guess);

    return out;
}

PolygonMesh *CPCLImplementation::CreatePolygonMesh(const PointCloud<PointXYZ>::Ptr &cloud)
{
    NormalEstimation<PointXYZ,Normal> n;
    PointCloud<Normal> normals;
    PointCloud<PointXYZ>::ConstPtr cloud_ptr(cloud);

    search::KdTree<PointXYZ>::Ptr tree_ptr(new search::KdTree<PointXYZ>);
    tree_ptr->setInputCloud(cloud_ptr);
    n.setInputCloud(cloud_ptr);
    n.setSearchMethod(tree_ptr);
    n.setKSearch(20);
    n.compute(normals);

    PointCloud<PointNormal>::Ptr cloud_normals_ptr(new PointCloud<PointNormal>);
    concatenateFields(*cloud_ptr,normals,*cloud_normals_ptr);

    search::KdTree<PointNormal>::Ptr tree2_ptr(new search::KdTree<PointNormal>);
    tree2_ptr->setInputCloud(cloud_normals_ptr);

    GreedyProjectionTriangulation<PointNormal> gp3;
    PolygonMesh *triangles = new PolygonMesh;

    gp3.setSearchRadius(0.025);

    gp3.setMu(2.5);
    gp3.setMaximumNearestNeighbors(100);
    gp3.setMaximumSurfaceAngle(CMath::pi/4);
    gp3.setMinimumAngle(CMath::pi/18);
    gp3.setMaximumAngle(2*CMath::pi/3);
    gp3.setNormalConsistency(false);

    gp3.setInputCloud(cloud_normals_ptr);
    gp3.setSearchMethod(tree2_ptr);
    gp3.reconstruct(*triangles);

    return triangles;
}

void CPCLImplementation::SavePCDFile(
        const PointCloud<PointXYZRGB>& cloud,
        cstring fname)
{
    io::savePCDFileASCII(fname,cloud);
}

PointCloud<PointXYZRGB>::Ptr CPCLImplementation::LoadPCDFile(cstring fname)
{
    PointCloud<PointXYZRGB>::Ptr cloud = PointCloud<PointXYZRGB>::Ptr(new PointCloud<PointXYZRGB>);
    io::loadPCDFile(fname,*cloud);
    return cloud;
}

}
}
