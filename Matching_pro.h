#pragma once
#ifndef MATCHING_PRO_H
#define MATCHING_PRO_H
#include "Project_Base.h"
#include "qdir.h"
#include <string>

// TODO
// 0. ��Ŀ�ļ��ṹδ��д
// 1. ��Ŀ��ȡ����δ��д
// 2. ����ʵ�ֽӿ�δ��д
// 3. �½���Ŀ�ݴ����δ��д

class EpipolarGeometry_pro : public Project_Base 
{
public:
    EpipolarGeometry_pro(QString path);

    // ��ȡƥ����Ŀ����
    QString matchingName() const;

    // ��ȡƥ����Ŀ����һ����Ϣ
    QString nextMatchingLine();

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
    // ��������ض���ƥ����Ŀ�ĳ�Ա�����򷽷�
};

#endif // !
