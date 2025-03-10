#ifndef CAMERA_H
#define CAMERA_H

#include <QThread>
#include <QImage>
#include <opencv4/opencv2/opencv.hpp>

class Camera : public QThread
{
    Q_OBJECT

public:
    explicit Camera(QObject *parent = nullptr);
    ~Camera();

    void startCapture();
    void stopCapture();

signals:
    void frameCaptured(const cv::Mat &frame); // 向推理线程发送新帧信号

private:
    cv::VideoCapture cap;
    bool running;

protected:
    void run() override;
};

#endif // CAMERA_H


// #ifndef CAMERA_H
// #define CAMERA_H

// #include <QThread>
// #include <QImage>
// #include <opencv4/opencv2/opencv.hpp>

// class Camera : public QThread
// {
//     Q_OBJECT

// public:
//     explicit Camera(QObject *parent = nullptr);
//     ~Camera();

//     void startCapture();
//     void stopCapture();

// signals:
//     void frameCaptured(cv::Mat frame);  // 修改后的信号，传递原始帧数据

// private:
//     cv::VideoCapture cap;
//     bool running;

// protected:
//     void run() override;
// };

// #endif // CAMERA_H
