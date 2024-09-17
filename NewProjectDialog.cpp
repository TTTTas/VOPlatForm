#include "NewProjectDialog.h"
#include <QHBoxLayout>
#include <QMessageBox>

NewProjectDialog::NewProjectDialog(QWidget* parent) : QDialog(parent) {
    // ���������ֿؼ�
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // ��Ŀ����
    QLabel* nameLabel = new QLabel(QString::fromLocal8Bit("��Ŀ����: "), this);
    nameEdit_ = new QLineEdit(this);
    nameEdit_->setText(QString::fromLocal8Bit("�½���Ŀ "));
    mainLayout->addWidget(nameLabel);
    mainLayout->addWidget(nameEdit_);

    // ��Ŀ����
    QLabel* typeLabel = new QLabel(QString::fromLocal8Bit("��Ŀ����: "), this);
    typeComboBox_ = new QComboBox(this);
    typeComboBox_->addItem(QString::fromLocal8Bit("�궨��Ŀ "), 1);
    typeComboBox_->addItem(QString::fromLocal8Bit("�Լ�������Ŀ "), 2);
    typeComboBox_->addItem(QString::fromLocal8Bit("VO��Ŀ "), 3);
    mainLayout->addWidget(typeLabel);
    mainLayout->addWidget(typeComboBox_);

    // ��Ŀ·��
    QLabel* pathLabel = new QLabel(QString::fromLocal8Bit("��Ŀ·��: "), this);
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

    // ȷ����ȡ����ť
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    okButton_ = new QPushButton(QString::fromLocal8Bit("ȷ�� "), this);
    cancelButton_ = new QPushButton(QString::fromLocal8Bit("ȡ�� "), this);
    connect(okButton_, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton_, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(okButton_);
    buttonLayout->addWidget(cancelButton_);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void NewProjectDialog::onSelectPath() {
    QString dir = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("ѡ����Ŀ·�� "), QString(), QFileDialog::ShowDirsOnly);
    if (!dir.isEmpty()) {
        pathEdit_->setText(dir);
    }
}

void NewProjectDialog::initializeProject(Project_Base& project) {
    if (nameEdit_->text().isEmpty() || pathEdit_->text().isEmpty()) {
        QMessageBox::warning(this, "����", "��Ŀ���ƺ�·������Ϊ�գ�");
        return;
    }
    // ��ʼ�� Project_Base ����
    project.setProName(nameEdit_->text());
    project.setProPath(pathEdit_->text());
    project.setProType(typeComboBox_->currentData().toInt());

    QString proFilePath = pathEdit_->text() + "/" + nameEdit_->text() + ".vopro";
    project.open_file(proFilePath);
    project.out_head_info();
    project.close();
}
