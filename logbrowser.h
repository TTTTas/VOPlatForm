#pragma once
#include <QTextBrowser>
#include <QTextCursor>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <iomanip>  // �������ø��㾫��
#include <string>

class LogBrowser : public QTextBrowser {
    Q_OBJECT

public:
    LogBrowser(QWidget* parent = nullptr) : QTextBrowser(parent) {
        setLineWrapMode(QTextBrowser::NoWrap);  // �����Զ����У������ֶ������ʽ
    }

    // ���� << ������������ QString ���
    LogBrowser& operator<<(const QString& text) {
        insertFormattedText(text);
        return *this;
    }

    LogBrowser& operator<<(const std::string& text) {
        QString qtext = QString::fromStdString(text);  // �� std::string ת��Ϊ QString
        insertFormattedText(qtext);  // �������еĲ��뷽��
        return *this;
    }

    LogBrowser& operator<<(const char* text) {
        // Ĭ��ת��Ϊ QString���������Ҫת��Ϊ std::string
        return *this << QString(text);  // ���� *this << std::string(text);
    }

    // ���� << ������������ int ���
    LogBrowser& operator<<(int value) {
        QString text = QString::number(value);
        insertFormattedText(text);
        return *this;
    }

    // ���� << ������������ float ���
    LogBrowser& operator<<(float value) {
        QString text = QString::number(value, 'f', 6);  // ���ø���������Ϊ 6 λ
        insertFormattedText(text);
        return *this;
    }

    // ���� << ������������ double ���
    LogBrowser& operator<<(double value) {
        QString text = QString::number(value, 'f', 10);  // ����˫���ȸ���������Ϊ 10 λ
        insertFormattedText(text);
        return *this;
    }

    // ���� << ������������ OpenCV Mat ���
    LogBrowser& operator<<(const cv::Mat& mat) {
        std::stringstream ss;

        // ȷ�������ǿյ�
        if (mat.empty()) {
            insertFormattedText("Empty matrix");
            return *this;
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

        QString matContent = QString::fromStdString(ss.str());
        insertFormattedText(matContent);  // �����ʽ����ľ�������
        return *this;
    }



private:
    // ����ʽ������ı����뵽 QTextBrowser ��
    void insertFormattedText(const QString& text) {
        QStringList lines = text.split('\n');  // ����������
        for (int i = 0; i < lines.size(); ++i) {
            if (i > 0) {
                append("    " + lines[i]);  // �Զ����ı���������
            }
            else {
                append(">> " + lines[i]);  // ���ǰ��� >>
            }
        }
    }
};
