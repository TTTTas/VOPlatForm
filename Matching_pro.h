#pragma once
#ifndef MATCHING_PRO_H
#define MATCHING_PRO_H
#include "Project_Base.h"

// TODO
// 0. ��Ŀ�ļ��ṹδ��д
// 1. ��Ŀ��ȡ����δ��д
// 2. ����ʵ�ֽӿ�δ��д
// 3. �½���Ŀ�ݴ����δ��д

class Matching_pro : public Project_Base 
{
public:
    Matching_pro(QString path);

    // ��ȡƥ����Ŀ����
    QString matchingName() const;

    // ��ȡƥ����Ŀ����һ����Ϣ
    QString nextMatchingLine();

private:
    // ��������ض���ƥ����Ŀ�ĳ�Ա�����򷽷�
};

#endif // !
