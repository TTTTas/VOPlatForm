#include "VO_pro.h"

VO_pro::VO_pro(QString path)
    : Project_Base(path)
{
    // ����ĳ�ʼ���������������
    QFileInfo fileInfo(getPro_Path());   // ��ȡ����·�����ļ���Ϣ
    QString parentDir = fileInfo.absolutePath();  // ��ȡ��Ŀ¼

    // �ϲ���Ŀ¼�� temp_line ���ɾ���·��
    QString temp_line = nextVOLine();
    inputfolder = parentDir + temp_line;

    temp_line = nextVOLine();
    outputfolder = parentDir + temp_line;

    temp_line = nextVOLine();
    resultpath = parentDir + "/" + temp_line;

}

QString VO_pro::voName() const
{
    return getPro_Name();  // �ӻ���̳е���Ŀ����
}

QString VO_pro::nextVOLine()
{
    return next();  // ���û����е� next() ����
}

void VO_pro::update(QString path1, QString path2, QString path3)
{
    inputfolder = path1;
    outputfolder = path2;
    resultpath = path3;
}

void VO_pro::save()
{
    QFile file(getPro_Path());  // ����Ŀ�ļ�·��

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        //qWarning() << "�޷����ļ�����д��: " << getPro_Path();
        return;
    }

    QTextStream out(&file);

    // ����Ŀ���ƺ���Ŀ����д���ļ�
    out << getPro_Name() << "\n";
    out << QString::number(getPro_Type()) << "\n";

    // д�����������ļ���·�������·����
    out << "/" + QString::fromStdString(convertToRelative(getPro_Path().toStdString(), inputfolder.toStdString())) << "\n";
    out << "/" + QString::fromStdString(convertToRelative(getPro_Path().toStdString(), outputfolder.toStdString())) << "\n";
    // д�����ļ�·��
    out << QString::fromStdString(convertToRelative(getPro_Path().toStdString(), resultpath.toStdString())) << "\n";
    // ����ļ�д���ر��ļ�
    file.close();
}
