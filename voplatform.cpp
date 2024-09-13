#include "voplatform.h"
#include <qimage.h>
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
{}

void VOPlatForm::Init_connect_slots()
{
    connect(ui.actionload_pic, &QAction::triggered, this, &VOPlatForm::onLoadPicture);
    connect(resizeTimer, &QTimer::timeout, this, &VOPlatForm::onResizeTimeout);
}

void VOPlatForm::Init_pro_tree()
{
    //���� QTreeView ������ģ��
    pro_tree = new QStandardItemModel();

    // ������Ŀ���
    calibrationItem = new QStandardItem(QString::fromLocal8Bit("�궨��Ŀ "));
    matchingItem = new QStandardItem(QString::fromLocal8Bit("ƥ����Ŀ "));
    voItem = new QStandardItem(QString::fromLocal8Bit("VO��Ŀ "));

    // ����Ŀ�����ӵ�ģ����
    pro_tree->appendRow(calibrationItem);
    pro_tree->appendRow(matchingItem);
    pro_tree->appendRow(voItem);

    //������Ŀ����
    calibrationItem->appendRow(new QStandardItem("test1"));
    calibrationItem->appendRow(new QStandardItem("test2"));
    matchingItem->appendRow(new QStandardItem("test3"));
    matchingItem->appendRow(new QStandardItem("test4"));
    matchingItem->appendRow(new QStandardItem("test5"));
    voItem->appendRow(new QStandardItem("test6"));
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

void VOPlatForm::onResizeTimeout()
{
    updateLabelPixmap();  // ��������ɺ����ͼƬ
}