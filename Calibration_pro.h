#pragma once
#ifndef CALIBRATION_PRO_H
#define CALIBRATION_PRO_H

#include "Project_Base.h"
#include "qdir.h"
#include <ChessboardCalibration.h>

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

    void update(int bw, int bh, float ss, double sf, const QString& fp, const QString& op, const QString& rp);
    void save();

    // �ļ�·��
    QString inputfolder;
    QString outputfolder;
    QString resultpath;

    // �������
    int board_width;
    int board_height;
    float square_size;
    double scale_factor;

    // ��ȡͼƬ�ļ��б�
    QStringList getImages() const {
        QDir dir(inputfolder);
        QStringList filters;
        filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp";  // ����Ը�����Ҫ�������ͼƬ��ʽ
        return dir.entryList(filters, QDir::Files);
    }

    // ��ȡͼƬ�ļ��б�
    QStringList getOutImages() const {
        QDir dir(outputfolder);
        QStringList filters;
        filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp";  // ����Ը�����Ҫ�������ͼƬ��ʽ
        return dir.entryList(filters, QDir::Files);
    }



private:
    // ����ĳ�Ա�����򷽷����Ը�����Ҫ���
    // ���磬���������洢�����ض��ı궨����
};


#endif // !

