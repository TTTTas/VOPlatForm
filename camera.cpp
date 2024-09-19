#include "camera.h"
#include <iostream>

// TODO
// 1. ��δ����Ŀ�ӹ�

// ���캯������ʼ���ڲξ���ͻ������
Camera_Param::Camera_Param()
{
    // ��ʼ��Ϊ3x3��λ����Ϳվ���
    intrinsic = cv::Mat::eye(3, 3, CV_64F);
    distortion = cv::Mat::zeros(5, 1, CV_64F);
}

// ��������ڲξ���
void Camera_Param::setIntrinsic(const cv::Mat& intr)
{
    if (intr.rows == 3 && intr.cols == 3 && intr.type() == CV_64F) {
        intrinsic = intr.clone();
    }
    else {
        std::cerr << "Error: Invalid intrinsic matrix size or type!" << std::endl;
    }
}

// ��ȡ����ڲξ���
cv::Mat Camera_Param::getIntrinsic() const
{
    return intrinsic;
}

// ��������������
void Camera_Param::setDistortion(const cv::Mat& dist)
{
    if (dist.rows == 5 && dist.cols == 1 && dist.type() == CV_64F) {
        distortion = dist.clone();
    }
    else {
        std::cerr << "Error: Invalid distortion matrix size or type!" << std::endl;
    }
}

// ��ȡ����������
cv::Mat Camera_Param::getDistortion() const
{
    return distortion;
}

// ��ӡ�������
void Camera_Param::printCameraParams() const
{
    std::cout << "Intrinsic Matrix:" << std::endl;
    std::cout << intrinsic << std::endl;

    std::cout << "Distortion Coefficients:" << std::endl;
    std::cout << distortion << std::endl;
}

// ���캯������ʼ��Ϊ��λ����
Camera_Motion::Camera_Motion()
{
    // ��ʼ��Ϊ4x4��λ����
    pose = cv::Mat::eye(4, 4, CV_64F);
}

// �����������ת�����ƽ������
void Camera_Motion::setPose(const cv::Mat& R, const cv::Mat& t)
{
    if (R.rows == 3 && R.cols == 3 && R.type() == CV_64F &&
        t.rows == 3 && t.cols == 1 && t.type() == CV_64F)
    {
        pose(cv::Rect(0, 0, 3, 3)) = R.clone();   // ������ת����
        pose(cv::Rect(3, 0, 1, 3)) = t.clone();   // ����ƽ������
    }
    else
    {
        std::cerr << "Error: Invalid rotation or translation matrix!" << std::endl;
    }
}

// ��ȡ�����λ����ξ���
cv::Mat Camera_Motion::getPose() const
{
    return pose;
}

// ���������λ��
void Camera_Motion::updatePose(const cv::Mat& dR, const cv::Mat& dt)
{
    if (dR.rows == 3 && dR.cols == 3 && dR.type() == CV_64F &&
        dt.rows == 3 && dt.cols == 1 && dt.type() == CV_64F)
    {
        // ��������λ�˵���ξ���
        cv::Mat deltaPose = cv::Mat::eye(4, 4, CV_64F);
        deltaPose(cv::Rect(0, 0, 3, 3)) = dR.clone();
        deltaPose(cv::Rect(3, 0, 1, 3)) = dt.clone();

        // ���µ�ǰλ��
        pose = pose * deltaPose;
    }
    else
    {
        std::cerr << "Error: Invalid update matrices!" << std::endl;
    }
}

// ��ӡ�����λ��
void Camera_Motion::printPose() const
{
    std::cout << "Current Pose:" << std::endl;
    std::cout << pose << std::endl;
}

Camera_System::Camera_System()
{
    // ��ʼ�� Camera_System����ʼ������ģ��
}

Camera_Param& Camera_System::getCameraParam()
{
    return cameraParam;
}

const Camera_Param& Camera_System::getCameraParam() const
{
    return cameraParam;
}

Camera_Motion& Camera_System::getCameraMotion()
{
    return cameraMotion;
}

const Camera_Motion& Camera_System::getCameraMotion() const
{
    return cameraMotion;
}

void Camera_System::printCameraState() const
{
    std::cout << "Camera Parameters:" << std::endl;
    cameraParam.printCameraParams();

    std::cout << "Camera Motion:" << std::endl;
    cameraMotion.printPose();
}