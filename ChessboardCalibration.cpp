#include "ChessboardCalibration.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>

using namespace cv;
using namespace std;

// 定义静态成员变量——以下常量需要改为输入类的参数
const int ChessboardCalibration::board_width = 5;
const int ChessboardCalibration::board_height = 8;
const float ChessboardCalibration::square_size = 27.0f;
const double ChessboardCalibration::scale_factor = 0.25f;
const string ChessboardCalibration::folderPath = "./ChessboardPicture";

Size ChessboardCalibration::board_size(board_width, board_height);

// 生成棋盘格的世界坐标
vector<Point3f> ChessboardCalibration::createKnownBoardPosition(Size boardSize, float squareEdgeLength) {
    vector<Point3f> corners;
    for (int i = 0; i < boardSize.height; i++) {
        for (int j = 0; j < boardSize.width; j++) {
            corners.push_back(Point3f(j * squareEdgeLength, i * squareEdgeLength, 0.0f));
        }
    }
    return corners;
}

// 降低图像分辨率
Mat ChessboardCalibration::resizeImage(const Mat& image, double scale_factor) {
    Mat resized;
    resize(image, resized, Size(), scale_factor, scale_factor, INTER_LINEAR);
    return resized;
}

// 图像预处理
Mat ChessboardCalibration::preprocessImage(const Mat& image) {
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

// 绘制角点和序号
void ChessboardCalibration::drawCornersWithIndex(Mat& image, const vector<Point2f>& imagePoints) {
    for (size_t i = 0; i < imagePoints.size(); i++) {
        circle(image, imagePoints[i], 5, Scalar(0, 0, 255), 2);
        putText(image, to_string(i), imagePoints[i] + Point2f(5, 5), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0), 1);
    }
}

// 检测棋盘格角点
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
        cout << "找到的角点数量: " << imagePoints.size() << endl;
    }

    return found;
}

// 执行相机标定并显示结果
void ChessboardCalibration::calibrateAndShowResults(cv::Mat& image, const vector<vector<Point3f>>& worldPoints, const vector<vector<Point2f>>& imagePoints) {
    if (imagePoints.size() > 0) {
        Mat cameraMatrix, distCoeffs;
        vector<Mat> rvecs, tvecs;
        calibrateCamera(worldPoints, imagePoints, cv::Size(image.rows, image.cols), cameraMatrix, distCoeffs, rvecs, tvecs);

        cout << "Camera Matrix: " << cameraMatrix << endl;
        cout << "Distortion Coefficients: " << distCoeffs << endl;
    }
    else {
        cout << "未找到足够的角点进行标定！" << endl;
    }
}

// 相机标定-主程序封装函数
void ChessboardCalibration::runCalibration() {
    using namespace std;
    using namespace cv;
    namespace fs = std::filesystem;  // 方便使用别名

    vector<vector<Point3f>> worldPoints;
    vector<vector<Point2f>> imagePoints;
    vector<Point2f> imageCorners;
    vector<Point3f> worldCorners = createKnownBoardPosition(board_size, square_size);

    // 检查路径是否存在
    if (!std::filesystem::exists(folderPath)) {
        std::cout << "指定的文件夹不存在: " << folderPath << std::endl;
        return;
    }

    // 遍历指定文件夹内的所有 jpg 文件
    Mat image;
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        // 只处理 .jpg 文件
        if (entry.path().extension() == ".JPG"|| entry.path().extension() == ".jpg"
            || entry.path().extension() == ".JPEG" || entry.path().extension() == ".jpeg"
            || entry.path().extension() == ".png" || entry.path().extension() == ".PNG")
        {
            string filename = entry.path().string();
            image = imread(filename);

            if (image.empty()) {
                cout << "无法读取图片: " << filename << endl;
                continue;
            }

            //不缩小分辨率重置大小
            //Mat resized_image = resizeImage(image, scale_factor);
            //Mat preprocessed_image = preprocessImage(resized_image);

            Mat preprocessed_image = preprocessImage(image);

            if (findChessboardCornersFromImage(preprocessed_image, imageCorners)) {
                imagePoints.push_back(imageCorners);
                worldPoints.push_back(worldCorners);
            }

            imshow("Preprocessed Chessboard Corners with Index", preprocessed_image);
            waitKey(500);
        }
    }

    destroyAllWindows();

    // 调用标定函数
    calibrateAndShowResults(image, worldPoints, imagePoints);
}