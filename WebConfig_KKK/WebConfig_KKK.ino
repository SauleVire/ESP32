
/*
  Based on  ESP_WebConfig Copyright (c) 2015 John Lassen. All rights reserved
  ------------------------------------------------------------------------------------
  Copyright (c) 2019 SauleVire.LT. All rights reserved.
  This is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Latest version: Pre-release 0.1  - 2019.10.15

  -----------------------------------------------------------------------------------------------
  History

  Version  0.1 LT - 2019.10.15
  First initial version to the public


*/
//#include <ESP8266WebServer.h>
//#include <ESP8266mDNS.h>
//#include <ESP8266WiFi.h>

#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#include <WebServer.h>
#else
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#endif

#include <NtpClientLib.h>
#include <WiFiClient.h>
//#include "ESP8266HTTPUpdateServer.h"
#include <Ticker.h>

#include <WiFiUdp.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PID_v1.h>
#include <EEPROM.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "secret.h"
#include "helpers.h"
#include "global.h"
#include "ds18b20.h"
#include <SimpleTimer.h>
#include "Relay.h"
/*
  Include the HTML, STYLE and Script "Pages"
*/
#include "Page_Root.h"
#include "Page_Admin.h"
#include "Page_Script.js.h"
#include "Page_Style.css.h"
#include "Page_NTPsettings.h"
#include "Page_Information.h"
#include "Page_General.h"
#include "Page_Naujinimas.h"
#include "Page_NetworkConfiguration.h"
#include "Page_NotFound.h"
#include "Page_Kolektorius.h"
#include "Page_Boileris.h"
#include "Page_Akumuliacine.h"
#include "Page_Emoncms.h"
#include "Page_Index.h"
#include "Page_DS18B20.h"
#include "Page_PriskirtiDS18B20.h"
#include "Page_Pvoztuvas.h"

// #define DEBUG_Kolektorius 1 // Naudojama tik testavimui
#define DEBUGpv 1 // Naudojama tik testavimui
#define DEBUG_akumuliacine 1 // Naudojama tik testavimui
#define DEBUGboileris 1 // Naudojama tik testavimui
#define DEBUGbusena 1 // Naudojama tik testavimui
#define DEBUGds18b20 1 // Naudojama tik testavimui
#define Diagnostika 0 // Naudojama tik testavimui

#define ACCESS_POINT_NAME  "SauleVire_PT"
#define ACCESS_POINT_PASSWORD  ""
#define AdminTimeOut 300  // Defines the Time in Seconds, when the Admin-Mode will be disabled
SimpleTimer timer;
const char* host = "SauleVire";

// #define CollectorRELAYpin 2 //išvadas kolektoriaus siurblio junginėjimui, Relė
// #define BoilerRELAYpin 32 // išvadas boilerio siurbliui, Relė 
// #define BoilerThermostatRELAYpin 33 // išvadas boilerio termostatui, Relė 
// #define HeatTanktRELAYpin 27 // išvadas ak. talpos siurblio junginėjimui, Relė 
// #define RadiatorPumpRELAYpin 14 // išvadas radijatorių siurbliui, Relė 
// #define MixingValveOffRELAYpin 13 // išvadas PV uždarymo, Relė 
// #define MixingValveOnRELAYpin 12 // išvadas PV atidarymo, Relė 
  
Relay BoilerRELAY(BoilerRELAYpin, false);
Relay BoilerThermostatRELAY(BoilerThermostatRELAYpin, false);

