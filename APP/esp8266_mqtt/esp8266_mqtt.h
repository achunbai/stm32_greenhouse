#ifndef _EXP8266_MQTT_H
#define _EXP8266_MQTT_H
#include "stm32f10x.h"
#include "esp8266.h"

#define   macUser_ESP8266_ApSsid              "YOUR_SSID"         //要连接的热点的名称
#define   macUser_ESP8266_ApPwd               "YOUR_PASSWORD"           //要连接的热点的密钥
void ESP8266_Mqtt_Connect(void);
void Esp8266_SendData_ReceiveMsg(u8 *control_mode, u8 *temp, u8 *humi, u8 *temp_2, u8 *humi_2, u8 *lsens_value, u8 *light_level, u8 *fan_status, u8 *pump_status, u16 *co2_level);
#endif
