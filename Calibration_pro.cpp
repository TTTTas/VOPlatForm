#include "Calibration_pro.h"

Calibration_pro::Calibration_pro(QString path)
    : Project_Base(path)
{
    // 在构造函数中可以进行额外的初始化
    // 如果需要读取更多信息，可以在这里添加代码
}

QString Calibration_pro::calibrationName() const
{
    return getPro_Name();  // 从基类继承的项目名称
}

QString Calibration_pro::nextCalibrationLine()
{
    return next();  // 调用基类中的 next() 方法
}