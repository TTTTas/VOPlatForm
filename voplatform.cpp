#include "voplatform.h"
#include <qimage.h>
#include <QFileDialog>
#include <QDirIterator>
#include <QDir>

#pragma execution_character_set("utf-8")

VOPlatForm::VOPlatForm(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    ui.splitter_2->setStretchFactor(0, 30);
    ui.splitter_2->setStretchFactor(1, 1);

    ui.Pic_show_label->setText("");
    ui.Pic_show_label->setStyleSheet("background-color:black");
    // ���� QTimer ���� resize ���ӳ�
    resizeTimer = new QTimer(this);
    resizeTimer->setSingleShot(true);
    Init_connect_slots();
    Init_pro_tree();
}

VOPlatForm::~VOPlatForm()
{
    qDeleteAll(calibrationFiles_);
    qDeleteAll(matchingFiles_);
    qDeleteAll(voFiles_);
}

void VOPlatForm::Init_connect_slots()
{
    // ͼƬ���źŲ�
    connect(ui.actionload_pic, &QAction::triggered, this, &VOPlatForm::onLoadPicture);
    connect(resizeTimer, &QTimer::timeout, this, &VOPlatForm::onResizeTimeout);

    // ��Ŀ�źŲ�

    // �½���Ŀ
    connect(ui.action_new_pro, &QAction::triggered, this, &VOPlatForm::onCreateNewPro);
    connect(ui.action_create_pro, &QAction::triggered, this, &VOPlatForm::onCreateNewPro);

    // ����Ŀ
    connect(ui.action_open_pro, &QAction::triggered, this, &VOPlatForm::onOpenPro);
    connect(ui.action_import_pro, &QAction::triggered, this, &VOPlatForm::onOpenPro);

    // ������Ŀ
    connect(ui.actionsave_pro, &QAction::triggered, this, &VOPlatForm::onSavePro);
    connect(ui.action_save_pro, &QAction::triggered, this, &VOPlatForm::onSavePro);

    // �򿪹���Ŀ¼
    connect(ui.select_pro_btn, &QToolButton::clicked, this, &VOPlatForm::onLoadProjects);
}

void VOPlatForm::Init_pro_tree()
{
    //���� QTreeView ������ģ��
    pro_tree = new QStandardItemModel();

    // ������Ŀ���
    calibrationItem = new QStandardItem(QIcon("./resource/camera1.png"), QString::fromLocal8Bit("�궨��Ŀ "));
    matchingItem = new QStandardItem(QIcon("./resource/match.png"), QString::fromLocal8Bit("�Լ�������Ŀ "));
    voItem = new QStandardItem(QIcon("./resource/video.png"), QString::fromLocal8Bit("VO��Ŀ "));

    // ����Ŀ�����ӵ�ģ����
    pro_tree->appendRow(calibrationItem);
    pro_tree->appendRow(matchingItem);
    pro_tree->appendRow(voItem);

    // ���� QTreeView ��ģ��
    ui.pro_treeView->setModel(pro_tree);
    ui.pro_treeView->expandAll();

}

void VOPlatForm::onLoadPicture()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image File"), "",
        tr("Images (*.png *.xpm *.jpg *.jpeg *.bmp)"));
    // ����û�û��ѡ���ļ���ֱ�ӷ���
    if (fileName.isEmpty()) {
        return;
    }
    // ����ѡ���ͼƬ�� QPixmap
    QPixmap pixmap(fileName);

    // ���ͼƬ�Ƿ���سɹ�
    if (pixmap.isNull()) {
        QMessageBox::warning(this, tr("Load Image"), tr("The image file could not be loaded."));
        return;
    }

    // ��ͼƬ���õ� Pic_show_label ��
    originalPixmap = pixmap;  // ����ԭʼͼƬ
    updateLabelPixmap();      // ����ͼƬ��С����ʾ
}

void VOPlatForm::updateLabelPixmap()
{
    if (!originalPixmap.isNull()) {
        // ��ͼƬ���������ŵ� QLabel �ĳߴ�
        ui.Pic_show_label->setPixmap(originalPixmap.scaled(ui.Pic_show_label->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation));
    }
}

