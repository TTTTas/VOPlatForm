#include "Matching_pro.h"


Matching_pro::Matching_pro(QString path)
    : Project_Base(path)
{
    // ����ĳ�ʼ���������������
}

QString Matching_pro::matchingName() const
{
    return getPro_Name();  // �ӻ���̳е���Ŀ����
}

QString Matching_pro::nextMatchingLine()
{
    return next();  // ���û����е� next() ����
}