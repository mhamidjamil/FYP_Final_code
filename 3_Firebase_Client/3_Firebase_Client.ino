
// $->  27/JAN/23 01:45 AM
#include "Network.h"
#include <Ticker.h>

// #include <SoftwareSerial.h>
// SoftwareSerial ESP32_listener(15, 2);

#ifndef ESP32
#pragma message(THIS EXAMPLE IS FOR ESP32 ONLY !)
#error Select ESP32 board.
#endif
// Display *display;
Network *network;
// ~ data to be managed in struct
// SKdRiOAIL4NMiAkgrfmq@25.15!non-defined position!1.23,4.56,7.8912!3.56!#
// 3a2v3Vlb5UD6RFx0Eyg3@25.15!non-defined position!1.23,4.56,7.8912!3.59!#
bool data_input = false;
void manage_struct();
//` ++++++++++++++++++++++++
typedef struct {
  String userID = "";
  float temperature = 0;

  String position_ = "";

  float conductance = 0;
  float resistance = 0.0;
  float resistance_voltage = 0.0;

  float snore_voltages = 0.0;
  bool to_be_send = false;
} PatientData;
#define NO_OF_PATIENTS 5
PatientData patient_data[NO_OF_PATIENTS];

String userID = "";
float filterd_temperatur = 0;

String position_ = "";

float filterd_Conductance = 0;
float filterd_Resistance = 0.0;
float filterd_Conductance_voltage = 0.0;

float filterd_snore_voltages = 0.0;

// ~ data manager variables end

// 18 -> Red , 23 -> Green , 19 -> Blue
// #define RED 18
// #define GREEN 23
// #define BLUE 19

void tempTask(void *pvParameters);
bool Update_values();
void triggerGetTemp();
void DataManager(String tempstr);
/** Task handle for the light value read task */
TaskHandle_t tempTaskHandle = NULL;
/** Ticker for temperature reading */
Ticker tempTicker;
/** Flag if task should run */
bool tasksEnabled = false;

bool initTemp() {
  byte resultValue = 0;
  xTaskCreatePinnedToCore(tempTask,        /* Function to implement the task */
                          "tempTask ",     /* Name of the task */
                          10000,           /* Stack size in words */
                          NULL,            /* Task input parameter */
                          5,               /* Priority of the task */
                          &tempTaskHandle, /* Task handle. */
                          1);              /* Core where the task should run */

  if (tempTaskHandle == NULL) {
    Serial.println("Failed to start task for temperature update");
    return false;
  } else {
    // Start update of environment data every 3 seconds
    tempTicker.attach(3, triggerGetTemp);
  }
  return true;
}

void triggerGetTemp() {
  if (tempTaskHandle != NULL) {
    xTaskResumeFromISR(tempTaskHandle);
  }
}

void tempTask(void *pvParameters) {
  Serial.println("tempTask loop started...");
  while (1) // tempTask loop
  {
    if (tasksEnabled) {
      Update_values();
    }
    // Got sleep again
    vTaskSuspend(NULL);
  }
}
bool Update_values() {

  //    Serial.println("DHT11 error status: " + String(dht.getStatusString()));
  // if (data_input) {
  //   network->firestoreDataUpdate(
  //       filterd_temperatur, position_, filterd_Conductance,
  //       filterd_Resistance, filterd_Conductance_voltage,
  //       filterd_snore_voltages);
  //   data_input = false;
  // }
  // if (data_input) {
  Serial.println("Sending : (" + userID + " , " + String(filterd_temperatur) +
                 " , " + position_ + " , " + String(filterd_Conductance) +
                 " , " + String(filterd_Resistance) + " , " +
                 String(filterd_Conductance_voltage) + " , " +
                 String(filterd_snore_voltages) + ")");

  //   network->firestoreDataUpdate_1(userID, filterd_temperatur, position_,
  //                                  filterd_Conductance, filterd_Resistance,
  //                                  filterd_Conductance_voltage,
  //                                  filterd_snore_voltages);
  //   userID = "";
  //   data_input = false;
  // }
  for (int i = 0; patient_data[i].userID != "" && i < NO_OF_PATIENTS; i++) {
    if (patient_data[i].to_be_send) {
      // Serial.print("Data on index ! " + String(i));
      network->firestoreDataUpdate_1(
          patient_data[i].userID, patient_data[i].temperature,
          patient_data[i].position_, patient_data[i].conductance,
          patient_data[i].resistance, patient_data[i].resistance_voltage,
          patient_data[i].snore_voltages);
      patient_data[i].to_be_send = false;
      delay(1000);
    }
  }
  // Serial.println("Data sent..?");
  return true;
}

void setup() {
  Serial.begin(115200);
  // ESP32_listener.begin(9600);
  // pinMode(RED, OUTPUT);
  // pinMode(GREEN, OUTPUT);
  // pinMode(BLUE, OUTPUT);
  Serial.println();
  Serial.println("Esp32 edge (server)");

  //  initDisplay();
  initNetwork();
  initTemp();
  // Signal end of setup() to tasks
  tasksEnabled = true;
}

