#include "EpipolarGeometry.h"
#include <iostream>

using namespace std;
using namespace cv;

// 初始化静态常量
const double ImageProcessing::scale_factor = 0.25f;
const string ImageProcessing::filename001 = "./Input/tea 1.JPG";
const string ImageProcessing::filename002 = "./Input/tea 2.JPG";
const Mat ImageProcessing::K = (Mat_<double>(3, 3) << 1354.222062716567, 0, 204.9442952614041,
    0, 1305.36634809663, 89.65190111179771,
    0, 0, 1);

// 降低图像画质到1080P
Mat ImageProcessing::resizeImage(const Mat& image) {
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

// 图像预处理函数 我认为需要调整
Mat ImageProcessing::preprocessImage(const Mat& image) {
    Mat gray, blurred, equalized, sharpened;

    // 1. 转换为灰度图像
    cvtColor(image, gray, COLOR_BGR2GRAY);

    // 2. 使用双边滤波
    bilateralFilter(gray, blurred, 9, 75, 75);

    // 3. 自适应直方图均衡化
    Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8, 8));
    clahe->apply(blurred, equalized);

    // 4. 图像锐化
    Mat laplacian;
    Laplacian(equalized, laplacian, CV_16S, 3);
    convertScaleAbs(laplacian, laplacian);
    sharpened = equalized - 0.3 * laplacian;

    return sharpened;
}

// 使用ORB或SIFT提取特征点
void ImageProcessing::detectAndMatchFeaturesBF(const Mat& img1, const Mat& img2,
    vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2,
    vector<DMatch>& matches, Mat& descriptors1, Mat& descriptors2) {

    // 创建ORB或SIFT特征提取器
    Ptr<Feature2D> detector = ORB::create();  // 也可以使用 SIFT::create()

    // 提取特征点和描述符
    detector->detectAndCompute(img1, noArray(), keypoints1, descriptors1);
    detector->detectAndCompute(img2, noArray(), keypoints2, descriptors2);

    // 使用BFMatcher进行匹配（暴力匹配，似乎效果一般）
    BFMatcher matcher(NORM_HAMMING, true);  // ORB使用NORM_HAMMING，SIFT使用NORM_L2
    matcher.match(descriptors1, descriptors2, matches);
}

// 14讲 ： 特征点较多的情况使用FB匹配方法更为快速
// 使用Flann-based matcher进行匹配，并应用Ratio Test
void ImageProcessing::detectAndMatchFeaturesFB(const Mat& img1, const Mat& img2,
    vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2,
    vector<DMatch>& matches, Mat& descriptors1, Mat& descriptors2) {

    // 创建ORB或SIFT特征提取器
    Ptr<Feature2D> detector = ORB::create();  // 可以切换为 SIFT::create() 或者 SURF::create()

    // 提取特征点和描述符
    detector->detectAndCompute(img1, noArray(), keypoints1, descriptors1);
    detector->detectAndCompute(img2, noArray(), keypoints2, descriptors2);

    // 如果使用的是ORB（产生二进制描述符），需要将其转换为浮点型以用于FLANN
    if (descriptors1.type() != CV_32F) {
        descriptors1.convertTo(descriptors1, CV_32F);
        descriptors2.convertTo(descriptors2, CV_32F);
    }

    // 创建 Flann-based Matcher
    FlannBasedMatcher fbmatcher;

    // KNN匹配：对于每个描述符，找到2个最邻近的匹配
    vector<vector<DMatch>> knn_matches;
    fbmatcher.knnMatch(descriptors1, descriptors2, knn_matches, 3);  // K = 2

    // 应用 Ratio Test 过滤匹配点
    const float ratio_thresh = 0.90f;  // 比率阈值
    for (size_t i = 0; i < knn_matches.size(); i++) {
        if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance) {
            matches.push_back(knn_matches[i][0]);  // 通过比率测试的匹配点
        }
    }

    // 输出匹配结果
    cout << "Number of good matches after Ratio Test: " << matches.size() << endl;
}

// 剔除粗差匹配
vector<DMatch> ImageProcessing::filterMatchesWithFundamentalMat(const vector<KeyPoint>& keypoints1,
    const vector<KeyPoint>& keypoints2, const vector<DMatch>& matches, Mat& F) {

    // 提取匹配点对
    vector<Point2f> points1, points2;
    for (const DMatch& match : matches) {
        points1.push_back(keypoints1[match.queryIdx].pt);
        points2.push_back(keypoints2[match.trainIdx].pt);
    }

    // 计算基础矩阵并剔除外点
    vector<uchar> inliersMask(points1.size());
    F = findFundamentalMat(points1, points2, inliersMask, FM_RANSAC);

    // 仅保留内点匹配
    vector<DMatch> inliers;
    for (size_t i = 0; i < inliersMask.size(); ++i) {
        if (inliersMask[i]) {
            inliers.push_back(matches[i]);
        }
    }

    return inliers;
}


void ImageProcessing::drawMatchesWithLines(const Mat& img1, const Mat& img2,
    const vector<Point2f>& points1, const vector<Point2f>& points2, const vector<DMatch>& matches) {

    // 创建一个拼接的图像
    Mat imgMatches;
    hconcat(img1, img2, imgMatches);  // 水平拼接两幅图像

    // 绘制匹配点和连线
    for (const auto& match : matches) {
        // 确保索引在范围内
        if (match.queryIdx >= points1.size() || match.trainIdx >= points2.size()) {
            cerr << "Index out of range: queryIdx or trainIdx exceeds points size" << endl;
            continue; // 跳过无效的匹配
        }

        // 图像1中的点
        Point2f pt1 = points1[match.queryIdx];
        // 图像2中的点（需要将x坐标偏移img1的宽度）
        Point2f pt2 = points2[match.trainIdx] + Point2f((float)img1.cols, 0);

        // 在拼接的图像上绘制点和连线
        circle(imgMatches, pt1, 5, Scalar(0, 255, 0), -1);  // 绿色点
        circle(imgMatches, pt2, 5, Scalar(0, 255, 0), -1);  // 绿色点
        line(imgMatches, pt1, pt2, Scalar(255, 0, 0), 1);   // 蓝色连线
    }

    // 显示结果
    // imshow("Matches with Lines", imgMatches);
    imwrite("特征点匹配.jpg", imgMatches);
    waitKey(0);
}

