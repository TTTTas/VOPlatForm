#include "VO_pro.h"

VO_pro::VO_pro(QString path)
    : Project_Base(path)
{
    // ����ĳ�ʼ���������������
}

QString VO_pro::voName() const
{
    return getPro_Name();  // �ӻ���̳е���Ŀ����
}

QString VO_pro::nextVOLine()
{
    return next();  // ���û����е� next() ����
}