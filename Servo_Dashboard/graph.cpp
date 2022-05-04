#include <QChartView>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "usb.h"

extern uint32_t usb_poll_time_ms;

QLineSeries *set_position_series;
QLineSeries *actual_position_series;
QLineSeries *motor_power_series;
QValueAxis *axisX;
QValueAxis *axisY;
QValueAxis *axisZ;

float graph_act_x = 0;

void MainWindow::Graph_Init(){
    set_position_series = new QLineSeries();
    set_position_series->setName("Set position: 000.0 %");
    set_position_series->setUseOpenGL(true);
    actual_position_series = new QLineSeries();
    actual_position_series->setName("Actual position: 000.0 %");
    actual_position_series->setUseOpenGL(true);
    motor_power_series = new QLineSeries();
    motor_power_series->setName("Motor power: +000.0 %");
    motor_power_series->setUseOpenGL(true);
    ui->chart_Device->setRenderHint(QPainter::Antialiasing);
    ui->chart_Device->chart()->addSeries(set_position_series);
    ui->chart_Device->chart()->addSeries(actual_position_series);
    ui->chart_Device->chart()->addSeries(motor_power_series);
    ui->chart_Device->chart()->legend()->show();
    ui->chart_Device->chart()->legend()->setAlignment(Qt::AlignBottom);
    ui->chart_Device->chart()->legend()->setFont(QFont("LM Mono 10", 9, 500, false));
    axisX = new QValueAxis;
    axisY = new QValueAxis;
    axisZ = new QValueAxis;
    ui->chart_Device->chart()->addAxis(axisY, Qt::AlignLeft);
    ui->chart_Device->chart()->addAxis(axisX, Qt::AlignBottom);
    ui->chart_Device->chart()->addAxis(axisZ, Qt::AlignRight);
    axisX->setRange(0, 60);
    axisY->setRange(0, 100);
    axisZ->setRange(-100, 100);
    axisX->setLabelFormat("%d");
    axisX->setTitleText("Time - Sec.");
    axisY->setLabelFormat("%d ");
    axisY->setTitleText("Position - %");
    axisZ->setLabelFormat("%d ");
    axisZ->setTitleText("Motor power - %");
    set_position_series->attachAxis(axisX);
    set_position_series->attachAxis(axisY);
    actual_position_series->attachAxis(axisX);
    actual_position_series->attachAxis(axisY);
    motor_power_series->attachAxis(axisX);
    motor_power_series->attachAxis(axisZ);
}
void MainWindow::Graph_Clear(){
    set_position_series->clear();
    actual_position_series->clear();
    motor_power_series->clear();
    graph_act_x = 0;
}
void MainWindow::Graph_Append(float setpos, float actpos, int16_t mpwr){
    if(usb_poll_time_ms < USB_STATUS_POLL_MS){
        return;
    }
    if(!ui->pushButton_Device_Pause_Resume->isChecked()){
        if((set_position_series->at(set_position_series->count() - 1).x() >= 60.0f)||(actual_position_series->at(actual_position_series->count() - 1).x() >= 60.0f)||(motor_power_series->at(motor_power_series->count() - 1).x() >= 60.0f)
                ||(ui->checkBox_Device_Buffer_End->isChecked() &&
                   ((set_position_series->at(set_position_series->count() - 1).x() >= ui->doubleSpinBox_Device_Horizontal_End->value())
                    ||(actual_position_series->at(actual_position_series->count() - 1).x() >= ui->doubleSpinBox_Device_Horizontal_End->value())
                    ||(motor_power_series->at(motor_power_series->count() - 1).x() >= ui->doubleSpinBox_Device_Horizontal_End->value())))){
            Graph_Clear();
        }
        qreal set_position_percent = (qreal)((setpos - ui->spinBox_Potentiometer_Start_Margin->value())/((ui->spinBox_Potentiometer_End_Margin->value() - ui->spinBox_Potentiometer_Start_Margin->value())/100.0f));
        qreal act_position_percent = (qreal)((actpos - ui->spinBox_Potentiometer_Start_Margin->value())/((ui->spinBox_Potentiometer_End_Margin->value() - ui->spinBox_Potentiometer_Start_Margin->value())/100.0f));
        if(set_position_percent < 0){
            set_position_percent = 0;
        }else if(set_position_percent > 100){
            set_position_percent = 100;
        }
        if(act_position_percent < 0){
            act_position_percent = 0;
        }else if(act_position_percent > 100){
            act_position_percent = 100;
        }
        qreal power_percent = (qreal)((float)mpwr/10.0f);
        set_position_series->append((qreal)graph_act_x, set_position_percent);
        actual_position_series->append((qreal)graph_act_x, act_position_percent);
        motor_power_series->append((qreal)graph_act_x, power_percent);
        graph_act_x += usb_poll_time_ms * 0.001;

        //Update graph labels with fixed length
        QString label;
        QString value;
        label.append("Set position: ");
        value.append(QString::number(set_position_percent, 'f', 1));
        if(value.length() < 5){
            value.prepend("0");
        }
        if(value.length() < 5){
            value.prepend("0");
        }
        label.append(value).append(" %");
        set_position_series->setName(label);

        label.clear();
        value.clear();
        label.append("Actual position: ");
        value.append(QString::number(act_position_percent, 'f', 1));
        if(value.length() < 5){
            value.prepend("0");
        }
        if(value.length() < 5){
            value.prepend("0");
        }
        label.append(value).append(" %");
        actual_position_series->setName(label);

        label.clear();
        value.clear();
        label.append("Motor power: ");
        value.append(QString::number(power_percent, 'f', 1));
        if(power_percent >= 0){
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
        label.append(value).append(" %");
        motor_power_series->setName(label);
    }
}

void MainWindow::on_actionGraph_ZoomAction_triggered(){
    axisY->setRange(ui->doubleSpinBox_Device_Vertical_Start->value(), ui->doubleSpinBox_Device_Vertical_End->value());
    axisZ->setRange((ui->doubleSpinBox_Device_Vertical_Start->value() * 2) - 100.0f, (ui->doubleSpinBox_Device_Vertical_End->value() * 2) - 100.0f);
    axisX->setRange(ui->doubleSpinBox_Device_Horizontal_Start->value(), ui->doubleSpinBox_Device_Horizontal_End->value());

    ui->doubleSpinBox_Device_Vertical_Start->setMaximum(ui->doubleSpinBox_Device_Vertical_End->value() - 1.0f);
    ui->doubleSpinBox_Device_Horizontal_Start->setMaximum(ui->doubleSpinBox_Device_Horizontal_End->value() - 1.0f);
    ui->doubleSpinBox_Device_Vertical_End->setMinimum(ui->doubleSpinBox_Device_Vertical_Start->value() + 1.0f);
    ui->doubleSpinBox_Device_Horizontal_End->setMinimum(ui->doubleSpinBox_Device_Horizontal_Start->value() + 1.0f);

    if(ui->doubleSpinBox_Device_Vertical_End->value() - ui->doubleSpinBox_Device_Vertical_Start->value() < 10.0f){
        axisY->setLabelFormat("%.1f ");
        axisZ->setLabelFormat("%.1f ");
    }else{
        axisY->setLabelFormat("%d ");
        axisZ->setLabelFormat("%d ");
    }
    if(ui->doubleSpinBox_Device_Horizontal_End->value() - ui->doubleSpinBox_Device_Horizontal_Start->value() < 10.0f){
        axisX->setLabelFormat("%.1f");
    }else{
        axisX->setLabelFormat("%d");
    }
}
