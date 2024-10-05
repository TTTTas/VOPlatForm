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

#include "Project_Base.h"
#include "Calibration_pro.h"
#include "Matching_pro.h"
#include "VO_pro.h"
#include "NewProjectDialog.h"
#include "logbrowser.h"
#include "CalibrationSettingsDialog.h"

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
    Calibration_pro* currentCalibrationPro;
    QList<EpipolarGeometry_pro*> epipolargeometryFiles_;
    EpipolarGeometry_pro* currentEpipolarGeometryPro;
    QList<VO_pro*> voFiles_;
    VO_pro* currentVOPro;

    QStandardItem* calibrationItem;
    QStandardItem* epipolargeometryItem;
    QStandardItem* voItem;

    int current_pro;

    void Init_connect_slots();
    void Init_pro_tree();
    void Update_pro_tree(Project_Base& pro);

private slots:
    // 图片框展示函数
    void onLoadPicture();
    void showMatImg(cv::Mat);
    void updateLabelPixmap();
    void onResizeTimeout();
    void resizeEvent(QResizeEvent* event) override;

    // 项目函数
    void onCreateNewPro();
    void onOpenPro();
    void onSavePro();
    void onLoadProjects();

    // 相机标定项目函数
    void onImageItemClicked(const QModelIndex& index);
    void onCalibrationRun();

    // 对极几何项目函数
    void onEpipolarGeometryRun();

    // 项目结束函数
    void finishPro_Solve();
};
