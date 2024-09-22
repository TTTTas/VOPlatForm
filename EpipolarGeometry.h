#pragma once
#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <opencv2/calib3d.hpp>
#include <string>

class ImageProcessing {
public:
    static void Run(); // 对极主程序

    // 参数和路径
    static const double scale_factor;  
    static const std::string filename001;
    static const std::string filename002;
    static const cv::Mat K;  // 相机内参矩阵

    // 图像处理与匹配相关的函数
    static cv::Mat resizeImage(const cv::Mat& image);
    static cv::Mat preprocessImage(const cv::Mat& image);

    static void detectAndMatchFeaturesBF(const cv::Mat& img1, const cv::Mat& img2,
        std::vector<cv::KeyPoint>& keypoints1, std::vector<cv::KeyPoint>& keypoints2,
        std::vector<cv::DMatch>& matches, cv::Mat& descriptors1, cv::Mat& descriptors2);

    static void detectAndMatchFeaturesFB(const cv::Mat& img1, const cv::Mat& img2,
        std::vector<cv::KeyPoint>& keypoints1, std::vector<cv::KeyPoint>& keypoints2,
        std::vector<cv::DMatch>& matches, cv::Mat& descriptors1, cv::Mat& descriptors2);

    static std::vector<cv::DMatch> filterMatchesWithFundamentalMat(const std::vector<cv::KeyPoint>& keypoints1,
        const std::vector<cv::KeyPoint>& keypoints2, const std::vector<cv::DMatch>& matches, cv::Mat& F);

    static void drawMatchesWithLines(const cv::Mat& img1, const cv::Mat& img2,
        const std::vector<cv::Point2f>& points1, const std::vector<cv::Point2f>& points2,
        const std::vector<cv::DMatch>& matches);

    static void drawEpilines(const cv::Mat& img1, const cv::Mat& img2,
        const std::vector<cv::Point2f>& points1, const std::vector<cv::Point2f>& points2, const cv::Mat& F);

    static void triangulatePointsFromMatches(const cv::Mat& K, const std::vector<cv::Point2f>& points1,
        const std::vector<cv::Point2f>& points2, const cv::Mat& R, const cv::Mat& t);
};

#endif // IMAGEPROCESSING_H
