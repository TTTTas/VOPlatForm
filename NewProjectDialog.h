#pragma once
#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include "Project_Base.h"

class NewProjectDialog : public QDialog {
    Q_OBJECT

public:
    explicit NewProjectDialog(QWidget* parent = nullptr);
    void initializeProject(Project_Base& project);

private slots:
    void onSelectPath();

private:
    QLineEdit* nameEdit_;
    QComboBox* typeComboBox_;
    QLineEdit* pathEdit_;
    QPushButton* selectPathButton_;
    QPushButton* okButton_;
    QPushButton* cancelButton_;
};

#endif // NEWPROJECTDIALOG_H
