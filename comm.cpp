#include "comm_funcs.h"

std::string Mat2string(cv::Mat mat)
{
    std::stringstream ss;

    // ȷ�������ǿյ�
    if (mat.empty()) {
        ss << "Empty Matrix\n";
        return ss.str();
    }

    // ���ø�ʽ����С�����4λ���̶���ȣ��ж���
    ss << std::fixed << std::setprecision(4);

    // �����п�ȷ������
    int columnWidth = 15;

    // ���ݾ�����������������ֵ����
    for (int i = 0; i < mat.rows; ++i) {
        for (int j = 0; j < mat.cols; ++j) {
            // ����ͬ���͵ľ�������
            switch (mat.type()) {
            case CV_8U:
                ss << std::setw(columnWidth) << static_cast<int>(mat.at<uchar>(i, j)) << " ";  // 8λ�޷�������
                break;
            case CV_32F:
                ss << std::setw(columnWidth) << mat.at<float>(i, j) << " ";  // 32λ������
                break;
            case CV_64F:
                ss << std::setw(columnWidth) << mat.at<double>(i, j) << " ";  // 64λ������
                break;
                // ������չ����������������
            default:
                ss << std::setw(columnWidth) << "Unknown" << " ";
                break;
            }
        }
        ss << "\n";  // ���У�������һ�е����
    }
    return ss.str();
}

std::string Point2d2string(cv::Point2d point)
{
    std::ostringstream oss;
    oss << "(" << point.x << ", " << point.y << ")";
    return oss.str();
}