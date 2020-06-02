#include <Adafruit_MonoOLED.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>

#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <fonts.h>
#include <DEV_Config.h>
#include <GUI_Paint.h>
#include <EPD.h>
#include <WiFi.h>
#include <HTTPClient.h>

/* Includes ------------------------------------------------------------------*/
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include "pubsub.h"
//#include "epaper.h"
#include <stdlib.h>

/* Entry point ----------------------------------------------------------------*/
void setup()
{
  Serial.begin(115200);
  DEV_Module_Init();
  pubsub_init();
}

/* The main loop -------------------------------------------------------------*/
void loop()
{
  mqtt_loop();
}
