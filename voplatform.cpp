#include "voplatform.h"
#include "ChessboardCalibration.h"
#include "EpipolarGeometry.h"
#include "qprogressdialog.h"
#include "EpipolarSettingsDialog.h"
#include <qimage.h>
#include <QFileDialog>
#include <QDirIterator>
#include <QDir>
#include <qthread.h>
#include <QPointer>
#include <memory>

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
    currentCalibrationPro = nullptr;
    currentEpipolarGeometryPro = nullptr;
    currentVOPro = nullptr;

    *ui.Log_Info_Browser << "welcome\n";
}

VOPlatForm::~VOPlatForm()
{
    qDeleteAll(calibrationFiles_);
    qDeleteAll(epipolargeometryFiles_);
    qDeleteAll(voFiles_);
}

void VOPlatForm::Init_connect_slots()
{
    // 图片框信号槽
    connect(ui.actionload_pic, &QAction::triggered, this, &VOPlatForm::onLoadPicture);
    connect(resizeTimer, &QTimer::timeout, this, &VOPlatForm::onResizeTimeout);

    // 项目信号槽
    connect(ui.pro_treeView, &QTreeView::clicked, this, &VOPlatForm::onImageItemClicked);

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

    // 标定项目
    connect(ui.camera_action, &QAction::triggered, this, &VOPlatForm::onCalibrationRun);
    connect(ui.actioncalibration, &QAction::triggered, this, &VOPlatForm::onCalibrationRun);

    // 对极几何项目
    connect(ui.actionmatching, &QAction::triggered, this, &VOPlatForm::onEpipolarGeometryRun);
    connect(ui.matching_action, &QAction::triggered, this, &VOPlatForm::onEpipolarGeometryRun);

}

void VOPlatForm::Init_pro_tree()
{
    //设置 QTreeView 的数据模型
    pro_tree = new QStandardItemModel();

    // 创建项目类别
    calibrationItem = new QStandardItem(QIcon("./resource/camera1.png"), QString::fromLocal8Bit("标定项目 "));
    epipolargeometryItem = new QStandardItem(QIcon("./resource/match.png"), QString::fromLocal8Bit("对极几何项目 "));
    voItem = new QStandardItem(QIcon("./resource/video.png"), QString::fromLocal8Bit("VO项目 "));

    // 将项目类别添加到模型中
    pro_tree->appendRow(calibrationItem);
    pro_tree->appendRow(epipolargeometryItem);
    pro_tree->appendRow(voItem);

    // 设置 QTreeView 的模型
    ui.pro_treeView->setModel(pro_tree);
    ui.pro_treeView->expandAll();

}

