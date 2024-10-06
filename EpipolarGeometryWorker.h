#pragma once
#ifndef EPIPOLARGEOMETRYWORKER
#define EPIPOLARGEOMETRYWORKER

#include <QObject>
#include <QString>
#include "opencv2/core.hpp"

class EpipolarGeometry;

class EpipolarGeometryWorker : public QObject {
    Q_OBJECT
public:
    explicit EpipolarGeometryWorker(QObject* parent = nullptr) : QObject(parent), canceled(false) {}

public slots:
    // ���߳���ִ�жԼ����δ���ĺ���
    void process();

    void process_VO();

    // ȡ������
    void cancel();

signals:
    // ������½��ȵ��ź�
    void updateProgress(int value);

    // ��־����ź�
    void logMessage(const QString& message);

    // ͼƬ���
    void showimg(cv::Mat);

    // ��������ź�
    void finished();

private:
    bool canceled;
};

#endif // EPIPOLARGEOMETRYWORKER
