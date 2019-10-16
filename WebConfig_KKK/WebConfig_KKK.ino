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

#include "helpers.h"
#include "global.h"
#include "ds18b20.h"
#include <SimpleTimer.h>

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
#include "PAGE_NetworkConfiguration.h"
#include "Page_NotFound.h"
#include "Page_Kolektorius.h"
#include "Page_Boileris.h"
#include "Page_Akumuliacine.h"
#include "Page_Emoncms.h"
#include "Page_Index.h"
#include "Page_DS18B20.h"
#include "Page_PriskirtiDS18B20.h"
#include "Page_Pvoztuvas.h"

#define Diagnostika 1 // Naudojama tik testavimui
#define ACCESS_POINT_NAME  "SauleVire_PT"				
#define ACCESS_POINT_PASSWORD  "" 
#define AdminTimeOut 300  // Defines the Time in Seconds, when the Admin-Mode will be disabled
SimpleTimer timer;
const char* host = "SauleVire";


void setup ( void ) {
	EEPROM.begin(4096);

#ifdef Diagnostika
	Serial.begin(115200);
	Serial.println("Čia SauleVire.lt pradžia\n");
#endif
	if (!ReadConfig())
	{
		// DEFAULT CONFIG
		config.ssid = "LEDE"; //belaidžio tinklo pavadinimas
		config.password = "12345678"; //slaptažodis
		config.dhcp = true;
    config.IP[0] = 192;config.IP[1] = 168;config.IP[2] = 8;config.IP[3] = 111;
    config.DNS[0] = 1;config.DNS[1] = 1;config.DNS[2] = 1;config.DNS[3] = 1;
		config.Netmask[0] = 255;config.Netmask[1] = 255;config.Netmask[2] = 255;config.Netmask[3] = 0;
		config.Gateway[0] = 192;config.Gateway[1] = 168;config.Gateway[2] = 8;config.Gateway[3] = 1;
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
    config.Bo_Rankinis_ijungimas = false; // Žymė rankiniam AT siurblio valdymui
    config.Bo_Termostatas_ON = false; // Žymė rankiniam termostato įjungimui
    config.Bo_Termostato_busena = false; // Žymė termostato busenai
/* ********** kintamieji Akumuliacinei talpai ******************* */
    config.At_ON_T = 90; // temperatūra akumuliacines talpos siurbliui įjungti
    config.At_OFF_T = 89; // temperatūra akumuliacines talpos siurbliui įšjungti
    config.At_Rankinis_ijungimas = 0; // Žymė rankiniam AT siurblio valdymui
/* ********** Pamaišymo vožtuvo nustatymai ************************************ */   
    config.PV_ON_T = 40; // temperatūra radijatorių siurbliui įjungti
    config.PV_OFF_T = 35; // temperatūra radijatorių siurbliui įšjungti
    config.PV_palaikoma_T = 40; // Į radiatorius tiekiamo termofikato temperatūra 
    config.PV_tolerancijos_T = 1.2; // Į radiatorius tiekiamo termofikato tolerancijos temperatūra 
    config.PV_rankinis_ijungimas = false; // Žymė rankiniam radijatorių siurblio valdymui
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
	

	server.on ( "/", processIndex  );
	server.on ( "/admin/filldynamicdata", filldynamicdata );
	server.on ( "/favicon.ico",   []() { 
#ifdef Diagnostika	  
	  Serial.println("favicon.ico"); 
#endif   
	  server.send ( 200, "text/html", "" );   }  );
	server.on ( "/admin.html", []() {
#ifdef Diagnostika   
	  Serial.println("admin.html"); 
#endif 
	  server.send ( 200, "text/html", PAGE_AdminMainPage );   }  );
	server.on ( "/config.html", send_network_configuration_html );
  server.on ( "/info.html", []() { 
#ifdef Diagnostika   
  Serial.println("info.html"); 
#endif  
  server.send ( 200, "text/html", PAGE_Information );   }  );
	server.on ( "/ntp.html", send_NTP_configuration_html  );
	server.on ( "/general.html", send_general_html  );
  server.on ( "/example.html", []() {
#ifdef Diagnostika    
    Serial.println("example.html"); 
#endif
    server.send ( 200, "text/html", PAGE_EXAMPLE );  } );
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
    server.send ( 200, "text/plain", PAGE_Style_css );  } );
	server.on ( "/microajax.js", []() { 
#ifdef Diagnostika
	  Serial.println("microajax.js"); 
#endif
	  server.send ( 200, "text/plain", PAGE_microajax_js );  } );
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
	server.begin();

//  MDNS.begin(host);
//  httpUpdater.setup(&server);
//  MDNS.addService("http", "tcp", 80);
#ifdef Diagnostika
  Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);
#endif

	tkSecond.attach(1,Second_Tick);
	UDPNTPClient.begin(2390);  // Port for NTP receive

//  Setup DS18b20 temperature sensor

  SetupDS18B20();
Setpoint = Boileris + config.k_skirtumas;
  //tell the PID to range between 0 and the full window size
  myPID.SetOutputLimits(0, config.WindowSize);

  //turn the PID on
  myPID.SetMode(AUTOMATIC);
