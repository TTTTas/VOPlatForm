#pragma once
#ifndef CALIBRATION_PRO_H
#define CALIBRATION_PRO_H
#include "projects.h"

// TODO
// 0. ��Ŀ�ļ��ṹδ��д
// 1. ��Ŀ��ȡ����δ��д
// 2. ����ʵ�ֽӿ�δ��д
// 3. �½���Ŀ�ݴ����δ��д

class Calibration_pro : public Project_Base 
{
public:
    Calibration_pro(QString path);

    // ��ȡ�궨��Ŀ����
    QString calibrationName() const;

    // ��ȡ�궨��Ŀ����һ����Ϣ
    QString nextCalibrationLine();

private:
    // ����ĳ�Ա�����򷽷����Ը�����Ҫ���
    // ���磬���������洢�����ض��ı궨����
};

#endif // !

