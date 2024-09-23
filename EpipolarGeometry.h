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

    // ͼƬ���ļ�·��
    static Mat img1, img2;  // ���ں�������3������������ͼƬ�ļ�������img������Ҫ����������
    const static string filename1, filename2;

    // ���λ��������
    static vector<KeyPoint> keypoints1, keypoints2;  // �ؼ���
    static Mat descriptors1, descriptors2;  // ������

    // ������ȡ����ƥ����
    static Ptr<FeatureDetector> detector;
    static Ptr<DescriptorMatcher> matcher;



    // ��ʼ��ͼ��
    static void initialize();

    // ���ͷֱ��ʵ� 1080P
    static cv::Mat resizeImage(const cv::Mat& image);

    // ���ͼ�������
    static void detectAndComputeFeatures();

    // ƥ������
    static vector<DMatch> matchFeatures();

    // ɸѡ�õ�ƥ���
    static vector<DMatch> filterGoodMatches(const vector<DMatch>& matches);

    // ���Ʋ���ʾƥ���
    static void drawAndShowMatches(const vector<DMatch>& matches, const vector<DMatch>& good_matches);

    // ������
    static void Run();

};

#endif 

