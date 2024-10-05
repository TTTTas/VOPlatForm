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
    // ���� QTimer ���� resize ���ӳ�
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
    // ͼƬ���źŲ�
    connect(ui.actionload_pic, &QAction::triggered, this, &VOPlatForm::onLoadPicture);
    connect(resizeTimer, &QTimer::timeout, this, &VOPlatForm::onResizeTimeout);

    // ��Ŀ�źŲ�
    connect(ui.pro_treeView, &QTreeView::clicked, this, &VOPlatForm::onImageItemClicked);

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

    // �궨��Ŀ
    connect(ui.camera_action, &QAction::triggered, this, &VOPlatForm::onCalibrationRun);
    connect(ui.actioncalibration, &QAction::triggered, this, &VOPlatForm::onCalibrationRun);

    // �Լ�������Ŀ
    connect(ui.actionmatching, &QAction::triggered, this, &VOPlatForm::onEpipolarGeometryRun);
    connect(ui.matching_action, &QAction::triggered, this, &VOPlatForm::onEpipolarGeometryRun);

}

void VOPlatForm::Init_pro_tree()
{
    //���� QTreeView ������ģ��
    pro_tree = new QStandardItemModel();

    // ������Ŀ���
    calibrationItem = new QStandardItem(QIcon("./resource/camera1.png"), QString::fromLocal8Bit("�궨��Ŀ "));
    epipolargeometryItem = new QStandardItem(QIcon("./resource/match.png"), QString::fromLocal8Bit("�Լ�������Ŀ "));
    voItem = new QStandardItem(QIcon("./resource/video.png"), QString::fromLocal8Bit("VO��Ŀ "));

    // ����Ŀ�����ӵ�ģ����
    pro_tree->appendRow(calibrationItem);
    pro_tree->appendRow(epipolargeometryItem);
    pro_tree->appendRow(voItem);

    // ���� QTreeView ��ģ��
    ui.pro_treeView->setModel(pro_tree);
    ui.pro_treeView->expandAll();

}

