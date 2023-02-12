#include "file.h"

/*!
    Reads file data, returns "true" on success, "false" otherwise.
    Handles browse dialog and error messages if any
*/
bool File::get(file_data_t *values)
{
    //Get file name via dialog
    QFile file(QFileDialog::getOpenFileName(qApp->activeWindow(), "Load configuraiton from file...", "", "Servo binary conf. (*.sbc);;All Files (*)"));

    //If file has no name return false without message
    if(!file.fileName().length())
    {
        return false;
    }

    //Open file for reading
    if(!file.open(QIODevice::ReadOnly | QIODevice::Unbuffered))
    {
        showMessage(MESSAGE_ERROR_TEXT_OPEN);
        return false;
    }

    //Read data
    QByteArray file_data = QByteArray(file.readAll());

    //Calculate what should be correct file data size
    int file_data_size = sizeof(file_data_t) + sizeof(uint32_t);
    while(file_data_size % sizeof(uint32_t))
    {
        file_data_size++;
    }
    //Check if actual data size equals correct data size
    if(file_data.length() != file_data_size){
        file.close();
        showMessage(MESSAGE_ERROR_TEXT_OPEN_WRONG_SIZE);
        return false;
    }

    //Check if CRC is correct
    uint32_t file_provided_crc = *(uint32_t *)&file_data.data()[file.size() - sizeof(uint32_t)];
    uint32_t file_data_actual_crc = Crc32().get((uint32_t *)&file_data.data()[0], (file_data.length() / sizeof(uint32_t) ) - 1);
    if(file_provided_crc != file_data_actual_crc)
    {
        file.close();
        showMessage(MESSAGE_ERROR_TEXT_OPEN_WRONG_CRC);
        return false;
    }

    //Save values
    *values = *(file_data_t *)&file_data.data()[0];

    file.close();
    return true;
}

/*!
    Writes confituration data into file, together with CRC
    and padding bytes if necessary. Returns "true" on success,
    "false" otherwise, handles browse dialog and error messages if any
*/
bool File::save(file_data_t *values)
{
    //Get file name via dialog
    QFile file(QFileDialog::getSaveFileName(qApp->activeWindow(), "Save configuration as...", "Configuration", "Servo binary conf. (*.sbc);;All Files (*)"));

    //If file has no name return false without message
    if(!file.fileName().length())
    {
        return false;
    }

    //Open file for writing
    if(!file.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Unbuffered))
    {
        showMessage(MESSAGE_ERROR_TEXT_OPEN);
        return false;
    }

    //Clear any existing content
    file.resize(0);

    //Assemble file data
    QByteArray file_data = QByteArray((char *)values, sizeof(file_data_t));
    //For 32 bit CRC we need actual data size should be even of 4
    while(file_data.length() % sizeof(uint32_t))
    {
        file_data.append(FILE_CRC_PADDING_SYMBOL);
    }
    //Append CRC
    uint32_t file_crc = Crc32().get((uint32_t *)&file_data.data()[0], file_data.length() / sizeof(uint32_t));
    file_data.append((char *)&file_crc, sizeof(uint32_t));

    //Write data to file
    if(file.write(file_data) != file_data.length())
    {
        showMessage(MESSAGE_ERROR_TEXT_SAVE);
        return false;
    }

    file.close();
    return true;
}

/*!
    Private function for displaying error messages
*/
void File::showMessage(const QString &message)
{
    QMessageBox message_box(qApp->activeWindow());
    message_box.setWindowIcon(QIcon(":/Resources/icon_error.svg"));
    message_box.setWindowTitle(MESSAGE_ERROR_TITLE);
    message_box.setText(message);
    message_box.exec();
}