void VOPlatForm::Update_pro_tree(Project_Base& pro)
{
    switch (pro.getPro_Type()) {
    case 1:
    {
        // 创建新的 Calibration_pro 项目并添加到 calibrationFiles_
        Calibration_pro* newCalibration = new Calibration_pro(pro.getPro_Path());
        calibrationFiles_.append(newCalibration);
        currentCalibrationPro = newCalibration;

        // 为 calibrationItem 添加项目名称
        QStandardItem* newItem = new QStandardItem(pro.getPro_Name());
        calibrationItem->appendRow(newItem);

        // 获取新项目文件夹中的图片文件
        QStringList images = newCalibration->getImages();
        foreach(const QString & imageName, images) {
            QString fullPath = currentCalibrationPro->inputfolder + "/" + imageName;
            // 将图片文件添加到 calibrationItem 的子项
            QStandardItem* imageItem = new QStandardItem(imageName);
            imageItem->setData(fullPath, Qt::UserRole);  // 将完整路径存储为子项的用户角色数据
            newItem->appendRow(imageItem);
        }
        images = newCalibration->getOutImages();
        foreach(const QString& imageName, images) {
            QString fullPath = currentCalibrationPro->outputfolder + "/" + imageName;
            // 将图片文件添加到 calibrationItem 的子项
            QStandardItem* imageItem = new QStandardItem(imageName);
            imageItem->setData(fullPath, Qt::UserRole);  // 将完整路径存储为子项的用户角色数据
            newItem->appendRow(imageItem);
        }
        break;
    }
    case 2:
    {
        // 创建新的 EpipolarGeometry_pro 项目并添加到 epipolargeometryFiles_
        EpipolarGeometry_pro* newEpipolarGeometry = new EpipolarGeometry_pro(pro.getPro_Path());
        epipolargeometryFiles_.append(newEpipolarGeometry);
        currentEpipolarGeometryPro = newEpipolarGeometry;  // 设置当前项目

        // 为 epipolargeometryItem 添加项目名称
        QStandardItem* newItem = new QStandardItem(pro.getPro_Name());
        epipolargeometryItem->appendRow(newItem);

        // 获取新项目文件夹中的图片文件（输入图片）
        QStringList inputImages = newEpipolarGeometry->getImages();
        foreach(const QString & imageName, inputImages) {
            QString fullPath = currentEpipolarGeometryPro->inputfolder + "/" + imageName;
            // 将图片文件添加到 epipolargeometryItem 的子项
            QStandardItem* imageItem = new QStandardItem(imageName);
            imageItem->setData(fullPath, Qt::UserRole);  // 将完整路径存储为子项的用户角色数据
            newItem->appendRow(imageItem);
        }

        // 获取新项目文件夹中的图片文件（输出图片）
        QStringList outputImages = newEpipolarGeometry->getOutImages();
        foreach(const QString & imageName, outputImages) {
            QString fullPath = currentEpipolarGeometryPro->outputfolder + "/" + imageName;
            // 将图片文件添加到 epipolargeometryItem 的子项
            QStandardItem* imageItem = new QStandardItem(imageName);
            imageItem->setData(fullPath, Qt::UserRole);  // 将完整路径存储为子项的用户角色数据
            newItem->appendRow(imageItem);
        }
        break;
    }
    case 3:
    {
        voFiles_.append(new VO_pro(pro.getPro_Path()));
        voItem->appendRow(new QStandardItem(pro.getPro_Name()));
        break;
    }
    default:
        break;
    }
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

void VOPlatForm::showMatImg(cv::Mat img)
{
    // 检查输入图像是否为空
    if (img.empty()) {
        QMessageBox::warning(this, tr("显示图像"), tr("无法显示图像，因为它是空的。"));
        return;
    }

    // 将 cv::Mat 转换为 QImage
    QImage qimg;
    if (img.type() == CV_8UC1) {
        // 如果图像是灰度图
        qimg = QImage(img.data, img.cols, img.rows, img.step, QImage::Format_Grayscale8);
    }
    else if (img.type() == CV_8UC3) {
        // 如果图像是 RGB 图
        qimg = QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888).rgbSwapped();
    }
    else {
        // 处理其他图像格式
        QMessageBox::warning(this, tr("显示图像"), tr("不支持的图像格式。"));
        return;
    }

    // 将 QImage 转换为 QPixmap
    QPixmap pixmap = QPixmap::fromImage(qimg);

    // 将图像设置到 originalPixmap 中
    originalPixmap = pixmap;

    // 在 QLabel 中显示图像
    updateLabelPixmap();
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
        QFileInfo folderinfo(newProject.getPro_Path());
        ui.pro_label->setText(folderinfo.absolutePath());
        workspace = folderinfo.absolutePath();
        switch (newProject.getPro_Type()) 
        {
        case 1:
        {
            Calibration_pro* newpro = new Calibration_pro(newProject.getPro_Path());
            currentCalibrationPro = newpro;
            calibrationFiles_.append(newpro);
            calibrationItem->appendRow(new QStandardItem(newProject.getPro_Name()));
            break;
        }
        case 2:
        {
            EpipolarGeometry_pro* newpro = new EpipolarGeometry_pro(newProject.getPro_Path());
            currentEpipolarGeometryPro = newpro;
            epipolargeometryFiles_.append(newpro);
            epipolargeometryItem->appendRow(new QStandardItem(newProject.getPro_Name()));
            break;
        }
        case 3:
        {
            VO_pro* newpro = new VO_pro(newProject.getPro_Path());
            currentVOPro = newpro;
            voFiles_.append(newpro);
            voItem->appendRow(new QStandardItem(newProject.getPro_Name()));
            break;
        }
        default:
            break;
        }
        *ui.Log_Info_Browser << QString::fromLocal8Bit("新建项目：") + newProject.getPro_Name() + "\n";
    }
}

void VOPlatForm::onOpenPro()
{
    // 使用 QFileDialog::getOpenFileNames 打开文件选择对话框
    QStringList filePaths = QFileDialog::getOpenFileNames(
        nullptr,                         // 父窗口
        QString::fromLocal8Bit("选择文件"),                      // 对话框标题
        "",                              // 默认路径
        QString::fromLocal8Bit("所有文件 (*.*);;文本文件 (*.vopro)")  // 过滤器
    );

    // 检查是否选择了文件
    if (!filePaths.isEmpty()) {
        QFileInfo folderinfo(filePaths[0]);
        ui.pro_label->setText(folderinfo.absolutePath());
        workspace = folderinfo.absolutePath();
        // 遍历选择的文件
        foreach(const QString & filePath, filePaths) 
        {
            Project_Base pro(filePath);
            *ui.Log_Info_Browser << QString::fromLocal8Bit("打开项目：") + pro.getPro_Name() + "\n";
            Update_pro_tree(pro);
        }
    }
    else {
        *ui.Log_Info_Browser << QString::fromLocal8Bit("未选中文件\n");
    }
}

