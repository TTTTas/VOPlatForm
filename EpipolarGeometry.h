#pragma once
#ifndef EPIPOLARGEOMETRY_H
#define EPIPOLARGEOMETRY_H

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <opencv2/calib3d.hpp>
#include <string>
#include "EpipolarGeometryWorker.h"

using namespace std;
using namespace cv;

class EpipolarGeometry {

public:

    // 图片和文件路径
    static Mat img1, img2, img3;  // 对于后续任务3，不再是两张图片文件，而是img，故需要这两个变量
    static string filename1, filename2, filename3;
    static string inputfolder, outputfolder, resultpath;

    // 点的位置与属性
    static vector<KeyPoint> keypoints1, keypoints2, keypoints3;  // 关键点
    static Mat descriptors1, descriptors2, descriptors3;  // 描述子

    // 特征提取器和匹配器
    static Ptr<FeatureDetector> detector;  
    static Ptr<DescriptorMatcher> matcher;

    // 参数
    static Mat K;  // 相机参数矩阵
    static Point2d principal_point; // 相机光心
    static double focal_length;  // 相机焦距

    static Mat R, t;  // 外部参数
    static Mat E, F;  // 本质矩阵和基础矩阵

    // 线程管理器
    static EpipolarGeometryWorker* worker;
    // 初始化图像
    static void Init(string path1, string path2, string path3);

    static int initialize();

    static Mat readNextImg(const std::string& folderPath);

    // 降低分辨率到 1080P
    static cv::Mat resizeImage(const cv::Mat& image);

    // 检测和计算特征
    static void detectAndComputeFeatures(Mat img1, Mat img2, vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2, Mat& descriptors1, Mat& descriptors2);

    // 匹配特征
    static vector<DMatch> matchFeatures();

    // 筛选好的匹配点
    static vector<DMatch> filterGoodMatches(const vector<DMatch>& matches);

    // 绘制并显示匹配点
    static void drawAndShowMatches(Mat img1, Mat img2, vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2,
        const vector<DMatch>& matches, const vector<DMatch>& good_matches);

    // 2D-2D估计相机位姿变化
    static void pose_estimation_2d2d(vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2, std::vector<DMatch> matches);

    // 进行E=Rt与对极约束验证
    static void verify(vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2, std::vector<DMatch> matches);

    // 三角测量
    static void triangulation(std::vector<DMatch> matches,std::vector<Point3d>& points);

    // 验证三角化点与特征点的重投影关系
    static void verifyReprojection(std::vector<DMatch> matches, std::vector<Point3d>& points);

    // 2D-3D
    static void pose_estimation_2d3d(const vector<Point2d>& imagePoints, const vector<Point3d>& objectPoints);

    //static std::vector<cv::Point2f> extractCommonPoints(const std::vector<cv::DMatch>& match1,        const std::vector<cv::DMatch>& match2,
    //    const std::vector<cv::KeyPoint>& keypoints2, const std::vector<cv::KeyPoint>& keypoints3);

    // 主函数
    static void Run(EpipolarGeometryWorker* w);

};

#endif 
