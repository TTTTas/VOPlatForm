#include "ChessboardCalibration.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>

using namespace cv;
using namespace std;

// ���徲̬��Ա�����������³�����Ҫ��Ϊ������Ĳ���
const int ChessboardCalibration::board_width = 5;
const int ChessboardCalibration::board_height = 8;
const float ChessboardCalibration::square_size = 27.0f;
const double ChessboardCalibration::scale_factor = 0.25f;
const string ChessboardCalibration::folderPath = "./ChessboardPicture";

Size ChessboardCalibration::board_size(board_width, board_height);

// �������̸����������
vector<Point3f> ChessboardCalibration::createKnownBoardPosition(Size boardSize, float squareEdgeLength) {
    vector<Point3f> corners;
    for (int i = 0; i < boardSize.height; i++) {
        for (int j = 0; j < boardSize.width; j++) {
            corners.push_back(Point3f(j * squareEdgeLength, i * squareEdgeLength, 0.0f));
        }
    }
    return corners;
}

// ����ͼ��ֱ���
Mat ChessboardCalibration::resizeImage(const Mat& image, double scale_factor) {
    Mat resized;
    resize(image, resized, Size(), scale_factor, scale_factor, INTER_LINEAR);
    return resized;
}

// ͼ��Ԥ����
Mat ChessboardCalibration::preprocessImage(const Mat& image) {
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

// ���ƽǵ�����
void ChessboardCalibration::drawCornersWithIndex(Mat& image, const vector<Point2f>& imagePoints) {
    for (size_t i = 0; i < imagePoints.size(); i++) {
        circle(image, imagePoints[i], 5, Scalar(0, 0, 255), 2);
        putText(image, to_string(i), imagePoints[i] + Point2f(5, 5), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0), 1);
    }
}

// ������̸�ǵ�
bool ChessboardCalibration::findChessboardCornersFromImage(Mat& image, vector<Point2f>& imagePoints) {
    Mat gray;
    if (image.channels() == 1) {
        gray = image;
    }
    else {
        cvtColor(image, gray, COLOR_BGR2GRAY);
    }

    bool found = findChessboardCorners(image, board_size, imagePoints, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE);

    if (found) {
        cornerSubPix(gray, imagePoints, Size(11, 11), Size(-1, -1), TermCriteria(TermCriteria::EPS + TermCriteria::COUNT, 30, 0.001));
        drawCornersWithIndex(image, imagePoints);
        cout << "�ҵ��Ľǵ�����: " << imagePoints.size() << endl;
    }

    return found;
}

// ִ������궨����ʾ���
void ChessboardCalibration::calibrateAndShowResults(const vector<vector<Point3f>>& worldPoints, const vector<vector<Point2f>>& imagePoints) {
    if (imagePoints.size() > 0) {
        Mat cameraMatrix, distCoeffs;
        vector<Mat> rvecs, tvecs;
        calibrateCamera(worldPoints, imagePoints, board_size, cameraMatrix, distCoeffs, rvecs, tvecs);

        cout << "Camera Matrix: " << cameraMatrix << endl;
        cout << "Distortion Coefficients: " << distCoeffs << endl;
    }
    else {
        cout << "δ�ҵ��㹻�Ľǵ���б궨��" << endl;
    }
}

// ����궨-�������װ����
void ChessboardCalibration::runCalibration() {
    using namespace std;
    using namespace cv;
    namespace fs = std::filesystem;  // ����ʹ�ñ���

    vector<vector<Point3f>> worldPoints;
    vector<vector<Point2f>> imagePoints;
    vector<Point2f> imageCorners;
    vector<Point3f> worldCorners = createKnownBoardPosition(board_size, square_size);

    // ���·���Ƿ����
    if (!std::filesystem::exists(folderPath)) {
        std::cout << "ָ�����ļ��в�����: " << folderPath << std::endl;
        return;
    }

    // ����ָ���ļ����ڵ����� jpg �ļ�
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        // ֻ���� .jpg �ļ�
        if (entry.path().extension() == ".JPG"|| entry.path().extension() == ".jpg"
            || entry.path().extension() == ".JPEG" || entry.path().extension() == ".jpeg"
            || entry.path().extension() == ".png" || entry.path().extension() == ".PNG")
        {
            string filename = entry.path().string();
            Mat image = imread(filename);

            if (image.empty()) {
                cout << "�޷���ȡͼƬ: " << filename << endl;
                continue;
            }

            Mat resized_image = resizeImage(image, scale_factor);
            Mat preprocessed_image = preprocessImage(resized_image);

            if (findChessboardCornersFromImage(preprocessed_image, imageCorners)) {
                imagePoints.push_back(imageCorners);
                worldPoints.push_back(worldCorners);
            }

            imshow("Preprocessed Chessboard Corners with Index", preprocessed_image);
            waitKey(500);
        }
    }

    destroyAllWindows();

    // ���ñ궨����
    calibrateAndShowResults(worldPoints, imagePoints);
}