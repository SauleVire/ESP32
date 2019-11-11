//------------------------------------------
//Convert device id to String
String GetAddressToString(DeviceAddress deviceAddress){
  String str = "";
  for (uint8_t i = 0; i < 8; i++){
    if( deviceAddress[i] < 16 ) str += String(0, HEX);
    str += String(deviceAddress[i], HEX);
  }
  return str;
}

//Loop measuring the temperature
void TempLoop(long now){
  if( now - lastTemp > durationTemp ){ //Take a measurement at a fixed time (durationTemp = 5000ms, 5s)
    for(int i=0; i<numberOfDevices; i++){
      float tempC = DS18B20.getTempC( devAddr[i] ); //Measuring temperature in Celsius
      tempDev[i] = tempC; //Save the measured value to the array
    }
    DS18B20.setWaitForConversion(false); //No waiting for measurement
    DS18B20.requestTemperatures(); //Initiate the temperature measurement
    lastTemp = millis();  //Remember the last time measurement
  }
}
//Setting the temperature sensor
void SetupDS18B20(){
  DS18B20.begin();

  Serial.print("Parasite power is: "); 
  if( DS18B20.isParasitePowerMode() ){ 
    Serial.println("ON");
  }else{
    Serial.println("OFF");
  }
  
  numberOfDevices = DS18B20.getDeviceCount();
  Serial.print( "Device count: " );
  Serial.println( numberOfDevices );delay(3000);

  lastTemp = millis();
  DS18B20.requestTemperatures();

  // Loop through each device, print out address
  for(int i=0;i<numberOfDevices; i++){
    // Search the wire for address
    if( DS18B20.getAddress(devAddr[i], i) ){
      //devAddr[i] = tempDeviceAddress;
      Serial.print("Found device ");
      Serial.print(i, DEC);
      Serial.print(" with address: " + GetAddressToString(devAddr[i]));
      Serial.println(); delay(30);
    }else{
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.print(" but could not detect address. Check power and cabling"); delay(30);
    }

    //Get resolution of DS18b20
    Serial.print("Resolution: ");
    Serial.print(DS18B20.getResolution( devAddr[i] ));
    Serial.println();

    //Read temperature from DS18b20
    float tempC = DS18B20.getTempC( devAddr[i] );
    Serial.print("Temp C: ");
    Serial.println(tempC);
  }
}

// reads the temp and humidity from the DHT sensor and sets the global variables for both
void TemteraturosMatavimas() {
  Kolektorius = DS18B20.getTempC(devAddr[config.Kid]);
  BoilerisV = DS18B20.getTempC(devAddr[config.BVid]);
  BoilerisA = DS18B20.getTempC(devAddr[config.BAid]);
  OrasL = DS18B20.getTempC(devAddr[config.OLid]);
  OrasK = DS18B20.getTempC(devAddr[config.OKid]);
  AkumuliacineV = DS18B20.getTempC(devAddr[config.AVid]);
  AkumuliacineA = DS18B20.getTempC(devAddr[config.AAid]);
  PVoztuvas = DS18B20.getTempC(devAddr[config.PVid]);
  Katilas = DS18B20.getTempC(devAddr[config.KKKid]);

  // Check if any reads failed and exit
#ifdef DEBUGds18b20
if (Kolektorius == -127 or Kolektorius == 85 or Kolektorius > 127 ) 
Serial.println("Klaida! Kolektorius rodmenys neteisingi");  
if (BoilerisV == -127 or BoilerisV == 85 or BoilerisV > 127 ) 
    Serial.println("Klaida! BoilerisV rodmenys neteisingi");  
if (BoilerisA == -127 or BoilerisA == 85 or BoilerisA > 127 ) 
    Serial.println("Klaida! BoilerisA rodmenys neteisingi");  
if (OrasL == -127 or OrasL == 85 or OrasL > 127 )
    Serial.println("Klaida! OrasL rodmenys neteisingi");  
if (OrasK == -127 or OrasK == 85 or OrasK > 127 ) 
    Serial.println("Klaida! OrasK rodmenys neteisingi");  
if (Katilas == -127 or Katilas == 85 or Katilas > 127 ) 
    Serial.println("Klaida! Katilas rodmenys neteisingi");  
if (AkumuliacineV == -127 or AkumuliacineV == 85 or AkumuliacineV > 127 ) 
    Serial.println("Klaida! AkumuliacineV rodmenys neteisingi");  
if (AkumuliacineA == -127 or AkumuliacineA == 85 or AkumuliacineA > 127 ) 
    Serial.println("Klaida! AkumuliacineA rodmenys neteisingi");  
if (PVoztuvas == -127 or PVoztuvas == 85 or PVoztuvas > 127 ) 
    Serial.println("Klaida! Ds18B20 rodmenys neteisingi");  
#endif


  DS18B20.requestTemperatures();
  
}
