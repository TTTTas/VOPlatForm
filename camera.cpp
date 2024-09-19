#include "camera.h"
#include <iostream>

// TODO
// 1. 尚未与项目接轨

// 构造函数，初始化内参矩阵和畸变参数
Camera_Param::Camera_Param()
{
    // 初始化为3x3单位矩阵和空矩阵
    intrinsic = cv::Mat::eye(3, 3, CV_64F);
    distortion = cv::Mat::zeros(5, 1, CV_64F);
}

// 设置相机内参矩阵
void Camera_Param::setIntrinsic(const cv::Mat& intr)
{
    if (intr.rows == 3 && intr.cols == 3 && intr.type() == CV_64F) {
        intrinsic = intr.clone();
    }
    else {
        std::cerr << "Error: Invalid intrinsic matrix size or type!" << std::endl;
    }
}

// 获取相机内参矩阵
cv::Mat Camera_Param::getIntrinsic() const
{
    return intrinsic;
}

// 设置相机畸变参数
void Camera_Param::setDistortion(const cv::Mat& dist)
{
    if (dist.rows == 5 && dist.cols == 1 && dist.type() == CV_64F) {
        distortion = dist.clone();
    }
    else {
        std::cerr << "Error: Invalid distortion matrix size or type!" << std::endl;
    }
}

// 获取相机畸变参数
cv::Mat Camera_Param::getDistortion() const
{
    return distortion;
}

// 打印相机参数
void Camera_Param::printCameraParams() const
{
    std::cout << "Intrinsic Matrix:" << std::endl;
    std::cout << intrinsic << std::endl;

    std::cout << "Distortion Coefficients:" << std::endl;
    std::cout << distortion << std::endl;
}

// 构造函数，初始化为单位矩阵
Camera_Motion::Camera_Motion()
{
    // 初始化为4x4单位矩阵
    pose = cv::Mat::eye(4, 4, CV_64F);
}

// 设置相机的旋转矩阵和平移向量
void Camera_Motion::setPose(const cv::Mat& R, const cv::Mat& t)
{
    if (R.rows == 3 && R.cols == 3 && R.type() == CV_64F &&
        t.rows == 3 && t.cols == 1 && t.type() == CV_64F)
    {
        pose(cv::Rect(0, 0, 3, 3)) = R.clone();   // 设置旋转矩阵
        pose(cv::Rect(3, 0, 1, 3)) = t.clone();   // 设置平移向量
    }
    else
    {
        std::cerr << "Error: Invalid rotation or translation matrix!" << std::endl;
    }
}

// 获取相机的位姿齐次矩阵
cv::Mat Camera_Motion::getPose() const
{
    return pose;
}

// 更新相机的位姿
void Camera_Motion::updatePose(const cv::Mat& dR, const cv::Mat& dt)
{
    if (dR.rows == 3 && dR.cols == 3 && dR.type() == CV_64F &&
        dt.rows == 3 && dt.cols == 1 && dt.type() == CV_64F)
    {
        // 构造增量位姿的齐次矩阵
        cv::Mat deltaPose = cv::Mat::eye(4, 4, CV_64F);
        deltaPose(cv::Rect(0, 0, 3, 3)) = dR.clone();
        deltaPose(cv::Rect(3, 0, 1, 3)) = dt.clone();

        // 更新当前位姿
        pose = pose * deltaPose;
    }
    else
    {
        std::cerr << "Error: Invalid update matrices!" << std::endl;
    }
}

// 打印相机的位姿
void Camera_Motion::printPose() const
{
    std::cout << "Current Pose:" << std::endl;
    std::cout << pose << std::endl;
}

Camera_System::Camera_System()
{
    // 初始化 Camera_System，初始化各个模块
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