void setup ( void ) {

  BoilerRELAY.begin(); // inicializes the pin
  BoilerThermostatRELAY.begin(); // inicializes the pin


  EEPROM.begin(4096);
  pinMode(CollectorRELAYpin, OUTPUT);
//  pinMode(BoilerRELAYpin, OUTPUT);
//  pinMode(BoilerThermostatRELAYpin, OUTPUT);
  //  pinMode(25, OUTPUT);
  //  pinMode(26, OUTPUT);
  pinMode(HeatTanktRELAYpin, OUTPUT);
  pinMode(RadiatorPumpRELAYpin, OUTPUT);
  pinMode(MixingValveOffRELAYpin, OUTPUT);
  pinMode(MixingValveOnRELAYpin, OUTPUT);
  
#ifdef Diagnostika
  Serial.begin(115200);
  Serial.println("Čia SauleVire.lt pradžia\n");
#endif
  if (!ReadConfig())
  {
    // DEFAULT CONFIG
    config.ssid = ssid; //belaidžio tinklo pavadinimas
    config.password = password; //slaptažodis
    config.dhcp = true;
    config.IP[0] = 192; config.IP[1] = 168; config.IP[2] = 8; config.IP[3] = 111;
    config.DNS[0] = 1; config.DNS[1] = 1; config.DNS[2] = 1; config.DNS[3] = 1;
    config.Netmask[0] = 255; config.Netmask[1] = 255; config.Netmask[2] = 255; config.Netmask[3] = 0;
    config.Gateway[0] = 192; config.Gateway[1] = 168; config.Gateway[2] = 8; config.Gateway[3] = 1;
    config.ntpServerName = "pool.ntp.org";
    config.Update_Time_Via_NTP_Every =  30;
    config.timezone = 20;
    config.daylight = true;
    config.DeviceName = "SauleVire.lt";
    config.AutoTurnOff = false;
    config.AutoTurnOn = false;
    config.TurnOffHour = 0;
    config.TurnOffMinute = 0;
    config.TurnOnHour = 0;
    config.TurnOnMinute = 0;
    /* ********** kintamieji saulės kolektoriui ******************* */
    config.k_skirtumas = 4;
    config.k_uzsalimas = true; // 1-įjungta, 0- išjungta , SK apsauga nuo šalčio, pašildymas
    config.k_nuorinimas = false; //  SK siurblio rankiniam valdymui (nuorinimas)
    config.k_intervalas = 99; // Numatytas laikas saulės kolektoriaus temperatūros matavimui 10s.

    config.reiksme1 = "a";
    config.reiksme2 = "b";
    config.reiksme3 = "c";
    config.katalogas = "d";
    config.emoncmsOn = false;
    config.intervalasEmon = 600;
    /* ********** kintamieji Boileriui ******************* */
    config.Bo_ON_T = 45; // temperatūra boilerio siurbliui įjungti
    config.Bo_OFF_T = 65; // temperatūra boilerio siurbliui įšjungti
//    config.Bo_Rankinis_ijungimas = false; // Žymė rankiniam AT siurblio valdymui
    config.Bo_Termostatas = false; // Žymė rankiniam termostato įjungimui
//    config.Bo_Termostato_busena = false; // Žymė termostato busenai
    /* ********** kintamieji Akumuliacinei talpai ******************* */
    config.At_ON_T = 90; // temperatūra akumuliacines talpos siurbliui įjungti
    config.At_OFF_T = 89; // temperatūra akumuliacines talpos siurbliui įšjungti
    config.At_Rankinis_ijungimas = 0; // Žymė rankiniam AT siurblio valdymui
    /* ********** Pamaišymo vožtuvo nustatymai ************************************ */
    config.PV_ON_T = 40; // temperatūra radijatorių siurbliui įjungti
    config.PV_OFF_T = 35; // temperatūra radijatorių siurbliui įšjungti
    config.PV_palaikoma_T = 40; // Į radiatorius tiekiamo termofikato temperatūra
    config.PV_tolerancijos_T = 1.2; // Į radiatorius tiekiamo termofikato tolerancijos temperatūra
    config.PV_rankinis_ijungimas = true; // Žymė rankiniam radijatorių siurblio valdymui
    /* ********** PID nustatymai ************************************ */
    config.Kp = 25;
    config.Ki = 1.5;
    config.Kd = 4;
    config.WindowSize = 160;
    WriteConfig();
    WriteConfigIP();
    WriteConfigDS18b20();
    WriteConfigGeneralInfo();
    WriteConfigNTP();
    WriteConfigEmonCMS();
    WriteConfigCollector();
    WriteConfigBoiler();
    WriteConfigHeatStorageTank();
    WriteConfigMixingValve();
#ifdef Diagnostika
    Serial.println("General config applied");
#endif
  }


  if (AdminEnabled)
  {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP( ACCESS_POINT_NAME , ACCESS_POINT_PASSWORD);
  }
  else
  {
    WiFi.mode(WIFI_STA);
  }

  ConfigureWifi();

  /*use mdns for host name resolution*/
    if (!MDNS.begin(host)) { //http://esp32.local
      Serial.println("Error setting up MDNS responder!");
      while (1) {
        delay(1000);
      }
    }
      Serial.print("\nmDNS responder started");
  server.on ( "/", processIndex  );
  server.on ( "/admin/filldynamicdata", filldynamicdata );
  server.on ( "/favicon.ico",   []() {
#ifdef Diagnostika
    Serial.println("favicon.ico");
#endif
    server.send ( 200, "text/html", "" );
  }  );
  server.on ( "/admin.html", []() {
#ifdef Diagnostika
    Serial.println("admin.html");
#endif
    server.send ( 200, "text/html", PAGE_AdminMainPage );
  }  );
  server.on ( "/config.html", send_network_configuration_html );
  server.on ( "/info.html", []() {
#ifdef Diagnostika
    Serial.println("info.html");
#endif
    server.send ( 200, "text/html", PAGE_Information );
  }  );
  server.on ( "/ntp.html", send_NTP_configuration_html  );
  server.on ( "/general.html", send_general_html  );
  server.on ( "/example.html", []() {
#ifdef Diagnostika
    Serial.println("example.html");
#endif
    server.send ( 200, "text/html", PAGE_EXAMPLE );
  } );
  server.on ( "/kolektorius.html", send_KolektoriausKonfiguracija_html );
  server.on ( "/boileris.html", send_BoilerioKonfiguracija_html );
  server.on ( "/akumuliacine.html", send_AkumuliacinesKonfiguracija_html );
  server.on ( "/pvoztuvas.html", send_PVoztuvoKonfiguracija_html );
  server.on ( "/emoncms.html", send_Emoncms_html );
  server.on ( "/ds18b20.html", Page_DS18B20 );
  server.on ( "/priskirtiDS18b20.html", send_PriskirtiDS18B20_html );
  server.on ( "/style.css", []() {
#ifdef Diagnostika
    Serial.println("style.css");
#endif
    server.send ( 200, "text/plain", PAGE_Style_css );
  } );
  server.on ( "/microajax.js", []() {
#ifdef Diagnostika
    Serial.println("microajax.js");
#endif
    server.send ( 200, "text/plain", PAGE_microajax_js );
  } );
  server.on ( "/admin/values", send_network_configuration_values_html );
  server.on ( "/admin/connectionstate", send_connection_state_values_html );
  server.on ( "/admin/infovalues", send_information_values_html );
  server.on ( "/admin/ntpvalues", send_NTP_configuration_values_html );
  server.on ( "/admin/kolektoriusvalues", send_KolektoriausKonfiguracija_values_html );
  server.on ( "/admin/pvoztuvovalues", send_PVoztuvoKonfiguracija_values_html );
  server.on ( "/admin/BoilerisValues", send_BoilerioKonfiguracija_values_html );
  server.on ( "/admin/AkumuliacineValues", send_AkumuliacinesKonfiguracija_values_html );
  server.on ( "/admin/emoncmsvalues", send_Emoncms_values_html );
  server.on ( "/admin/priskirtids18b20values", send_PriskirtiDS18B20_values_html );
  server.on ( "/admin/generalvalues", send_general_configuration_values_html);
  server.on ( "/admin/devicename", send_devicename_value_html);

  server.onNotFound ( []() {
#ifdef Diagnostika
    Serial.println("Page Not Found");
#endif
    server.send ( 200, "text/html", PAGE_NotFound );
  }  );
  ///////////////////////////// OTAWebUpdate start /////////////////////////////
  /*handling uploading firmware file */
  server.on("/naujinimas", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", PAGE_Naujinimas);
  });
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    delay(1000);
    ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.setDebugOutput(true);
      Serial.printf("Update: %s\n", upload.filename.c_str());
      uint32_t maxSketchSpace = (1048576 - 0x1000) & 0xFFFFF000;
      if (!Update.begin(maxSketchSpace)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
      Serial.setDebugOutput(false);
    }
    yield();
  });
  ///////////////////////////// OTAWebUpdate finito /////////////////////////////

  server.begin();


