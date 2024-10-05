#pragma once
#pragma once
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFormLayout>
#include <QString>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTextEdit>

class EpipolarSettingsDialog : public QDialog {
    Q_OBJECT

public:
    EpipolarSettingsDialog(QWidget* parent = nullptr)
        : QDialog(parent)
    {
        setWindowTitle(tr("Epipolar Project Settings"));

        // 文件夹选择按钮
        QPushButton* inputFolderBtn = new QPushButton(tr("Select Input Data Folder"));
        QPushButton* outputFolderBtn = new QPushButton(tr("Select Output Result Folder"));
        QPushButton* resultFileBtn = new QPushButton(tr("Select Output Result File"));

        // 输入框
        inputFolderPath = new QLineEdit(this);
        outputFolderPath = new QLineEdit(this);
        resultFilePath = new QLineEdit(this);

        // 布局
        QFormLayout* formLayout = new QFormLayout;
        formLayout->addRow(tr("Input Data Folder:"), inputFolderPath);
        formLayout->addRow(inputFolderBtn);
        formLayout->addRow(tr("Output Result Folder:"), outputFolderPath);
        formLayout->addRow(outputFolderBtn);
        formLayout->addRow(tr("Output Result File:"), resultFilePath);
        formLayout->addRow(resultFileBtn);

        // 确认与取消按钮
        QPushButton* okButton = new QPushButton(tr("OK"));
        QPushButton* cancelButton = new QPushButton(tr("Cancel"));

        QHBoxLayout* buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(okButton);
        buttonLayout->addWidget(cancelButton);

        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->addLayout(formLayout);
        mainLayout->addLayout(buttonLayout);

        connect(inputFolderBtn, &QPushButton::clicked, this, &EpipolarSettingsDialog::selectInputFolder);
        connect(outputFolderBtn, &QPushButton::clicked, this, &EpipolarSettingsDialog::selectOutputFolder);
        connect(resultFileBtn, &QPushButton::clicked, this, &EpipolarSettingsDialog::selectResultFile);
        connect(okButton, &QPushButton::clicked, this, &EpipolarSettingsDialog::accept);
        connect(cancelButton, &QPushButton::clicked, this, &EpipolarSettingsDialog::reject);
    }

    QString getInputFolderPath() const { return inputFolderPath->text(); }
    QString getOutputFolderPath() const { return outputFolderPath->text(); }
    QString getResultFilePath() const { return resultFilePath->text(); }

private slots:
    void selectInputFolder() {
        QString folder = QFileDialog::getExistingDirectory(this, tr("Select Input Data Folder"));
        if (!folder.isEmpty()) {
            inputFolderPath->setText(folder);
        }
    }

    void selectOutputFolder() {
        QString folder = QFileDialog::getExistingDirectory(this, tr("Select Output Result Folder"));
        if (!folder.isEmpty()) {
            outputFolderPath->setText(folder);
        }
    }

    void selectResultFile() {
        QString file = QFileDialog::getSaveFileName(this, tr("Select Output Result File"), "", tr("Text Files (*.txt);;All Files (*)"));
        if (!file.isEmpty()) {
            resultFilePath->setText(file);
        }
    }

private:
    QLineEdit* inputFolderPath;
    QLineEdit* outputFolderPath;
    QLineEdit* resultFilePath;
};
