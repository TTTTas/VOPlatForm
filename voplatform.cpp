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
    // 设置 QTimer 处理 resize 的延迟
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
    // 图片框信号槽
    connect(ui.actionload_pic, &QAction::triggered, this, &VOPlatForm::onLoadPicture);
    connect(resizeTimer, &QTimer::timeout, this, &VOPlatForm::onResizeTimeout);

    // 项目信号槽

    // 新建项目
    connect(ui.action_new_pro, &QAction::triggered, this, &VOPlatForm::onCreateNewPro);
    connect(ui.action_create_pro, &QAction::triggered, this, &VOPlatForm::onCreateNewPro);

    // 打开项目
    connect(ui.action_open_pro, &QAction::triggered, this, &VOPlatForm::onOpenPro);
    connect(ui.action_import_pro, &QAction::triggered, this, &VOPlatForm::onOpenPro);

    // 保存项目
    connect(ui.actionsave_pro, &QAction::triggered, this, &VOPlatForm::onSavePro);
    connect(ui.action_save_pro, &QAction::triggered, this, &VOPlatForm::onSavePro);

    // 打开工作目录
    connect(ui.select_pro_btn, &QToolButton::clicked, this, &VOPlatForm::onLoadProjects);
}

void VOPlatForm::Init_pro_tree()
{
    //设置 QTreeView 的数据模型
    pro_tree = new QStandardItemModel();

    // 创建项目类别
    calibrationItem = new QStandardItem(QIcon("./resource/camera1.png"), QString::fromLocal8Bit("标定项目 "));
    matchingItem = new QStandardItem(QIcon("./resource/match.png"), QString::fromLocal8Bit("对极几何项目 "));
    voItem = new QStandardItem(QIcon("./resource/video.png"), QString::fromLocal8Bit("VO项目 "));

    // 将项目类别添加到模型中
    pro_tree->appendRow(calibrationItem);
    pro_tree->appendRow(matchingItem);
    pro_tree->appendRow(voItem);

    // 设置 QTreeView 的模型
    ui.pro_treeView->setModel(pro_tree);
    ui.pro_treeView->expandAll();

}

void VOPlatForm::onLoadPicture()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image File"), "",
        tr("Images (*.png *.xpm *.jpg *.jpeg *.bmp)"));
    // 如果用户没有选择文件，直接返回
    if (fileName.isEmpty()) {
        return;
    }
    // 加载选择的图片到 QPixmap
    QPixmap pixmap(fileName);

    // 检查图片是否加载成功
    if (pixmap.isNull()) {
        QMessageBox::warning(this, tr("Load Image"), tr("The image file could not be loaded."));
        return;
    }

    // 将图片设置到 Pic_show_label 中
    originalPixmap = pixmap;  // 缓存原始图片
    updateLabelPixmap();      // 调整图片大小并显示
}

void VOPlatForm::updateLabelPixmap()
{
    if (!originalPixmap.isNull()) {
        // 将图片按比例缩放到 QLabel 的尺寸
        ui.Pic_show_label->setPixmap(originalPixmap.scaled(ui.Pic_show_label->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation));
    }
}

void VOPlatForm::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    // 拖动时取消之前的定时器，延迟调用图片更新
    resizeTimer->start(100);  // 延迟 100ms 更新
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
    // 使用 QFileDialog::getOpenFileNames 打开文件选择对话框
    QStringList filePaths = QFileDialog::getOpenFileNames(
        nullptr,                         // 父窗口
        QString::fromLocal8Bit("选择文件"),                      // 对话框标题
        "",                              // 默认路径
        QString::fromLocal8Bit("所有文件 (*.*);;文本文件 (*.txt)")  // 过滤器
    );

    // 检查是否选择了文件
    if (!filePaths.isEmpty()) {
        // 遍历选择的文件
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
    // 打开文件夹选择对话框
    QString workspace = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("选择工作目录"), QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (workspace.isEmpty()) {
        qWarning() << "未选择文件夹.";
        return;
    }
    ui.pro_label->setText(workspace);

    // 清除之前的项目
    calibrationFiles_.clear();
    calibrationItem->clearData();
    matchingFiles_.clear();
    matchingItem->clearData();
    voFiles_.clear();
    voItem->clearData();

    // 递归搜索 .pro 文件
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
    updateLabelPixmap();  // 当调整完成后更新图片
}