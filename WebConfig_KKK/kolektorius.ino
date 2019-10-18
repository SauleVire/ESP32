void Saules_Kolektoriaus_Siurblys(){
  Input = Kolektorius;
  Setpoint = BoilerisV + config.k_skirtumas;

     myPID.SetTunings(config.Kp, config.Ki, config.Kd); 
     myPID.Compute();  
       if (millis() - windowStartTime > WindowSize)
  {
    //time to shift the Relay Window
    windowStartTime += WindowSize;
  }
  Serial.print("\nInput: ");  Serial.print(Input);
  Serial.print("\nSetpoint: ");  Serial.print(Setpoint);
  Serial.print("\nOutput: ");  Serial.print(Output);
  Serial.print("\nconfig.Kp reikšmė: ");  Serial.print(config.Kp);
  Serial.print("\nwindowStartTime: ");  Serial.print(windowStartTime);
  Serial.print("\nmill-Wstart: ");  Serial.print((millis() - windowStartTime) / 1000);
  Serial.print("\nmilliseconds ("); Serial.print((Output * 100.0) / config.WindowSize, 0);  Serial.println("%)");
  
  if ((Output < (millis() - windowStartTime) / 1000) ) 
      { digitalWrite(CollectorRELAYPIN, HIGH); 
      RelayState = "Įjungtas";
       Serial.print("\nSiurblio rele įjungta ON (Siurblio ciklas)\n");
      }
  else 
      { digitalWrite(CollectorRELAYPIN, LOW); 
      RelayState = "Išjungtas";
      Serial.print("\nSiurblio rele išjungta OFF (Siurblio ciklas)\n");
      }
 
}

     
  // Tikrinama ar įjungta ir reikalinga k_uzsalimas nuo užšalimo
void k_uzsalimas(){
      if (((Kolektorius < 0.25) & (config.k_uzsalimas == 1)) or (config.k_nuorinimas == 1)) {
        digitalWrite(CollectorRELAYPIN, HIGH); 
        RelayState = "Įjungtas";
        Serial.print("\nSiurblio rele įjungta ON (Apsaugos ciklas)\n");}

}
