#include "EpipolarGeometry.h"
#include "comm_funcs.h"
#include "fstream"
#include <iostream>
#include <tuple>
#include <unordered_map>
#include <filesystem>
#include <iomanip>  // �������������ʽ
#include <sstream>  // ���ڸ�ʽ���ַ���
#include <vector>   // ���ڴ洢�ļ���
#include <fstream>
#include "qfileinfo.h"

namespace fs = std::filesystem;

fstream f;

// ��ʼ��·��
string EpipolarGeometry::filename1 = "";
string EpipolarGeometry::filename2 = "";
string EpipolarGeometry::filename3 = "";
string EpipolarGeometry::inputfolder = "";
string EpipolarGeometry::outputfolder = "";
string EpipolarGeometry::resultpath = "";
EpipolarGeometryWorker* EpipolarGeometry::worker = nullptr;

//const string EpipolarGeometry::filename4 = "./data/0000000002.png";
Mat EpipolarGeometry::img1, EpipolarGeometry::img2, EpipolarGeometry::img3;

// �������������ΪORB��ƥ�䷽��ΪBF
Ptr<FeatureDetector> EpipolarGeometry::detector = ORB::create();
Ptr<DescriptorMatcher> EpipolarGeometry::matcher = DescriptorMatcher::create("BruteForce-Hamming");

// ��ʼ����̬�����㼯��������
vector<KeyPoint> EpipolarGeometry::keypoints1, EpipolarGeometry::keypoints2, EpipolarGeometry::keypoints3;
Mat EpipolarGeometry::descriptors1, EpipolarGeometry::descriptors2, EpipolarGeometry::descriptors3;

// �������
Mat EpipolarGeometry::E, EpipolarGeometry::F,
    EpipolarGeometry::R, EpipolarGeometry::t;

// ����ڲ�
Mat EpipolarGeometry::K = (Mat_<double>(3, 3) << 621.18428,0, 404.0076, 0, 621.18428, 309.05989,0 ,0, 1);

Point2d EpipolarGeometry::principal_point(K.at<double>(0, 2), K.at<double>(1, 2));

double EpipolarGeometry::focal_length = K.at<double>(0, 0);


using namespace cv;
using namespace std;

void EpipolarGeometry::Init(string input, string output, string result)
{
    inputfolder = input;
    outputfolder = output;
    resultpath = result;
}

int EpipolarGeometry::initialize()
{
    img1 = readNextImg(inputfolder);
    filename1 = filename3;
    img2 = readNextImg(inputfolder);
    filename2 = filename3;
    img3 = readNextImg(inputfolder);

    // ȷ��ͼ����ȷ��ȡ
    if (img1.empty() || img2.empty() ) {
        emit worker->logMessage("δ�ҵ�Ӱ��!\n");
        return -1;
    }

}

// ����ͼ���ʵ�1080P
Mat EpipolarGeometry::resizeImage(const Mat& image) 
{
    Mat resized;
    const int maxHeight = 1080;
    const int maxWidth = 1920;

    int originalHeight = image.rows;
    int originalWidth = image.cols;

    // ���Ӱ��ķֱ��ʴ���1080P�����������
    if (originalHeight > maxHeight || originalWidth > maxWidth) {
        // �������ű��������ֳ����
        double scaleFactor = std::min((double)maxHeight / originalHeight, (double)maxWidth / originalWidth);
        resize(image, resized, Size(), scaleFactor, scaleFactor, INTER_LINEAR);
    }
    else {
        // �ֱ���С�ڻ����1080P�����������ţ�ֱ�ӷ���ԭͼ
        resized = image.clone();
    }

    return resized;
}

