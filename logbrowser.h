#pragma once
#include <QTextBrowser>
#include <QTextCursor>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <iomanip>  // 用于设置浮点精度
#include <string>

class LogBrowser : public QTextBrowser {
    Q_OBJECT

public:
    LogBrowser(QWidget* parent = nullptr) : QTextBrowser(parent) {
        setLineWrapMode(QTextBrowser::NoWrap);  // 禁用自动换行，便于手动管理格式
    }

    // 重载 << 操作符，用于 QString 输出
    LogBrowser& operator<<(const QString& text) {
        insertFormattedText(text);
        return *this;
    }

    LogBrowser& operator<<(const std::string& text) {
        QString qtext = QString::fromStdString(text);  // 将 std::string 转换为 QString
        insertFormattedText(qtext);  // 调用现有的插入方法
        return *this;
    }

    LogBrowser& operator<<(const char* text) {
        // 默认转换为 QString，或根据需要转换为 std::string
        return *this << QString(text);  // 或者 *this << std::string(text);
    }

    // 重载 << 操作符，用于 int 输出
    LogBrowser& operator<<(int value) {
        QString text = QString::number(value);
        insertFormattedText(text);
        return *this;
    }

    // 重载 << 操作符，用于 float 输出
    LogBrowser& operator<<(float value) {
        QString text = QString::number(value, 'f', 6);  // 设置浮点数精度为 6 位
        insertFormattedText(text);
        return *this;
    }

    // 重载 << 操作符，用于 double 输出
    LogBrowser& operator<<(double value) {
        QString text = QString::number(value, 'f', 10);  // 设置双精度浮点数精度为 10 位
        insertFormattedText(text);
        return *this;
    }

    // 重载 << 操作符，用于 OpenCV Mat 输出
    LogBrowser& operator<<(const cv::Mat& mat) {
        std::stringstream ss;

        // 确保矩阵不是空的
        if (mat.empty()) {
            insertFormattedText("Empty matrix");
            return *this;
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

        QString matContent = QString::fromStdString(ss.str());
        insertFormattedText(matContent);  // 插入格式化后的矩阵内容
        return *this;
    }



private:
    // 将格式化后的文本插入到 QTextBrowser 中
    void insertFormattedText(const QString& text) {
        QStringList lines = text.split('\n');  // 处理多行情况
        for (int i = 0; i < lines.size(); ++i) {
            if (i > 0) {
                append("    " + lines[i]);  // 对多行文本增加缩进
            }
            else {
                append(">> " + lines[i]);  // 输出前添加 >>
            }
        }
    }
};
