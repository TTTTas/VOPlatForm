#pragma once

#ifndef CHESSBOARD_CALIBRATION_H
#define CHESSBOARD_CALIBRATION_H

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

class ChessboardCalibration {
public:
    // ��̬�����ͳ�������
    static const int board_width;
    static const int board_height;
    static const float square_size;
    static const double scale_factor;
    static const std::string folderPath;
    static cv::Size board_size;

    // ��̬��������
    static std::vector<cv::Point3f> createKnownBoardPosition(cv::Size boardSize, float squareEdgeLength);
    static cv::Mat resizeImage(const cv::Mat& image, double scale_factor);
    static cv::Mat preprocessImage(const cv::Mat& image);
    static bool findChessboardCornersFromImage(cv::Mat& image, std::vector<cv::Point2f>& imagePoints);
    static void drawCornersWithIndex(cv::Mat& image, const std::vector<cv::Point2f>& imagePoints);
    static void calibrateAndShowResults(const std::vector<std::vector<cv::Point3f>>& worldPoints, const std::vector<std::vector<cv::Point2f>>& imagePoints);

    // �������װ�ɾ�̬����
    static void runCalibration();
};

#endif // CHESSBOARD_CALIBRATION_H
