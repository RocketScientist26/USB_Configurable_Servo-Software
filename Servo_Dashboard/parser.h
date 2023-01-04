//! Class for parsing/assembling servo motor packets for transferring via USB
/*!
    Length, CRC Padding and CRC bytes are not prepended/appended by this class.
    Complete packets structures, including length CRC padding and CRC bytes:

    -- Configuraiton request --

    To servo:
                [uint8_t	LENGTH 					1]
                [uint8_t	CMD_CONFIG 				1]
                [uint8_t	CMD_SETTINGS_KEEP       1]
                [uint8_t	USB_CRC_PADDING_SYMBOL	1]
                [uint32_t	CRC32					4]

    From servo:
                [uint8_t	LENGTH					1]
                [uint8_t	CMD_CONFIG  			1]

                [uint16_t	potentiometer_min		2]
                [uint16_t	potentiometer_max		2]
                [uint8_t	motor_max_power			1]
                [bool		motor_invert			1]
                [bool		signal_ignore			1]
                [float		signal_min				4]
                [float		signal_max				4]
                [uint32_t	signal_timeout			4]
                [uint8_t	led_mode				1]
                [bool		pid_on_e_m				1]
                [float		pid_kp_1				4]
                [float		pid_ki_1				4]
                [float		pid_kd_1				4]
                [float		pid_kp_2				4]
                [float		pid_ki_2				4]
                [float		pid_kd_2				4]
                [float		pid_kp_3				4]
                [float		pid_ki_3				4]
                [float		pid_kd_3				4]
                [uint16_t	pid_split_steps_1		2]
                [uint16_t	pid_split_steps_2		2]
                [uint8_t	pid_sampling_ms			1]

                [uint32_t	CRC32					4]

    -- Status request --

    To servo:
                [uint8_t	LENGTH					1]
                [uint8_t	CMD_STATUS				1]

                [uint16_t	potentiometer_min		2]
                [uint16_t	potentiometer_max		2]
                [uint8_t	motor_max_power			1]
                [bool		motor_invert			1]
                [bool		signal_ignore			1]
                [float		signal_min				4]
                [float		signal_max				4]
                [uint32_t	signal_timeout			4]
                [uint8_t	led_mode				1]
                [bool		pid_on_e_m				1]
                [float		pid_kp_1				4]
                [float		pid_ki_1				4]
                [float		pid_kd_1				4]
                [float		pid_kp_2				4]
                [float		pid_ki_2				4]
                [float		pid_kd_2				4]
                [float		pid_kp_3				4]
                [float		pid_ki_3				4]
                [float		pid_kd_3				4]
                [uint16_t	pid_split_steps_1		2]
                [uint16_t	pid_split_steps_2		2]
                [uint8_t	pid_sampling_ms			1]

                [float		pid_setpoint			4]
                [bool		usb_rq_stat_phold		1]
                [uint8_t	usb_rq_stat_motor		1]

                [uint8_t	USB_CRC_PADDING_SYMBOL	1]
                [uint8_t	USB_CRC_PADDING_SYMBOL  1]
                [uint32_t	CRC32					4]

    From servo:
                [uint8_t	LENGTH					1]
                [uint8_t	CMD_STATUS				1]

                [bool		pid_running				1]
                [float		pid_setpoint			4]
                [float		potentiometer_position	4]
                [int16_t	motor_power				2]

                [uint8_t	USB_CRC_PADDING_SYMBOL	1]
                [uint8_t	USB_CRC_PADDING_SYMBOL	1]
                [uint8_t	USB_CRC_PADDING_SYMBOL	1]
                [uint32_t	CRC32					4]
*/

#ifndef PARSER_H
#define PARSER_H

#include <QObject>

class Parser : public QObject
{
    Q_OBJECT

    public:
        //Commands
        enum{
            CMD_CONFIG,
            CMD_STATUS
        };

        //Settings keep/revert/flash command byte
        enum{
            CMD_SETTINGS_KEEP,
            CMD_SETTINGS_REVERT,
            CMD_SETTINGS_FLASH
        };

        //Configuration data from/to servo
        typedef struct{
            uint16_t potentiometer_min;
            uint16_t potentiometer_max;
            uint8_t motor_max_power;
            bool motor_invert;
            bool signal_ignore;
            float signal_min;
            float signal_max;
            uint32_t signal_timeout;
            uint8_t led_mode;
            bool pid_on_e_m;
            float pid_kp_1;
            float pid_ki_1;
            float pid_kd_1;
            float pid_kp_2;
            float pid_ki_2;
            float pid_kd_2;
            float pid_kp_3;
            float pid_ki_3;
            float pid_kd_3;
            uint16_t pid_split_steps_1;
            uint16_t pid_split_steps_2;
            uint8_t pid_sampling_ms;
        }__attribute__ ((__packed__)) parser_config_t;

        //Status data sent to servo
        typedef struct{
            float pid_setpoint;
            bool usb_rq_stat_phold;
            uint8_t usb_rq_stat_motor;
        }__attribute__ ((__packed__)) parser_tx_status_t;

        //Status data received from servo
        typedef struct{
            bool pid_running;
            float pid_setpoint;
            float potentiometer_position;
            int16_t motor_power;
        }__attribute__ ((__packed__)) parser_rx_status_t;

        QByteArray configRequest(uint8_t keep_revert_flash);
        QByteArray statusRequest(parser_config_t *config, parser_tx_status_t *status);

    public slots:
        void parseData(QByteArray data, int packet_time_ms);

    signals:
        //Emitted after parsing if "parseData" function identified data array as configuration packet
        void configReceived(Parser::parser_config_t config);
        //Emitted after parsing if "parseData" function identified data array as status packet
        void statusReceived(Parser::parser_rx_status_t rx_status, int packet_time_ms);
};

#endif // PARSER_H
