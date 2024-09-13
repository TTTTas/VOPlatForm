#include "projects.h"

Project_Base::Project_Base(QString path) : file_(path)
{
    if (!file_.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //throw QException();  // 可以根据需要改为更具体的异常处理
    }
    Pro_Path = path;
    textStream_.setDevice(&file_);
    Pro_Name = "";
    Pro_Type = -1;
    if (!textStream_.atEnd())
    {
        Pro_Name = textStream_.readLine();
        Pro_Type = textStream_.readLine().toInt();
    }
}

Project_Base::Project_Base(QString path, QString Pro_type, QString Pro_name) : file_(path), Pro_Name(Pro_name), Pro_Type(Pro_type.toInt())
{
    if (!file_.open(QIODevice::WriteOnly | QIODevice::Text)) {
        //qWarning() << "Unable to open file for writing:" << file_.fileName();
        return;
    }

    QTextStream out(&file_);
    out << Pro_Name << "\n";
    out << Pro_Type << "\n";

    file_.close();
}

QString Project_Base::next()
{
    if (file_.atEnd())
    {
        return QString();
    }
    return textStream_.readLine();
}