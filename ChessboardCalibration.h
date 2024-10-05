#pragma once

#ifndef CHESSBOARD_CALIBRATION_H
#define CHESSBOARD_CALIBRATION_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <logbrowser.h>
#include <qprogressdialog.h>
#include "CalibrationWorker.h"

class ChessboardCalibration {
public:
    // 静态变量和常量定义
    static int board_width;
    static int board_height;
    static float square_size;
    static double scale_factor;
    static std::string folderPath;
    static cv::Size board_size;
    static CalibrationWorker* worker;
    // 文件输出
    static std::string outPath;
    static std::string resultPath;

    // 声明静态初始化函数
    static void init_Calibration(int bw, int bh, float ss, double sf, const std::string& fp, const std::string& op, const std::string& rp);
    // 静态方法声明
    static std::vector<cv::Point3f> createKnownBoardPosition(cv::Size boardSize, float squareEdgeLength);
    static cv::Mat resizeImage(const cv::Mat& image, double scale_factor);
    static cv::Mat preprocessImage(const cv::Mat& image);
    static bool findChessboardCornersFromImage(cv::Mat& image, std::vector<cv::Point2f>& imagePoints);
    static void drawCornersWithIndex(cv::Mat& image, const std::vector<cv::Point2f>& imagePoints);
    static void calibrateAndShowResults(cv::Mat& image, const std::vector<std::vector<cv::Point3f>>& worldPoints, const std::vector<std::vector<cv::Point2f>>& imagePoints);

    // 主程序封装成静态方法
    static void runCalibration(CalibrationWorker* worker);
};
#endif // CHESSBOARD_CALIBRATION_H
