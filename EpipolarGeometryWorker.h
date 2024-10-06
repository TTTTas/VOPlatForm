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
    // 子线程中执行对极几何处理的函数
    void process();

    void process_VO();

    // 取消操作
    void cancel();

signals:
    // 发射更新进度的信号
    void updateProgress(int value);

    // 日志输出信号
    void logMessage(const QString& message);

    // 图片输出
    void showimg(cv::Mat);

    // 任务完成信号
    void finished();

private:
    bool canceled;
};

#endif // EPIPOLARGEOMETRYWORKER
