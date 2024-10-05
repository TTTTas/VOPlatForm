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
// 1. ��Ŀ��������δ��д
// 2. ��Ŀɾ������δ��д
// 3. ��Ŀ���溯��δ��д
// 4. ����ʵ�ֽӿ�δ��д
// 5. ��������Ŀ����δ��д
// 6. ����UI�Ż�

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
    // ������Ŀ���
    QString workspace;                  // �����ռ�
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
    // ͼƬ��չʾ����
    void onLoadPicture();
    void showMatImg(cv::Mat);
    void updateLabelPixmap();
    void onResizeTimeout();
    void resizeEvent(QResizeEvent* event) override;

    // ��Ŀ����
    void onCreateNewPro();
    void onOpenPro();
    void onSavePro();
    void onLoadProjects();

    // ����궨��Ŀ����
    void onImageItemClicked(const QModelIndex& index);
    void onCalibrationRun();

    // �Լ�������Ŀ����
    void onEpipolarGeometryRun();

    // ��Ŀ��������
    void finishPro_Solve();
};
