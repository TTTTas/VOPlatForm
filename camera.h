#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <opencv2/opencv.hpp>

// TODO
// 1. ��Ҫ�����еķ��������޸�Ϊ���ı�������log������¼
// 2. ��Ҫ���Ӹ����ע��
// 3. Camera_System��ʼ������δ��ɣ���Ҫ��Ӹ����ʼ�������ô���
// 4.

class Camera_Param
{
public:
    // ���캯��
    Camera_Param();

    // ��������ڲξ���
    void setIntrinsic(const cv::Mat& intrinsic);

    // ��ȡ����ڲξ���
    cv::Mat getIntrinsic() const;

    // ��������������
    void setDistortion(const cv::Mat& distortion);

    // ��ȡ����������
    cv::Mat getDistortion() const;

    // ��ӡ�������
    void printCameraParams() const;

private:
    cv::Mat intrinsic;  // ����ڲξ���
    cv::Mat distortion; // ����������
};

class Camera_Motion
{
public:
    // ���캯��
    Camera_Motion();

    // ���õ�ǰ����ת�����ƽ������
    void setPose(const cv::Mat& R, const cv::Mat& t);

    // ��ȡ��ǰ��λ����ξ���
    cv::Mat getPose() const;

    // ���������λ�ˣ����������������ת��ƽ��
    void updatePose(const cv::Mat& dR, const cv::Mat& dt);

    // ��ӡ���λ��
    void printPose() const;

private:
    cv::Mat pose;  // ���λ�˵�4x4��ξ���
};

class Camera_System
{
public:
    Camera_System();

    // ��ȡ�������
    Camera_Param& getCameraParam();
    const Camera_Param& getCameraParam() const;

    // ��ȡ����˶�
    Camera_Motion& getCameraMotion();
    const Camera_Motion& getCameraMotion() const;

    // ��ӡ��ǰ���״̬
    void printCameraState() const;

private:
    Camera_Param cameraParam;   // ����ڲι���
    Camera_Motion cameraMotion; // ����˶�����
};

#endif