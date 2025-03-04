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
    void imageIsReady(const QImage &frame); // 向主线程发送新帧信号

private:
    cv::VideoCapture cap;
    bool running;

protected:
    void run() override;
};

#endif // CAMERA_H
