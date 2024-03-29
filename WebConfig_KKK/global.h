#ifndef GLOBAL_H
#define GLOBAL_H

String FIRMWARE_VERSION = "v0.1";
/* 2019_04_16 19:59 v1.1 Programos naujinimas, pataisyti užšalimo tikrinimo, temperatūrų matavimo algoritmai,
pridėtas nuorinimas*/
/*2019_09_25 v2.0 pridėta akumuliacinės talpos, boilerio, KKK valdymas*/
 
WebServer server(80);                  // The Webserver
//ESP8266HTTPUpdateServer httpUpdater;

boolean firstStart = true;                    // On firststart = true, NTP will try to get a valid time
int AdminTimeOutCounter = 0;                  // Counter for Disabling the AdminMode
strDateTime DateTime;                     // Global DateTime structure, will be refreshed every Second
WiFiUDP UDPNTPClient;                     // NTP Client
unsigned long UnixTimestamp = 0;                // GLOBALTIME  ( Will be set by NTP)
boolean Refresh = false; // For Main Loop, to refresh things like GPIO / WS2812
int cNTP_Update = 0;                      // Counter for Updating the time via NTP
Ticker tkSecond;                        // Second - Timer for Updating Datetime Structure
boolean AdminEnabled = true;    // Enable Admin Mode for a given Time
byte Minute_Old = 240;        // Helpvariable for checking, when a new Minute comes up (for Auto Turn On / Off)

// Generally, you should use "unsigned long" for variables that hold time to handle rollover
unsigned long previousMillis = 0;        // will store last temp was read
unsigned long previousMillis1 = 0;       // will store last temp was read, emoncms write data
unsigned long previousMillis2 = 0;       // will store last temp was read, emoncms write data
 long interval = 20000;              // interval at which to read sensor

struct strConfig {
  String ssid;
  String password;
  byte  IP[4];
  byte  DNS[4];
  byte  Netmask[4];
  byte  Gateway[4];
  boolean dhcp;
  String ntpServerName;
  long Update_Time_Via_NTP_Every;
  int timezone;
  boolean daylight;
  String DeviceName;
  boolean AutoTurnOff;
  boolean AutoTurnOn;
  byte TurnOffHour;
  byte TurnOffMinute;
  byte TurnOnHour;
  byte TurnOnMinute;
  byte LED_R;
  byte LED_G;
  byte LED_B;
/* ********** kintamieji saulės kolektoriui ******************* */
 byte k_skirtumas;
 byte k_intervalas;
 boolean k_uzsalimas;
 boolean k_nuorinimas;

  String emoncmsSrv;
  String apikey;
  String reiksme1;
  String reiksme2;
  String reiksme3;
  String katalogas;
  long intervalasEmon;
  boolean emoncmsOn;
  byte Kid; //Kolektoriaus Ds18b20 id
  byte BVid; //Boilerio viršaus Ds18b20 id
  byte BAid; //Boilerio apačios Ds18b20 id
  byte OLid; //Oro lauko Ds18b20 id
  byte OKid; //Oro kambario Ds18b20 id
/* ********** kintamieji Boileriui ******************* */
  int Bo_ON_T; // temperatūra boilerio siurbliui įjungti
  int Bo_OFF_T; // temperatūra boilerio siurbliui įšjungti
  boolean Bo_Termostatas; // Žymė termostato įjungimui
   /* ********** kintamieji Akumuliacinei talpai ******************* */
  byte At_ON_T;//   temperatūra akumuliacines talpos siurbliui įjungti
  byte At_OFF_T;//  temperatūra akumuliacines talpos siurbliui įšjungti
  byte At_Rankinis_ijungimas; // žymė rankiniam Akumuliacinės talpos siurblio valdymui
  byte AVid; //Akumuliacinės viršaus Ds18b20 id
  byte AAid; //Akumuliacinės apačios Ds18b20 id
/* ************** kintamieji KKK ************************************ */
  byte KKKid; //KKK Ds18b20 id
/* ************** kintamieji PID algoritmui ************************************ */
  double Kp;
  double Ki;
  double Kd;
  int WindowSize;
/* ************************************************** */
/* ********** kintamieji Pamaišymo vožtuvui ******************* */
  byte  PV_palaikoma_T; // PV palaikoma temperatūra 
  float PV_tolerancijos_T; // PV palaikomos temperatūros riba 
  byte  PV_ON_T; // PV įjungimo temperatūra 
  byte  PV_OFF_T; // PV išjungimo temperatūra 
  byte  PV_rankinis_ijungimas; // Žymė rankiniam PV valdymui
  byte  PV_rezimas; // 1- rankinis, 0- automatinis
  byte  PVid; //Pamaišymo vožtuvo Ds18b20 id

/* ********************************************************************** */
}   config;

