#ifndef PARSER_H
#define PARSER_H

/*
    Packets structures

    Configuraiton request
    Outgoing:	[LENGTH 1][CONF_CMD 1][SETTINGS_REVERT 1][SETTINGS_FLASH 1][CRC32 4]
    Incoming:	[LENGTH 1][CONF_CMD 1]
                [POTENTIOMETER_START 2][POTENTIOMETER_END 2]
                [MOTOR_MAX_PWR 1][MOTOR_INVERT 1]
                [SIGNAL_IGNORE_ON_USB 1][SIGNAL_LEN 4][SIGNAL_TIMEOUT 4]
                [LED 1]
                [PID_ON 1]
                [PID_P 4]
                [PID_I 4]
                [PID_D 4]
                [PID_SAMPLE_TIME 1]
                [CRC32 4]

    Status request (incoming configuration)
    Outgoing:	[LENGTH 1][STAT_CMD 1]
                [POTENTIOMETER_START 2][POTENTIOMETER_END 2]
                [MOTOR_MAX_PWR 1][MOTOR_INVERT 1]
                [SIGNAL_IGNORE_ON_USB 1][SIGNAL_LEN 4][SIGNAL_TIMEOUT 4]
                [LED 1]
                [PID_ON 1]
                [PID_P 4]
                [PID_I 4]
                [PID_D 4]
                [PID_SAMPLE_TIME 1]
                [RQ_POSITION 4][RQ_MOTOR_RUN 1][RQ_SIGNAL_HOLD 1]
                [CRC_PADDING 2][CRC32 4]
    Incoming:	[LENGTH 1][STAT_CMD 1]
                [PID_RUNNING 1][PID_SET_POS 4][POTENTIOMETER_POSITION 4][MOTOR_POWER 2]
                [USB_CRC_PADDING 3][CRC32 4]
*/

#define PARSER_TX_CONFIG_LENGTH 8
#define PARSER_RX_CONFIG_LENGTH 64
#define PARSER_TX_STATUS_LENGTH 76
#define PARSER_RX_STATUS_LENGTH 20

#define PARSER_CMD_CONFIG (char)0
#define PARSER_CMD_STATUS 1

#define PARSER_RQ_NONE (char)0
#define PARSER_RQ_CONF 1
#define PARSER_RQ_STAT 2

#define PARSER_CRC_PADDING (char)0

#define PARSER_SETTINGS_KEEP 0
#define PARSER_SETTINGS_REVERT 1
#define PARSER_SETTINGS_FLASH 2

#endif // PARSER_H
