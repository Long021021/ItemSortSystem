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
        return;  //打开摄像头失败
    }

    while (true) {
        cv::Mat frame;
        if(cap.read(frame)){
            // cv::transpose(frame, frame);
            // cv::flip(frame, frame, 0);
            QImage tmpImage(frame.data, frame.cols, frame.rows,
                            QImage::Format_BGR888);

            if(!tmpImage.isNull())
                emit imageIsReady(tmpImage);
        }
    }

    cap.release();
}
