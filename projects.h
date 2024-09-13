#pragma once
#ifndef PROJECTS_H
#define PROJECTS_H

#include <qstring.h>
#include <QFile>
#include <QTextStream>

// 文件格式基本格式
// Pro_Name
// Pro_Type
// Pro_Data
class Project_Base {
public:
    Project_Base(QString path);
    Project_Base(QString path, QString Pro_type, QString Pro_name = "新建项目");
    ~Project_Base() 
    {
        if (file_.isOpen())
            file_.close();
    }

    // 关闭文件
    void close() {
        if (file_.isOpen()) 
        {
            file_.close();
        }
    }

    // 判断文件是否打开
    bool isOpen() const 
    {
        return file_.isOpen();
    }

    // 判断是否到达文件末尾
    bool isEof() const 
    {
        if (!isOpen())
            return true;
        return file_.atEnd();
    }

    QString next();

    QString getPro_Name() const
    {
        return Pro_Name;
    }

    int getPro_Type() const
    {
        return Pro_Type;
    }

private:
    QFile file_;
    QTextStream textStream_;
    QString Pro_Name;
    int Pro_Type;       //1. 标定项目 2. 匹配项目 3. VO项目
    QString Pro_Path;
};

#endif
