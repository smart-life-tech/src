// ##################### VERSion 4.10 NTP server Fixed ####################
//"sender_email":"inteliqo.iq@gmail.com",
//"sender_email_pass":"ms645346763254j33",
#include <Arduino.h>
#include "stepper.h"
#define SIM800L_AXP192_VERSION_20200327
#include "utilities.h"
#define SerialAT Serial1
char receivedSMS[160]; // Buffer to store received SMS
char *paramValue;
void sendSMS(String msg);
unsigned long previousMillisSMS = 0;
void SendMessage(String msg, String numb);
String gsm_send_serial(String command, int delay);

String gsm_send_serial(String command, int delay)
{
  String buff_resp = "";
  Serial.println("Send ->: " + command);
  SerialAT.println(command);
  long wtimer = millis();
  while (wtimer + delay > millis())
  {
    while (SerialAT.available())
    {
      buff_resp = SerialAT.readString();
      Serial.println(buff_resp);
    }
  }
  Serial.println();
  return buff_resp;
}

void processSMS()
{
  // msg="+CMT: \"+923335814110\","",\"23/09/25,00:33:59+20\"PARAMS:1000,5000,1";
  String msg = receivedSMS;
  // Check if th
  msg = msg.substring(msg.indexOf("PARAMS:") + 7, msg.length() + 1);
  Serial.println(msg);
  START_DELAY_MS = msg.substring(0, msg.indexOf(",")).toDouble();
  msg = msg.substring(msg.indexOf(",") + 1, msg.length() + 1);
  MOTORRUN_MS = msg.substring(0, msg.indexOf(",")).toDouble();
  msg = msg.substring(msg.indexOf(",") + 1, msg.length() + 1);
  STEPS_PER_MS = msg.substring(0, msg.indexOf(",")).toDouble();
  // Print the extracted values for debugging
  Serial.print("START_DELAY_MS: ");
  Serial.println(START_DELAY_MS);
  Serial.print("MOTORRUN_MS: ");
  Serial.println(MOTORRUN_MS);
  Serial.print("STEPS_PER_MS: ");
  Serial.println(STEPS_PER_MS);

  loopStepper(MOTORRUN_MS, STEPS_PER_MS);
  sendSMS("Motor Executed for seconds: " + (String)(MOTORRUN_MS));
}
void readSMS(int index)
{
  String command = "AT+CMGR=" + String(index) + "\r\n";
  gsm_send_serial(command, 1000);
  String smsRCVD;
  // Read and print the SMS content
  delay(1000); // Wait for the module to respond
  while (SerialAT.available())
  {
    smsRCVD = SerialAT.readString();

    Serial.println(smsRCVD);
  }
  sendSMS("Message Arrived: " + smsRCVD);
}

void deleteSMS(int index)
{
  String command = "AT+CMGD=DEL ALL";
  gsm_send_serial(command, 1000);
}

void deleteAllSMS()
{
  gsm_send_serial("AT+CMGDA=DEL ALL", 1000);
}
void sendSMS(String msg)
{
  gsm_send_serial("AT+CMGS=\"+923335814110\"", 1000); // change ZZ with country code and xxxxxxxxxxx with phone number to sms
  gsm_send_serial(msg, 500);                          // text content
  SerialAT.write(26);
}

void setup()
{
  Serial.begin(115200);
  setupStepper();
  // while (1)
  // {
  //   /* code */
  //   loopStepper();
  // }
  processSMS();
  setupModem();
  // attachInterrupt(BUTTON_1, isr1,ONLOW);
  delay(5000);
  // Set GSM module baud rate and UART pins
  SerialAT.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  Serial.println("Initializing...");
  delay(1000);

  gsm_send_serial("AT", 100); // Once the handshake test is successful, it will back to OK
  // gsm_send_serial("AT+SIMEI=\"865456052786934\"",100);//865456052786934

  gsm_send_serial("AT+CMGF=1", 500); // Configuring TEXT mode

  gsm_send_serial("AT+CNMI=1,2,0,0,0", 1000);
}

void loop()
{
  if (SerialAT.available())
  {
    delay(100); // Wait for the complete message
    int bytesRead = SerialAT.readBytes(receivedSMS, 160);
    receivedSMS[bytesRead] = '\0'; // Null-terminate the string
    String msg = (String)receivedSMS;
    if (msg.indexOf("+CM") > -1)
    {
      sendSMS("Message Arrived: " + (String)receivedSMS);

      processSMS();
      deleteAllSMS();
    }
  }
}