#ifdef Diagnostika
  Serial.printf("\nHTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);
#endif

  tkSecond.attach(1, Second_Tick);
  UDPNTPClient.begin(2390);  // Port for NTP receive

  //  Setup DS18b20 temperature sensor

  SetupDS18B20();
  Setpoint = BoilerisV + config.k_skirtumas;
  //tell the PID to range between 0 and the full window size
  myPID.SetOutputLimits(0, config.WindowSize);

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
  //  timer.setInterval(15000L, KolektoriusT);

  pinMode(CollectorRELAYpin, OUTPUT);

}


void loop ( void ) {

  if (AdminEnabled) {
    if (AdminTimeOutCounter > AdminTimeOut) {
      AdminEnabled = false;
      Serial.println("\n\nAdmin Mode disabled!\n\n");
      WiFi.mode(WIFI_STA);
      Serial.println(WiFi.localIP().toString());
      ConfigureWifi();
    }
  }
  if (config.Update_Time_Via_NTP_Every  > 0 ) {
    if (cNTP_Update > 5 && firstStart)
    { NTPRefresh();
      cNTP_Update = 0;
      firstStart = false;
    }
    else if ( cNTP_Update > (config.Update_Time_Via_NTP_Every * 60) )
    { NTPRefresh();
      cNTP_Update = 0;
    }
  }
  if (DateTime.minute != Minute_Old)
  { Minute_Old = DateTime.minute;
    if (config.AutoTurnOn)
    { if (DateTime.hour == config.TurnOnHour && DateTime.minute == config.TurnOnMinute)
      {
        Serial.println("SwitchON");
        Laikmatis = true;
      }
    }
    Minute_Old = DateTime.minute;
    if (config.AutoTurnOff)
    { if (DateTime.hour == config.TurnOffHour && DateTime.minute == config.TurnOffMinute)
      {
        Serial.println("SwitchOff");
        Laikmatis = false;
      }
    }
  }
  server.handleClient();
  ///////////// *   START Your Code here    * //////////////////////
  //**************** vykdoma programa TemteraturosMatavimas() ************************************************//
  if (millis() > Temperaturos_matavimu_laikas) {
  if (config.k_intervalas < 3) config.k_intervalas = 5;
        TemteraturosMatavimas(); 
Serial.println("\n************ vykdoma programa TemteraturosMatavimas() ********************\n");
#ifdef DEBUGbusena
Serial.print("KKK- ");Serial.print(Katilas);Serial.print(" SK- ");Serial.print(Kolektorius);Serial.print(" PV- ");Serial.println(PVoztuvas);
Serial.print("AV- ");Serial.print(AkumuliacineV);Serial.print(" AA- ");Serial.print(AkumuliacineA);Serial.print(" KO- ");Serial.println(OrasK);
Serial.print("BV- ");Serial.print(BoilerisV);Serial.print(" BA- ");Serial.print(BoilerisA);Serial.print(" OL- ");Serial.println(OrasL);
Serial.println("----");
Serial.print("millis- ");Serial.println(millis()/1000);

if (PV_uzdarytas == false & PV_atidarytas == false) Serial.println("Pamaisymo voztuvo padėtis NEŽINOMA"); 
if (PV_stop == true) {Serial.println("Pamaisymo voztuvas NEJUDA"); 
   }else{ 
if (PV_atidarinejamas = true)  Serial.println("Pamaisymo voztuvas ATIDARINEJAMAS");
if (PV_uzdarinejamas = true)   Serial.println("Pamaisymo voztuvas UZDARINEJAMAS");
   }
    Serial.printf("\nFreeMem: %d \nDabar- %d:%d:%d %d.%d.%d \n", ESP.getFreeHeap(), DateTime.year, DateTime.month, DateTime.day, DateTime.hour, DateTime.minute, DateTime.second);
Temperaturos_matavimu_laikas = millis()+config.k_intervalas * 1000;

#endif
  } 

  // Taimeris nustato laiko intervalus temperatūrų matavimui
  unsigned long currentMillis = millis();
  unsigned long currentMillis1 = millis();

      //------------------------ Boilerio valdymo pradžia ---------------------------------
// boilerio siurblio paleidimas/stabdymas tikrinami kas 1 min (kintamasis Boilerio_siurblio_pertrauka)
   if (millis()> Boilerio_siurblio_ijungimo_laikas ){
#ifdef DEBUGboileris
Serial.println("\n************ vykdoma programa Boileris() ********************\n");
#endif
  Boilerio_sildymas();
  Boilerio_termostatas();
  Boilerio_siurblio_ijungimo_laikas=millis() + Boilerio_siurblio_pertrauka;}
      //------------------------ Boilerio valdymo pabaiga ----------------------------------  
/*
//------------------------------ Akumuliacinės talpos valdymo pradžia -------------------
// Akumuliacinės talpos siurblio paleidimas/stabdymas tikrinami kas 1 min (kintamasis Boilerio_siurblio_pertrauka)
   if (millis()> Akumuliacines_siurblio_ijungimo_laikas ){
#ifdef DEBUG_akumuliacine
Serial.println("\n\n************ vykdoma programa Akumuliacine_talpa() ********************");
#endif
Akumuliacine_talpa ();
  Akumuliacines_siurblio_ijungimo_laikas=millis() + Akumuliacines_siurblio_pertrauka;}
      //------------------------ Akumuliacinės talpos valdymo pabaiga -----------------------------    
      //------------------------ Kolektoriaus valdymo ppradžia -----------------------------    

// tikrinama ar jau laikas tikrinti kolektoriaus siurblio būseną
  if ((unsigned long)(currentMillis - previousMillis) >= config.k_intervalas * 1000)
  { // įsimenamas paskutinio matavimo laikas
    previousMillis = currentMillis;

//------------------- Jei įjungtas nuorinimo režimas ------------------------------
// arba apsauga nuo užšalimo ir kolektoriaus temperatūra artėja prie 0, įjungiamas siurblys
    if (config.k_nuorinimas == 1 or ((Kolektorius < 0.68) & (config.k_uzsalimas == 1)))
    { digitalWrite(CollectorRELAYpin, Ijungta); CollectorRelayState = "Įjungtas";
      Serial.print("\nSiurblio rele įjungta ON (Nuorinimas, užšalimas)\n");
    } else {
      //Jei laikas sutampa su laiku, kai kolektoriaus šiluma niekinė, siurblys išjungiamas
      if (DateTime.hour == config.TurnOffHour or DateTime.hour == config.TurnOffHour + 1 )
      { digitalWrite(CollectorRELAYpin, Isjungta); CollectorRelayState = "Išjungtas(laikas)";
        Serial.print("\nSiurblio rele įjungta OFF (nurodytas išjungimo laikas)\n");
      } else {
        Saules_Kolektoriaus_Siurblys();
      }
    }
  }
      //------------------------ Kolektoriaus valdymo pradžia -----------------------------    
*/
/*
// ---------------------- pamaisymo voztuvo darbas ---------------------------- //
// 1- rankinis, 0- automatinis
if (config.PV_rankinis_ijungimas == 0){
  PamaisymoVoztuvoSiusblys();
  PamaisymoVoztuvoDarbas();
}
  else {
    digitalWrite(RadiatorPumpRELAYpin, Isjungta); // siurblys išjungiamas
    digitalWrite(MixingValveOffRELAYpin, Isjungta); // vožtuvas nebevaldomas
    digitalWrite(MixingValveOnRELAYpin,Isjungta); // vožtuvas nebevaldomas
    PamaisymoV_siurblio_busena = false;
    }
      //--------------------------------------------------------------------------    
*/
/*
// ---------------------- emoncms ---------------------------- //  
  //ar aktyvuotas duomenų siuntimas į emoncms ir jau galima siųsti duomenis
  if ((config.emoncmsOn  == 1) & ((unsigned long)(currentMillis1 - previousMillis1) >= config.intervalasEmon * 1000))
  {
    // įsimenamas paskutinio matavimo laikas
    previousMillis1 = currentMillis1;
    emoncms();
  }
*/



  timer.run();
  ///////////// *    Your Code here END   * //////////////////////

  //	if (Refresh)
  //	{	Refresh = false;
  //		Serial.println("Refreshing...");
  //	}
}
