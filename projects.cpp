#include "Project_Base.h"
#include <QMessageBox>

Project_Base::Project_Base()
    : file_(),  // 默认构造 QFile
    textStream_(),  // 默认构造 QTextStream
    Pro_Name(""),  // 初始化 Pro_Name
    Pro_Type(-1)   // 初始化 Pro_Type
{
    // 默认构造函数不打开文件
}

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
        return;
    }

    out_head_info();

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

void Project_Base::out_head_info()
{
    QTextStream out(&file_);
    out << Pro_Name << "\n";
    out << Pro_Type << "\n";
}

bool Project_Base::open_file(QString path) {
    if (file_.isOpen()) {
        file_.close();
    }

    file_.setFileName(path);
    if (!file_.open(QIODevice::ReadWrite | QIODevice::Text)) {
        // 打开文件失败
        return false;
    }

    // 设置 QTextStream 到文件
    textStream_.setDevice(&file_);
    Pro_Path = path;

    return true;
}
