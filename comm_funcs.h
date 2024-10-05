#pragma once
#include <string>
#include <iomanip>
#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>

std::string Mat2string(cv::Mat mat);

std::string Point2d2string(cv::Point2d point);