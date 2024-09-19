#pragma once
#ifndef MATCHING_PRO_H
#define MATCHING_PRO_H
#include "Project_Base.h"

// TODO
// 0. 项目文件结构未编写
// 1. 项目读取函数未编写
// 2. 功能实现接口未编写
// 3. 新建项目容错机制未编写

class Matching_pro : public Project_Base 
{
public:
    Matching_pro(QString path);

    // 获取匹配项目名称
    QString matchingName() const;

    // 读取匹配项目的下一行信息
    QString nextMatchingLine();

private:
    // 可以添加特定于匹配项目的成员变量或方法
};

#endif // !
