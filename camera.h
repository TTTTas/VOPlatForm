#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <opencv2/opencv.hpp>

// TODO
// 1. 需要将所有的返回类型修改为长文本，便于log输出与记录
// 2. 需要增加更多的注释
// 3. Camera_System初始化代码未完成，需要添加更多初始化与设置代码
// 4.

class Camera_Param
{
public:
    // 构造函数
    Camera_Param();

    // 设置相机内参矩阵
    void setIntrinsic(const cv::Mat& intrinsic);

    // 获取相机内参矩阵
    cv::Mat getIntrinsic() const;

    // 设置相机畸变参数
    void setDistortion(const cv::Mat& distortion);

    // 获取相机畸变参数
    cv::Mat getDistortion() const;

    // 打印相机参数
    void printCameraParams() const;

private:
    cv::Mat intrinsic;  // 相机内参矩阵
    cv::Mat distortion; // 相机畸变参数
};

class Camera_Motion
{
public:
    // 构造函数
    Camera_Motion();

    // 设置当前的旋转矩阵和平移向量
    void setPose(const cv::Mat& R, const cv::Mat& t);

    // 获取当前的位姿齐次矩阵
    cv::Mat getPose() const;

    // 更新相机的位姿，输入的是增量的旋转和平移
    void updatePose(const cv::Mat& dR, const cv::Mat& dt);

    // 打印相机位姿
    void printPose() const;

private:
    cv::Mat pose;  // 相机位姿的4x4齐次矩阵
};

class Camera_System
{
public:
    Camera_System();

    // 获取相机参数
    Camera_Param& getCameraParam();
    const Camera_Param& getCameraParam() const;

    // 获取相机运动
    Camera_Motion& getCameraMotion();
    const Camera_Motion& getCameraMotion() const;

    // 打印当前相机状态
    void printCameraState() const;

private:
    Camera_Param cameraParam;   // 相机内参管理
    Camera_Motion cameraMotion; // 相机运动管理
};

#endif