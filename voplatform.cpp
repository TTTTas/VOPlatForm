#include "voplatform.h"
#include "ChessboardCalibration.h"
#include "qprogressdialog.h"
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
    currentCalibrationPro = new Calibration_pro("");
    currentMatchingPro = new Matching_pro("");
    currentVOPro = new VO_pro("");

    *ui.Log_Info_Browser << "welcome\n";
    ChessboardCalibration::set_loger(ui.Log_Info_Browser);
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
        matchingFiles_.append(new Matching_pro(pro.getPro_Path()));
        matchingItem->appendRow(new QStandardItem(pro.getPro_Name()));
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
        QString::fromLocal8Bit("�����ļ� (*.*);;�ı��ļ� (*.vopro)")  // ������
    );

    // ����Ƿ�ѡ�����ļ�
    if (!filePaths.isEmpty()) {
        QFileInfo folderinfo(filePaths[0]);
        ui.pro_label->setText(folderinfo.absolutePath());
        // ����ѡ����ļ�
        foreach(const QString & filePath, filePaths) 
        {
            Project_Base pro(filePath);
            
            Update_pro_tree(pro);
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
    calibrationItem->removeRows(0, calibrationItem->rowCount());
    matchingFiles_.clear();
    matchingItem->removeRows(0, matchingItem->rowCount());
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
    if (item->parent() && item->parent()->parent() == calibrationItem) {
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
    CalibrationSettingsDialog dialog;
    if (dialog.exec() == QDialog::Accepted) {
        QString inputFolder = dialog.getInputFolderPath();
        QString outputFolder = dialog.getOutputFolderPath();
        QString resultFile = dialog.getResultFilePath();
        int bw = dialog.getBoardWidth();
        int bh = dialog.getBoardHeight();
        float ss = dialog.getSquareSize();
        double sf = dialog.getScaleFactor();

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
        QPointer<QThread> thread = new QThread();
        std::shared_ptr<CalibrationWorker> worker = std::make_shared<CalibrationWorker>();

        worker->moveToThread(thread);

        // �����ź����
        connect(worker.get(), &CalibrationWorker::finished, thread, &QThread::quit);
        connect(worker.get(), &CalibrationWorker::finished, worker.get(), &CalibrationWorker::deleteLater);
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);

        // �����߳�
        connect(thread, &QThread::started, worker.get(), &CalibrationWorker::process);
        thread->start();

        // ���֮ǰ����Ŀ
        calibrationFiles_.clear();
        calibrationItem->removeRows(0, calibrationItem->rowCount());
        matchingFiles_.clear();
        matchingItem->removeRows(0, matchingItem->rowCount());
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
}

void VOPlatForm::onResizeTimeout()
{
    updateLabelPixmap();  // ��������ɺ����ͼƬ
}