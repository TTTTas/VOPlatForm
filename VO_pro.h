#pragma once
#ifndef VO_PRO_H
#define VO_PRO_H
#include "Project_Base.h"

// TODO
// 0. 项目文件结构未编写
// 1. 项目读取函数未编写
// 2. 功能实现接口未编写
// 3. 新建项目容错机制未编写

class VO_pro : public Project_Base 
{
public:
    VO_pro(QString path);

    // 获取VO项目名称
    QString voName() const;

    // 读取VO项目的下一行信息
    QString nextVOLine();

private:
    // 可以添加特定于VO项目的成员变量或方法
};

#endif // !