void VOPlatForm::onSavePro()
{
    
    switch (current_pro)
    {
    case 1:
    {
        currentCalibrationPro->save();
        break;
    }
    case 2:
    {
        currentEpipolarGeometryPro->save();
        break;
    }
    case 3:
    {
        currentVOPro->save();
        break;
    }
    default:
        break;
    }
    finishPro_Solve();
}

void VOPlatForm::onLoadProjects()
{
    // 打开文件夹选择对话框
    QString workspace = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("选择工作目录"), QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (workspace.isEmpty()) {
        *ui.Log_Info_Browser << QString::fromLocal8Bit("未选择文件夹.");
        return;
    }
    ui.pro_label->setText(workspace);

    // 清除之前的项目
    calibrationFiles_.clear();
    calibrationItem->removeRows(0, calibrationItem->rowCount());
    epipolargeometryFiles_.clear();
    epipolargeometryItem->removeRows(0, epipolargeometryItem->rowCount());
    voFiles_.clear();
    voItem->removeRows(0, voItem->rowCount());

    // 递归搜索 .pro 文件
    QDir dir(workspace);
    QDirIterator it(dir.absolutePath(), QStringList() << "*.vopro", QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        QString filePath = it.next();
        Project_Base pro(filePath);

        Update_pro_tree(pro);
    }
}

void VOPlatForm::onImageItemClicked(const QModelIndex& index)
{
    // 确保索引有效
    if (!index.isValid()) return;

    // 获取模型
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui.pro_treeView->model());
    if (!model) return; // 确保模型有效

    // 获取 QStandardItem
    QStandardItem* item = model->itemFromIndex(index);
    if (!item) return; // 确保获取到有效的项

    // 判断该项是否是 calibrationItem 的子项的子项
    if ((item->parent() && item->parent()->parent() == calibrationItem) || (item->parent() && item->parent()->parent() == epipolargeometryItem))
    {
        // 该项是 calibrationItem 的子项的子项，获取对应的完整路径
        QString imagePath = index.data(Qt::UserRole).toString();  // 获取点击子项的文本信息（即图片文件名）

        // 加载图片
        QPixmap pixmap(imagePath);

        // 检查图片是否加载成功
        if (pixmap.isNull()) {
            QMessageBox::warning(this, tr("Load Image"), tr("The image file could not be loaded."));
            return;
        }

        // 将图片设置到 Pic_show_label 中
        originalPixmap = pixmap;  // 缓存原始图片
        updateLabelPixmap();      // 调整图片大小并显示
    }
}

void VOPlatForm::onCalibrationRun()
{
    current_pro = 1;
    if (currentCalibrationPro == nullptr)
    {
        *ui.Log_Info_Browser << QString::fromLocal8Bit("未选择项目\n");
        return;
    }
    ui.statusBar->showMessage(QString::fromLocal8Bit("当前标定项目：") + currentCalibrationPro->getPro_Name());
    CalibrationSettingsDialog dialog;
    if (dialog.exec() == QDialog::Accepted) {
        QString inputFolder = dialog.getInputFolderPath();
        QString outputFolder = dialog.getOutputFolderPath();
        QString resultFile = dialog.getResultFilePath();
        int bw = dialog.getBoardWidth();
        int bh = dialog.getBoardHeight();
        float ss = dialog.getSquareSize();
        double sf = dialog.getScaleFactor();

        currentCalibrationPro->update(bw, bh, ss, sf, inputFolder, outputFolder, resultFile);
        currentCalibrationPro->save();

        // 将获取的参数传入标定项目
        ChessboardCalibration::init_Calibration(bw, bh, ss, sf, inputFolder.toStdString(),outputFolder.toStdString(),resultFile.toStdString());

        int totalImages = 0;

        for (const auto& entry : std::filesystem::directory_iterator(inputFolder.toStdString())) {
            if (entry.path().extension() == ".JPG" || entry.path().extension() == ".jpg"
                || entry.path().extension() == ".JPEG" || entry.path().extension() == ".jpeg"
                || entry.path().extension() == ".png" || entry.path().extension() == ".PNG") {
                ++totalImages;
            }
        }

        QProgressDialog* progressDialog = new QProgressDialog(QString::fromLocal8Bit("正在进行标定..."), QString::fromLocal8Bit("取消"), 0, totalImages, this);
        progressDialog->setWindowModality(Qt::WindowModal);
        progressDialog->setValue(0);
        progressDialog->show();

        // 创建并启动工作线程
        QThread* thread = new QThread();
        CalibrationWorker* worker = new CalibrationWorker();

        worker->moveToThread(thread);

        // 连接信号与槽
        connect(worker, &CalibrationWorker::finished, this, &VOPlatForm::finishPro_Solve);
        connect(worker, &CalibrationWorker::updateProgress, progressDialog, &QProgressDialog::setValue);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        // 日志信号槽
        connect(worker, &CalibrationWorker::logMessage, ui.Log_Info_Browser, &LogBrowser::insertFormattedText);
        connect(worker, &CalibrationWorker::showimg, this, &VOPlatForm::showMatImg);

        // 启动线程
        connect(thread, &QThread::started, worker, &CalibrationWorker::process);
        thread->start();

    }
}

