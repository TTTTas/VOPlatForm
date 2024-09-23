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

using namespace std;
using namespace cv;

class EpipolarGeometry {

public:

    // 图片和文件路径
    static Mat img1, img2;  // 对于后续任务3，不再是两张图片文件，而是img，故需要这两个变量
    const static string filename1, filename2;

    // 点的位置与属性
    static vector<KeyPoint> keypoints1, keypoints2;  // 关键点
    static Mat descriptors1, descriptors2;  // 描述子

    // 特征提取器和匹配器
    static Ptr<FeatureDetector> detector;
    static Ptr<DescriptorMatcher> matcher;



    // 初始化图像
    static void initialize();

    // 降低分辨率到 1080P
    static cv::Mat resizeImage(const cv::Mat& image);

    // 检测和计算特征
    static void detectAndComputeFeatures();

    // 匹配特征
    static vector<DMatch> matchFeatures();

    // 筛选好的匹配点
    static vector<DMatch> filterGoodMatches(const vector<DMatch>& matches);

    // 绘制并显示匹配点
    static void drawAndShowMatches(const vector<DMatch>& matches, const vector<DMatch>& good_matches);

    // 主函数
    static void Run();

};

#endif 

