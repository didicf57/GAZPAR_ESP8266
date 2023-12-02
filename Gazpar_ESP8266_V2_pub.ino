/*
 * From ESP8266 Deep sleep mode example
 * Rui Santos
 * Complete Project Details https://randomnerdtutorials.com
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <InfluxDbClient.h>
#include <user_interface.h>

//ifdef _TRIEL    // Reseau Triel/seine
#define wifi_ssid "Votre_SSID"
#define wifi_password "Votre_Passe_Wifi"
uint8_t home_mac[7] = {0xB8, 0xD9, 0x4D, 0x0B, 0xD6, 0xB6}; // MAC add de BOX
int channel = 4;                                            // the wifi channel to be used
IPAddress staticIP(192,168,0,198);
IPAddress dns(192,168,0,1);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);
//#define INFLUXDB_URL "http://192.168.0.25:8086"   // Serveur Influx (PC portable)
#define INFLUXDB_URL "http://192.168.0.200:8086"   // Serveur Influx (Raspberry PI)
// #endif

// Serveur Influx (PC portable)

#define INFLUXDB_DB_NAME "Gazpar"

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_DB_NAME);

uint32_t reset_counter = 0;

unsigned long TimeWakeUp = 0;   // Horodate au reveil

int TimeOut = 4000;    // Time out accroche WiFi
int cumul = 5;        // Nb Pulse avant dépot

ADC_MODE(ADC_VCC);

void setup()
{
  rst_info *resetInfo;
  Serial.begin(115200);

  // Wait for serial to initialize.
 while (!Serial)
  {
  }
  //  delay(100);
  Serial.println("Start");
  // Deep sleep mode for 30 seconds, the ESP8266 wakes up by itself when GPIO 16 (D0 in NodeMCU board) is connected to the RESET pin
  // Serial.println("I'm awake, but I'm going into deep sleep mode for 30 seconds");
  // ESP.deepSleep(30e6);

  //      REASON_DEFAULT_RST      = 0,    /* normal startup by power on */
  //      REASON_WDT_RST          = 1,    /* hardware watch dog reset */
  //      REASON_EXCEPTION_RST    = 2,    /* exception reset, GPIO status won’t change */
  //      REASON_SOFT_WDT_RST     = 3,    /* software watch dog reset, GPIO status won’t change */
  //      REASON_SOFT_RESTART     = 4,    /* software restart ,system_restart , GPIO status won’t change */
  //      REASON_DEEP_SLEEP_AWAKE = 5,    /* wake up from deep-sleep */
  //      REASON_EXT_SYS_RST      = 6     /* external system reset */

  resetInfo = ESP.getResetInfoPtr();
  //Serial.println(millis());
  Serial.print("Reset Reason is : ");
  Serial.println((*resetInfo).reason);
  
  if (((*resetInfo).reason) == 0)  
    {reset_counter = 0;
    ESP.rtcUserMemoryWrite(0, &reset_counter, sizeof(reset_counter));
    }
  // Deep sleep mode until RESET pin is connected to a LOW signal (for example pushbutton or magnetic reed switch)
  
  ESP.rtcUserMemoryRead(0, &reset_counter, sizeof(reset_counter));
  reset_counter++;
  ESP.rtcUserMemoryWrite(0, &reset_counter, sizeof(reset_counter));
  
  Serial.print("RESET nb : ");
  Serial.println(reset_counter);
  TimeWakeUp = millis();     // Horodate du reveil
}  

void loop()
{
if (reset_counter >= cumul)
  { 
  Serial.println("Depot lancé");
  
  WiFi.persistent(false);
  WiFi.config(staticIP, dns, gateway, subnet);
  WiFi.mode(WIFI_STA); //Wifi en mode station pour échange de requête
  //WiFi.begin(wifi_ssid, wifi_password);
  WiFi.begin(wifi_ssid, wifi_password, channel, home_mac, true);
  while ((WiFi.status() != WL_CONNECTED) & (millis() < TimeWakeUp + TimeOut))   // Attente connecté et depuis 3 secondes
    { 
    delay(1000); Serial.print('.');
    }
  //Serial.print("ESP Board MAC Address:  ");
  //Serial.println(WiFi.macAddress());
  //Serial.print("-------> Connected : IP = ");
  //Serial.println(WiFi.localIP());
  Point pulse("Pulse");
  pulse.addField("pulse", cumul);
  pulse.addField("rssi", WiFi.RSSI());
  pulse.addField("Vcc", ESP.getVcc());

  if (!client.writePoint(pulse))
    {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
    client.resetBuffer(); // pas envoyé, vide le buffer
    }
  else 
    {
    Serial.println("Depot fait -> Decompte des pulses déposees"); 
    //reset_counter = reset_counter - cumul;   
    reset_counter = 0;
    ESP.rtcUserMemoryWrite(0, &reset_counter,sizeof(reset_counter));
    }  // Si déposé enleve 'cumul" pulses  sinon ne réinit pas. 
       // Cela permet de ne pas perdre de pulse si RESET pendant le dépot
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin(); // mise en sleep (radio off)
  }
//delay(1000);
Serial.print("Go to Sleep : ");
ESP.deepSleep(0);
}