void VOPlatForm::Update_pro_tree(Project_Base& pro)
{
    switch (pro.getPro_Type()) {
    case 1:
    {
        // �����µ� Calibration_pro ��Ŀ����ӵ� calibrationFiles_
        Calibration_pro* newCalibration = new Calibration_pro(pro.getPro_Path());
        calibrationFiles_.append(newCalibration);
        currentCalibrationPro = newCalibration;

        // Ϊ calibrationItem �����Ŀ����
        QStandardItem* newItem = new QStandardItem(pro.getPro_Name());
        calibrationItem->appendRow(newItem);

        // ��ȡ����Ŀ�ļ����е�ͼƬ�ļ�
        QStringList images = newCalibration->getImages();
        foreach(const QString & imageName, images) {
            QString fullPath = currentCalibrationPro->inputfolder + "/" + imageName;
            // ��ͼƬ�ļ���ӵ� calibrationItem ������
            QStandardItem* imageItem = new QStandardItem(imageName);
            imageItem->setData(fullPath, Qt::UserRole);  // ������·���洢Ϊ������û���ɫ����
            newItem->appendRow(imageItem);
        }
        images = newCalibration->getOutImages();
        foreach(const QString& imageName, images) {
            QString fullPath = currentCalibrationPro->outputfolder + "/" + imageName;
            // ��ͼƬ�ļ���ӵ� calibrationItem ������
            QStandardItem* imageItem = new QStandardItem(imageName);
            imageItem->setData(fullPath, Qt::UserRole);  // ������·���洢Ϊ������û���ɫ����
            newItem->appendRow(imageItem);
        }
        break;
    }
    case 2:
    {
        // �����µ� EpipolarGeometry_pro ��Ŀ����ӵ� epipolargeometryFiles_
        EpipolarGeometry_pro* newEpipolarGeometry = new EpipolarGeometry_pro(pro.getPro_Path());
        epipolargeometryFiles_.append(newEpipolarGeometry);
        currentEpipolarGeometryPro = newEpipolarGeometry;  // ���õ�ǰ��Ŀ

        // Ϊ epipolargeometryItem �����Ŀ����
        QStandardItem* newItem = new QStandardItem(pro.getPro_Name());
        epipolargeometryItem->appendRow(newItem);

        // ��ȡ����Ŀ�ļ����е�ͼƬ�ļ�������ͼƬ��
        QStringList inputImages = newEpipolarGeometry->getImages();
        foreach(const QString & imageName, inputImages) {
            QString fullPath = currentEpipolarGeometryPro->inputfolder + "/" + imageName;
            // ��ͼƬ�ļ���ӵ� epipolargeometryItem ������
            QStandardItem* imageItem = new QStandardItem(imageName);
            imageItem->setData(fullPath, Qt::UserRole);  // ������·���洢Ϊ������û���ɫ����
            newItem->appendRow(imageItem);
        }

        // ��ȡ����Ŀ�ļ����е�ͼƬ�ļ������ͼƬ��
        QStringList outputImages = newEpipolarGeometry->getOutImages();
        foreach(const QString & imageName, outputImages) {
            QString fullPath = currentEpipolarGeometryPro->outputfolder + "/" + imageName;
            // ��ͼƬ�ļ���ӵ� epipolargeometryItem ������
            QStandardItem* imageItem = new QStandardItem(imageName);
            imageItem->setData(fullPath, Qt::UserRole);  // ������·���洢Ϊ������û���ɫ����
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

void VOPlatForm::showMatImg(cv::Mat img)
{
    // �������ͼ���Ƿ�Ϊ��
    if (img.empty()) {
        QMessageBox::warning(this, tr("��ʾͼ��"), tr("�޷���ʾͼ����Ϊ���ǿյġ�"));
        return;
    }

    // �� cv::Mat ת��Ϊ QImage
    QImage qimg;
    if (img.type() == CV_8UC1) {
        // ���ͼ���ǻҶ�ͼ
        qimg = QImage(img.data, img.cols, img.rows, img.step, QImage::Format_Grayscale8);
    }
    else if (img.type() == CV_8UC3) {
        // ���ͼ���� RGB ͼ
        qimg = QImage(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888).rgbSwapped();
    }
    else {
        // ��������ͼ���ʽ
        QMessageBox::warning(this, tr("��ʾͼ��"), tr("��֧�ֵ�ͼ���ʽ��"));
        return;
    }

    // �� QImage ת��Ϊ QPixmap
    QPixmap pixmap = QPixmap::fromImage(qimg);

    // ��ͼ�����õ� originalPixmap ��
    originalPixmap = pixmap;

    // �� QLabel ����ʾͼ��
    updateLabelPixmap();
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
        *ui.Log_Info_Browser << QString::fromLocal8Bit("�½���Ŀ��") + newProject.getPro_Name() + "\n";
    }
}

void VOPlatForm::onOpenPro()
{
    // ʹ�� QFileDialog::getOpenFileNames ���ļ�ѡ��Ի���
    QStringList filePaths = QFileDialog::getOpenFileNames(
        nullptr,                         // ������
        QString::fromLocal8Bit("ѡ���ļ�"),                      // �Ի������
        "",                              // Ĭ��·��
        QString::fromLocal8Bit("�����ļ� (*.*);;�ı��ļ� (*.vopro)")  // ������
    );

    // ����Ƿ�ѡ�����ļ�
    if (!filePaths.isEmpty()) {
        QFileInfo folderinfo(filePaths[0]);
        ui.pro_label->setText(folderinfo.absolutePath());
        workspace = folderinfo.absolutePath();
        // ����ѡ����ļ�
        foreach(const QString & filePath, filePaths) 
        {
            Project_Base pro(filePath);
            *ui.Log_Info_Browser << QString::fromLocal8Bit("����Ŀ��") + pro.getPro_Name() + "\n";
            Update_pro_tree(pro);
        }
    }
    else {
        *ui.Log_Info_Browser << QString::fromLocal8Bit("δѡ���ļ�\n");
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
    // ���ļ���ѡ��Ի���
    QString workspace = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("ѡ����Ŀ¼"), QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (workspace.isEmpty()) {
        *ui.Log_Info_Browser << QString::fromLocal8Bit("δѡ���ļ���.");
        return;
    }
    ui.pro_label->setText(workspace);

    // ���֮ǰ����Ŀ
    calibrationFiles_.clear();
    calibrationItem->removeRows(0, calibrationItem->rowCount());
    epipolargeometryFiles_.clear();
    epipolargeometryItem->removeRows(0, epipolargeometryItem->rowCount());
    voFiles_.clear();
    voItem->removeRows(0, voItem->rowCount());

    // �ݹ����� .pro �ļ�
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
    // ȷ��������Ч
    if (!index.isValid()) return;

    // ��ȡģ��
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui.pro_treeView->model());
    if (!model) return; // ȷ��ģ����Ч

    // ��ȡ QStandardItem
    QStandardItem* item = model->itemFromIndex(index);
    if (!item) return; // ȷ����ȡ����Ч����

    // �жϸ����Ƿ��� calibrationItem �����������
    if ((item->parent() && item->parent()->parent() == calibrationItem) || (item->parent() && item->parent()->parent() == epipolargeometryItem))
    {
        // ������ calibrationItem ������������ȡ��Ӧ������·��
        QString imagePath = index.data(Qt::UserRole).toString();  // ��ȡ���������ı���Ϣ����ͼƬ�ļ�����

        // ����ͼƬ
        QPixmap pixmap(imagePath);

        // ���ͼƬ�Ƿ���سɹ�
        if (pixmap.isNull()) {
            QMessageBox::warning(this, tr("Load Image"), tr("The image file could not be loaded."));
            return;
        }

        // ��ͼƬ���õ� Pic_show_label ��
        originalPixmap = pixmap;  // ����ԭʼͼƬ
        updateLabelPixmap();      // ����ͼƬ��С����ʾ
    }
}

void VOPlatForm::onCalibrationRun()
{
    current_pro = 1;
    if (currentCalibrationPro == nullptr)
    {
        *ui.Log_Info_Browser << QString::fromLocal8Bit("δѡ����Ŀ\n");
        return;
    }
    ui.statusBar->showMessage(QString::fromLocal8Bit("��ǰ�궨��Ŀ��") + currentCalibrationPro->getPro_Name());
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

        // ����ȡ�Ĳ�������궨��Ŀ
        ChessboardCalibration::init_Calibration(bw, bh, ss, sf, inputFolder.toStdString(),outputFolder.toStdString(),resultFile.toStdString());

        int totalImages = 0;

        for (const auto& entry : std::filesystem::directory_iterator(inputFolder.toStdString())) {
            if (entry.path().extension() == ".JPG" || entry.path().extension() == ".jpg"
                || entry.path().extension() == ".JPEG" || entry.path().extension() == ".jpeg"
                || entry.path().extension() == ".png" || entry.path().extension() == ".PNG") {
                ++totalImages;
            }
        }

        QProgressDialog* progressDialog = new QProgressDialog(QString::fromLocal8Bit("���ڽ��б궨..."), QString::fromLocal8Bit("ȡ��"), 0, totalImages, this);
        progressDialog->setWindowModality(Qt::WindowModal);
        progressDialog->setValue(0);
        progressDialog->show();

        // ���������������߳�
        QThread* thread = new QThread();
        CalibrationWorker* worker = new CalibrationWorker();

        worker->moveToThread(thread);

        // �����ź����
        connect(worker, &CalibrationWorker::finished, this, &VOPlatForm::finishPro_Solve);
        connect(worker, &CalibrationWorker::updateProgress, progressDialog, &QProgressDialog::setValue);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        // ��־�źŲ�
        connect(worker, &CalibrationWorker::logMessage, ui.Log_Info_Browser, &LogBrowser::insertFormattedText);
        connect(worker, &CalibrationWorker::showimg, this, &VOPlatForm::showMatImg);

        // �����߳�
        connect(thread, &QThread::started, worker, &CalibrationWorker::process);
        thread->start();

    }
}

void VOPlatForm::onEpipolarGeometryRun()
{
    current_pro = 2;
    if (currentEpipolarGeometryPro == nullptr)
    {
        *ui.Log_Info_Browser << QString::fromLocal8Bit("δѡ����Ŀ\n");
        return;
    }
    ui.statusBar->showMessage(QString::fromLocal8Bit("��ǰ�Լ�������Ŀ��") + currentEpipolarGeometryPro->getPro_Name());
    EpipolarSettingsDialog dialog;
    if (dialog.exec() == QDialog::Accepted) {
        QString inputFolder = dialog.getInputFolderPath();
        QString outputFolder = dialog.getOutputFolderPath();
        QString resultFile = dialog.getResultFilePath();

        currentEpipolarGeometryPro->update(inputFolder, outputFolder, resultFile);
        currentEpipolarGeometryPro->save();

        // ����ȡ�Ĳ�������궨��Ŀ
        EpipolarGeometry::Init(inputFolder.toStdString(), outputFolder.toStdString(), resultFile.toStdString());

        int totalImages = 0;

        for (const auto& entry : std::filesystem::directory_iterator(inputFolder.toStdString())) {
            if (entry.path().extension() == ".JPG" || entry.path().extension() == ".jpg"
                || entry.path().extension() == ".JPEG" || entry.path().extension() == ".jpeg"
                || entry.path().extension() == ".png" || entry.path().extension() == ".PNG") {
                ++totalImages;
            }
        }

        QProgressDialog* progressDialog = new QProgressDialog(QString::fromLocal8Bit("���ڽ��жԼ�����ƥ��..."), QString::fromLocal8Bit("ȡ��"), 0, totalImages - 1, this);
        progressDialog->setWindowModality(Qt::WindowModal);
        progressDialog->setValue(0);
        progressDialog->show();

        // ���������������߳�
        QThread* thread = new QThread();
        EpipolarGeometryWorker* worker = new EpipolarGeometryWorker();

        worker->moveToThread(thread);

        // �����ź����
        connect(worker, &EpipolarGeometryWorker::finished, this, &VOPlatForm::finishPro_Solve);
        connect(worker, &EpipolarGeometryWorker::updateProgress, progressDialog, &QProgressDialog::setValue);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        // ��־�źŲ�
        connect(worker, &EpipolarGeometryWorker::logMessage, ui.Log_Info_Browser, &LogBrowser::insertFormattedText);
        connect(worker, &EpipolarGeometryWorker::showimg, this, &VOPlatForm::showMatImg);

        // �����߳�
        connect(thread, &QThread::started, worker, &EpipolarGeometryWorker::process);
        thread->start();

    }
}

void VOPlatForm::finishPro_Solve()
{
    // ���֮ǰ����Ŀ
    calibrationFiles_.clear();
    calibrationItem->removeRows(0, calibrationItem->rowCount());
    epipolargeometryFiles_.clear();
    epipolargeometryItem->removeRows(0, epipolargeometryItem->rowCount());
    voFiles_.clear();
    voItem->removeRows(0, voItem->rowCount());

    // �ݹ����� .pro �ļ�
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
    updateLabelPixmap();  // ��������ɺ����ͼƬ
}