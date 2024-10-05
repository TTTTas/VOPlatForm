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

    // ͼƬ���ļ�·��
    static Mat img1, img2, img3;  // ���ں�������3������������ͼƬ�ļ�������img������Ҫ����������
    static string filename1, filename2, filename3;
    static string inputfolder, outputfolder, resultpath;

    // ���λ��������
    static vector<KeyPoint> keypoints1, keypoints2, keypoints3;  // �ؼ���
    static Mat descriptors1, descriptors2, descriptors3;  // ������

    // ������ȡ����ƥ����
    static Ptr<FeatureDetector> detector;  
    static Ptr<DescriptorMatcher> matcher;

    // ����
    static Mat K;  // �����������
    static Point2d principal_point; // �������
    static double focal_length;  // �������

    static Mat R, t;  // �ⲿ����
    static Mat E, F;  // ���ʾ���ͻ�������

    // �̹߳�����
    static EpipolarGeometryWorker* worker;
    // ��ʼ��ͼ��
    static void Init(string path1, string path2, string path3);

    static int initialize();

    static Mat readNextImg(const std::string& folderPath);

    // ���ͷֱ��ʵ� 1080P
    static cv::Mat resizeImage(const cv::Mat& image);

    // ���ͼ�������
    static void detectAndComputeFeatures(Mat img1, Mat img2, vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2, Mat& descriptors1, Mat& descriptors2);

    // ƥ������
    static vector<DMatch> matchFeatures();

    // ɸѡ�õ�ƥ���
    static vector<DMatch> filterGoodMatches(const vector<DMatch>& matches);

    // ���Ʋ���ʾƥ���
    static void drawAndShowMatches(Mat img1, Mat img2, vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2,
        const vector<DMatch>& matches, const vector<DMatch>& good_matches);

    // 2D-2D�������λ�˱仯
    static void pose_estimation_2d2d(vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2, std::vector<DMatch> matches);

    // ����E=Rt��Լ�Լ����֤
    static void verify(vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2, std::vector<DMatch> matches);

    // ���ǲ���
    static void triangulation(std::vector<DMatch> matches,std::vector<Point3d>& points);

    // ��֤���ǻ��������������ͶӰ��ϵ
    static void verifyReprojection(std::vector<DMatch> matches, std::vector<Point3d>& points);

    // 2D-3D
    static void pose_estimation_2d3d(const vector<Point2d>& imagePoints, const vector<Point3d>& objectPoints);

    //static std::vector<cv::Point2f> extractCommonPoints(const std::vector<cv::DMatch>& match1,        const std::vector<cv::DMatch>& match2,
    //    const std::vector<cv::KeyPoint>& keypoints2, const std::vector<cv::KeyPoint>& keypoints3);

    // ������
    static void Run(EpipolarGeometryWorker* w);

};

#endif 