void VOPlatForm::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    // �϶�ʱȡ��֮ǰ�Ķ�ʱ�����ӳٵ���ͼƬ����
    resizeTimer->start(100);  // �ӳ� 100ms ����
}

void VOPlatForm::onCreateNewPro()
{
    NewProjectDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Project_Base newProject;
        dialog.initializeProject(newProject);
        switch (newProject.getPro_Type()) 
        {
        case 1:
            calibrationFiles_.append(new Calibration_pro(newProject.getPro_Path()));
            calibrationItem->appendRow(new QStandardItem(newProject.getPro_Name()));
            break;
        case 2:
            matchingFiles_.append(new Matching_pro(newProject.getPro_Path()));
            matchingItem->appendRow(new QStandardItem(newProject.getPro_Name()));
            break;
        case 3:
            voFiles_.append(new VO_pro(newProject.getPro_Path()));
            voItem->appendRow(new QStandardItem(newProject.getPro_Name()));
            break;
        default:
            break;
        }
    }
}

void VOPlatForm::onOpenPro()
{
    // ʹ�� QFileDialog::getOpenFileNames ���ļ�ѡ��Ի���
    QStringList filePaths = QFileDialog::getOpenFileNames(
        nullptr,                         // ������
        QString::fromLocal8Bit("ѡ���ļ�"),                      // �Ի������
        "",                              // Ĭ��·��
        QString::fromLocal8Bit("�����ļ� (*.*);;�ı��ļ� (*.txt)")  // ������
    );

    // ����Ƿ�ѡ�����ļ�
    if (!filePaths.isEmpty()) {
        // ����ѡ����ļ�
        foreach(const QString & filePath, filePaths) 
        {
            Project_Base pro(filePath);
            switch (pro.getPro_Type()) {
            case 1:
                calibrationFiles_.append(new Calibration_pro(filePath));
                calibrationItem->appendRow(new QStandardItem(pro.getPro_Name()));
                break;
            case 2:
                matchingFiles_.append(new Matching_pro(filePath));
                matchingItem->appendRow(new QStandardItem(pro.getPro_Name()));
                break;
            case 3:
                voFiles_.append(new VO_pro(filePath));
                voItem->appendRow(new QStandardItem(pro.getPro_Name()));
                break;
            default:
                break;
            }
        }
    }
    else {
        qDebug() << "No files selected.";
    }
}

void VOPlatForm::onSavePro()
{

}

void VOPlatForm::onLoadProjects()
{
    // ���ļ���ѡ��Ի���
    QString workspace = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("ѡ����Ŀ¼"), QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (workspace.isEmpty()) {
        qWarning() << "δѡ���ļ���.";
        return;
    }
    ui.pro_label->setText(workspace);

    // ���֮ǰ����Ŀ
    calibrationFiles_.clear();
    calibrationItem->clearData();
    matchingFiles_.clear();
    matchingItem->clearData();
    voFiles_.clear();
    voItem->clearData();

    // �ݹ����� .pro �ļ�
    QDir dir(workspace);
    QDirIterator it(dir.absolutePath(), QStringList() << "*.vopro", QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        QString filePath = it.next();
        Project_Base pro(filePath);
        switch (pro.getPro_Type()) {
        case 1:
            calibrationFiles_.append(new Calibration_pro(filePath));
            calibrationItem->appendRow(new QStandardItem(pro.getPro_Name()));
            break;
        case 2:
            matchingFiles_.append(new Matching_pro(filePath));
            matchingItem->appendRow(new QStandardItem(pro.getPro_Name()));
            break;
        case 3:
            voFiles_.append(new VO_pro(filePath));
            voItem->appendRow(new QStandardItem(pro.getPro_Name()));
            break;
        default:
            break;
        }

    }
}

void VOPlatForm::onResizeTimeout()
{
    updateLabelPixmap();  // ��������ɺ����ͼƬ
}