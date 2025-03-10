#include "weatherinfo.h"
#include <QThread>
#include <QEventLoop>
#include <QDebug>

WeatherInfo::WeatherInfo(QObject *parent) : QObject(parent)
{
    btn_fresh = false;
}

void WeatherInfo::getWeatherInfo()
{
    weatherTimer = new QTimer(this);
    connect(weatherTimer,&QTimer::timeout,this,[=]{
        netWorkManager = new QNetworkAccessManager(this);
        netWorkManager->get(QNetworkRequest(QUrl(QUERY_WEATHER_URL)));
        cityName = "福州";
        connect(netWorkManager,SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinished_getWeather(QNetworkReply*)));
    });
    weatherTimer->start(5000);
}


void WeatherInfo::replyFinished_getWeather(QNetworkReply* reply)
{
    QTextCodec*textCodec = QTextCodec::codecForName("utf8");
    //使用utf8编码，这样才可以显示中文
    QString weatherInfo = textCodec->toUnicode(reply->readAll());
    reply->deleteLater();//最后要释放reply对象

    /*     解析天气的json数据        */
    jsonDocument = QJsonDocument::fromJson(weatherInfo.toUtf8(),&jsonError);
    if(jsonError.error==QJsonParseError::NoError)
    {
        if(jsonDocument.isNull()||jsonDocument.isEmpty())
        {
            emit updateWeather("null","null","border-image:/image_wea/image/refresh.png","null");

        }else {
            if(jsonDocument.isObject())
            {
                jsonObject = jsonDocument.object();

                QJsonArray livesArray = jsonObject.value("lives").toArray();

                weather_data = livesArray.at(0).toObject().value("temperature").toString()+"℃";
                //qDebug() << livesArray.at(0).toObject().value("temperature").toString();

                weatherInfo = livesArray.at(0).toObject().value("weather").toString();
                if(weatherInfo=="大雨") imagePath = QString("border-image:url("":/image_wea/image/heavy.png"")");
                else if(weatherInfo=="小雨") imagePath = QString("border-image:url("":/image_wea/image/lightrain.png"")");
                else if(weatherInfo=="多云") imagePath = QString("border-image:url("":/image_wea/image/cloudy.png"")");
                else if(weatherInfo=="阵雨") imagePath = QString("border-image:url("":/image_wea/image/shower.png"")");
                else if(weatherInfo=="中雨") imagePath = QString("border-image:url("":/image_wea/image/middlerain.png"")");
                else if(weatherInfo=="晴") imagePath = QString("border-image:url("":/image_wea/image/sunny.png"")");
                else if(weatherInfo=="雷阵雨") imagePath = QString("border-image:url("":/image_wea/image/thundershower.png"")");
                else imagePath = QString("border-image:url("":/image_wea/image/cloudy.png"")");
                weatherTimer->stop();
                qDebug() << "Get Weather Info";
                emit updateWeather(cityName,weather_data,imagePath,weatherInfo);//emit signal to mainThread
            }
        }

    }
}
