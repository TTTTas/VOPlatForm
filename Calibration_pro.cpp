#include "Calibration_pro.h"

Calibration_pro::Calibration_pro(QString path)
    : Project_Base(path)
{
    // �ڹ��캯���п��Խ��ж���ĳ�ʼ��
    // �����Ҫ��ȡ������Ϣ��������������Ӵ���
}

QString Calibration_pro::calibrationName() const
{
    return getPro_Name();  // �ӻ���̳е���Ŀ����
}

QString Calibration_pro::nextCalibrationLine()
{
    return next();  // ���û����е� next() ����
}