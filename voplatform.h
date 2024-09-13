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

#include "projects.h"
#include "Calibration_pro.h"
#include "Matching_pro.h"
#include "VO_pro.h"

// TODO
// 1. 项目创建函数未编写
// 2. 项目删除函数未编写
// 3. 项目保存函数未编写
// 4. 功能实现接口未编写
// 5. 工具栏项目部分未编写
// 6. 部分UI优化

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
    QString workspace;                  // 工作空间
    QList<Calibration_pro*> calibrationFiles_;
    QList<Matching_pro*> matchingFiles_;
    QList<VO_pro*> voFiles_;


    QStandardItem* calibrationItem;
    QStandardItem* matchingItem;
    QStandardItem* voItem;

    void Init_connect_slots();
    void Init_pro_tree();

private slots:
    // 图片框展示函数
    void onLoadPicture();
    void updateLabelPixmap();
    void onResizeTimeout();
    void resizeEvent(QResizeEvent* event) override;

    // 项目函数
    void onLoadProjects();
};
