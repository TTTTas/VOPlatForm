#include "Matching_pro.h"


Matching_pro::Matching_pro(QString path)
    : Project_Base(path)
{
    // 额外的初始化可以在这里完成
}

QString Matching_pro::matchingName() const
{
    return getPro_Name();  // 从基类继承的项目名称
}

QString Matching_pro::nextMatchingLine()
{
    return next();  // 调用基类中的 next() 方法
}