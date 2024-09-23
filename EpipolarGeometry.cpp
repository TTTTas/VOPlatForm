#include "EpipolarGeometry.h"
#include <iostream>

// ��ʼ��·��
const string EpipolarGeometry::filename1 = "./Input/Image_5.JPG";
const string EpipolarGeometry::filename2 = "./Input/Image_6.JPG";
Mat EpipolarGeometry::img1, EpipolarGeometry::img2;

// �������������ΪORB��ƥ�䷽��ΪBF
Ptr<FeatureDetector> EpipolarGeometry::detector = ORB::create();
Ptr<DescriptorMatcher> EpipolarGeometry::matcher = DescriptorMatcher::create("BruteForce-Hamming");

// ��ʼ����̬�����㼯��������
vector<KeyPoint> EpipolarGeometry::keypoints1, EpipolarGeometry::keypoints2;
Mat EpipolarGeometry::descriptors1, EpipolarGeometry::descriptors2;

using namespace cv;
using namespace std;

void EpipolarGeometry::initialize()
{
    // ��ȡͼ��
    img1 = imread(filename1);
    img2 = imread(filename2);

    // ���ͻ��ʵ�1080P
    img1 = resizeImage(img1);
    img2 = resizeImage(img2);

    // ȷ��ͼ����ȷ��ȡ
    if (img1.empty() || img2.empty()) {
        cout << "δ�ҵ�Ӱ��!" << endl;
        return;
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

void EpipolarGeometry::detectAndComputeFeatures()
{
    // ����������������
    chrono::steady_clock::time_point t1 = chrono::steady_clock::now();

    detector->detect(img1, keypoints1);
    detector->detect(img2, keypoints2);

    detector->compute(img1, keypoints1, descriptors1);
    detector->compute(img2, keypoints2, descriptors2);

    chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
    chrono::duration<double> time_used = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
    cout << "Feature detection and computation took " << time_used.count() << " seconds." << endl;

    // ����������
    Mat outImg1;

    drawKeypoints(img1, keypoints1, outImg1, Scalar::all(-1), DrawMatchesFlags::DEFAULT);

    imshow("ORB features", outImg1);

    waitKey(0);
}

vector<DMatch> EpipolarGeometry::matchFeatures()
{
    // ƥ��������
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
    // ɸѡƥ���
    auto min_max = minmax_element(matches.begin(), matches.end(),
        [](const DMatch& m1, const DMatch& m2) { return m1.distance < m2.distance; });
    double min_dist = min_max.first->distance;
    double max_dist = min_max.second->distance;

    printf("-- Max dist : %f \n", max_dist);
    printf("-- Min dist : %f \n", min_dist);

    vector<DMatch> good_matches;
    // ɸѡ���á���ƥ��㣬����С��2������С�����30�ľ���ֵ
    for (const auto& match : matches) {
        if (match.distance <= max(2 * min_dist, 30.0)) {
            good_matches.push_back(match);
        }
    }

    return good_matches;
}

void EpipolarGeometry::drawAndShowMatches(const vector<DMatch>& matches, const vector<DMatch>& good_matches)
{
    // ����ƥ����
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
    // ��ʼ���࣬����ͼ��·��
    initialize();

    // 1. ���ͼ��������㼰������
    detectAndComputeFeatures();

    // 2. ƥ������
    vector<DMatch> matches = matchFeatures();

    // 3. ɸѡ�õ�ƥ���
    vector<DMatch> good_matches = filterGoodMatches(matches);

    // 4. ���Ʋ���ʾƥ����
    drawAndShowMatches(matches, good_matches);

}