void EpipolarGeometry::detectAndComputeFeatures(Mat img1, Mat img2, vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2, Mat& descriptors1, Mat& descriptors2)
{
    keypoints1.clear(); keypoints2.clear();

    // ����������������
    detector->detect(img1, keypoints1);
    detector->detect(img2, keypoints2);

    detector->compute(img1, keypoints1, descriptors1);
    detector->compute(img2, keypoints2, descriptors2);

    // ����������
    Mat outImg1;

    drawKeypoints(img1, keypoints1, outImg1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

    //imshow("ORB features", outImg1);
    //waitKey(0);
}

vector<DMatch> EpipolarGeometry::matchFeatures() 
{
    // ƥ�����������ɼӼ�ʱ
    vector<DMatch> matches;

    matcher->match(descriptors1, descriptors2, matches);

    return matches;
}

vector<DMatch> EpipolarGeometry::filterGoodMatches(const vector<DMatch>& matches) 
{
    // ɸѡƥ���
    auto min_max = minmax_element(matches.begin(), matches.end(),
        [](const DMatch& m1, const DMatch& m2) { return m1.distance < m2.distance; });
    double min_dist = min_max.first->distance;
    double max_dist = min_max.second->distance;

    emit worker->logMessage("-- Max dist : \n" + QString::number(max_dist));
    emit worker->logMessage("-- Min dist :\n" + QString::number(min_dist));

    vector<DMatch> good_matches;
    // ɸѡ�õ�ƥ��㣬����С��2������С�����30�ľ���ֵ
    for (const auto& match : matches) {
        if (match.distance <= max(2 * min_dist, 30.0)) {
            good_matches.push_back(match);
        }
    }

    return good_matches;
}

void extractCommonPoints( const std::vector<cv::DMatch>& match1, const std::vector<cv::DMatch>& match2,
    const std::vector<cv::KeyPoint>& keypoints1, const std::vector<cv::KeyPoint>& keypoints2, const std::vector<cv::KeyPoint>& keypoints3,
    std::vector<cv::Point2d>& pointsInImage1, std::vector<cv::Point2d>& pointsInImage2, std::vector<cv::Point2d>& pointsInImage3,
    std::vector<cv::Point3d> points3D,std::vector<cv::Point3d>& pointsIn3D )   { 

    pointsInImage1.clear();
    pointsInImage2.clear();
    pointsInImage3.clear();

    pointsIn3D.clear();

    // ��unordered_map����¼�ڶ���ͼ���е�ƥ��� (trainIdx)
    std::unordered_map<int, int> match1Map; // key: �ڶ���ͼ���е����� (trainIdx), value: ��һ��ͼ���е����� (queryIdx)

    // ����match1, ����trainIdx�����ϣ��
    for (const auto& m : match1) {
        match1Map[m.trainIdx] = m.queryIdx; // ��¼�ڶ���ͼ���е�����
    }
    // ����match2��Ѱ����match1��Ҳ���ֵ�trainIdx
    for (const auto& m2 : match2) {
        auto it = match1Map.find(m2.queryIdx);
        if (it != match1Map.end()) { // ������match1���Ƿ���ڴ˵�
            // ��ȡ��һ��ͼ���еĶ�ά��
            pointsInImage1.push_back(keypoints1[it->second].pt); // ʹ�� match1Map �еĵ�һ��ͼ������
            // ��ȡ�ڶ���ͼ���еĶ�ά��
            pointsInImage2.push_back(keypoints2[m2.queryIdx].pt); // ʹ�� match2 �е� queryIdx
            // ��ȡ������ͼ���еĶ�ά��
            pointsInImage3.push_back(keypoints3[m2.trainIdx].pt); // ʹ�� match2 �е� trainIdx

            pointsIn3D.push_back(points3D[it->second]); // ��ȡ��ά��
        }
    }

}


void EpipolarGeometry::drawAndShowMatches(Mat img1, Mat img2,vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2,
    const vector<DMatch>& matches, const vector<DMatch>& good_matches)
{
    // ����ƥ����
    Mat img_match, img_goodmatch;

    drawMatches(img1, keypoints1, img2, keypoints2, matches, img_match);
    drawMatches(img1, keypoints1, img2, keypoints2, good_matches, img_goodmatch);

    //imshow("All matches", img_match);
    //imshow("Good matches", img_goodmatch);
    emit worker->showimg(img_goodmatch);
    QString filename= QString::fromStdString(filename1).split(".")[0] + "_" + QString::fromStdString(filename2).split(".")[0];
    imwrite(outputfolder + "/" + filename.toStdString() + "_processed.jpg", img_goodmatch);
    //waitKey(0);
}

// 2D-2D�Լ�
void EpipolarGeometry::pose_estimation_2d2d(vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2,std::vector<DMatch> matches)
{
    ofstream out;
    out.open(resultpath, ios::app | ios::out);
    
    // ��ƥ���ת��Ϊvector<Point2f>����ʽ
    vector<Point2f> points1, points2;

    for (int i = 0; i < (int)matches.size(); i++) {
        points1.push_back(keypoints1[matches[i].queryIdx].pt);
        points2.push_back(keypoints2[matches[i].trainIdx].pt);
    }

    // �����������
    F = findFundamentalMat(points1, points2, FM_RANSAC);
    QString message;
    message = "F Matrix \n" + QString::fromStdString(Mat2string(F)) + "\n";
    emit worker->logMessage(message);
    out << "F Matrix \n" << F << endl;

    // ���㱾�� Matrix
    E = findEssentialMat(points1, points2, focal_length, principal_point);
    message = QString::fromStdString("E Matrix \n") + QString::fromStdString(Mat2string(E)) + "\n";
    emit worker->logMessage(message);
    out << "E Matrix \n" << endl << E << endl;

    // �ӱ��� Matrix�лָ���ת��ƽ����Ϣ.
    recoverPose(E, points1, points2, R, t, focal_length, principal_point);

    message = QString::fromStdString("R Matrix \n") + QString::fromStdString(Mat2string(R)) + "\n";
    emit worker->logMessage(message);
    out << "R Matrix \n" << endl << R << endl;
    message = QString::fromStdString("t Matrix \n") + QString::fromStdString(Mat2string(t)) + "\n";
    emit worker->logMessage(message);
    out << "t Matrix \n" << endl << t << endl;

    out.close();
    f << t.at<double>(0, 0) << " " << t.at<double>(1, 0) << " " << t.at<double>(2, 0) << endl;

}

// ��������ת�������
Point2d pixel2cam(const Point2d& p, const Mat& K) {
    return Point2d
    (
        (p.x - K.at<double>(0, 2)) / K.at<double>(0, 0),
        (p.y - K.at<double>(1, 2)) / K.at<double>(1, 1)
    );
}

// ���б��ʾ�����֤�ͶԼ�Լ��
void EpipolarGeometry::verify(vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2, std::vector<DMatch> matches)
{
    Mat t_x = (Mat_<double>(3, 3) << 0, -t.at<double>(2, 0), t.at<double>(1, 0), 
        t.at<double>(2, 0), 0, -t.at<double>(0, 0),
        -t.at<double>(1, 0), t.at<double>(0, 0), 0);

    emit worker->logMessage("t^R= \n" + QString::fromStdString(Mat2string(t_x * R)) + "\n");

    for (DMatch m : matches) 
    {
        Point2d pt1 = pixel2cam(keypoints1[m.queryIdx].pt, K);
        Mat y1 = (Mat_<double>(3, 1) << pt1.x, pt1.y, 1);

        Point2d pt2 = pixel2cam(keypoints2[m.trainIdx].pt, K);
        Mat y2 = (Mat_<double>(3, 1) << pt2.x, pt2.y, 1);

        Mat d = y2.t() * t_x * R * y1;

        emit worker->logMessage("epipolar constraint = " + QString::fromStdString(Mat2string(d)) + "\n");
    }
}

// ���ǻ��ǳ�ʼ���õİɣ���������Ҫ����������Ҫ����
void EpipolarGeometry::triangulation(std::vector<DMatch> matches,vector<Point3d>& points)
{
    points.clear();

    Mat T1 = (Mat_<float>(3, 4) <<
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0);
    Mat T2 = (Mat_<float>(3, 4) <<
        R.at<double>(0, 0), R.at<double>(0, 1), R.at<double>(0, 2), t.at<double>(0, 0),
        R.at<double>(1, 0), R.at<double>(1, 1), R.at<double>(1, 2), t.at<double>(1, 0),
        R.at<double>(2, 0), R.at<double>(2, 1), R.at<double>(2, 2), t.at<double>(2, 0)
        );

    Mat K = EpipolarGeometry::K;
    vector<Point2f> pts_1, pts_2;

    for (DMatch m : matches) {
        // ����������ת�����������
        pts_1.push_back(pixel2cam(keypoints1[m.queryIdx].pt, K));
        pts_2.push_back(pixel2cam(keypoints2[m.trainIdx].pt, K));
    }

    Mat pts_4d;
    cv::triangulatePoints(T1, T2, pts_1, pts_2, pts_4d);
    
    // ת���ɷ��������
    for (int i = 0; i < pts_4d.cols; i++) {
        Mat x = pts_4d.col(i);
        x /= x.at<float>(3, 0); // ��һ��
        Point3d p(
            x.at<float>(0, 0),
            x.at<float>(1, 0),
            x.at<float>(2, 0)
        );
        points.push_back(p);
    }
}

/// ��ͼ��
inline cv::Scalar get_color(float depth) {
    float up_th = 50, low_th = 10, th_range = up_th - low_th;
    if (depth > up_th) depth = up_th;
    if (depth < low_th) depth = low_th;
    return cv::Scalar(255 * depth / th_range, 0, 255 * (1 - depth / th_range));
}

// ��ͶӰ��֤��û̫��
void EpipolarGeometry::verifyReprojection(std::vector<DMatch> matches, std::vector<Point3d>& points)
{
    Mat K = EpipolarGeometry::K;
    Mat img1_plot = img1.clone();
    Mat img2_plot = img2.clone();

    // �������������ľ��룬�����г߶Ȳ�ȷ����
    for (int i = 0; i < matches.size(); i++)
    {
        Point2d pt1_cam = pixel2cam(keypoints1[matches[i].queryIdx].pt, K);

        Point2d pt1_cam_3d( points[i].x / points[i].z, points[i].y / points[i].z );

        emit worker->logMessage("point in the first camera frame: " + QString::fromStdString(Point2d2string(pt1_cam)) +"\n");
        emit worker->logMessage("point projected from 3D: " + QString::fromStdString(Point2d2string(pt1_cam_3d)) + ", d=" + QString::number(points[i].z) + "\n");

        // ��2��ͼ
        Point2f pt2_cam = pixel2cam(keypoints2[matches[i].trainIdx].pt, K);
        Mat pt2_trans = R * (Mat_<double>(3, 1) << points[i].x, points[i].y, points[i].z) + t;
        pt2_trans /= pt2_trans.at<double>(2, 0);

        emit worker->logMessage("point in the second camera frame: " + QString::fromStdString(Point2d2string(pt2_cam)) + "\n");
        emit worker->logMessage("point reprojected from second frame: " + QString::fromStdString(Mat2string(pt2_trans.t()))+"\n");
        
        emit worker->logMessage("\n");
    }


    // ����
    for (int i = 0; i < matches.size(); i++) {
        // ��һ��ͼ
        float depth1 = points[i].z;
        emit worker->logMessage("depth: " + QString::number(depth1) + "\n");
        Point2d pt1_cam = pixel2cam(keypoints1[matches[i].queryIdx].pt, K);
        cv::circle(img1_plot, keypoints1[matches[i].queryIdx].pt, 2, get_color(depth1), 2);

        // �ڶ���ͼ
        Mat pt2_trans = R * (Mat_<double>(3, 1) << points[i].x, points[i].y, points[i].z) + t;
        float depth2 = pt2_trans.at<double>(2, 0);
        cv::circle(img2_plot, keypoints2[matches[i].trainIdx].pt, 2, get_color(depth2), 2);
    }
    emit worker->logMessage(QString::fromLocal8Bit("��ͶӰ��֤��"));
    emit worker->logMessage("IMG1:  " + QString::fromStdString(filename1));
    emit worker->showimg(img1_plot);
    emit worker->logMessage("IMG2:  " + QString::fromStdString(filename2));
    emit worker->showimg(img2_plot);
    //cv::imshow("img 1", img1_plot);
    //cv::imshow("img 2", img2_plot);
    //cv::waitKey(500);
}

// 2D-3D PnP �������λ��
void EpipolarGeometry::pose_estimation_2d3d(const vector<Point2d>& imagePoints, const vector<Point3d>& objectPoints) {
    // ����ڲξ���
    Mat K = EpipolarGeometry::K;

    // ����ϵ���������޻��䣩
    Mat distCoeffs = Mat::zeros(4, 1, CV_64F);

    // �������ת������ƽ������
    Mat rvec, tvec;

    // ���� solvePnP
    bool success = solvePnP(objectPoints, imagePoints, K, distCoeffs, rvec, tvec);

    Rodrigues(rvec, R);

    if (success) {
        emit worker->logMessage("��ת���� (rvec): " + QString::fromStdString(Mat2string(rvec)) + "\n");
        emit worker->logMessage("ƽ������ (tvec): " + QString::fromStdString(Mat2string(tvec)) + "\n");
        emit worker->logMessage("��ת��������: " + QString::fromStdString(Mat2string(R)) + "\n");

    }
    else {
        emit worker->logMessage("PnP ����ʧ�ܣ�\n");
    }
}

Mat SgbmTest(Mat left_ud, Mat right_ud) {
    int SADWindowSize = 11;//����������
    Ptr<StereoSGBM> sgbm = StereoSGBM::create();
    sgbm->setBlockSize(SADWindowSize);
    sgbm->setP1(8 * 1 * SADWindowSize * SADWindowSize);
    sgbm->setP2(32 * 1 * SADWindowSize * SADWindowSize);
    sgbm->setMinDisparity(0);
    sgbm->setNumDisparities(128);//128//num_disp good
    sgbm->setUniquenessRatio(5);//good
    sgbm->setSpeckleWindowSize(100);//good
    sgbm->setSpeckleRange(32);
    sgbm->setDisp12MaxDiff(1);
    sgbm->setPreFilterCap(64);// good
    sgbm->setMode(StereoSGBM::MODE_HH);//good
    Mat disp, disp_8;
    sgbm->compute(left_ud, right_ud, disp);

    disp = disp.colRange(128, disp.cols);        //ת���������һ����û���ã�ɾȥ
    disp.convertTo(disp_8, CV_8U, 255 / (128 * 16.));

    imshow("disp", disp_8);
    waitKey(0);
    return disp;

}

std::vector<cv::DMatch> findCommonMatches(const std::vector<cv::DMatch>& match1,const std::vector<cv::DMatch>& match2) {

    std::vector<cv::DMatch> commonMatches;

    // ��unordered_map����¼�ڶ���ͼ���е�ƥ��� (trainIdx)
    std::unordered_map<int, int> match1Map;

    // ����match1, ����trainIdx�����ϣ��
    for (const auto& m : match1) {
        match1Map[m.trainIdx] = m.queryIdx; // key: �ڶ���ͼ���е�����
    }

    // ����match2��Ѱ����match1��Ҳ���ֵ�trainIdx
    for (const auto& m2 : match2) {
        if (match1Map.find(m2.queryIdx) != match1Map.end()) {
            // ������ƥ������commonMatches
            commonMatches.push_back(m2);
        }
    }

    return commonMatches;
}

void drawCommonPointsOnImages(
    const cv::Mat& img1, const cv::Mat& img2, const cv::Mat& img3,
    const std::vector<cv::KeyPoint>& keypoints1,
    const std::vector<cv::KeyPoint>& keypoints2,
    const std::vector<cv::KeyPoint>& keypoints3,
    const std::vector<cv::DMatch>& match1,
    const std::vector<cv::DMatch>& match2) {

    // ������ͼ��ˮƽƴ��
    cv::Mat combinedImg;
    cv::hconcat(img1, img2, combinedImg);
    cv::hconcat(combinedImg, img3, combinedImg);

    // ����ͼ���ƫ������������ȷ���Ƶ��λ��
    int offsetImg1 = 0;                     // ��һ��ͼ��û��ƫ��
    int offsetImg2 = img1.cols;             // �ڶ���ͼ���ƫ���ǵ�һ��ͼ��Ŀ��
    int offsetImg3 = img1.cols + img2.cols; // ������ͼ���ƫ����ǰ����ͼ��Ŀ�Ⱥ�

    // ����һ�����湲ͬƥ����vector
    std::vector<cv::DMatch> commonMatches = findCommonMatches(match1, match2);

    // ��ƴ�Ӻ��ͼ���ϻ��ƹ�ͬ�������㼰����
    for (const auto& match : commonMatches) {
        // ��commonMatches����ȡƥ���
        cv::Point2f pt1 = keypoints1[match.queryIdx].pt;  // ��һ��ͼ���ϵĵ�
        cv::Point2f pt2 = keypoints2[match.trainIdx].pt;  // �ڶ���ͼ���ϵĵ�
        cv::Point2f pt3 = keypoints3[match.trainIdx].pt;  // ������ͼ���ϵĵ�

        // ����ƫ�����������λ��
        pt1.x += offsetImg1;
        pt2.x += offsetImg2;
        pt3.x += offsetImg3;

        // ����������
        cv::circle(combinedImg, pt1, 5, cv::Scalar(0, 255, 0), -1);
        cv::circle(combinedImg, pt2, 5, cv::Scalar(0, 0, 255), -1);
        cv::circle(combinedImg, pt3, 5, cv::Scalar(255, 0, 0), -1);

        // ��������
        cv::line(combinedImg, pt1, pt2, cv::Scalar(0, 255, 255), 2);
        cv::line(combinedImg, pt2, pt3, cv::Scalar(0, 255, 255), 2);
    }

    // ��ʾƴ�Ӻ��ͼ��
    //cv::imshow("Common Feature Points", combinedImg);
    //cv::waitKey(0);
}

void drawPointsOnImage(cv::Mat& image, const std::vector<cv::Point2d>& points) {
    for (const auto& pt : points) {
        cv::circle(image, pt, 2, cv::Scalar(0, 255, 0), -1); // ��ÿ�������һ����ɫԲȦ
    }
}

// ������ͼ���ϻ���ƥ����
void drawLinesBetweenImages(
    cv::Mat& img1, cv::Mat& img2, cv::Mat& img3,
    const std::vector<cv::Point2d>& points1,
    const std::vector<cv::Point2d>& points2,
    const std::vector<cv::Point2d>& points3,
    int offset2, int offset3) {

    for (size_t i = 0; i < points1.size(); ++i) {
        // ��ͼ��1��ͼ��2����
        cv::line(img1, points1[i], cv::Point2f(points2[i].x + offset2, points2[i].y), cv::Scalar(255, 0, 0), 1);
        // ��ͼ��2��ͼ��3����
        cv::line(img2, cv::Point2f(points2[i].x + offset2, points2[i].y),
            cv::Point2f(points3[i].x + offset3, points3[i].y), cv::Scalar(255, 0, 0), 1);
    }
}

void draw3ImagePointsLine(cv::Mat& img1, cv::Mat& img2, cv::Mat& img3,
    std::vector<cv::Point2d>& pointsInImage1, std::vector<cv::Point2d>& pointsInImage2, std::vector<cv::Point2d>& pointsInImage3)
{
    // ��ͼ���ϻ��ƹ�����
    drawPointsOnImage(img1, pointsInImage1);
    drawPointsOnImage(img2, pointsInImage2);
    drawPointsOnImage(img3, pointsInImage3);

    // ����һ�Ŵ󻭲���������ͼ��ƴ����һ��
    int totalWidth = img1.cols + img2.cols + img3.cols;
    int maxHeight = std::max({ img1.rows, img2.rows, img3.rows });

    cv::Mat combinedImage(maxHeight, totalWidth, img1.type(), cv::Scalar(0, 0, 0));

    // ������ͼ�񿽱���������
    img1.copyTo(combinedImage(cv::Rect(0, 0, img1.cols, img1.rows)));
    img2.copyTo(combinedImage(cv::Rect(img1.cols, 0, img2.cols, img2.rows)));
    img3.copyTo(combinedImage(cv::Rect(img1.cols + img2.cols, 0, img3.cols, img3.rows)));

    cv::imwrite("Combined Image with Common Points.jpg", combinedImage);

    // ����������
    int offset2 = img1.cols;
    int offset3 = img1.cols + img2.cols;

    drawLinesBetweenImages(combinedImage, combinedImage, combinedImage, pointsInImage1, pointsInImage2, pointsInImage3, offset2, offset3);

    cv::imwrite("Combined Lined Image with Common Points.jpg", combinedImage);   // д��ƴ�Ӻ��ͼ��

}

// ��ȡ�ļ����е�PNGͼƬ�������򷵻�һ��ͼƬ
Mat EpipolarGeometry::readNextImg(const std::string& folderPath) {
    // ��̬���������ڱ��浱ǰ�������ļ��б�
    static std::vector<std::string> imageFiles;
    static size_t currentIndex = 0;

    // ��ʼ��ʱ��ȡ�ļ����е�ͼƬ�ļ�
    if (imageFiles.empty()) {
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            // ��ȡ�ļ���չ��
            std::string ext = entry.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);  // ����չ��תΪСд

            // �ж��Ƿ�Ϊ������ͼƬ��ʽ
            if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".bmp") {
                imageFiles.push_back(entry.path().string());
            }
        }

        // ����ļ�����û��ͼƬ�ļ������ؿվ���
        if (imageFiles.empty()) {
            std::cerr << "No image files found in the folder." << std::endl;
            return cv::Mat();
        }

        // ���ļ���������ȷ�������ļ���˳���ȡ
        std::sort(imageFiles.begin(), imageFiles.end());
    }

    // ��������ļ�����ȡ��ϣ����ؿվ���
    if (currentIndex >= imageFiles.size()) {
        std::cout << "All images have been read." << std::endl;
        return cv::Mat();
    }

    // ��ȡ��ǰ������ͼƬ
    std::string imagePath = imageFiles[currentIndex];
    QFileInfo imginfo(QString::fromStdString(imagePath));
    filename3 = imginfo.fileName().toStdString();
    cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);
    emit worker->logMessage(QString::fromLocal8Bit("����ͼƬ��") + QString::fromStdString(filename3));
    emit worker->showimg(image);

    if (image.empty()) {
        std::cerr << "Failed to load image: " << imagePath << std::endl;
    }

    // ����������׼���´ζ�ȡ
    currentIndex++;

    return image;
}


