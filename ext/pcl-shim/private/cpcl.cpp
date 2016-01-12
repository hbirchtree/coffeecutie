#include <cpcl.h>

//TODO: Create PCL binding

namespace CoffeeExt{
namespace CPCL{

pcl::PointCloud<pcl::PointXYZRGB> *CPCLImplementation::GenPointCloud(
        const CVec3 *points, const CRGBA *cPoints,
        const szptr &numPoints)
{
    pcl::PointCloud<pcl::PointXYZRGB>* pcloud = new pcl::PointCloud<pcl::PointXYZRGB>;

    pcloud->width = numPoints;
    pcloud->height = 1;

    pcloud->points.resize(numPoints);

    for(szptr i=0;i<numPoints;i++)
    {
        pcloud->points[i].x = points[i].x();
        pcloud->points[i].y = points[i].y();
        pcloud->points[i].z = points[i].z();

        if(cPoints)
        {
            pcloud->points[i].r = cPoints[i].r;
            pcloud->points[i].g = cPoints[i].g;
            pcloud->points[i].b = cPoints[i].b;
            pcloud->points[i].a = 1.0;
        }
    }

    return pcloud;
}

PointCloud<PointXYZ>::Ptr CPCLImplementation::ExtractXYZCloud(const PointCloud<PointXYZRGB> *colorcloud)
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

pcl::PointCloud<pcl::PointXYZRGB> *CPCLImplementation::MergeClouds(
        const pcl::PointCloud<pcl::PointXYZRGB> *c1, const pcl::PointCloud<pcl::PointXYZRGB> *c2)
{
    return nullptr;
}

PolygonMesh *CPCLImplementation::CreatePolygonMesh(const PointCloud<PointXYZ>::Ptr &cloud)
{

    NormalEstimation<PointXYZ,pcl::Normal> n;
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

void CPCLImplementation::SavePCDFile(const pcl::PointCloud<pcl::PointXYZRGB>& cloud,
                                         cstring fname)
{
    pcl::io::savePCDFileASCII(fname,cloud);
}

pcl::PointCloud<pcl::PointXYZRGB>* CPCLImplementation::LoadPCDFile(cstring fname)
{
    pcl::PointCloud<pcl::PointXYZRGB>* cloud = new pcl::PointCloud<pcl::PointXYZRGB>;
    pcl::io::loadPCDFile(fname,*cloud);

    return cloud;
}

}
}
