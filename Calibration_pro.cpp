#include "Calibration_pro.h"

Calibration_pro::Calibration_pro(QString path)
    : Project_Base(path)
{
    // 在构造函数中可以进行额外的初始化
    // 如果需要读取更多信息，可以在这里添加代码
    QFileInfo fileInfo(getPro_Path());   // 获取完整路径的文件信息
    QString parentDir = fileInfo.absolutePath();  // 获取父目录

    // 合并父目录和 temp_line 生成绝对路径
    QString temp_line = nextCalibrationLine();
    inputfolder = parentDir + temp_line;

    temp_line = nextCalibrationLine();
    outputfolder = parentDir + temp_line;

    temp_line = nextCalibrationLine();
    resultpath = parentDir + "/" + temp_line;

    temp_line = nextCalibrationLine();
    board_width = temp_line.toInt();

    temp_line = nextCalibrationLine();
    board_height = temp_line.toInt();

    temp_line = nextCalibrationLine();
    square_size = temp_line.toFloat();

    temp_line = nextCalibrationLine();
    scale_factor = temp_line.toDouble();
}

QString Calibration_pro::calibrationName() const
{
    return getPro_Name();  // 从基类继承的项目名称
}

QString Calibration_pro::nextCalibrationLine()
{
    return next();  // 调用基类中的 next() 方法
}

void Calibration_pro::update(int bw, int bh, float ss, double sf, const QString& fp, const QString& op, const QString& rp)
{
    namespace fs = std::filesystem;
    board_width = bw;
    board_height = bh;
    square_size = ss;
    scale_factor = sf;
    inputfolder = fp;
    outputfolder = op;
    resultpath = rp;
}

void Calibration_pro::save()
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

    // 写入 board_width、board_height、square_size、scale_factor
    out << QString::number(board_width) << "\n";
    out << QString::number(board_height) << "\n";
    out << QString::number(square_size) << "\n";
    out << QString::number(scale_factor) << "\n";

    // 完成文件写入后关闭文件
    file.close();
}
