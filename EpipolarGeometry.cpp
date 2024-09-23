#include "EpipolarGeometry.h"
#include <iostream>

// 初始化路径
const string EpipolarGeometry::filename1 = "./Input/Image_5.JPG";
const string EpipolarGeometry::filename2 = "./Input/Image_6.JPG";
Mat EpipolarGeometry::img1, EpipolarGeometry::img2;

// 设置特征检测器为ORB，匹配方法为BF
Ptr<FeatureDetector> EpipolarGeometry::detector = ORB::create();
Ptr<DescriptorMatcher> EpipolarGeometry::matcher = DescriptorMatcher::create("BruteForce-Hamming");

// 初始化静态变量点集和描述子
vector<KeyPoint> EpipolarGeometry::keypoints1, EpipolarGeometry::keypoints2;
Mat EpipolarGeometry::descriptors1, EpipolarGeometry::descriptors2;

using namespace cv;
using namespace std;

void EpipolarGeometry::initialize()
{
    // 读取图像
    img1 = imread(filename1);
    img2 = imread(filename2);

    // 降低画质到1080P
    img1 = resizeImage(img1);
    img2 = resizeImage(img2);

    // 确保图像被正确读取
    if (img1.empty() || img2.empty()) {
        cout << "未找到影像!" << endl;
        return;
    }
}

// 降低图像画质到1080P
Mat EpipolarGeometry::resizeImage(const Mat& image)
{
    Mat resized;
    const int maxHeight = 1080;
    const int maxWidth = 1920;

    int originalHeight = image.rows;
    int originalWidth = image.cols;

    // 如果影像的分辨率大于1080P，则进行缩放
    if (originalHeight > maxHeight || originalWidth > maxWidth) {
        // 计算缩放比例，保持长宽比
        double scaleFactor = std::min((double)maxHeight / originalHeight, (double)maxWidth / originalWidth);
        resize(image, resized, Size(), scaleFactor, scaleFactor, INTER_LINEAR);
    }
    else {
        // 分辨率小于或等于1080P，不进行缩放，直接返回原图
        resized = image.clone();
    }

    return resized;
}

void EpipolarGeometry::detectAndComputeFeatures()
{
    // 检测特征点和描述符
    chrono::steady_clock::time_point t1 = chrono::steady_clock::now();

    detector->detect(img1, keypoints1);
    detector->detect(img2, keypoints2);

    detector->compute(img1, keypoints1, descriptors1);
    detector->compute(img2, keypoints2, descriptors2);

    chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
    chrono::duration<double> time_used = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
    cout << "Feature detection and computation took " << time_used.count() << " seconds." << endl;

    // 绘制特征点
    Mat outImg1;

    drawKeypoints(img1, keypoints1, outImg1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

    imshow("ORB features", outImg1);

    waitKey(0);
}

vector<DMatch> EpipolarGeometry::matchFeatures()
{
    // 匹配描述符
    vector<DMatch> matches;
    chrono::steady_clock::time_point t1 = chrono::steady_clock::now();

    matcher->match(descriptors1, descriptors2, matches);

    chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
    chrono::duration<double> time_used = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
    cout << "Feature matching took " << time_used.count() << " seconds." << endl;

    return matches;
}

vector<DMatch> EpipolarGeometry::filterGoodMatches(const vector<DMatch>& matches)
{
    // 筛选匹配点
    auto min_max = minmax_element(matches.begin(), matches.end(),
        [](const DMatch& m1, const DMatch& m2) { return m1.distance < m2.distance; });
    double min_dist = min_max.first->distance;
    double max_dist = min_max.second->distance;

    printf("-- Max dist : %f \n", max_dist);
    printf("-- Min dist : %f \n", min_dist);

    vector<DMatch> good_matches;
    // 筛选“好”的匹配点，距离小于2倍的最小距离或30的经验值
    for (const auto& match : matches) {
        if (match.distance <= max(2 * min_dist, 30.0)) {
            good_matches.push_back(match);
        }
    }

    return good_matches;
}

void EpipolarGeometry::drawAndShowMatches(const vector<DMatch>& matches, const vector<DMatch>& good_matches)
{
    // 绘制匹配结果
    Mat img_match, img_goodmatch;
    drawMatches(img1, keypoints1, img2, keypoints2, matches, img_match);
    drawMatches(img1, keypoints1, img2, keypoints2, good_matches, img_goodmatch);

    //imshow("All matches", img_match);
    imshow("Good matches", img_goodmatch);

    imwrite("Good matches.JPG", img_goodmatch);
    waitKey(0);
}

void EpipolarGeometry::Run()
{
    // 初始化类，传入图像路径
    initialize();

    // 1. 检测和计算特征点及描述符
    detectAndComputeFeatures();

    // 2. 匹配特征
    vector<DMatch> matches = matchFeatures();

    // 3. 筛选好的匹配点
    vector<DMatch> good_matches = filterGoodMatches(matches);

    // 4. 绘制并显示匹配结果
    drawAndShowMatches(matches, good_matches);

}