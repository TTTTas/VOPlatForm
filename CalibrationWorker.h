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

    // 取消操作
    void cancel();

signals:
    void updateProgress(int value);  // 发射更新进度的信号
    void logMessage(const QString& message);  // 日志输出
    void showimg(cv::Mat);
    void finished();

public:
    bool canceled;
};

#endif // CALIBRATIONWORKER