/*
**
********* temperatūros jutikliai ds18b20 **************
**
*/

//------------------------------------------
//DS18B20
#define ONE_WIRE_BUS 15 //Pin to which is attached a temperature sensor
#define ONE_WIRE_MAX_DEV 15 //The maximum number of devices

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
int numberOfDevices; //Number of temperature devices found
DeviceAddress devAddr[ONE_WIRE_MAX_DEV];  //An array device temperature sensors
float tempDev[ONE_WIRE_MAX_DEV]; //Saving the last measurement of temperature
float tempDevLast[ONE_WIRE_MAX_DEV]; //Previous temperature measurement
long lastTemp; //The last measurement
const int durationTemp = 5000; //The frequency of temperature measurement

//kintamieji saugoti ismatuotoms temperaturu reiksmems
// Katilas- katilo pagaminta šiluma
// Boileris- boilerio viršaus temperatūra
// PVoztuvas- į radijatorius tiekiamo termofikato temperatūra
// LOras, KOras- lauko, kambario oras
float Kolektorius, BoilerisV, BoilerisA, OrasL, OrasK, Katilas, AkumuliacineV, AkumuliacineA, PVoztuvas;
float KolektoriusOld=0, BoilerisVOld=0, BoilerisAOld=0, OrasLOld=0, OrasKOld=0, KatilasOld=0, AkumuliacineVOld=0, AkumuliacineAOld=0, PVoztuvasOld=0;

// Pamaišymo vožtuvo darbiniai kintamieji
  float   PV_klaida;
  boolean PV_atidarytas = false;
  boolean PV_uzdarytas = false;
  boolean PV_atidarinejamas = false;
  boolean PV_uzdarinejamas = false;
  boolean PV_pusiausvyra = true;
  boolean PV_stop = true;
  /* ********** kintamieji siurblio būsenai ******************* */
boolean SK_siurblys = false;
boolean PamaisymoV_siurblio_busena = false;
boolean AkumuliacinesT_Siurblio_busena = false; //akumuliacinės talpos siurblio būsena  0-išjungta, 1-įjungta
bool Boilerio_Siurblio_busena = 0;

bool Boilerio_Termostato_busena = 0; // Žymė termostato busenai
 
#define REQUEST_freezing 5000   // 5000 millis= 5 sekundės
static long timer_freezing=0;   // apsaugos nuo užšalimo tikrinimo laikas

unsigned long Temperaturos_matavimu_laikas = 0;
unsigned long Temperaturos_matavimu_pertrauka = 30000;
unsigned long PV_pauze =20000;
unsigned long PV_pauzes_pertrauka =20000;
unsigned long PV_atidarinejimo_laikas =0;
unsigned long PV_uzdarinejimo_laikas =0;
unsigned long PV_atidarinejimo_pertrauka =6000;
unsigned long PV_uzdarinejimo_pertrauka =6000;
unsigned long Boilerio_siurblio_ijungimo_laikas =0;
unsigned long Boilerio_siurblio_pertrauka =30000;
unsigned long Akumuliacines_siurblio_ijungimo_laikas =0;
unsigned long Akumuliacines_siurblio_pertrauka =30000;
byte PV_darinejimas = 60;
/*
**
********* PID nustatymai **************
**
*/
//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
double Kp=12000, Ki=0, Kd=0;
PID myPID(&Input, &Output, &Setpoint, config.Kp, config.Ki, config.Kd, DIRECT);
//PID myPID(&Input, &Output, &Setpoint, 12000.00, config.Ki, config.Kd, DIRECT);

int WindowSize = 30000;
unsigned long windowStartTime;
/////////////////////////////////////////////////////////////////////////////////
boolean Laikmatis = false;
/////////////////////////////////////////////////////////////////////////////////

String CollectorRelayState = "Išjungta";
String kuzsalimas = "Išjungta";
String knuorinimas = "Išjungta";
String ThermostatRelayState = "Išjungta";
String CollectorState = "Išjungta";
String FreezingState = "Išjungta";

// Relių būsena
#define Ijungta HIGH
#define Isjungta LOW

