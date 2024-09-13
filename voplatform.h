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
    QList<Matching_pro*> matchingFiles_;
    QList<VO_pro*> voFiles_;


    QStandardItem* calibrationItem;
    QStandardItem* matchingItem;
    QStandardItem* voItem;

    void Init_connect_slots();
    void Init_pro_tree();

private slots:
    // ͼƬ��չʾ����
    void onLoadPicture();
    void updateLabelPixmap();
    void onResizeTimeout();
    void resizeEvent(QResizeEvent* event) override;

    // ��Ŀ����
    void onLoadProjects();
};
