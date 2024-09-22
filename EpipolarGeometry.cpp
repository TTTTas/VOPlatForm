#include "EpipolarGeometry.h"
#include <iostream>

using namespace std;
using namespace cv;

// ��ʼ����̬����
const double ImageProcessing::scale_factor = 0.25f;
const string ImageProcessing::filename001 = "./Input/tea 1.JPG";
const string ImageProcessing::filename002 = "./Input/tea 2.JPG";
const Mat ImageProcessing::K = (Mat_<double>(3, 3) << 1354.222062716567, 0, 204.9442952614041,
    0, 1305.36634809663, 89.65190111179771,
    0, 0, 1);

// ����ͼ���ʵ�1080P
Mat ImageProcessing::resizeImage(const Mat& image) {
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

// ͼ��Ԥ������ ����Ϊ��Ҫ����
Mat ImageProcessing::preprocessImage(const Mat& image) {
    Mat gray, blurred, equalized, sharpened;

    // 1. ת��Ϊ�Ҷ�ͼ��
    cvtColor(image, gray, COLOR_BGR2GRAY);

    // 2. ʹ��˫���˲�
    bilateralFilter(gray, blurred, 9, 75, 75);

    // 3. ����Ӧֱ��ͼ���⻯
    Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8, 8));
    clahe->apply(blurred, equalized);

    // 4. ͼ����
    Mat laplacian;
    Laplacian(equalized, laplacian, CV_16S, 3);
    convertScaleAbs(laplacian, laplacian);
    sharpened = equalized - 0.3 * laplacian;

    return sharpened;
}

// ʹ��ORB��SIFT��ȡ������
void ImageProcessing::detectAndMatchFeaturesBF(const Mat& img1, const Mat& img2,
    vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2,
    vector<DMatch>& matches, Mat& descriptors1, Mat& descriptors2) {

    // ����ORB��SIFT������ȡ��
    Ptr<Feature2D> detector = ORB::create();  // Ҳ����ʹ�� SIFT::create()

    // ��ȡ�������������
    detector->detectAndCompute(img1, noArray(), keypoints1, descriptors1);
    detector->detectAndCompute(img2, noArray(), keypoints2, descriptors2);

    // ʹ��BFMatcher����ƥ�䣨����ƥ�䣬�ƺ�Ч��һ�㣩
    BFMatcher matcher(NORM_HAMMING, true);  // ORBʹ��NORM_HAMMING��SIFTʹ��NORM_L2
    matcher.match(descriptors1, descriptors2, matches);
}

// 14�� �� ������϶�����ʹ��FBƥ�䷽����Ϊ����
// ʹ��Flann-based matcher����ƥ�䣬��Ӧ��Ratio Test
void ImageProcessing::detectAndMatchFeaturesFB(const Mat& img1, const Mat& img2,
    vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2,
    vector<DMatch>& matches, Mat& descriptors1, Mat& descriptors2) {

    // ����ORB��SIFT������ȡ��
    Ptr<Feature2D> detector = ORB::create();  // �����л�Ϊ SIFT::create() ���� SURF::create()

    // ��ȡ�������������
    detector->detectAndCompute(img1, noArray(), keypoints1, descriptors1);
    detector->detectAndCompute(img2, noArray(), keypoints2, descriptors2);

    // ���ʹ�õ���ORB������������������������Ҫ����ת��Ϊ������������FLANN
    if (descriptors1.type() != CV_32F) {
        descriptors1.convertTo(descriptors1, CV_32F);
        descriptors2.convertTo(descriptors2, CV_32F);
    }

    // ���� Flann-based Matcher
    FlannBasedMatcher fbmatcher;

    // KNNƥ�䣺����ÿ�����������ҵ�2�����ڽ���ƥ��
    vector<vector<DMatch>> knn_matches;
    fbmatcher.knnMatch(descriptors1, descriptors2, knn_matches, 3);  // K = 2

    // Ӧ�� Ratio Test ����ƥ���
    const float ratio_thresh = 0.90f;  // ������ֵ
    for (size_t i = 0; i < knn_matches.size(); i++) {
        if (knn_matches[i][0].distance < ratio_thresh * knn_matches[i][1].distance) {
            matches.push_back(knn_matches[i][0]);  // ͨ�����ʲ��Ե�ƥ���
        }
    }

    // ���ƥ����
    cout << "Number of good matches after Ratio Test: " << matches.size() << endl;
}

