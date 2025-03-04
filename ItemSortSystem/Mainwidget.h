#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QDateTime>
#include <QLabel>
#include <QTimer>
#include <QThread>
#include <weatherinfo.h>
#include <QPushButton>
#include <camera.h>
#include <environment.h>
#include "ThreadedTcpServer.h"

class Widget : public QWidget
{
    Q_OBJECT

public:

    Widget(QWidget *parent = nullptr);
    ~Widget();

    void layoutInit();
    /************************时钟界面************************************/

    /*   时钟定时器   */
    QTimer *clock_timer;

    /* 保存当前时间  */
    QDateTime currentDateTime;

    /*   显示时间标签   */
    QLabel *time_QLabel;

    /*   显示日期标签   */
    QLabel *date_QLabel;

    /*   显示上下午标签   */
    QLabel *ampm_QLabel;

    /*   显示时间日期分隔线标签   */
    QLabel *line_QLabel;

    /************************天气界面************************************/

    bool eventFilter(QObject *obj, QEvent *e);

    QLabel *weather_cityName_QLabel;

    QLabel *weather_temperature_QLabel;

    QLabel *weather_image_QLabel;

    QLabel *weather_imageName_QLabel;

    QThread *weatherInfoThread ;

    WeatherInfo *weatherInfo;

    QPushButton *environmentPushButton;

    QPushButton *controlPushButton;

    /*************************摄像头界面*****************************************/

    QLabel *video_display;

    QThread *cameraInfoThread;

    Camera *camera;

    /*************************environment界面*****************************************/

    Environment *environment;


private:

};
#endif // MAINWIDGET_H
