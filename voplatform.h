#pragma once
#pragma execution_character_set("utf-8")

#include <QtWidgets/QMainWindow>
#include <QFileDialog>
#include <QLabel>
#include <QPixmap>
#include <qmessagebox.h>
#include <qtimer.h>
#include "ui_voplatform.h"
#include <qstandarditemmodel.h>

class VOPlatForm : public QMainWindow
{
    Q_OBJECT

public:
    VOPlatForm(QWidget *parent = nullptr);
    ~VOPlatForm();

private:
    Ui::VOPlatFormClass ui;
    QPixmap originalPixmap;
    QTimer* resizeTimer;
    QStandardItemModel* pro_tree;
    // 创建项目类别
    QStandardItem* calibrationItem;
    QStandardItem* matchingItem;
    QStandardItem* voItem;

    void Init_connect_slots();
    void Init_pro_tree();

private slots:
    void onLoadPicture();
    void updateLabelPixmap();
    void onResizeTimeout();
    void resizeEvent(QResizeEvent* event) override;
};
