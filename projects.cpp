#include "Project_Base.h"
#include <QMessageBox>

Project_Base::Project_Base()
    : file_(),  // Ĭ�Ϲ��� QFile
    textStream_(),  // Ĭ�Ϲ��� QTextStream
    Pro_Name(""),  // ��ʼ�� Pro_Name
    Pro_Type(-1)   // ��ʼ�� Pro_Type
{
    // Ĭ�Ϲ��캯�������ļ�
}

Project_Base::Project_Base(QString path) : file_(path)
{
    if (!file_.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //throw QException();  // ���Ը�����Ҫ��Ϊ��������쳣����
    }
    Pro_Path = path;
    textStream_.setDevice(&file_);
    Pro_Name = "";
    Pro_Type = -1;
    if (!textStream_.atEnd())
    {
        Pro_Name = next();
        Pro_Type = next().toInt();
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
    if (textStream_.atEnd())
    {
        return QString();
    }
    return textStream_.readLine();
}

std::string Project_Base::convertToRelative(const std::string& basePath, const std::string& absolutePath)
{
    namespace fs = std::filesystem;
    try {
        // ��ȡ basePath ��Ŀ¼���֣��� cb_test_.vopro ���ڵ��ļ���·��
        fs::path baseDir = fs::absolute(basePath).parent_path();  // ��ȡĿ¼
        fs::path absolute = fs::absolute(absolutePath);  // ȷ�� absolutePath �Ǿ���·��

        // ʹ�� std::filesystem::relative �������·��
        fs::path relativePath = fs::relative(absolute, baseDir);

        // �������·�����ַ�����ʾ
        return relativePath.string();
    }
    catch (const fs::filesystem_error& e) {
        // �����쳣�����������Ϣ
        return absolutePath;  // ���ת��ʧ�ܣ�����ԭʼ����·��
    }
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
        // ���ļ�ʧ��
        return false;
    }

    // ���� QTextStream ���ļ�
    textStream_.setDevice(&file_);
    Pro_Path = path;

    return true;
}
