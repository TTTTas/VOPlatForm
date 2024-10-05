#pragma once
#ifndef PROJECT_BASE_H
#define PROJECT_BASE_H

#include <qstring.h>
#include <QFile>
#include <QTextStream>

// �ļ���ʽ������ʽ
// Pro_Name
// Pro_Type
// Pro_Data
class Project_Base {
public:
    Project_Base();
    Project_Base(QString path);
    Project_Base(QString path, QString Pro_type, QString Pro_name = "�½���Ŀ");
    ~Project_Base() 
    {
        if (file_.isOpen())
            file_.close();
    }

    // �ر��ļ�
    void close() {
        if (file_.isOpen()) 
        {
            file_.close();
        }
    }

    // �ж��ļ��Ƿ��
    bool isOpen() const 
    {
        return file_.isOpen();
    }

    // �ж��Ƿ񵽴��ļ�ĩβ
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

    QString getPro_Path() const
    {
        return Pro_Path;
    }
    
    int getPro_Type() const
    {
        return Pro_Type;
    }

    void setProName(const QString& name) { Pro_Name = name; }
    void setProPath(const QString& path) { Pro_Path = path; }
    void setProType(int type) { Pro_Type = type; }
    std::string convertToRelative(const std::string& basePath, const std::string& absolutePath);

    void out_head_info();
    bool open_file(QString path);
private:
    QFile file_;
    QTextStream textStream_;
    QString Pro_Name;
    int Pro_Type;       //1. �궨��Ŀ 2. ƥ����Ŀ 3. VO��Ŀ
    QString Pro_Path;
};

#endif
