#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QChart>
#include <QWidget>
#include <QThread>
#include <QLabel>
#include <QPushButton>

#include <QLineSeries>
#include <QDateTimeAxis>
#include <QTimer>
#include <QSplineSeries>

#include <QValueAxis>
#include <QDateTime>
#include <QDebug>
#include <qchartview.h>

using namespace QtCharts;

class Environment : public QWidget
{
    Q_OBJECT
public:
    explicit Environment(QWidget *parent = 0);

    void layoutInit();

    QWidget *chartWidget;

    QChartView *chartview;

    QPushButton *exitPushButton;

    QLabel *title;
    QLabel *TempHumlbl;

    QLineSeries *temp_series;

    QLineSeries *hum_series;

    QDateTimeAxis *ax;

    QVector<QPointF> temperatureData;

    QTimer *timer;

    void TempChart();
    void HumChart();

public slots:
    void updateTemp(const QString &data);
};

#endif // ENVIRONMENT_H
