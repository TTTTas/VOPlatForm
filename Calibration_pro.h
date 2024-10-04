#pragma once
#ifndef CALIBRATION_PRO_H
#define CALIBRATION_PRO_H

#include "Project_Base.h"
#include "qdir.h"
#include <ChessboardCalibration.h>

// TODO
// 0. 项目文件结构未编写
// 1. 项目读取函数未编写
// 2. 功能实现接口未编写
// 3. 新建项目容错机制未编写

class Calibration_pro : public Project_Base 
{
public:
    Calibration_pro(QString path);

    // 获取标定项目名称
    QString calibrationName() const;

    // 读取标定项目的下一行信息
    QString nextCalibrationLine();

    void update(int bw, int bh, float ss, double sf, const QString& fp, const QString& op, const QString& rp);
    void save();

    // 文件路径
    QString inputfolder;
    QString outputfolder;
    QString resultpath;

    // 输出参数
    int board_width;
    int board_height;
    float square_size;
    double scale_factor;

    // 获取图片文件列表
    QStringList getImages() const {
        QDir dir(inputfolder);
        QStringList filters;
        filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp";  // 你可以根据需要添加其他图片格式
        return dir.entryList(filters, QDir::Files);
    }

    // 获取图片文件列表
    QStringList getOutImages() const {
        QDir dir(outputfolder);
        QStringList filters;
        filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp";  // 你可以根据需要添加其他图片格式
        return dir.entryList(filters, QDir::Files);
    }



private:
    // 额外的成员变量或方法可以根据需要添加
    // 例如，可以用来存储或处理特定的标定数据
};


#endif // !

