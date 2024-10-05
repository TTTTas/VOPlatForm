#pragma once
#ifndef CALIBRATIONWORKER
#define CALIBRATIONWORKER

#include "qobject.h"
#include "opencv2/core.hpp"

class ChessboardCalibration;

class CalibrationWorker : public QObject {
    Q_OBJECT
public:
    CalibrationWorker(QObject* parent = nullptr) : QObject(parent), canceled(false) {}

public slots:
    void process();

    // ȡ������
    void cancel();

signals:
    void updateProgress(int value);  // ������½��ȵ��ź�
    void logMessage(const QString& message);  // ��־���
    void showimg(cv::Mat);
    void finished();

public:
    bool canceled;
};

#endif // CALIBRATIONWORKER
