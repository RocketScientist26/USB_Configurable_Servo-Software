#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    //USB
    void USB_Detect_Timer_Ovf();
    void USB_RX_Timeout_Timer_Ovf();
    void USB_Poll_Timer_Ovf();
    void USB_Data_Received();
    void USB_Error(QSerialPort::SerialPortError error);
    //Graph
    void on_actionGraph_ZoomAction_triggered();
    //UI
    void UI_PID_Test_Oscillate_Timer_Overflowed();
    void on_actionPotentiometer_Set_StartAction_triggered();
    void on_actionPotentiometer_Set_EndAction_triggered();
    void on_actionPotentiometer_Start_margin_SpinboxAction_triggered();
    void on_actionPotentiometer_End_margin_SpinboxAction_triggered();
    void on_actionMotor_Max_Power_ChangedAction_triggered();
    void on_actionPID_Test_OscillateAction_triggered();
    void on_actionRevert_flash_triggered();
    void on_actionUpdate_flash_triggered();
    void on_actionSignal_Test_HoldAction_triggered();
    void on_actionSignal_Min_MaxAction_triggered();
    void on_actionDevice_Signal_Ignore_On_USBAction_triggered();
    void on_actionSave_Configuration_to_file_triggered();
    void on_actionLoad_configuration_triggered();
    void on_actionAbout_triggered();
    void on_actionManual_triggered();

private:
    Ui::MainWindow *ui;
    //CRC
    uint32_t CRC32_Get(uint32_t *data, uint32_t length);
    //USB
    void USB_Init();
    void USB_Connected();
    void USB_Disconnected();
    void USB_Send();
    //Graph
    void Graph_Init();
    void Graph_Clear();
    void Graph_Append(float setpos, float actpos, int16_t mpwr);
    //Parser
    void Parser_Request_Config(uint8_t revert_flash);
    void Parser_Parse_Config();
    void Parser_Request_Status();
    void Parser_Parse_Status();
    //UI
    void UI_Init();
    void UI_Clear();
    void UI_Enble(bool enable);
};
#endif // MAINWINDOW_H
