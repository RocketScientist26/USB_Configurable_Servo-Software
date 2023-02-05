#include "parser.h"

/*!
    Returns configuration request USB data as QByteArray
*/
QByteArray Parser::configRequest(uint8_t keep_revert_flash)
{
    return (QByteArray(1, (uint8_t)CMD_CONFIG).append(keep_revert_flash));
}


/*!
    Returns status request USB data as QByteArray
*/
QByteArray Parser::statusRequest(parser_config_t *config, parser_tx_status_t *status)
{
    QByteArray data;

    data.append(CMD_STATUS);
    data.append((char *)config, sizeof(parser_config_t));
    data.append((char *)status, sizeof(parser_tx_status_t));

    return data;
}

/*!
    Parses received from servo data, emits appropriate signal if
    data has correct size and configuraton or status data identifier byte
*/
void Parser::parseData(QByteArray data, int packet_time_ms)
{
    //Make sure we have at least command identifier byte
    if(data.length() < 2){
        return;
    }

    if(data.data()[1] == CMD_CONFIG){
        if(data.length() >= (int)sizeof(parser_config_t)){
            parser_config_t config = *(parser_config_t *)&data.data()[2];
            emit configReceived(config);
            return;
        }
    }

    if(data.data()[1] == CMD_STATUS){
        if(data.length() >= (int)sizeof(parser_rx_status_t)){
            parser_rx_status_t rx_status = *(parser_rx_status_t *)&data.data()[2];
            emit statusReceived(rx_status, packet_time_ms);
            return;
        }
    }
}