#define CollectorRELAYpin 2 //išvadas kolektoriaus siurblio junginėjimui, Relė
#define BoilerRELAYpin 32 // išvadas boilerio siurbliui, Relė 
#define BoilerThermostatRELAYpin 33 // išvadas boilerio termostatui, Relė 
#define HeatTankRELAYpin 27 // išvadas ak. talpos siurblio junginėjimui, Relė 
#define RadiatorPumpRELAYpin 14 // išvadas radijatorių siurbliui, Relė 
#define MixingValveOffRELAYpin 13 // išvadas PV uždarymo, Relė 
#define MixingValveOnRELAYpin 12 // išvadas PV atidarymo, Relė 
/*
**
** emoncms duomenų siuntimas
**
*/
// viskas konfigūruojama per naršyklę!
//
/*
**
** CONFIGURATION HANDLING
**
*/


void ConfigureWifi()
{
  Serial.println("Configuring Wifi");
  WiFi.begin (config.ssid.c_str(), config.password.c_str());
  WiFi.setHostname("SauleVire"); Serial.print("Jungiasi.");
    int attempt = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
Serial.print(".");
      attempt++;
      if (attempt >= 5 ) {
//  WiFi.softAP( ACCESS_POINT_NAME , ACCESS_POINT_PASSWORD);
//  Serial.print("PT adresas: " + WiFi.softAPIP().toString()+ "\n");
//  Serial.print("PT vardas: " + String(ACCESS_POINT_NAME) + "\n");
//  Serial.print("Slaptažodis: " + String(ACCESS_POINT_PASSWORD) + "\n\n");
        break;
      }
  }
  if (!config.dhcp)
  {
    WiFi.config(IPAddress(config.IP[0],config.IP[1],config.IP[2],config.IP[3] ),  IPAddress(config.Gateway[0],config.Gateway[1],config.Gateway[2],config.Gateway[3] ) , IPAddress(config.Netmask[0],config.Netmask[1],config.Netmask[2],config.Netmask[3] ));
      Serial.println("Statinis adresas: " + WiFi.localIP().toString()+ "\n");
  }
}


