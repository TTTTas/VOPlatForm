#pragma once
#ifndef VO_PRO_H
#define VO_PRO_H
#include "Project_Base.h"
#include "qfileinfo.h"
#include "qdir.h"

// TODO
// 0. ��Ŀ�ļ��ṹδ��д
// 1. ��Ŀ��ȡ����δ��д
// 2. ����ʵ�ֽӿ�δ��д
// 3. �½���Ŀ�ݴ����δ��д

class VO_pro : public Project_Base 
{
public:
    VO_pro(QString path);

    // ��ȡVO��Ŀ����
    QString voName() const;

    // ��ȡVO��Ŀ����һ����Ϣ
    QString nextVOLine();

    void update(QString path1, QString path2, QString path3);

    void save();

    // ��ȡͼƬ�ļ��б�
    QStringList getImages() const {
        QDir dir(inputfolder);
        QStringList filters;
        filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp";  // ����Ը�����Ҫ�������ͼƬ��ʽ
        return dir.entryList(filters, QDir::Files);
    }

    // ��ȡͼƬ�ļ��б�
    QStringList getOutImages() const {
        QDir dir(outputfolder);
        QStringList filters;
        filters << "*.png" << "*.jpg" << "*.jpeg" << "*.bmp";  // ����Ը�����Ҫ�������ͼƬ��ʽ
        return dir.entryList(filters, QDir::Files);
    }

    QString inputfolder;
    QString outputfolder;
    QString resultpath;

private:
    // ��������ض���VO��Ŀ�ĳ�Ա�����򷽷�
};

#endif // !
