#include "camera.h"
#include <QDebug>

Camera::Camera(QObject *parent)
    : QThread(parent), running(false)
{
}

Camera::~Camera()
{
    stopCapture();
    wait();
}

void Camera::startCapture()
{
    running = true;
    start();
}

void Camera::stopCapture()
{
    running = false;
    wait(); // 等待线程结束
}

void Camera::run()
{
    cv::VideoCapture cap(0); // RK3568MIPI 摄像头是 0 usb 摄像头是 9。其他板子请参考
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);

    if (!cap.isOpened()) {
        qDebug() << "Failed to open camera";
        return;  // 打开摄像头失败
    }

    while (running) {
        cv::Mat frame;
        if (cap.read(frame)) {
            emit frameCaptured(frame);
        }
    }

    cap.release();
}


// #include "camera.h"
// #include <QDebug>

// Camera::Camera(QObject *parent)
//     : QThread(parent), running(false)
// {
// }

// Camera::~Camera()
// {
//     stopCapture();
//     wait();
// }

// void Camera::startCapture()
// {
//     running = true;
//     start();
// }

// void Camera::stopCapture()
// {
//     running = false;
//     wait();
// }

// void Camera::run()
// {
//     cv::VideoCapture cap(0);
//     cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
//     cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);

//     if (!cap.isOpened()) {
//         qDebug() << "Failed to open camera";
//         return;
//     }

//     cv::Mat frame;
//     while(running) {
//         if(cap.read(frame)) {
//             if(!frame.empty()) {
//                 emit frameCaptured(frame.clone());  // 发送克隆的帧数据
//             }
//         }
//         QThread::usleep(1000); // 防止CPU占用过高
//     }
//     cap.release();
// }