void WriteConfig()
{   Serial.println("Writing Config");
  EEPROM.write(0,'C');  EEPROM.write(1,'F');  EEPROM.write(2,'G');
  EEPROM.commit();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void WriteConfigIP()
{    EEPROM.write(463,config.dhcp);
  EEPROM.write(464,config.IP[0]);
  EEPROM.write(465,config.IP[1]);
  EEPROM.write(466,config.IP[2]);
  EEPROM.write(467,config.IP[3]);
    EEPROM.write(468,config.DNS[0]);
    EEPROM.write(469,config.DNS[1]);
    EEPROM.write(470,config.DNS[2]);
    EEPROM.write(471,config.DNS[3]);
  EEPROM.write(472,config.Netmask[0]);
  EEPROM.write(473,config.Netmask[1]);
  EEPROM.write(474,config.Netmask[2]);
  EEPROM.write(475,config.Netmask[3]);
    EEPROM.write(476,config.Gateway[0]);
    EEPROM.write(477,config.Gateway[1]);
    EEPROM.write(478,config.Gateway[2]);
    EEPROM.write(479,config.Gateway[3]);
  WriteStringToEEPROM(480,config.ssid); // 16
  WriteStringToEEPROM(496,config.password);  //16
    EEPROM.commit();}

void ReadConfigIP()
{    config.dhcp =   EEPROM.read(463);
    config.IP[0] = EEPROM.read(464);
    config.IP[1] = EEPROM.read(465);
    config.IP[2] = EEPROM.read(466);
    config.IP[3] = EEPROM.read(467);
      config.DNS[0] = EEPROM.read(468);
      config.DNS[1] = EEPROM.read(469);
      config.DNS[2] = EEPROM.read(470);
      config.DNS[3] = EEPROM.read(471);
    config.Netmask[0] = EEPROM.read(472);
    config.Netmask[1] = EEPROM.read(473);
    config.Netmask[2] = EEPROM.read(474);
    config.Netmask[3] = EEPROM.read(475);
      config.Gateway[0] = EEPROM.read(476);
      config.Gateway[1] = EEPROM.read(477);
      config.Gateway[2] = EEPROM.read(478);
      config.Gateway[3] = EEPROM.read(479);
    config.ssid = ReadStringFromEEPROM(480);
    config.password = ReadStringFromEEPROM(496);}
    
void WriteConfigGeneralInfo()
{   WriteStringToEEPROM(441,config.DeviceName); //16
  EEPROM.write(457,config.AutoTurnOn);
  EEPROM.write(458,config.AutoTurnOff);
  EEPROM.write(459,config.TurnOnHour);
  EEPROM.write(460,config.TurnOnMinute);
  EEPROM.write(461,config.TurnOffHour);
  EEPROM.write(462,config.TurnOffMinute);
    EEPROM.commit();}

void ReadConfigGeneralInfo()
{   config.DeviceName= ReadStringFromEEPROM(441);
    config.AutoTurnOn = EEPROM.read(457);
    config.AutoTurnOff = EEPROM.read(458);
    config.TurnOnHour = EEPROM.read(459);
    config.TurnOnMinute = EEPROM.read(460);
    config.TurnOffHour = EEPROM.read(461);
    config.TurnOffMinute = EEPROM.read(462);}
        
void WriteConfigDS18b20()
{ EEPROM.write(432,config.Kid);
  EEPROM.write(433,config.BVid);
  EEPROM.write(434,config.BAid);
  EEPROM.write(435,config.OLid);
  EEPROM.write(436,config.OKid);
  EEPROM.write(437,config.AAid);
  EEPROM.write(438,config.AVid);
  EEPROM.write(439,config.PVid);
  EEPROM.write(440,config.KKKid);
    EEPROM.commit();}

void ReadConfigDS18b20()
{   config.Kid =   EEPROM.read(432);
    config.BVid =   EEPROM.read(433);
    config.BAid =   EEPROM.read(434);
    config.OLid =   EEPROM.read(435);
    config.OKid = EEPROM.read(436);
    config.AAid = EEPROM.read(437);
    config.AVid = EEPROM.read(438);
    config.PVid = EEPROM.read(439);
    config.KKKid = EEPROM.read(440);}

void WriteConfigNTP()
{ WriteStringToEEPROM(405,config.ntpServerName);//16
  EEPROM.write(421,config.daylight);
  EEPROMWritelong(425,config.Update_Time_Via_NTP_Every); // 4 Byte
  EEPROM.write(429,config.timezone);  // 4 Byte
    EEPROM.commit();}

void ReadConfigNTP()
{   config.ntpServerName = ReadStringFromEEPROM(405);//16
    config.daylight = EEPROM.read(421);
    config.Update_Time_Via_NTP_Every = EEPROMReadlong(425); // 4 Byte
    config.timezone = EEPROM.read(429);} // 4 Byte
        
void WriteConfigEmonCMS()
{ EEPROM.write(50,config.emoncmsOn);
  EEPROM.write(51,config.intervalasEmon); 
  WriteStringToEEPROM(52,config.reiksme1);//6
  WriteStringToEEPROM(58,config.reiksme2);//6
  WriteStringToEEPROM(64,config.reiksme3);//6
  WriteStringToEEPROM(72,config.katalogas);//6
  WriteStringToEEPROM(78,config.emoncmsSrv);//20
  WriteStringToEEPROM(98,config.apikey);//32
      EEPROM.commit();}
      
void ReadConfigEmonCMS()
{   config.emoncmsOn =   EEPROM.read(50);
    config.intervalasEmon = EEPROM.read(51);
    config.reiksme1= ReadStringFromEEPROM(52);//6
    config.reiksme2= ReadStringFromEEPROM(58);//6
    config.reiksme3= ReadStringFromEEPROM(64);//6
    config.katalogas= ReadStringFromEEPROM(72);//6
    config.emoncmsSrv= ReadStringFromEEPROM(78);//20
    config.apikey = ReadStringFromEEPROM(98);}   //32 

void WriteConfigCollector()
{ EEPROM.write(131,config.k_nuorinimas);
  EEPROM.write(132,config.k_uzsalimas);
  EEPROM.write(133,config.k_intervalas); //
  EEPROM.write(134,config.k_skirtumas);
  EEPROM.put(142,config.Kp);
  EEPROM.put(150,config.Ki);
  EEPROM.put(158,config.Kd);
  EEPROM.put(166,config.WindowSize);
        EEPROM.commit();}

void ReadConfigCollector()
{   config.k_nuorinimas =   EEPROM.read(131);
    config.k_uzsalimas =   EEPROM.read(132);
    config.k_intervalas = EEPROM.read(133);
    config.k_skirtumas = EEPROM.read(134);
  EEPROM.get (142,config.Kp);
  EEPROM.get (150,config.Ki);
  EEPROM.get (158,config.Kd);
  EEPROM.get(166,config.WindowSize);} //

void WriteConfigBoiler()
{  EEPROM.write(174,config.Bo_ON_T);
  EEPROM.write(175,config.Bo_OFF_T);
//  EEPROM.write(176,config.Bo_Rankinis_ijungimas);
  EEPROM.write(177,config.Bo_Termostatas);
//  EEPROM.write(178,config.Bo_Termostato_busena);
        EEPROM.commit();}

void ReadConfigBoiler()
{   config.Bo_ON_T = EEPROM.read(174);
    config.Bo_OFF_T = EEPROM.read(175);
//    config.Bo_Rankinis_ijungimas = EEPROM.read(176);
    config.Bo_Termostatas = EEPROM.read(177);
//    config.Bo_Termostato_busena = EEPROM.read(178);
} //

void WriteConfigHeatStorageTank()
{ EEPROM.write(179,config.At_ON_T);
  EEPROM.write(180,config.At_OFF_T);
  EEPROM.write(181,config.At_Rankinis_ijungimas);
        EEPROM.commit();}
        
void ReadConfigHeatStorageTank()
{   config.At_ON_T = EEPROM.read(179);
    config.At_OFF_T = EEPROM.read(180);
    config.At_Rankinis_ijungimas = EEPROM.read(181);}
            
//////////////////////////////////////////////////////////////////////////////////////////////////
void WriteConfigMixingValve()
{  EEPROM.write(182,config.PV_palaikoma_T);
  EEPROM.write(183,config.PV_ON_T);
  EEPROM.write(184,config.PV_OFF_T);
  EEPROM.write(185,config.PV_tolerancijos_T);
  EEPROM.write(193,config.PV_rankinis_ijungimas);
        EEPROM.commit();}
          
void ReadConfigMixingValve()
{ config.PV_palaikoma_T = EEPROM.read(182);
 config.PV_ON_T = EEPROM.read(183);
 config.PV_OFF_T = EEPROM.read(184);
 config.PV_tolerancijos_T = EEPROM.read(185);
 config.PV_rankinis_ijungimas = EEPROM.read(193);}
//////////////////////////////////////////////////////////////////////////////////////////////////
boolean ReadConfig()
{

  Serial.println("Reading Configuration");
  if (EEPROM.read(0) == 'C' && EEPROM.read(1) == 'F'  && EEPROM.read(2) == 'G' )
  {
    Serial.println("Configurarion Found!");
ReadConfigGeneralInfo();
ReadConfigIP();
ReadConfigDS18b20();
ReadConfigNTP();
ReadConfigEmonCMS();
ReadConfigCollector();
ReadConfigBoiler();
ReadConfigHeatStorageTank();
ReadConfigMixingValve();
    return true;
  }  else {
    Serial.println("Configurarion NOT FOUND!!!!");
    return false;}
}
/*
**
**  NTP 
**
*/
//const int NTP_PACKET_SIZE = 48; 
byte packetBuffer[ NTP_PACKET_SIZE]; 
void NTPRefresh()
{

  if (WiFi.status() == WL_CONNECTED)
  {
    IPAddress timeServerIP; 
    WiFi.hostByName(config.ntpServerName.c_str(), timeServerIP); 
    //sendNTPpacket(timeServerIP); // send an NTP packet to a time server


    Serial.println("sending NTP packet...");
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    packetBuffer[12]  = 49;
    packetBuffer[13]  = 0x4E;
    packetBuffer[14]  = 49;
    packetBuffer[15]  = 52;
    UDPNTPClient.beginPacket(timeServerIP, 123); 
    UDPNTPClient.write(packetBuffer, NTP_PACKET_SIZE);
    UDPNTPClient.endPacket();


    delay(1000);
  
    int cb = UDPNTPClient.parsePacket();
    if (!cb) {
      Serial.println("NTP no packet yet");
    }
    else 
    {
      Serial.print("NTP packet received, length=");
      Serial.println(cb);
      UDPNTPClient.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
      unsigned long secsSince1900 = highWord << 16 | lowWord;
      const unsigned long seventyYears = 2208988800UL;
      unsigned long epoch = secsSince1900 - seventyYears;
      UnixTimestamp = epoch;
    }
  }
}

void Second_Tick()
{
  strDateTime tempDateTime;
  AdminTimeOutCounter++;
  cNTP_Update++;
  UnixTimestamp++;
  ConvertUnixTimeStamp(UnixTimestamp +  (config.timezone *  360) , &tempDateTime);
  if (config.daylight) // Sommerzeit beachten
    if (summertime(tempDateTime.year,tempDateTime.month,tempDateTime.day,tempDateTime.hour,0))
    {
      ConvertUnixTimeStamp(UnixTimestamp +  (config.timezone *  360) + 3600, &DateTime);
    }
    else
    {
      DateTime = tempDateTime;
    }
  else
  {
      DateTime = tempDateTime;
  }
  Refresh = true;
}
 

#endif
