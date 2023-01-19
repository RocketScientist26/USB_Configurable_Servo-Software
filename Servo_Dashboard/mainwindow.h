#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QDesktopServices>
#include "file.h"
#include "graph.h"
#include "parser.h"
#include "usb.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();

    private slots:
        //USB
        void usbConnected();
        void usbDisconnected();
        //Parser
        void parserConfigReceived(Parser::parser_config_t config);
        void parserStatusReceived(Parser::parser_rx_status_t rx_status, int packet_time_ms);
        //PID Test oscillation timer
        void pidTestOscTimOvf();
        //UI Interrupts
        void on_actionPotentiometerSetStart_triggered();
        void on_actionPotentiometerSetEnd_triggered();
        void on_actionPotentiometerStartMarginSpinbox_triggered();
        void on_actionPotentiometerEndMarginSpinbox_triggered();
        void on_actionMotorMaxPowerChanged_triggered();
        void on_actionPIDTestOscillate_triggered();
        void on_actionRevertFromFlash_triggered();
        void on_actionStoreToFlash_triggered();
        void on_actionSignalTestHold_triggered();
        void on_actionSignalMinMaxChanged_triggered();
        void on_actionSignalIgnoreOnUSB_triggered();
        void on_actionLoadConfiguration_triggered();
        void on_actionSaveConfiguration_triggered();
        void on_actionAbout_triggered();
        void on_actionManual_triggered();
        void on_actionGraphZoom_triggered();
        void on_actionGraphBufferClip_triggered();

    private:
        //Minimum allowed difference between minimum and maximum signal length
        #define SIGNAL_POSITION_MIN_LENGTH 0.4f
        //Connection status texts
        #define STATUS_CONNECTED "Connected!"
        #define STATUS_DISCONNECTED "Disonnected!"

        //UI States variable type
        typedef struct{
            bool group_pot;
            bool group_motor;
            bool group_signal;
            bool group_led;
            bool group_pid;
            bool checkbox_signal_ignore;
            bool button_pause_resume;
            bool action_update_flash;
            bool action_revert_flash;
            bool action_load_configuration;
            bool action_save_configuration;
            bool button_signal_hold;
            bool button_pid_test_oscillate;
            bool button_motor_test_backward;
            bool button_motor_test_forward;
        }ui_enable_t;

        //All UI states enum we use
        enum{
            UI_ENABLE_STATE_ALL_DISABLED,
            UI_ENABLE_STATE_RUNNING,
            UI_ENABLE_STATE_RUNNING_SIGNAL_IGNORE_DISABLED,
            UI_ENABLE_STATE_RUNNING_SIGNAL_TEST_HOLD,
            UI_ENABLE_STATE_RUNNING_PID_TEST_OSCILLATE,
            UI_ENABLE_STATES_TOTAL //How many enum elements above we have in this enum
        };

        //Array of UI state variables for all UI states in above enum
        ui_enable_t ui_enable[UI_ENABLE_STATES_TOTAL] = {
            {
                //UI_ENABLE_STATE_ALL_DISABLED
                .group_pot = false,
                .group_motor = false,
                .group_signal = false,
                .group_led = false,
                .group_pid = false,
                .checkbox_signal_ignore = false,
                .button_pause_resume = false,
                .action_update_flash = false,
                .action_revert_flash = false,
                .action_load_configuration = false,
                .action_save_configuration = false,
                .button_signal_hold = false,
                .button_pid_test_oscillate = false,
                .button_motor_test_backward = false,
                .button_motor_test_forward = false
            },
            {
                //UI_ENABLE_STATE_RUNNING
                .group_pot = true,
                .group_motor = true,
                .group_signal = true,
                .group_led = true,
                .group_pid = true,
                .checkbox_signal_ignore = true,
                .button_pause_resume = true,
                .action_update_flash = true,
                .action_revert_flash = true,
                .action_load_configuration = true,
                .action_save_configuration = true,
                .button_signal_hold = true,
                .button_pid_test_oscillate = true,
                .button_motor_test_backward = true,
                .button_motor_test_forward = true
            },
            {
                //UI_ENABLE_STATE_RUNNING_SIGNAL_IGNORE_DISABLED
                .group_pot = true,
                .group_motor = true,
                .group_signal = true,
                .group_led = true,
                .group_pid = true,
                .checkbox_signal_ignore = true,
                .button_pause_resume = true,
                .action_update_flash = true,
                .action_revert_flash = true,
                .action_load_configuration = true,
                .action_save_configuration = true,
                .button_signal_hold = false,
                .button_pid_test_oscillate = false,
                .button_motor_test_backward = false,
                .button_motor_test_forward = false
            },
            {
                //UI_ENABLE_STATE_RUNNING_SIGNAL_TEST_HOLD
                .group_pot = true,
                .group_motor = true,
                .group_signal = true,
                .group_led = true,
                .group_pid = true,
                .checkbox_signal_ignore = false,
                .button_pause_resume = true,
                .action_update_flash = false,
                .action_revert_flash = false,
                .action_load_configuration = false,
                .action_save_configuration = false,
                .button_signal_hold = true,
                .button_pid_test_oscillate = false,
                .button_motor_test_backward = false,
                .button_motor_test_forward = false
            },
            {
                //UI_ENABLE_STATE_RUNNING_PID_TEST_OSCILLATE
                .group_pot = true,
                .group_motor = true,
                .group_signal = true,
                .group_led = true,
                .group_pid = true,
                .checkbox_signal_ignore = false,
                .button_pause_resume = true,
                .action_update_flash = false,
                .action_revert_flash = false,
                .action_load_configuration = false,
                .action_save_configuration = false,
                .button_signal_hold = false,
                .button_pid_test_oscillate = true,
                .button_motor_test_backward = false,
                .button_motor_test_forward = false
            }
        };

        //These objects does not requre to be updated or cleared at connect/disconnect
        Ui::MainWindow *ui;
        Usb usb;
        Parser parser;

        //We should update/clear these
        Graph *graph;
        QTimer pid_test_osc_timer;
        uint8_t pid_test_oscillate_edge = 0;
        uint8_t keep_revert_flash = Parser::CMD_SETTINGS_KEEP;

        //UI enable function, should be run with one of the element of the array above
        void uiEnable(ui_enable_t *enable);
};
#endif // MAINWINDOW_H
