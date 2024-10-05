#include "Calibration_pro.h"

Calibration_pro::Calibration_pro(QString path)
    : Project_Base(path)
{
    // �ڹ��캯���п��Խ��ж���ĳ�ʼ��
    // �����Ҫ��ȡ������Ϣ��������������Ӵ���
    QFileInfo fileInfo(getPro_Path());   // ��ȡ����·�����ļ���Ϣ
    QString parentDir = fileInfo.absolutePath();  // ��ȡ��Ŀ¼

    // �ϲ���Ŀ¼�� temp_line ���ɾ���·��
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
    return getPro_Name();  // �ӻ���̳е���Ŀ����
}

QString Calibration_pro::nextCalibrationLine()
{
    return next();  // ���û����е� next() ����
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

    // д�� board_width��board_height��square_size��scale_factor
    out << QString::number(board_width) << "\n";
    out << QString::number(board_height) << "\n";
    out << QString::number(square_size) << "\n";
    out << QString::number(scale_factor) << "\n";

    // ����ļ�д���ر��ļ�
    file.close();
}