// �޳��ֲ�ƥ��
vector<DMatch> ImageProcessing::filterMatchesWithFundamentalMat(const vector<KeyPoint>& keypoints1,
    const vector<KeyPoint>& keypoints2, const vector<DMatch>& matches, Mat& F) {

    // ��ȡƥ����
    vector<Point2f> points1, points2;
    for (const DMatch& match : matches) {
        points1.push_back(keypoints1[match.queryIdx].pt);
        points2.push_back(keypoints2[match.trainIdx].pt);
    }

    // ������������޳����
    vector<uchar> inliersMask(points1.size());
    F = findFundamentalMat(points1, points2, inliersMask, FM_RANSAC);

    // �������ڵ�ƥ��
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

    // ����һ��ƴ�ӵ�ͼ��
    Mat imgMatches;
    hconcat(img1, img2, imgMatches);  // ˮƽƴ������ͼ��

    // ����ƥ��������
    for (const auto& match : matches) {
        // ȷ�������ڷ�Χ��
        if (match.queryIdx >= points1.size() || match.trainIdx >= points2.size()) {
            cerr << "Index out of range: queryIdx or trainIdx exceeds points size" << endl;
            continue; // ������Ч��ƥ��
        }

        // ͼ��1�еĵ�
        Point2f pt1 = points1[match.queryIdx];
        // ͼ��2�еĵ㣨��Ҫ��x����ƫ��img1�Ŀ�ȣ�
        Point2f pt2 = points2[match.trainIdx] + Point2f((float)img1.cols, 0);

        // ��ƴ�ӵ�ͼ���ϻ��Ƶ������
        circle(imgMatches, pt1, 5, Scalar(0, 255, 0), -1);  // ��ɫ��
        circle(imgMatches, pt2, 5, Scalar(0, 255, 0), -1);  // ��ɫ��
        line(imgMatches, pt1, pt2, Scalar(255, 0, 0), 1);   // ��ɫ����
    }

    // ��ʾ���
    // imshow("Matches with Lines", imgMatches);
    imwrite("������ƥ��.jpg", imgMatches);
    waitKey(0);
}

// ���Ƽ���
void ImageProcessing::drawEpilines(const Mat& img1, const Mat& img2,
    const vector<Point2f>& points1, const vector<Point2f>& points2, const Mat& F) {

    // ���㼫��
    vector<Vec3f> lines1, lines2;
    computeCorrespondEpilines(points1, 1, F, lines1); // ����ͼ1�е��Ӧ��ͼ2�еļ���
    computeCorrespondEpilines(points2, 2, F, lines2); // ����ͼ2�е��Ӧ��ͼ1�еļ���

    Mat img1_copy = img1.clone();
    Mat img2_copy = img2.clone();

    // ���Ƽ��ߺͶ�Ӧ��
    for (size_t i = 0; i < points1.size(); i++) {
        // ����ͼ1�еļ���
        Vec3f l = lines2[i];
        line(img1_copy, Point(0, -l[2] / l[1]), Point(img1.cols, -(l[2] + l[0] * img1.cols) / l[1]), Scalar(255, 0, 0)); // ��ɫ����
        circle(img1_copy, points1[i], 5, Scalar(0, 255, 0), -1);  // ��ɫ��

        // ����ͼ2�еļ���
        l = lines1[i];
        line(img2_copy, Point(0, -l[2] / l[1]), Point(img2.cols, -(l[2] + l[0] * img2.cols) / l[1]), Scalar(255, 0, 0)); // ��ɫ����
        circle(img2_copy, points2[i], 5, Scalar(0, 255, 0), -1);  // ��ɫ��
    }

    // ��ʾ���
    imshow("Image 1 with Epilines", img1_copy);
    imshow("Image 2 with Epilines", img2_copy);
    waitKey(0);

}

