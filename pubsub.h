#include "epaper-gxepd.h"
#include "network.h"

/* Server and IP address ------------------------------------------------------*/
IPAddress myIP; // IP address in your local wifi net

WiFiClient espClient;
PubSubClient client(espClient);
HTTPClient http;

long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void parseOutput(byte *message)
{
  // Deserialize the JSON document
  //  StaticJsonDocument<200> doc;
  DynamicJsonDocument doc(4096);
  DeserializationError error = deserializeJson(doc, message);

  // Test if parsing succeeds.
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  LinkedList<PaperString> strings;
  LinkedList<PaperRect> rectangles;

  Dashboard dashboard = {&strings, &rectangles, false, 0};

  if (doc.containsKey("partial"))
  {
    dashboard.partial = doc["partial"].as<bool>();
  }
  if (doc.containsKey("rotation"))
  {
    dashboard.rotation = doc["rotation"].as<int>();
  }

  if (doc.containsKey("strings"))
  {
    JsonArray arr = doc["strings"].as<JsonArray>();
    int size = arr.size();
    //    PaperString* strings[size];
    int i = 0;
    //    Serial.printf("Building array of size %d\n", size);
    for (JsonVariant value : arr)
    {
      //      Serial.printf("Building %d\n", i);
      const char *message = value["message"].as<char *>();
      int x = value["x"].as<int>();
      int y = value["y"].as<int>();
      const char *font = "robotothin72";
      const char *color = "black";
      PaperString s = {message, x, y, font, color};
      if (value.containsKey("font"))
      {
        s.font = value["font"].as<char *>();
      }
      if (value.containsKey("color"))
      {
        s.color = value["color"].as<char *>();
      }
      Serial.printf("%s %d\n", message, i);

      strings.add(s);
      //      dashboard.strings[i] = s;
      i++;
    }
    //    dashboard.strings = &strings;
    //    dashboard.num_strings = size;
  }

  if (doc.containsKey("rectangles"))
  {
    JsonArray arr = doc["rectangles"].as<JsonArray>();
    int size = arr.size();
    //    PaperRect rectangles[size];
    int i = 0;
    //    Serial.printf("Building array of size %d\n", size);
    for (JsonVariant value : arr)
    {
      //      Serial.printf("Building %d\n", i);
      int x = value["x"].as<int>();
      int y = value["y"].as<int>();
      int w = value["w"].as<int>();
      int h = value["h"].as<int>();
      const char *color = "black";
      PaperRect r = {x, y, w, h, color};
      //      dashboard.rectangles[i] = r;
      if (value.containsKey("color"))
      {
        r.color = value["color"].as<char *>();
      }
      rectangles.add(r);
      i++;
    }
    //    dashboard.rectangles = &rectangles;
    //    dashboard.num_rectangles = size;
  }

  drawDashboard(dashboard);
}

void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
  }
  Serial.println();

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
  // Changes the output state according to the message
  if (String(topic) == "esp32/output")
  {
    parseOutput(message);
  }
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("MQTTePaper", mqtt_user, mqtt_passwd))
    {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

//
//void fetch_image() {
//  http.begin("http://192.168.86.85:8000/monosml.bmp");
//  int httpCode = http.GET();
//  // httpCode will be negative on error
//  if(httpCode > 0) {
//      // HTTP header has been send and Server response header has been handled
//      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
//
//      // file found at server
//      if(httpCode == HTTP_CODE_OK) {
//          // get lenght of document (is -1 when Server sends no Content-Length header)
//          int len = http.getSize();
//
//          // create buffer for read
//          uint8_t buff[len] = { 0 };
//
//          // get tcp stream
//          WiFiClient * stream = http.getStreamPtr();
//
//          // read all data from server
//          while(http.connected() && (len > 0 || len == -1)) {
//              // get available data size
//              size_t size = stream->available();
//
//              if(size) {
//                  // read up to 128 byte
//                  int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
//
//                  Serial.printf("%d read\n\n");
//
//                  // write it to Serial
//                  // Serial.write(buff, c);
//
//                  Serial.println("...");
//
//                  if(len > 0) {
//                      len -= c;
//                  }
//
//
//
//              }
////              delay(1);
//          }
//
//          draw_image(buff);
//
//          Serial.println("Left while");
//          Serial.print("[HTTP] connection closed or file end.\n");
//          Serial.printf("BYTES %d\n", len);
//      }
//  } else {
//      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
//  }
//
//  http.end();
//}
//

void mqtt_loop()
{
  if (!client.connected())
  {
    printf("Not connected. Connecting\r\n");
    reconnect();
  }
  client.loop();
  //  do_publish();
}

void do_publish()
{

  //  long now = millis();
  //  if (now - lastMsg > 5000) {
  //    lastMsg = now;

  // Temperature in Celsius
  //    temperature = bme.readTemperature();
  // Uncomment the next line to set temperature in Fahrenheit
  // (and comment the previous temperature line)
  //temperature = 1.8 * bme.readTemperature() + 32; // Temperature in Fahrenheit

  // Convert the value to a char array
  //    char tempString[8];
  //    dtostrf(temperature, 1, 2, tempString);
  //    Serial.print("Temperature: ");
  //    Serial.println(tempString);
  //    client.publish("esp32/temperature", tempString);
  //    client.publish("esp32/temperature", "20");

  //    humidity = bme.readHumidity();

  // Convert the value to a char array
  //    char humString[8];
  //    dtostrf(humidity, 1, 2, humString);
  //    Serial.print("Humidity: ");
  //    Serial.println(humString);
  //    client.publish("esp32/humidity", humString);
  //  }
}

void pubsub_init()
{
  setup_wifi();
  epaper_init();

  client.setBufferSize(512);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