// 绘制极线
void ImageProcessing::drawEpilines(const Mat& img1, const Mat& img2,
    const vector<Point2f>& points1, const vector<Point2f>& points2, const Mat& F) {

    // 计算极线
    vector<Vec3f> lines1, lines2;
    computeCorrespondEpilines(points1, 1, F, lines1); // 计算图1中点对应的图2中的极线
    computeCorrespondEpilines(points2, 2, F, lines2); // 计算图2中点对应的图1中的极线

    Mat img1_copy = img1.clone();
    Mat img2_copy = img2.clone();

    // 绘制极线和对应点
    for (size_t i = 0; i < points1.size(); i++) {
        // 绘制图1中的极线
        Vec3f l = lines2[i];
        line(img1_copy, Point(0, -l[2] / l[1]), Point(img1.cols, -(l[2] + l[0] * img1.cols) / l[1]), Scalar(255, 0, 0)); // 蓝色线条
        circle(img1_copy, points1[i], 5, Scalar(0, 255, 0), -1);  // 绿色点

        // 绘制图2中的极线
        l = lines1[i];
        line(img2_copy, Point(0, -l[2] / l[1]), Point(img2.cols, -(l[2] + l[0] * img2.cols) / l[1]), Scalar(255, 0, 0)); // 蓝色线条
        circle(img2_copy, points2[i], 5, Scalar(0, 255, 0), -1);  // 绿色点
    }

    // 显示结果
    imshow("Image 1 with Epilines", img1_copy);
    imshow("Image 2 with Epilines", img2_copy);
    waitKey(0);

}

// 计算本质矩阵并进行三角化
void ImageProcessing::triangulatePointsFromMatches(const Mat& K, const vector<Point2f>& points1,
    const vector<Point2f>& points2, const Mat& R, const Mat& t) {

    // 投影矩阵
    Mat P1 = K * Mat::eye(3, 4, CV_64F);  // 左相机矩阵 [I|0]
    Mat P2 = K * (Mat_<double>(3, 4) << R.at<double>(0, 0), R.at<double>(0, 1), R.at<double>(0, 2), t.at<double>(0),
        R.at<double>(1, 0), R.at<double>(1, 1), R.at<double>(1, 2), t.at<double>(1),
        R.at<double>(2, 0), R.at<double>(2, 1), R.at<double>(2, 2), t.at<double>(2));

    // 三角化
    Mat points4D;
    triangulatePoints(P1, P2, points1, points2, points4D);

    // 转换为齐次坐标
    for (int i = 0; i < points4D.cols; i++) {
        Mat col = points4D.col(i);
        col /= col.at<float>(3);  // 归一化
        cout << "3D Point: [" << col.at<float>(0) << ", " << col.at<float>(1) << ", " << col.at<float>(2) << "]\n";
    }

}

void ImageProcessing::Run() {
    // 读取影像
    Mat img1 = imread(filename001);
    Mat img2 = imread(filename002);

    if (img1.empty() || img2.empty()) {
        cout << "未找到影像!" << endl;
        return;
    }

    // 降低图像分辨率
    img1 = resizeImage(img1);
    img2 = resizeImage(img2);

    // 进行图像预处理，这个好像也没处理得太好
    img1 = preprocessImage(img1);
    img2 = preprocessImage(img2);

    // 相机内参矩阵 (假设已知)
    Mat K = (Mat_<double>(3, 3) << 1354.222062716567, 0, 204.9442952614041,
        0, 1305.36634809663, 89.65190111179771,
        0, 0, 1);

    // 特征点和匹配
    vector<KeyPoint> keypoints1, keypoints2;
    Mat descriptors1, descriptors2;
    vector<DMatch> matches;

    //detectAndMatchFeaturesFB(img1, img2, keypoints1, keypoints2, matches, descriptors1, descriptors2);

    detectAndMatchFeaturesBF(img1, img2, keypoints1, keypoints2, matches, descriptors1, descriptors2);
    // 剔除粗差并计算基础矩阵
    Mat F;
    vector<DMatch> inliers = filterMatchesWithFundamentalMat(keypoints1, keypoints2, matches, F);

    // 提取内点的坐标
    vector<Point2f> inlierPoints1, inlierPoints2;
    for (const DMatch& match : inliers) {
        inlierPoints1.push_back(keypoints1[match.queryIdx].pt);
        inlierPoints2.push_back(keypoints2[match.trainIdx].pt);
    }

    // 绘制特征点匹配的连线
    drawMatchesWithLines(img1, img2, inlierPoints1, inlierPoints2, inliers);

    // 绘制极线
    drawEpilines(img1, img2, inlierPoints1, inlierPoints2, F);



    // 本质矩阵估计
    Mat R, t;
    Mat E = findEssentialMat(inlierPoints1, inlierPoints2, K, RANSAC);
    recoverPose(E, inlierPoints1, inlierPoints2, K, R, t);

    // 三维点前方交会
    triangulatePointsFromMatches(K, inlierPoints1, inlierPoints2, R, t);


}
