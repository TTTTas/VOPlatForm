#include "VO_pro.h"

VO_pro::VO_pro(QString path)
    : Project_Base(path)
{
    // 额外的初始化可以在这里完成
}

QString VO_pro::voName() const
{
    return getPro_Name();  // 从基类继承的项目名称
}

QString VO_pro::nextVOLine()
{
    return next();  // 调用基类中的 next() 方法
}