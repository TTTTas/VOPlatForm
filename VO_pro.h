#pragma once
#ifndef VO_PRO_H
#define VO_PRO_H
#include "Project_Base.h"

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

private:
    // ��������ض���VO��Ŀ�ĳ�Ա�����򷽷�
};

#endif // !