// ���Լ��ͽ⣬2D-2D�Ӿ���̼ƺ���
void EpipolarGeometry::Run(EpipolarGeometryWorker* w)
{
    worker = w;
    // ��ʼ���࣬����ͼ��·��
    if (initialize() == -1)   return;
    ofstream out;
    out.open(resultpath, ios::out | ios::trunc);
    emit worker->logMessage(QString::fromLocal8Bit("��ʼ����: ")+ QString::fromStdString(filename1) + "  ->  " + QString::fromStdString(filename2) + "\n");
    out << "��ʼ����: " << filename1 << "  ->  " << filename2 << "\n";
    out.close();
    // 1. ���ͼ��������㼰������
    detectAndComputeFeatures(img1, img2, keypoints1, keypoints2, descriptors1, descriptors2);

    // 2. ƥ������
    vector<DMatch> matches = matchFeatures();

    // 3. ɸѡ�õ�ƥ���
    vector<DMatch> good_matches = filterGoodMatches(matches);

    // 4. ���Ʋ���ʾƥ����
    drawAndShowMatches(img1, img2, keypoints1, keypoints2, matches, good_matches);

    // 5. ��������˶�
    pose_estimation_2d2d(keypoints1, keypoints2, good_matches);

    // 6. ��֤
    verify(keypoints1, keypoints2, good_matches);

    emit worker->updateProgress(1);

    // 7. ���ǲ���
    vector<Point3d> points3D, pointsAll3D, pointsCDIn3D, pointCDIn34;

    triangulation(good_matches, points3D);
    triangulation(matches, pointsAll3D);

    // 8. ��ͶӰ��֤
    verifyReprojection(good_matches, points3D);

    ofstream out_good_point, out_all_point;
    out_good_point.open(outputfolder + "/points.txt", ios::out | ios::trunc);
    out_all_point.open(outputfolder + "/All_points.txt", ios::out | ios::trunc);

    out_good_point << filename1 << " -> " << filename2 << "\n";
    out_good_point << fixed << setprecision(6);
    out_all_point << fixed << setprecision(6);

    out_good_point << left << setw(12) << "x\t" << setw(12) << "y\t" << setw(12) << "z\n";
    for (const Point3d point : points3D)
    {
        out_good_point << left << setw(12) << point.x << "\t" << setw(12) << point.y << "\t" << setw(12) << point.z << "\n";
    }

    out_all_point << filename1 << " -> " << filename2 << "\n";
    out_all_point << fixed << setprecision(6);
    out_all_point << left << setw(12) << "x\t" << setw(12) << "y\t" << setw(12) << "z\n";
    for (const Point3d point : pointsAll3D)
    {
        out_all_point << left << setw(12) << point.x << "\t" << setw(12) << point.y << "\t" << setw(12) << point.z << "\n";
    }

    emit worker->updateProgress(2);

    //f.open("data.txt", ios::out);

    //// �ͽ���Ӿ���̼�
    //while (true) 
    //{
    //    img1 = img2;
    //    img2 = readNextPNG(inputfolder);
    //    if (img2.empty()) {
    //        std::cout << "No more images to read or failed to load image." << std::endl;
    //        break;
    //    }
    //
    //    detectAndComputeFeatures(img1, img2, keypoints1, keypoints2, descriptors1, descriptors2);

    //    matches = matchFeatures();
    //    good_matches = filterGoodMatches(matches);

    //    pose_estimation_2d2d(keypoints1, keypoints2, good_matches);
    //}

    //f.close();



    //// 9. 2D-3D PnP����
    //vector<Point2d> imagePoints; // ���ڴ洢��Ӧ�Ķ�ά��

    //for (const DMatch& m : good_matches) 
    //{
    //    imagePoints.push_back(keypoints2[m.trainIdx].pt); // ѡȡ img2 �е�������
    //}
    //// pose_estimation_2d3d(imagePoints, points3D); // ���� PnP ���ƣ��ڶ���ͼ���1/2ͼ���γɵ�3D����PnP��λ�ƽӽ���0 ?Ϊʲô�������

    //// 10. �Ե�2��3��Ӱ���������ƥ��

    //// ���ɵ�����Ӱ����������������
    //detector->detect(img3, keypoints3);
    //detector->compute(img3, keypoints3, descriptors3);

    //// ���ɵ�2��3��Ӱ���ƥ����
    //vector<DMatch> matches23;
    //matcher->match(descriptors2, descriptors3, matches23);
    //vector<DMatch> good_matches23 = filterGoodMatches(matches23);

    //// ��ȡ�����ص�����������ڵ�����ͼ�Ķ�άλ��
    //vector<cv::Point2d> pointsInImage1, pointsInImage2, pointsInImage3;

    //extractCommonPoints(good_matches, good_matches23, keypoints1, keypoints2, keypoints3,
    //    pointsInImage1, pointsInImage2, pointsInImage3, pointsAll3D, pointsCDIn3D);

    //// ��ͼ���ϻ��ƹ�����
    //draw3ImagePointsLine(img1, img2, img3, pointsInImage1, pointsInImage2, pointsInImage3);

    //pose_estimation_2d3d(pointsInImage3, pointsCDIn3D);
    


    //
    //while (true)
    //{
    //    img1 = img2.clone();
    //    img2 = img3.clone();
    //    img3 = readNextPNG(inputfolder);

    //    if (img3.empty()) {
    //        std::cout << "No more images to read or failed to load image." << std::endl;
    //        break;
    //    }

    //    detector->detect(img1, keypoints1);
    //    detector->compute(img1, keypoints1, descriptors1);

    //    detector->detect(img2, keypoints2);
    //    detector->compute(img2, keypoints2, descriptors2);

    //    // ���ɵ�����Ӱ����������������
    //    detector->detect(img3, keypoints3);
    //    detector->compute(img3, keypoints3, descriptors3);

    //    // ���ɵ�2��3��Ӱ���ƥ����
    //    matcher->match(descriptors2, descriptors3, matches23);
    //    good_matches23 = filterGoodMatches(matches23);

    //    // ��ȡ�����ص�����������ڵ�����ͼ�Ķ�άλ��

    //    extractCommonPoints(good_matches, good_matches23, keypoints1, keypoints2, keypoints3,
    //        pointsInImage1, pointsInImage2, pointsInImage3, pointsAll3D, pointsCDIn3D);

    //    // ��ͼ���ϻ��ƹ�����
    //    draw3ImagePointsLine(img1, img2, img3, pointsInImage1, pointsInImage2, pointsInImage3);

    //    pose_estimation_2d3d(pointsInImage3, pointsCDIn3D);
    //}

    //pose_estimation_2d2d(keypoints1, keypoints2, good_matches);


    //// ���������ͼ��-------------------------------------------------------------------------------------

    //// ��ʼ������˳����ǰ
    ////img1 = img2.clone();
    ////img2 = img3.clone();
    ////img3 = readNextPNG(inputfolder);

    ////keypoints1 = keypoints2;
    ////keypoints2 = keypoints3;

    ////descriptors1 = descriptors2.clone();
    ////descriptors2 = descriptors3.clone();

    ////detector->detect(img3, keypoints3);
    ////detector->compute(img3, keypoints3, descriptors3);

    ////

    ////matches = matches23;
    ////matcher->match(descriptors2, descriptors3, matches23);

    ////good_matches23 = filterGoodMatches(matches23);  //����ֻʹ��good_matches��

    //// ��ȡ�����ص�����������ڵ�����ͼ�Ķ�άλ��
    ////triangulation(matches, pointsAll3D);  // 34,23?����һ��������������


}

