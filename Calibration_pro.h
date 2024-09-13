#pragma once
#ifndef CALIBRATION_PRO_H
#define CALIBRATION_PRO_H
#include "projects.h"

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

private:
    // 额外的成员变量或方法可以根据需要添加
    // 例如，可以用来存储或处理特定的标定数据
};

#endif // !

