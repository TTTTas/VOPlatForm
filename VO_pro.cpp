#include "VO_pro.h"

VO_pro::VO_pro(QString path)
    : Project_Base(path)
{
    // 额外的初始化可以在这里完成
    QFileInfo fileInfo(getPro_Path());   // 获取完整路径的文件信息
    QString parentDir = fileInfo.absolutePath();  // 获取父目录

    // 合并父目录和 temp_line 生成绝对路径
    QString temp_line = nextVOLine();
    inputfolder = parentDir + temp_line;

    temp_line = nextVOLine();
    outputfolder = parentDir + temp_line;

    temp_line = nextVOLine();
    resultpath = parentDir + "/" + temp_line;

}

QString VO_pro::voName() const
{
    return getPro_Name();  // 从基类继承的项目名称
}

QString VO_pro::nextVOLine()
{
    return next();  // 调用基类中的 next() 方法
}

void VO_pro::update(QString path1, QString path2, QString path3)
{
    inputfolder = path1;
    outputfolder = path2;
    resultpath = path3;
}

void VO_pro::save()
{
    QFile file(getPro_Path());  // 打开项目文件路径

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        //qWarning() << "无法打开文件进行写入: " << getPro_Path();
        return;
    }

    QTextStream out(&file);

    // 将项目名称和项目类型写入文件
    out << getPro_Name() << "\n";
    out << QString::number(getPro_Type()) << "\n";

    // 写入输入和输出文件夹路径（相对路径）
    out << "/" + QString::fromStdString(convertToRelative(getPro_Path().toStdString(), inputfolder.toStdString())) << "\n";
    out << "/" + QString::fromStdString(convertToRelative(getPro_Path().toStdString(), outputfolder.toStdString())) << "\n";
    // 写入结果文件路径
    out << QString::fromStdString(convertToRelative(getPro_Path().toStdString(), resultpath.toStdString())) << "\n";
    // 完成文件写入后关闭文件
    file.close();
}
