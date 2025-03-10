#include "environment.h"
#include <QPainter>
#include <QRandomGenerator>

Environment::Environment(QWidget *parent) : QWidget(parent)
{
    layoutInit();//界面初始化
    this->setAttribute(Qt::WA_DeleteOnClose,false);//关闭界面，不释放资源
}


void Environment::layoutInit()
{
    this->setStyleSheet("background-color:#000000");
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->showFullScreen();

    exitPushButton= new QPushButton("返 回",this);
    exitPushButton->setGeometry(1550,50,300,100);
    exitPushButton->show();
    exitPushButton->setStyleSheet("color:black;border-radius:15px;font-size:60px;font-weight:bold;background-color:white");

    connect(exitPushButton,&QPushButton::clicked,this,[=](){
        this->hide();
    });

    TempHumlbl = new QLabel(this);
    TempHumlbl->setGeometry(100,50,1450,100);
    TempHumlbl->show();
    TempHumlbl->setStyleSheet("color:white;font-size:80px;letter-spacing:5px;font-weight:bold;");


    /*显示温度湿度曲线*/

    TempChart();

}

void Environment::TempChart()
{

    chartWidget = new QWidget(this);
    chartWidget->setGeometry(0,200,2000,1000);
    chartWidget->show();

    QChart *chart = new QChart();
    chartview = new QChartView(chart,chartWidget);
    chart->setTitle("温度(°C)/湿度(%)");/*设置图例标题*/
    chartview->resize(1850,900);
    chartview->setRenderHint(QPainter::Antialiasing, true);/*抗锯齿*/
    chartview->setChart(chart);
    chart->setBackgroundBrush(QBrush(QColor("#000000")));


    // 设置标题字体大小
    QFont titleFont = chart->titleFont();
    titleFont.setPointSize(30);
    titleFont.setWeight(5);
    chart->setTitleFont(titleFont);
    chart->setTitleBrush(QBrush(QColor("white")));

    QFont labelFont;
    labelFont.setPointSize(25);


    /*x轴*/
    ax = new QDateTimeAxis();
    ax->setTitleText("时间");
    ax->setLabelsColor(Qt::white);
    ax->setTickCount(15);
    ax->setLineVisible(true);
    ax->setGridLineVisible(true);
    ax->setFormat("hh:mm");
    ax->setRange(QDateTime::currentDateTime(), QDateTime::currentDateTime().addSecs(15));
    ax->setLabelsColor(Qt::white);
    ax->setLabelsFont(labelFont);
    ax->setTitleFont(labelFont);
    ax->setTitleBrush(QBrush(QColor("white")));

    /*y轴*/
    QValueAxis *ay = new QValueAxis();
    ay->setTitleText("温度/湿度");
    ay->setTickCount(15);
    ay->setLabelFormat("%.1f");//让y轴显示出小数部分
    ay->setRange(0, 100);
    ay->setLineVisible(true);
    ay->setGridLineVisible(true);
    ay->setLabelsColor(Qt::white);
    ay->setLabelsFont(labelFont);
    ay->setTitleFont(labelFont);
    ay->setTitleBrush(QBrush(QColor("white")));

    chartview->show();
    chart->show();

    // 获取图例对象
    QLegend *legend = chart->legend();

    // 创建字体对象并设置字体大小
    QFont legendFont;
    legendFont.setPointSize(22);  // 设置字体大小为 12

    // 设置图例的字体
    legend->setFont(legendFont);
    legend->setLabelBrush(QBrush(QColor("white")));
    /*温度曲线*/

    temp_series = new QLineSeries();
    temp_series->setName("温度");

    QPen pen(QColor(255, 200, 20),10);
    temp_series->setPen(pen);

    /*设置初始值*/

    temp_series->append(QDateTime::currentDateTime().toMSecsSinceEpoch(), 30);

    /*湿度曲线*/

    hum_series = new QLineSeries();
    hum_series->setName("湿度");
    hum_series->setColor(QColor(150, 100, 200));

    QPen pen1(QColor(150, 100, 200),10);
    hum_series->setPen(pen1);

    /*设置初始值*/

    hum_series->append(QDateTime::currentDateTime().toMSecsSinceEpoch(), 50);

    /*将温度曲线添加进chart*/

    chart->addSeries(temp_series);
    chart->setAxisX(ax, temp_series);
    chart->setAxisY(ay, temp_series);

    /*将湿度曲线添加进chart*/

    chart->addSeries(hum_series);
    chart->setAxisX(ax, hum_series);
    chart->setAxisY(ay, hum_series);

}


void Environment::updateTemp(const QString &data)
{
    // 提取温度
    QString temperature = data.section(':', 1).section(',', 0, 0);
    // 提取湿度
    QString humidity = data.section(':', 1).section(',', 1, 1);

    double temp = temperature.toDouble();
    double humi = humidity.toDouble();

    if(temp == 0 || humi == 0)
    {
        qDebug() << "data error";
    }else{
        // 更新温度曲线数据
        qint64 timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();

        /*显示当前温度和湿度*/
        QString TempHum = QString("当前温度:") + QString::number(temp, 'f', 1) + "°C "
                          + QString("当前湿度:") + QString::number(humi, 'f', 1) + "%";

        TempHumlbl->setText(TempHum);

        /*添加当前时间点的温度和湿度*/
        if (temp_series != nullptr && hum_series != nullptr)
        {
            temp_series->append(timestamp, temp);
            hum_series->append(timestamp, humi);
            if (temp_series->count() > 16)
            {
                /*移除第一个点*/
                temp_series->removePoints(0, 1);
                hum_series->removePoints(0, 1);

            }

            // 调整温度曲线的 x 轴范围
            if (temp_series->count() > 15)
            {
                ax->setRange(QDateTime::fromMSecsSinceEpoch(temp_series->at(0).x()),
                             QDateTime::fromMSecsSinceEpoch(temp_series->at(temp_series->count()-1).x()));
            }
        }
    }
}

