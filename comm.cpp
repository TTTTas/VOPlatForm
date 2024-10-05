#include "comm_funcs.h"

std::string Mat2string(cv::Mat mat)
{
    std::stringstream ss;

    // 确保矩阵不是空的
    if (mat.empty()) {
        ss << "Empty Matrix\n";
        return ss.str();
    }

    // 设置格式化：小数点后4位，固定宽度，列对齐
    ss << std::fixed << std::setprecision(4);

    // 计算列宽，确保对齐
    int columnWidth = 15;

    // 根据矩阵类型输出具体的数值内容
    for (int i = 0; i < mat.rows; ++i) {
        for (int j = 0; j < mat.cols; ++j) {
            // 处理不同类型的矩阵数据
            switch (mat.type()) {
            case CV_8U:
                ss << std::setw(columnWidth) << static_cast<int>(mat.at<uchar>(i, j)) << " ";  // 8位无符号整数
                break;
            case CV_32F:
                ss << std::setw(columnWidth) << mat.at<float>(i, j) << " ";  // 32位浮点数
                break;
            case CV_64F:
                ss << std::setw(columnWidth) << mat.at<double>(i, j) << " ";  // 64位浮点数
                break;
                // 可以扩展处理其他数据类型
            default:
                ss << std::setw(columnWidth) << "Unknown" << " ";
                break;
            }
        }
        ss << "\n";  // 换行，处理下一行的输出
    }
    return ss.str();
}

std::string Point2d2string(cv::Point2d point)
{
    std::ostringstream oss;
    oss << "(" << point.x << ", " << point.y << ")";
    return oss.str();
}