void loop() {
  // read data from serail terminal and assign that to double -> temperature

  String tempStr;
  if (Serial.available() > 0) {
    data_input = true;
    while (Serial.available()) {
      tempStr = Serial.readStringUntil('#');
    }
    if (tempStr.length() > 10) {
      DataManager(tempStr);
    } else {
      Serial.println("Data not valid");
    }
    // Serial.println(tempStr);

    // filterd_temperatur = Serial.parseFloat();
    // network->firestoreDataUpdate(temperature, 0);
  }
  // if (ESP32_listener.available() > 0) {
  //   data_input = true;
  //   tempStr = "";
  //   while (ESP32_listener.available()) {
  //     tempStr = ESP32_listener.readStringUntil('#');
  //   }
  //   if (tempStr.length() > 10) {
  //     DataManager(tempStr);
  //   } else {
  //     Serial.println("Data not valid");
  //   }
  // }
  if (!tasksEnabled) {
    // Wait 2 seconds to let system settle down
    delay(2000);
    // Enable task that will read values from the DHT sensor
    tasksEnabled = true;
    if (tempTaskHandle != NULL) {
      vTaskResume(tempTaskHandle);
    }
  }
  yield();
}

void initNetwork() {
  network = new Network();
  network->initWiFi();
}

void DataManager(String tempstr) {
  Serial.println("Working on : " + tempstr);
  float func_temperature = 0;

  String func_position = "";
  userID = tempstr.substring(0, tempstr.indexOf('@'));
  Serial.println("userID : " + userID);

  float func_Conductance = 0;
  float func_Resistance = 0.0;
  float func_Resistance_voltage = 0.0;
  float func_snore_voltages = 0.0;

  int first_at = tempstr.indexOf('@');
  int first_exclamation = tempstr.indexOf('!');
  int second_exclamation = tempstr.indexOf('!', first_exclamation + 1);
  int third_exclamation = tempstr.indexOf('!', second_exclamation + 1);
  int fourth_exclamation = tempstr.indexOf('!', third_exclamation + 1);

  // Serial.println("1st ! sppoted at position : " +
  // String(first_exclamation)); Serial.println("2nd ! sppoted at position : "
  // + String(second_exclamation)); Serial.println("3rd ! sppoted at position
  // : "
  // + String(third_exclamation));

  // String temp = tempstr.substring(2, first_exclamation);
  String temp = tempstr.substring(first_at + 1, first_exclamation);
  func_temperature = temp.toFloat();

  Serial.println("func_temperature: " + String(func_temperature));

  func_position = tempstr.substring(first_exclamation + 1, second_exclamation);
  Serial.println("position: " + func_position);

  String temp1 = tempstr.substring(second_exclamation + 1, third_exclamation);
  int first_coma = temp1.indexOf(',');
  int second_coma = temp1.indexOf(',', first_coma + 1);
  int third_coma = temp1.indexOf(',', second_coma + 1);

  String temp2 = temp1.substring(0, first_coma);
  func_Conductance = temp2.toFloat();
  Serial.println("filterd_Conductance: " + String(func_Conductance));

  String temp3 = temp1.substring(first_coma + 1, second_coma);
  func_Resistance = temp3.toFloat();
  Serial.println("filterd_Resistance: " + String(func_Resistance));

  String temp4 = temp1.substring(second_coma + 1, third_exclamation);
  func_Resistance_voltage = temp4.toFloat();
  Serial.println("filterd_Conductance_voltage: " +
                 String(func_Resistance_voltage));

  String temp5 = tempstr.substring(third_exclamation + 1, fourth_exclamation);
  func_snore_voltages = temp5.toFloat();
  Serial.println("filterd_snore_voltages: " + String(func_snore_voltages));
  // int data_input = 0;

  filterd_temperatur = func_temperature;

  position_ = func_position;

  filterd_Conductance = func_Conductance;
  filterd_Resistance = func_Resistance;
  filterd_Conductance_voltage = func_Resistance_voltage;
  filterd_snore_voltages = func_snore_voltages;
  manage_struct();
}
void manage_struct() {
  bool data_written = false;
  for (int i = 0; i < NO_OF_PATIENTS;
       i++) { // if user already exixts in the struct then update the data
    if (userID == patient_data[i].userID) {
      patient_data[i].temperature = filterd_temperatur;
      patient_data[i].position_ = position_;
      patient_data[i].conductance = filterd_Conductance;
      patient_data[i].resistance = filterd_Resistance;
      patient_data[i].resistance_voltage = filterd_Conductance_voltage;
      patient_data[i].snore_voltages = filterd_snore_voltages;
      patient_data[i].to_be_send = true;
      data_written = true;
      i = NO_OF_PATIENTS + 1;
    }
  }
  if (!data_written) { //  if user does not exixts in the struct then add user
    int target_index = 0;
    for (; target_index < NO_OF_PATIENTS; target_index++) {
      if (patient_data[target_index].userID == "") {
        // Serial.println("target_index : " + String(target_index));
        patient_data[target_index].userID = userID;
        patient_data[target_index].temperature = filterd_temperatur;
        patient_data[target_index].position_ = position_;
        patient_data[target_index].conductance = filterd_Conductance;
        patient_data[target_index].resistance = filterd_Resistance;
        patient_data[target_index].resistance_voltage =
            filterd_Conductance_voltage;
        patient_data[target_index].snore_voltages = filterd_snore_voltages;
        patient_data[target_index].to_be_send = true;
        break;
      }
    }
  }
}
