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

class CalibrationSettingsDialog : public QDialog {
    Q_OBJECT

public:
    CalibrationSettingsDialog(QWidget* parent = nullptr)
        : QDialog(parent)
    {
        setWindowTitle(tr("Calibration Project Settings"));

        // 文件夹选择按钮
        QPushButton* inputFolderBtn = new QPushButton(tr("Select Input Folder"));
        QPushButton* outputFolderBtn = new QPushButton(tr("Select Output Folder"));
        QPushButton* resultFileBtn = new QPushButton(tr("Select Result File"));

        // 输入框
        inputFolderPath = new QLineEdit(this);
        outputFolderPath = new QLineEdit(this);
        resultFilePath = new QLineEdit(this);

        // 标定参数
        boardWidthSpinBox = new QSpinBox(this);
        boardWidthSpinBox->setMinimum(1);
        boardWidthSpinBox->setMaximum(100);
        boardWidthSpinBox->setValue(9);

        boardHeightSpinBox = new QSpinBox(this);
        boardHeightSpinBox->setMinimum(1);
        boardHeightSpinBox->setMaximum(100);
        boardHeightSpinBox->setValue(6);

        squareSizeSpinBox = new QDoubleSpinBox(this);
        squareSizeSpinBox->setMinimum(0.1);
        squareSizeSpinBox->setMaximum(100.0);
        squareSizeSpinBox->setDecimals(2);
        squareSizeSpinBox->setValue(1.0);

        scaleFactorSpinBox = new QDoubleSpinBox(this);
        scaleFactorSpinBox->setMinimum(0.1);
        scaleFactorSpinBox->setMaximum(10.0);
        scaleFactorSpinBox->setDecimals(3);
        scaleFactorSpinBox->setValue(1.0);

        // 布局
        QFormLayout* formLayout = new QFormLayout;
        formLayout->addRow(tr("Input Folder:"), inputFolderPath);
        formLayout->addRow(inputFolderBtn);
        formLayout->addRow(tr("Output Folder:"), outputFolderPath);
        formLayout->addRow(outputFolderBtn);
        formLayout->addRow(tr("Result File:"), resultFilePath);
        formLayout->addRow(resultFileBtn);
        formLayout->addRow(tr("Board Width:"), boardWidthSpinBox);
        formLayout->addRow(tr("Board Height:"), boardHeightSpinBox);
        formLayout->addRow(tr("Square Size:"), squareSizeSpinBox);
        formLayout->addRow(tr("Scale Factor:"), scaleFactorSpinBox);

        // 确认与取消按钮
        QPushButton* okButton = new QPushButton(tr("OK"));
        QPushButton* cancelButton = new QPushButton(tr("Cancel"));

        QHBoxLayout* buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(okButton);
        buttonLayout->addWidget(cancelButton);

        QVBoxLayout* mainLayout = new QVBoxLayout(this);
        mainLayout->addLayout(formLayout);
        mainLayout->addLayout(buttonLayout);

        connect(inputFolderBtn, &QPushButton::clicked, this, &CalibrationSettingsDialog::selectInputFolder);
        connect(outputFolderBtn, &QPushButton::clicked, this, &CalibrationSettingsDialog::selectOutputFolder);
        connect(resultFileBtn, &QPushButton::clicked, this, &CalibrationSettingsDialog::selectResultFile);
        connect(okButton, &QPushButton::clicked, this, &CalibrationSettingsDialog::accept);
        connect(cancelButton, &QPushButton::clicked, this, &CalibrationSettingsDialog::reject);
    }

    QString getInputFolderPath() const { return inputFolderPath->text(); }
    QString getOutputFolderPath() const { return outputFolderPath->text(); }
    QString getResultFilePath() const { return resultFilePath->text(); }
    int getBoardWidth() const { return boardWidthSpinBox->value(); }
    int getBoardHeight() const { return boardHeightSpinBox->value(); }
    float getSquareSize() const { return squareSizeSpinBox->value(); }
    double getScaleFactor() const { return scaleFactorSpinBox->value(); }

private slots:
    void selectInputFolder() {
        QString folder = QFileDialog::getExistingDirectory(this, tr("Select Input Folder"));
        if (!folder.isEmpty()) {
            inputFolderPath->setText(folder);
        }
    }

    void selectOutputFolder() {
        QString folder = QFileDialog::getExistingDirectory(this, tr("Select Output Folder"));
        if (!folder.isEmpty()) {
            outputFolderPath->setText(folder);
        }
    }

    void selectResultFile() {
        QString file = QFileDialog::getSaveFileName(this, tr("Select Result File"), "", tr("Text Files (*.txt)"));
        if (!file.isEmpty()) {
            resultFilePath->setText(file);
        }
    }

private:
    QLineEdit* inputFolderPath;
    QLineEdit* outputFolderPath;
    QLineEdit* resultFilePath;

    QSpinBox* boardWidthSpinBox;
    QSpinBox* boardHeightSpinBox;
    QDoubleSpinBox* squareSizeSpinBox;
    QDoubleSpinBox* scaleFactorSpinBox;
};