//  timer.setInterval(15000L, KolektoriusT);

  pinMode(RELAYPIN,OUTPUT);

}

 
void loop ( void ) {
//  digitalWrite(2, HIGH);
	if (AdminEnabled) {
		if (AdminTimeOutCounter > AdminTimeOut) {
			 AdminEnabled = false;
			 Serial.println("\n\nAdmin Mode disabled!\n\n");
//        WiFi.reconnect();
        WiFi.mode(WIFI_STA);
          Serial.println(WiFi.localIP().toString());
//        WiFi.begin(ssid, password);
      ConfigureWifi();
		             }	}
	if (config.Update_Time_Via_NTP_Every  > 0 ) {
		if (cNTP_Update > 5 && firstStart)
		{	NTPRefresh();
			cNTP_Update =0;
			firstStart = false;
		}
		else if ( cNTP_Update > (config.Update_Time_Via_NTP_Every * 60) )
		{	NTPRefresh();
			cNTP_Update =0;
		}
	}
	if(DateTime.minute != Minute_Old)
	{	 Minute_Old = DateTime.minute;
		 if (config.AutoTurnOn)
		 { if (DateTime.hour == config.TurnOnHour && DateTime.minute == config.TurnOnMinute)
			 { Serial.println("SwitchON"); Laikmatis = true;}
		 }
		 Minute_Old = DateTime.minute;
		 if (config.AutoTurnOff)
		 { if (DateTime.hour == config.TurnOffHour && DateTime.minute == config.TurnOffMinute)
			 {Serial.println("SwitchOff"); Laikmatis = false;}
		 }
	}
	server.handleClient();
///////////// *   START Your Code here    * //////////////////////
/****************************************************************/


// Taimeris nustato laiko intervalus temperatūrų matavimui
  unsigned long currentMillis = millis();
  unsigned long currentMillis1 = millis();

// tikrinama ar jau laikas matuoti temperatūrą
if (config.k_intervalas < 3) config.k_intervalas = 3;
  if ((unsigned long)(currentMillis - previousMillis) >= config.k_intervalas * 1000)
  { // įsimenamas paskutinio matavimo laikas
    previousMillis = currentMillis;

    TemteraturosMatavimas();
// Jei įjungtas nuorinimo režimas arba apsauga nuo užšalimo ir kolektoriaus temperatūra artėja prie 0, įjungiamas siurblys
    if (config.k_nuorinimas == 1 or ((Kolektorius < 0.68) & (config.k_uzsalimas == 1)))
          { digitalWrite(RELAYPIN, HIGH); RelayState = "Įjungtas";
       Serial.print("\nSiurblio rele įjungta ON (Nuorinimas, užšalimas)\n");
       } else {
//Jei laikas sutampa su laiku, kai kolektoriaus šiluma niekinė, siurblys išjungiamas
        if (DateTime.hour == config.TurnOffHour or DateTime.hour == config.TurnOffHour +1 )
          { digitalWrite(RELAYPIN, LOW); RelayState = "Išjungtas(laikas)";
       Serial.print("\nSiurblio rele įjungta OFF (nurodytas išjungimo laikas)\n");
       } else {Siurblys();}
       }
    
}
/* ****************************** emoncms ****************************** */
//ar aktyvuotas duomenų siuntimas į emoncms ir jau galima siųsti duomenis
    if ((config.emoncmsOn  == 1) & ((unsigned long)(currentMillis1 - previousMillis1) >= config.intervalasEmon * 1000)) 
    {
      // įsimenamas paskutinio matavimo laikas
      previousMillis1 = currentMillis1;
      emoncms();
//      postEmoncms();
    }
#ifdef Diagnostika
// tikrinama ar jau laikas matuoti temperatūrą
  if (millis() - previousMillis2 >= 10000)
  { // įsimenamas paskutinio matavimo laikas
    previousMillis2 = millis();
    Serial.print("\nemoncmsOn - "); Serial.print(config.emoncmsOn);
    Serial.print(", k_uzsalimas - "); Serial.print(config.k_uzsalimas);
    Serial.print("\nKol- "); Serial.print(Kolektorius);
    Serial.print(", Boi- "); Serial.print(Boileris);
    Serial.print(", OLa- "); Serial.print(OrasL);
    Serial.print("\nOKa- "); Serial.print(OrasK);
    Serial.print(", AAp- "); Serial.print(AkumuliacineA);
    Serial.print(", AVi- "); Serial.print(AkumuliacineV);
    Serial.print("\nPVo- "); Serial.print(PVoztuvas);
    Serial.print(", KKK- "); Serial.println(Katilas);
    
    Serial.printf("\nFreeMem: %d \nDabar- %d:%d:%d %d.%d.%d \n",ESP.getFreeHeap(), DateTime.year, DateTime.month, DateTime.day, DateTime.hour, DateTime.minute, DateTime.second);
  }
#endif
//  if (WiFi.mode(WIFI_STA))
//    machineIOs.SetLeds(noChange, noChange, (((millis() / 125) & 7) == 0) ? On : Off); // 1 Hz blink with 12.5% duty cycle
//  else
//    machineIOs.SetLeds(noChange, noChange, (((millis() / 125) & 7) != 0) ? On : Off); // 1 Hz blink with 87.5% duty cycle
//  machine.ModulateSound(((millis() / 63) & 7) == 0); // 2 Hz blink with 12.5% duty cycle (1.984... Hz)
/****************************************************************/

//  MDNS.update();
//  timer.run();
///////////// *    Your Code here END   * //////////////////////
	
	if (Refresh)  
	{	Refresh = false;
//		Serial.println("Refreshing...");
//		Serial.printf("FreeMem:%d %d:%d:%d %d.%d.%d \n",ESP.getFreeHeap() , DateTime.hour,DateTime.minute, DateTime.second, DateTime.year, DateTime.month, DateTime.day);
	}
}
