//! Class draws set position, actual position and motor power series on chart, handles zoom

#ifndef GRAPH_H
#define GRAPH_H

#include <QObject>
#include <QtCharts>

class Graph : public QObject
{
    Q_OBJECT
    public:
        //Axis X maximum
        const qreal X_AXIS_MAX_SEC = 60.0;

        explicit Graph(QChartView *chart = nullptr);
        ~Graph();

        void clear();
        void append(qreal setpos, qreal actpos, qreal mpwr, qreal xsec);
        void zoom(qreal v_min, qreal v_max, qreal h_min, qreal h_max);
        void clipBufferEnd(bool clip);

    private:
        QLineSeries *set_position_series = new QLineSeries();
        QLineSeries *actual_position_series = new QLineSeries();
        QLineSeries *motor_power_series = new QLineSeries();
        QValueAxis *axisX = new QValueAxis;
        QValueAxis *axisY = new QValueAxis;
        QValueAxis *axisZ = new QValueAxis;

        //Axis X maximum currently zoomed
        qreal cx_max_sec = X_AXIS_MAX_SEC;
        //Current X axis value
        qreal cx_sec = 0;
        //Clip buffer to currently zoomed X axis maximum or not
        bool clipbuff = false;
};

#endif // GRAPH_H
