#include "CalibrationWorker.h"
#include "ChessboardCalibration.h" // 确保在实现文件中包含完整定义

void CalibrationWorker::process() {
    ChessboardCalibration::runCalibration(this);
    emit finished();
}

void CalibrationWorker::cancel() {
    canceled = true;
}
