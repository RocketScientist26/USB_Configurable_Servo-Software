//! Class draws set position, actual position and motor power series on chart, handles zoom

#ifndef GRAPH_H
#define GRAPH_H

#include <QObject>
#include <QtCharts>

class Graph : public QObject
{
    Q_OBJECT
    public:
        explicit Graph(QChartView *chart = new QChartView());

        void clear();
        void append(qreal setpos, qreal actpos, qreal mpwr, float xsec);
        void zoom(float v_min, float v_max, float h_min, float h_max);
        void clipBufferEnd(bool clip);

    private:
        QLineSeries *set_position_series = new QLineSeries();
        QLineSeries *actual_position_series = new QLineSeries();
        QLineSeries *motor_power_series = new QLineSeries();
        QValueAxis *axisX = new QValueAxis;
        QValueAxis *axisY = new QValueAxis;
        QValueAxis *axisZ = new QValueAxis;

        //Axis X maximum
        #define X_AXIS_MAX_SEC 60.0f

        //Axis X maximum currently zoomed
        float cx_max_sec = X_AXIS_MAX_SEC;
        //Current X axis value
        float cx_sec = 0;
        //Clip buffer to currently zoomed X axis maximum or not
        bool clipbuff = false;
};

#endif // GRAPH_H
