#include "graph.h"

/*!
    Constructor function, sets up empty chart
*/
Graph::Graph(QChartView *chart)
{
    //Enable OpenGL and antialiasing
    set_position_series->setUseOpenGL(true);
    actual_position_series->setUseOpenGL(true);
    motor_power_series->setUseOpenGL(true);
    chart->setRenderHint(QPainter::Antialiasing);

    //axes ranges and texts
    axisX->setRange(0, X_AXIS_MAX_SEC);
    axisX->setLabelFormat("%d");
    axisX->setTitleText("Time - Sec.");

    axisY->setRange(0, 100);
    axisY->setLabelFormat("%d ");
    axisY->setTitleText("Position - %");

    axisZ->setRange(-100, 100);
    axisZ->setLabelFormat("%d ");
    axisZ->setTitleText("Motor power - %");

    //Add series to chart
    chart->chart()->addSeries(set_position_series);
    chart->chart()->addSeries(actual_position_series);
    chart->chart()->addSeries(motor_power_series);

    //Add axes to chart
    chart->chart()->addAxis(axisY, Qt::AlignLeft);
    chart->chart()->addAxis(axisX, Qt::AlignBottom);
    chart->chart()->addAxis(axisZ, Qt::AlignRight);

    //Attach series to axes
    set_position_series->attachAxis(axisX);
    set_position_series->attachAxis(axisY);

    actual_position_series->attachAxis(axisX);
    actual_position_series->attachAxis(axisY);

    motor_power_series->attachAxis(axisX);
    motor_power_series->attachAxis(axisZ);

    //Configure legend
    chart->chart()->legend()->setAlignment(Qt::AlignBottom);
    chart->chart()->legend()->setFont(QFont("LM Mono 10", 9, 500, false));
    chart->chart()->legend()->show();

    set_position_series->setName("Set position: 000.0 %");
    actual_position_series->setName("Actual position: 000.0 %");
    motor_power_series->setName("Motor power: +000.0 %");
}

/*!
    Destructor, we should free memory of used objects
*/
Graph::~Graph()
{
    delete set_position_series;
    delete actual_position_series;
    delete motor_power_series;
    delete axisX;
    delete axisY;
    delete axisZ;
}

/*!
    Clears graph and sets X axis counter to zero
*/
void Graph::clear()
{
    set_position_series->clear();
    actual_position_series->clear();
    motor_power_series->clear();
    cx_sec = 0;
}

/*!
    Appends values to chart, at "current position + xsec" position. Where "xsec" is time in seconds
*/
void Graph::append(qreal setpos, qreal actpos, qreal mpwr, qreal xsec)
{
    //Clear graph if buffer reached max limit or clipped end limit
    if(
        //If any of values got X >= X_AXIS_MAX_SEC seconds
        (set_position_series->at(set_position_series->count() - 1).x() >= X_AXIS_MAX_SEC) ||
        (actual_position_series->at(actual_position_series->count() - 1).x() >= X_AXIS_MAX_SEC) ||
        (motor_power_series->at(motor_power_series->count() - 1).x() >= X_AXIS_MAX_SEC) ||
        (
            //Or if clipping buffer is enabled and any of values reached horizontal X axis value currently zoomed to
            clipbuff &&
            (
                (set_position_series->at(set_position_series->count() - 1).x() >= cx_max_sec) ||
                (actual_position_series->at(actual_position_series->count() - 1).x() >= cx_max_sec) ||
                (motor_power_series->at(motor_power_series->count() - 1).x() >= cx_max_sec)
            )
        )
    )
    {
        clear();
    }

    //Prevent values from going beyond limits
    if(setpos < 0){
        setpos = 0;
    }else if(setpos > 100){
        setpos = 100;
    }
    if(actpos < 0){
        actpos = 0;
    }else if(actpos > 100){
        actpos = 100;
    }

    //Store updated X position
    if(
        set_position_series->count() &&
        actual_position_series->count() &&
        motor_power_series->count()
    ){
        cx_sec += xsec;
    }

    //Append data to graph
    set_position_series->append(cx_sec, setpos);
    actual_position_series->append(cx_sec, actpos);
    motor_power_series->append(cx_sec, mpwr);

    /*
        Update graph labels with fixed character count
    */

    //Set position
    QString value = QString::number(setpos, 'f', 1);
    if(value.length() < 5){
        value.prepend("0");
    }
    if(value.length() < 5){
        value.prepend("0");
    }
    set_position_series->setName(QString("Set position: ").append(value).append(" %"));

    //Actual position
    value = QString::number(actpos, 'f', 1);
    if(value.length() < 5){
        value.prepend("0");
    }
    if(value.length() < 5){
        value.prepend("0");
    }
    actual_position_series->setName(QString("Actual position: ").append(value).append(" %"));

    //Motor power
    value = QString::number(mpwr, 'f', 1);
    if(mpwr >= 0){
        if(value.length() < 5){//000.0
            value.prepend("0");
        }
        if(value.length() < 5){
            value.prepend("0");
        }
        value.prepend("+");
    }else{
        if(value.length() < 6){//-000.0
            value.insert(1, '0');
        }
        if(value.length() < 6){
            value.insert(1, '0');
        }
    }
    motor_power_series->setName(QString("Motor power: ").append(value).append(" %"));
}

/*!
    Sets graph buffer to clip X end to currently zoomed value
*/
void Graph::clipBufferEnd(bool clip)
{
    clipbuff = clip;
}

/*!
    Zooms graph
*/
void Graph::zoom(qreal v_min, qreal v_max, qreal h_min, qreal h_max)
{
    //Store currently zoomed X axis maximum for buffer clipping
    cx_max_sec = h_max;

    //Set ranges for axes
    axisY->setRange(v_min, v_max);
    axisZ->setRange((v_min * 2) - 100.0f, (v_max * 2) - 100.0f);
    axisX->setRange(h_min, h_max);

    //Set label format "0.1" or "1" for X axis
    if(h_max - h_min < 10.0f){
        axisX->setLabelFormat("%.1f");
    }else{
        axisX->setLabelFormat("%d");
    }

    //Set label format "0.1" or "1" for Y and Z axes
    if(v_max - v_min < 10.0f){
        axisY->setLabelFormat("%.1f ");
        axisZ->setLabelFormat("%.1f ");
    }else{
        axisY->setLabelFormat("%d ");
        axisZ->setLabelFormat("%d ");
    }
}