// ���㱾�ʾ��󲢽������ǻ�
void ImageProcessing::triangulatePointsFromMatches(const Mat& K, const vector<Point2f>& points1,
    const vector<Point2f>& points2, const Mat& R, const Mat& t) {

    // ͶӰ����
    Mat P1 = K * Mat::eye(3, 4, CV_64F);  // ��������� [I|0]
    Mat P2 = K * (Mat_<double>(3, 4) << R.at<double>(0, 0), R.at<double>(0, 1), R.at<double>(0, 2), t.at<double>(0),
        R.at<double>(1, 0), R.at<double>(1, 1), R.at<double>(1, 2), t.at<double>(1),
        R.at<double>(2, 0), R.at<double>(2, 1), R.at<double>(2, 2), t.at<double>(2));

    // ���ǻ�
    Mat points4D;
    triangulatePoints(P1, P2, points1, points2, points4D);

    // ת��Ϊ�������
    for (int i = 0; i < points4D.cols; i++) {
        Mat col = points4D.col(i);
        col /= col.at<float>(3);  // ��һ��
        cout << "3D Point: [" << col.at<float>(0) << ", " << col.at<float>(1) << ", " << col.at<float>(2) << "]\n";
    }

}

void ImageProcessing::Run() {
    // ��ȡӰ��
    Mat img1 = imread(filename001);
    Mat img2 = imread(filename002);

    if (img1.empty() || img2.empty()) {
        cout << "δ�ҵ�Ӱ��!" << endl;
        return;
    }

    // ����ͼ��ֱ���
    img1 = resizeImage(img1);
    img2 = resizeImage(img2);

    // ����ͼ��Ԥ�����������Ҳû�����̫��
    img1 = preprocessImage(img1);
    img2 = preprocessImage(img2);

    // ����ڲξ��� (������֪)
    Mat K = (Mat_<double>(3, 3) << 1354.222062716567, 0, 204.9442952614041,
        0, 1305.36634809663, 89.65190111179771,
        0, 0, 1);

    // �������ƥ��
    vector<KeyPoint> keypoints1, keypoints2;
    Mat descriptors1, descriptors2;
    vector<DMatch> matches;

    //detectAndMatchFeaturesFB(img1, img2, keypoints1, keypoints2, matches, descriptors1, descriptors2);

    detectAndMatchFeaturesBF(img1, img2, keypoints1, keypoints2, matches, descriptors1, descriptors2);
    // �޳��ֲ�����������
    Mat F;
    vector<DMatch> inliers = filterMatchesWithFundamentalMat(keypoints1, keypoints2, matches, F);

    // ��ȡ�ڵ������
    vector<Point2f> inlierPoints1, inlierPoints2;
    for (const DMatch& match : inliers) {
        inlierPoints1.push_back(keypoints1[match.queryIdx].pt);
        inlierPoints2.push_back(keypoints2[match.trainIdx].pt);
    }

    // ����������ƥ�������
    drawMatchesWithLines(img1, img2, inlierPoints1, inlierPoints2, inliers);

    // ���Ƽ���
    drawEpilines(img1, img2, inlierPoints1, inlierPoints2, F);



    // ���ʾ������
    Mat R, t;
    Mat E = findEssentialMat(inlierPoints1, inlierPoints2, K, RANSAC);
    recoverPose(E, inlierPoints1, inlierPoints2, K, R, t);

    // ��ά��ǰ������
    triangulatePointsFromMatches(K, inlierPoints1, inlierPoints2, R, t);


}
