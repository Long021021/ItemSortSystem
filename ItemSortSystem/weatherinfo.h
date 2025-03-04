#ifndef WEATHERINFO_H
#define WEATHERINFO_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QNetworkAccessManager>
#include <QTextCodec>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTimer>
#define QUERY_WEATHER_URL "https://restapi.amap.com/v3/weather/weatherInfo?key=b0d6767d35e194ebae473511643f3e06&city=350100"

class WeatherInfo : public QObject
{
    Q_OBJECT
public:
    explicit WeatherInfo(QObject *parent = nullptr);
    QNetworkAccessManager *netWorkManager;
    QTextCodec *textCodec;
    QJsonDocument jsonDocument;
    QJsonObject jsonObject;
    QJsonParseError jsonError;
    QString cityName;
    QString imagePath;
    QString weather_data;
    int  count;
    QTimer *weatherTimer;
    bool first_fresh;
    bool btn_fresh;
signals:
    void updateWeather(QString cityName,QString cityTemperture,QString weatherStatus,QString weatherImagePath);
public slots:
    void replyFinished_getWeather(QNetworkReply *reply);
    void getWeatherInfo(void);
    void startTimer() {
        weatherTimer->start(5000);  // 启动定时器，5秒触发一次
    }
};

#endif // WEATHERINFO_H
