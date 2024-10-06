#pragma once
#ifndef VO_PRO_H
#define VO_PRO_H
#include "Project_Base.h"
#include "qfileinfo.h"
#include "qdir.h"

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

    void update(QString path1, QString path2, QString path3);

    void save();

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

    QString inputfolder;
    QString outputfolder;
    QString resultpath;

private:
    // 可以添加特定于VO项目的成员变量或方法
};

#endif // !
