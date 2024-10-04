#include "CalibrationWorker.h"
#include "ChessboardCalibration.h" // ȷ����ʵ���ļ��а�����������

void CalibrationWorker::process() {
    ChessboardCalibration::runCalibration(this);
    emit finished();
}

void CalibrationWorker::cancel() {
    canceled = true;
}