void EpipolarGeometry::VORun(EpipolarGeometryWorker* w)
{
    worker = w;
    // ��ʼ���࣬����ͼ��·��
    if (initialize() == -1)   return;

    ofstream out, out_good_point, out_all_point;
    out.open(resultpath, ios::out | ios::trunc); out.close();
    out_good_point.open(outputfolder + "/points.txt", ios::out | ios::trunc); out_good_point.close();
    out_all_point.open(outputfolder + "/All_points.txt", ios::out | ios::trunc); out_all_point.close();
    int totalimgs = 0;

    Point3d trajectory(0, 0, 0);

    while (!img3.empty())
    {
        img1 = img2.clone();
        filename1 = filename2;
        img2 = img3.clone();
        filename2 = filename3;

        ofstream out;
        out.open(resultpath, ios::out | ios::app);
        emit worker->logMessage(QString::fromLocal8Bit("��ʼ����: ") + QString::fromStdString(filename1) + "  ->  " + QString::fromStdString(filename2) + "\n");
        out << "��ʼ����: " << filename1 << "  ->  " << filename2 << "\n";
        out.close();
        // 1. ���ͼ��������㼰������
        detectAndComputeFeatures(img1, img2, keypoints1, keypoints2, descriptors1, descriptors2);

        // 2. ƥ������
        vector<DMatch> matches = matchFeatures();

        // 3. ɸѡ�õ�ƥ���
        vector<DMatch> good_matches = filterGoodMatches(matches);

        // 4. ���Ʋ���ʾƥ����
        drawAndShowMatches(img1, img2, keypoints1, keypoints2, matches, good_matches);

        // 5. ��������˶�
        pose_estimation_2d2d(keypoints1, keypoints2, good_matches);

        // 6. ��֤
        verify(keypoints1, keypoints2, good_matches);


        // 7. ���ǲ���
        vector<Point3d> points3D, pointsAll3D, pointsCDIn3D, pointCDIn34;

        triangulation(good_matches, points3D);
        triangulation(matches, pointsAll3D);

        // 8. ��ͶӰ��֤
        verifyReprojection(good_matches, points3D);

        ofstream out_good_point, out_all_point, out_trajectory;
        out_good_point.open(outputfolder + "/points.txt", ios::out | ios::app);
        out_all_point.open(outputfolder + "/All_points.txt", ios::out | ios::app);
        out_trajectory.open(outputfolder + "/trajectory.txt", ios::out | ios::app);

        out_good_point << filename1 << " -> " << filename2 << "\n";
        out_good_point << fixed << setprecision(6);
        out_good_point << left << setw(12) << "x\t" << setw(12) << "y\t" << setw(12) << "z\n";
        for (const Point3d point : points3D)
        {
            out_good_point << left << setw(12) << point.x << "\t" << setw(12) << point.y << "\t" << setw(12) << point.z << "\n";
            out_good_point << "������������������������������������������������������������������������������������������������������������������������" << std::endl;
        }

        out_all_point << filename1 << " -> " << filename2 << "\n";
        out_all_point << fixed << setprecision(6);
        out_all_point << left << setw(12) << "x\t" << setw(12) << "y\t" << setw(12) << "z\n";
        for (const Point3d point : pointsAll3D)
        {
            out_all_point << left << setw(12) << point.x << "\t" << setw(12) << point.y << "\t" << setw(12) << point.z << "\n";
            out_all_point << "������������������������������������������������������������������������������������������������������������������������" << std::endl;
        }

        trajectory.x += t.at<double>(0, 0);
        trajectory.y += t.at<double>(1, 0);
        trajectory.z += t.at<double>(2, 0);

        out_trajectory << fixed << setprecision(6);
        out_trajectory << left << setw(12) << "x\t" << setw(12) << "y\t" << setw(12) << "z\n";
        out_trajectory << left << setw(12) << trajectory.x << setw(12) << trajectory.y << setw(12) << trajectory.z << "\n";
        

        totalimgs++;
        emit worker->updateProgress(totalimgs);
        img3 = readNextImg(inputfolder);
    }
    
}