void VOPlatForm::onEpipolarGeometryRun()
{
    current_pro = 2;
    if (currentEpipolarGeometryPro == nullptr)
    {
        *ui.Log_Info_Browser << QString::fromLocal8Bit("未选择项目\n");
        return;
    }
    ui.statusBar->showMessage(QString::fromLocal8Bit("当前对极几何项目：") + currentEpipolarGeometryPro->getPro_Name());
    EpipolarSettingsDialog dialog;
    if (dialog.exec() == QDialog::Accepted) {
        QString inputFolder = dialog.getInputFolderPath();
        QString outputFolder = dialog.getOutputFolderPath();
        QString resultFile = dialog.getResultFilePath();

        currentEpipolarGeometryPro->update(inputFolder, outputFolder, resultFile);
        currentEpipolarGeometryPro->save();

        // 将获取的参数传入标定项目
        EpipolarGeometry::Init(inputFolder.toStdString(), outputFolder.toStdString(), resultFile.toStdString());

        int totalImages = 0;

        for (const auto& entry : std::filesystem::directory_iterator(inputFolder.toStdString())) {
            if (entry.path().extension() == ".JPG" || entry.path().extension() == ".jpg"
                || entry.path().extension() == ".JPEG" || entry.path().extension() == ".jpeg"
                || entry.path().extension() == ".png" || entry.path().extension() == ".PNG") {
                ++totalImages;
            }
        }

        QProgressDialog* progressDialog = new QProgressDialog(QString::fromLocal8Bit("正在进行对极几何匹配..."), QString::fromLocal8Bit("取消"), 0, totalImages - 1, this);
        progressDialog->setWindowModality(Qt::WindowModal);
        progressDialog->setValue(0);
        progressDialog->show();

        // 创建并启动工作线程
        QThread* thread = new QThread();
        EpipolarGeometryWorker* worker = new EpipolarGeometryWorker();

        worker->moveToThread(thread);

        // 连接信号与槽
        connect(worker, &EpipolarGeometryWorker::finished, this, &VOPlatForm::finishPro_Solve);
        connect(worker, &EpipolarGeometryWorker::updateProgress, progressDialog, &QProgressDialog::setValue);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        // 日志信号槽
        connect(worker, &EpipolarGeometryWorker::logMessage, ui.Log_Info_Browser, &LogBrowser::insertFormattedText);
        connect(worker, &EpipolarGeometryWorker::showimg, this, &VOPlatForm::showMatImg);

        // 启动线程
        connect(thread, &QThread::started, worker, &EpipolarGeometryWorker::process);
        thread->start();

    }
}

void VOPlatForm::finishPro_Solve()
{
    // 清除之前的项目
    calibrationFiles_.clear();
    calibrationItem->removeRows(0, calibrationItem->rowCount());
    epipolargeometryFiles_.clear();
    epipolargeometryItem->removeRows(0, epipolargeometryItem->rowCount());
    voFiles_.clear();
    voItem->removeRows(0, voItem->rowCount());

    // 递归搜索 .pro 文件
    QDir dir(workspace);
    QDirIterator it(dir.absolutePath(), QStringList() << "*.vopro", QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        QString filePath = it.next();
        Project_Base pro(filePath);

        Update_pro_tree(pro);
    }
}

void VOPlatForm::onResizeTimeout()
{
    updateLabelPixmap();  // 当调整完成后更新图片
}