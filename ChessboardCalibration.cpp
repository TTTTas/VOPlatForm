#include "ChessboardCalibration.h"
#include "qstring.h"
#include "qfileinfo.h"
#include "comm_funcs.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>
#include <fstream>

using namespace cv;
using namespace std;

// 初始化静态成员变量
int ChessboardCalibration::board_width = 5;
int ChessboardCalibration::board_height = 8;
float ChessboardCalibration::square_size = 27.0f;
double ChessboardCalibration::scale_factor = 0.25;
std::string ChessboardCalibration::folderPath = "";
cv::Size ChessboardCalibration::board_size(board_width, board_height);
std::string ChessboardCalibration::outPath = "";
std::string ChessboardCalibration::resultPath = "";
CalibrationWorker* ChessboardCalibration::worker = nullptr;

void ChessboardCalibration::init_Calibration(int bw, int bh, float ss, double sf, const std::string& fp, const std::string& op, const std::string& rp)
{
    board_width = bw;
    board_height = bh;
    square_size = ss;
    scale_factor = sf;
    folderPath = fp;
    outPath = op;
    resultPath = rp;
    board_size = cv::Size(board_width, board_height);
}

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
        circle(image, imagePoints[i], 5, Scalar(139, 34, 34), 4);
        putText(image, to_string(i), imagePoints[i] + Point2f(5, 5), FONT_HERSHEY_SIMPLEX, 3, Scalar(144, 30, 255), 3);
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
        QString ss;
        ss = "找到的角点数量: " + QString::number(imagePoints.size()) + "\n";
        emit worker->logMessage(ss);
         
    }

    return found;
}

// 执行相机标定并显示结果
void ChessboardCalibration::calibrateAndShowResults(cv::Mat& image, const vector<vector<Point3f>>& worldPoints, const vector<vector<Point2f>>& imagePoints) {
    ofstream output;
    output.open(resultPath, ios::out | ios::trunc);
    if (imagePoints.size() > 0) {
        Mat cameraMatrix, distCoeffs;
        vector<Mat> rvecs, tvecs;
        calibrateCamera(worldPoints, imagePoints, cv::Size(image.rows, image.cols), cameraMatrix, distCoeffs, rvecs, tvecs);

        // 将字符串流的内容转换为 QString
        QString message = "Camera Matrix: \n" + QString::fromStdString(Mat2string(cameraMatrix)) + "\n";
        emit worker->logMessage(message);
        output << "Camera Matrix: \n" << cameraMatrix << "\n";

        message = "Distortion Coefficients: \n" + QString::fromStdString(Mat2string(distCoeffs)) + "\n";
        emit worker->logMessage(message);
        output<< "Distortion Coefficients: \n" << distCoeffs << "\n";
    }
    else {
        emit worker->logMessage("未找到足够的角点进行标定！\n");
        output << "未找到足够的角点进行标定！\n";
    }
    output.close();
}

// 相机标定-主程序封装函数
void ChessboardCalibration::runCalibration(CalibrationWorker* w) {
    using namespace std;
    using namespace cv;
    namespace fs = std::filesystem;
    worker = w;

    vector<vector<Point3f>> worldPoints;
    vector<vector<Point2f>> imagePoints;
    vector<Point2f> imageCorners;
    vector<Point3f> worldCorners = createKnownBoardPosition(board_size, square_size);

    if (!fs::exists(folderPath)) {
        QString message;
        message = "指定的文件夹不存在: " + QString::fromStdString(folderPath) + "\n";
        emit worker->logMessage(message);
        return;
    }

    int processedImages = 0;

    Mat image;
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.path().extension() == ".JPG" || entry.path().extension() == ".jpg"
            || entry.path().extension() == ".JPEG" || entry.path().extension() == ".jpeg"
            || entry.path().extension() == ".png" || entry.path().extension() == ".PNG") {
            string filename = entry.path().string();
            image = imread(filename);

            if (image.empty()) {
                QString message = "无法读取图片: " + QString::fromStdString(filename) + "\n";
                continue;
            }

            QFileInfo fileinfo(QString::fromStdString(filename));
            QString message = "开始处理图片 " + fileinfo.fileName() + "\n";
            emit worker->logMessage(message);

            Mat preprocessed_image = preprocessImage(image);

            if (findChessboardCornersFromImage(preprocessed_image, imageCorners)) {
                imagePoints.push_back(imageCorners);
                worldPoints.push_back(worldCorners);
            }

            QStringList file_names = fileinfo.fileName().split(".");
            emit worker->showimg(preprocessed_image);
            imwrite(outPath + "/" + file_names[0].toStdString() + "_processed.jpg", preprocessed_image);

            // 发射进度更新信号
            ++processedImages;
            emit worker->updateProgress(processedImages);

            if (worker->canceled) {
                emit worker->logMessage("操作被用户取消.\n");
                break;
            }

            //waitKey(500);
        }
    }

    emit worker->logMessage("图片处理完成.\n");
    emit worker->logMessage("开始计算相机参数\n");
    calibrateAndShowResults(image, worldPoints, imagePoints);
}
