#include "NewProjectDialog.h"
#include <QHBoxLayout>
#include <QMessageBox>

NewProjectDialog::NewProjectDialog(QWidget* parent) : QDialog(parent) {
    // 创建并布局控件
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // 项目名称
    QLabel* nameLabel = new QLabel(QString::fromLocal8Bit("项目名称: "), this);
    nameEdit_ = new QLineEdit(this);
    nameEdit_->setText(QString::fromLocal8Bit("新建项目 "));
    mainLayout->addWidget(nameLabel);
    mainLayout->addWidget(nameEdit_);

    // 项目类型
    QLabel* typeLabel = new QLabel(QString::fromLocal8Bit("项目类型: "), this);
    typeComboBox_ = new QComboBox(this);
    typeComboBox_->addItem(QString::fromLocal8Bit("标定项目 "), 1);
    typeComboBox_->addItem(QString::fromLocal8Bit("对极几何项目 "), 2);
    typeComboBox_->addItem(QString::fromLocal8Bit("VO项目 "), 3);
    mainLayout->addWidget(typeLabel);
    mainLayout->addWidget(typeComboBox_);

    // 项目路径
    QLabel* pathLabel = new QLabel(QString::fromLocal8Bit("项目路径: "), this);
    QHBoxLayout* pathLayout = new QHBoxLayout();
    pathEdit_ = new QLineEdit(this);
    selectPathButton_ = new QPushButton(this);
    QIcon icon_select(QIcon::fromTheme(QIcon::ThemeIcon::FolderOpen));
    selectPathButton_->setIcon(icon_select);
    connect(selectPathButton_, &QPushButton::clicked, this, &NewProjectDialog::onSelectPath);
    pathLayout->addWidget(pathEdit_);
    pathLayout->addWidget(selectPathButton_);
    mainLayout->addWidget(pathLabel);
    mainLayout->addLayout(pathLayout);

    // 确定和取消按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    okButton_ = new QPushButton(QString::fromLocal8Bit("确定 "), this);
    cancelButton_ = new QPushButton(QString::fromLocal8Bit("取消 "), this);
    connect(okButton_, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton_, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(okButton_);
    buttonLayout->addWidget(cancelButton_);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void NewProjectDialog::onSelectPath() {
    QString dir = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("选择项目路径 "), QString(), QFileDialog::ShowDirsOnly);
    if (!dir.isEmpty()) {
        pathEdit_->setText(dir);
    }
}

void NewProjectDialog::initializeProject(Project_Base& project) {
    if (nameEdit_->text().isEmpty() || pathEdit_->text().isEmpty()) {
        QMessageBox::warning(this, "错误", "项目名称和路径不能为空！");
        return;
    }
    // 初始化 Project_Base 对象
    project.setProName(nameEdit_->text());
    project.setProPath(pathEdit_->text());
    project.setProType(typeComboBox_->currentData().toInt());

    QString proFilePath = pathEdit_->text() + "/" + nameEdit_->text() + ".vopro";
    project.open_file(proFilePath);
    project.out_head_info();
    project.close();
}
