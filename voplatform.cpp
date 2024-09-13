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
    // 设置 QTimer 处理 resize 的延迟
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
    //设置 QTreeView 的数据模型
    pro_tree = new QStandardItemModel();

    // 创建项目类别
    calibrationItem = new QStandardItem(QString::fromLocal8Bit("标定项目 "));
    matchingItem = new QStandardItem(QString::fromLocal8Bit("匹配项目 "));
    voItem = new QStandardItem(QString::fromLocal8Bit("VO项目 "));

    // 将项目类别添加到模型中
    pro_tree->appendRow(calibrationItem);
    pro_tree->appendRow(matchingItem);
    pro_tree->appendRow(voItem);

    //测试项目属性
    calibrationItem->appendRow(new QStandardItem("test1"));
    calibrationItem->appendRow(new QStandardItem("test2"));
    matchingItem->appendRow(new QStandardItem("test3"));
    matchingItem->appendRow(new QStandardItem("test4"));
    matchingItem->appendRow(new QStandardItem("test5"));
    voItem->appendRow(new QStandardItem("test6"));
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

void VOPlatForm::onResizeTimeout()
{
    updateLabelPixmap();  // 当调整完成后更新图片
}