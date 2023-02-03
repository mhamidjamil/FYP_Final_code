// code send output to python script, which will filter out values
//  and send those values to esp32 which upload that data on firebase
#include "SPI.h"
#include <MySignals.h>
#include <Wire.h>

void setup()
{
    Serial.begin(115200);
    MySignals.begin();
    MySignals.initBodyPosition();
}

void loop()
{
    float temperature = MySignals.getTemperature();
    // Serial.print("Temperature (*C): <1_");
    Serial.print(temperature, 2);
    Serial.print("!");
    uint8_t position = MySignals.getBodyPosition();
    // Serial.print("Current position : <2_");
    // print body position
    // MySignals.printPosition(position);
    if (position == 1)
    {
        Serial.print(F("Prone position"));
    }
    else if (position == 2)
    {
        Serial.print(F("Left lateral decubitus"));
    }
    else if (position == 3)
    {
        Serial.print(F("Rigth lateral decubitus"));
    }
    else if (position == 4)
    {
        Serial.print(F("Supine position"));
    }
    else if (position == 5)
    {
        Serial.print(F("Stand or sit position"));
    }
    else
    {
        Serial.print(F("non-defined position"));
    }
    Serial.print("!");
    float conductance = MySignals.getGSR(CONDUCTANCE);
    float resistance = MySignals.getGSR(RESISTANCE);
    float conductanceVol = MySignals.getGSR(VOLTAGE);

    // Serial.print("<3_");
    // Serial.print("Conductance : <3.1_");
    // Serial.print("<3.1_C:");
    // Serial.print("<3.1_");
    Serial.print(conductance, 2);
    Serial.print(",");

    // Serial.print("Resistance : <3.2_");
    // Serial.print("<3.2_R:");
    // Serial.print("<3.2_");
    Serial.print(resistance, 2);
    Serial.print(",");

    // Serial.print("Conductance Voltage : <3.3_");
    // Serial.print("<3.3_V:");
    // Serial.print("<3.3_");
    Serial.print(conductanceVol, 4);
    Serial.print("!");

    // Serial.print("_3>");

    float air = MySignals.getAirflow(VOLTAGE);
    // Serial.print("<4_");
    Serial.print(air, 2);
    Serial.println("!#");

    // delay(100);

    // print acc values
    MySignals.getAcceleration();

    // convert float to strintg
    // char bufferAcc[50];
    // char x_acc_string[10];
    // char y_acc_string[10];
    // char z_acc_string[10];
    // dtostrf(MySignals.x_data, 2, 2, x_acc_string);
    // dtostrf(MySignals.y_data, 2, 2, y_acc_string);
    // dtostrf(MySignals.z_data, 2, 2, z_acc_string);

    // // print the X Y Z acceleration
    // sprintf(bufferAcc, "Acceleration: X= %s  Y= %s  Z= %s  ", x_acc_string,
    //         y_acc_string, z_acc_string);
    // Serial.println(bufferAcc);

    delay(1000);
}
