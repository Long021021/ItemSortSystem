#include "Mainwidget.h"
#include <QGuiApplication>
#include <QScreen>
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{

    this->setStyleSheet("background-color:rgb(30,41,61)");

    layoutInit();//初始化界面
}


void Widget::layoutInit()
{
    /******************界面时钟功能**********************/
    clock_timer = new QTimer(this);//创建一个定时器对象
    time_QLabel = new QLabel(this);//显示标签
    ampm_QLabel = new QLabel(this);//显示上下午标签
    line_QLabel = new QLabel(this);//显示横线的标签
    date_QLabel = new QLabel(this);//显示日期的标签

    clock_timer->start(1000);//开启时钟定时

    connect(clock_timer,&QTimer::timeout,time_QLabel,[=](){//定时器发出timeout信号
        currentDateTime = QDateTime::currentDateTime();
        // qDebug()<<QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz dddd");
        time_QLabel->setText(currentDateTime.toString("hh:mm"));
        ampm_QLabel->setText(currentDateTime.toString("A"));
        date_QLabel->setText(currentDateTime.toString("yyyy 年 MM 月 dd 日"));
        line_QLabel->setText("————————");
    });


    time_QLabel->setGeometry(100,100,350,100);
    time_QLabel->setStyleSheet("font-size:120px;color:white");

    ampm_QLabel->setAlignment(Qt::AlignCenter);
    ampm_QLabel->setGeometry(450,100,200,100);
    ampm_QLabel->setStyleSheet("font-size:100px;color:white");

    line_QLabel->setAlignment(Qt::AlignCenter);
    line_QLabel->setGeometry(100,220,510,2);
    line_QLabel->setStyleSheet("border: 5px solid;border-color:white;font-size:20px;color:white");

    date_QLabel->setAlignment(Qt::AlignCenter);
    date_QLabel->setGeometry(100,250,500,45);
    date_QLabel->setStyleSheet("font-size:50px;color:white");


    /********************天气界面***********************/
    weather_cityName_QLabel = new QLabel("位置",this);//显示城市名称
    weather_cityName_QLabel->setGeometry(100,380,200,95);
    weather_cityName_QLabel->setAlignment(Qt::AlignCenter);
    weather_cityName_QLabel->setStyleSheet("font-size:100px;color:white;");
    weather_cityName_QLabel->installEventFilter(this);//给控件安装事件过滤器,给标签增加点击功能

    weather_temperature_QLabel = new QLabel("℃",this);//显示城市天气温度
    weather_temperature_QLabel->setGeometry(100,500,200,95);
    weather_temperature_QLabel->setAlignment(Qt::AlignCenter);
    weather_temperature_QLabel->setStyleSheet("font-size:90px;color:white;");
    weather_temperature_QLabel->installEventFilter(this);

    weather_image_QLabel = new QLabel(this);//显示天气状态图片
    weather_image_QLabel->setGeometry(400,350,150,150);
    weather_image_QLabel->setAlignment(Qt::AlignCenter);
    weather_image_QLabel->setStyleSheet("border-image:url("":/image_wea/image/refresh.png"")");
    weather_image_QLabel->installEventFilter(this);

    weather_imageName_QLabel = new QLabel("点击刷新",this);//显示天气状态文字
    weather_imageName_QLabel->setGeometry(335,485,285,120);
    weather_imageName_QLabel->setAlignment(Qt::AlignCenter);
    weather_imageName_QLabel->setStyleSheet("font-size:75px;color:white;");
    weather_imageName_QLabel->installEventFilter(this);

    weatherInfoThread = new QThread;//获取天气信息的子线程
    weatherInfo = new WeatherInfo;//创建天气信息对象
    weatherInfo->moveToThread(weatherInfoThread);//对象移动到线程中
    weatherInfoThread->start();//启动线程，但是不会执行获取天气函数，需要通过信号触发getWeatherInfo

    connect(weatherInfoThread, &QThread::finished, weatherInfoThread, &QObject::deleteLater);
    connect(weatherInfoThread,&QThread::started,weatherInfo,&WeatherInfo::getWeatherInfo);

    connect(weatherInfo,&WeatherInfo::updateWeather,this,[=](QString cityName,QString cityTemperture,QString weatherImagePath,QString weatherStatus){
        weather_cityName_QLabel->setText(cityName);
        weather_temperature_QLabel->setText(cityTemperture);
        weather_imageName_QLabel->setText(weatherStatus);
        weather_image_QLabel->setStyleSheet(weatherImagePath);
    });

    /* environment */

    environmentPushButton = new QPushButton("温湿度检测",this);
    environmentPushButton->setGeometry(100,650,500,175);
    environmentPushButton->setStyleSheet("border-image:url("":/image_app/image/environment.png"");font-weight:bold;border-radius:15px;text-align:bottom;color:white;font-size:60px;padding-top:100px;");
    environment = new Environment();
    environment->hide();
    connect(environmentPushButton,&QPushButton::clicked,this,[=](){
        if(environment!=nullptr)
        {
            environment->show();

        }
    });

    ThreadedTcpServer *server = new ThreadedTcpServer(environment);

    if (!server->listen(QHostAddress::Any, 1123)) {
        qCritical() << "Server failed to listen:" << server->errorString();
        return;
    } else {
        qDebug() << "Server started on port 1123";
    }


    /* handcontrol */
    controlPushButton= new QPushButton("家电控制",this);
    controlPushButton->setGeometry(100,870,500,180);
    controlPushButton->setStyleSheet("border-image:url("":/image_app/image/control.png"");border-radius:15px;text-align:bottom;color:white;font-size:60px;padding-top:110px;font-weight:bold;");
    // connect(controlPushButton,&QPushButton::clicked,this,[=](){
    //    homeControl = new HomeControl();
    // });


    /**************************摄像头界面****************************/

    video_display = new QLabel(this);
    video_display->setGeometry(700,95,1150,920);
    video_display->setStyleSheet("border:1px solid;border-color:white;background-color:black;font-size: 100px;color:white");
    video_display->setContentsMargins(0,0,0,0);
    video_display->setAlignment(Qt::AlignCenter);
    video_display->setText("打开摄像头失败");


    cameraInfoThread = new QThread;
    camera = new Camera;
    camera->moveToThread(cameraInfoThread);
    cameraInfoThread->start();

    connect(cameraInfoThread, &QThread::finished, cameraInfoThread, &QObject::deleteLater);
    connect(cameraInfoThread,&QThread::started,camera,&Camera::startCapture);

    connect(camera,&Camera::imageIsReady,this,[=](const QImage &frame){
        if(!frame.isNull())
        {
            QImage rotatedImage = frame.transformed(QTransform().rotate(270));
            video_display->setPixmap(QPixmap::fromImage(rotatedImage));
        }else{
            video_display->clear();
            video_display->setText("打开摄像头失败");
        }
    });

}


/**************************事件过滤******************************/

bool Widget::eventFilter(QObject * watched, QEvent * event)
{
   if (watched == weather_cityName_QLabel||watched == weather_temperature_QLabel||watched == weather_image_QLabel||watched == weather_imageName_QLabel)
   {
       if (event->type() == QEvent::MouseButtonPress)
       {
           // 安全地调用 startTimer 槽函数 获取天气
           QMetaObject::invokeMethod(weatherInfo, "startTimer", Qt::QueuedConnection);
       }
   }
   return false;//处理完事件后，不需要事件继续传播

}



Widget::~Widget() {}
