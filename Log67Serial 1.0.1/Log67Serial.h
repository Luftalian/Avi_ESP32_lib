// version: 1.0.1
#pragma once

#ifndef Log67Serial_H
#define Log67Serial_H
#include <Arduino.h>

class Log67Serial
{
private:
    unsigned long time_serial1 = 0;
    unsigned long time_serial2 = 0;
    // Serial2を送るときに使う
    bool sendFlag = false;
    char sendChar = '\0';

    uint64_t serialFrequency = 500000;

    char commandReturn = 'j';
    char commandDelete = 'd';

public:
    void setup();
    void sendSerial2();
    void setCommand(char command);
    void stopCommand();
    // マルチタスク用
    static void sendTask(void *pvParameters);
};

void Log67Serial::setup(char setCommandReturn = 'j', char setCommandDelete = 'd')
{
    commandReturn = setCommandReturn;
    commandDelete = setCommandDelete;
    time_serial1 = micros();
}

// この関数はループの中で呼び出す
void Log67Serial::sendSerial2()
{
    if (sendFlag)
    {
        time_serial2 = micros();
        if (time_serial2 - time_serial1 > serialFrequency)
        {
            time_serial1 = time_serial2;
            Serial2.write(sendChar);
            Serial.printf("sendChar1: %c", sendChar);
            Serial.print("\n");
        }
    }
}

void Log67Serial::setCommand(char command)
{
    sendFlag = true;
    sendChar = command;
}

void Log67Serial::stopCommand()
{
    sendFlag = false;
}

// マルチタスク用
// SPI Flashのデータを削除しているときのみ使用
// 呼び出し方は以下の通り。適宜呼び出し側の変数を変える
// xTaskCreatePinnedToCore(Log67Serial1.sendTask, "sendTask1", 8192, NULL, 2, &taskHandle, 0);
void Log67Serial::sendTask(void *pvParameters)
{
    Log67Serial Log67Serial2;
    Log67Serial2.setup();
    Log67Serial2.setCommand(commandDelete);
    while (1)
    {
        Log67Serial2.sendSerial2();

        char pre3 = Serial2.read();
        if (pre3 == commandReturn) // 'j'
        {
            Serial.println("return text2");
            Log67Serial2.sendFlag = false;
        }
        vTaskDelay(1);
    }
}

#endif