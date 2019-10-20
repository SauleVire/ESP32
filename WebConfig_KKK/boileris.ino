void Boilerio_sildymas(){

// jei boilerio išjungimo temperatūra nustatoma mažesnė, negu įjungimo, tai ji keičiama į didesnęę
if (config.Bo_OFF_T <= config.Bo_ON_T) 
    {config.Bo_OFF_T = config.Bo_ON_T + 1;
    void WriteConfigBoiler();
    Serial.print("\n pakeistos boilerio on/off temperatūros");  }

  #ifdef DEBUGboileris
  if (Boilerio_Siurblio_busena == true) Serial.print("\nBoilerio siublio busena: IJUNGTAS *** ON ***");
  else Serial.print("\nBoilerio siublio busena: ISUNGTAS *** OFF ***");
  #endif 

// Jei  boilerio viršus šaltesnis už boilerio šildymo išjungimo temperatūrą, 
// o katilo išėjime yra daugiau šilumos, negu nustatyta boilerio šildymo pradžios  
// temperatūra +10 C, tai įjungiamas boilerio siurblys
     if ((BoilerisV < config.Bo_ON_T || BoilerisV < config.Bo_OFF_T) && Katilas > config.Bo_ON_T + 10 && Boilerio_Siurblio_busena == false){
       if (Katilas > BoilerisV + 10){
       digitalWrite(BoilerRELAYPIN, Ijungta); 
       Boilerio_Siurblio_busena = true;
#ifdef DEBUGboileris
Serial.print("\nBoilerio siublys IJUNGTAS ***_ON_ ***__PRIEZASTIS:");
Serial.print("\nB isjungimo temperatura- ");Serial.print(config.Bo_OFF_T);Serial.print("°C");
Serial.print("\nB ijungimo temperatura- ");Serial.print(config.Bo_ON_T);Serial.print("°C");
Serial.print("\nI isejime temperatura- ");Serial.print(Katilas);Serial.print("°C");
#endif 
   }}
// Jei boilerio viršuje yra tiek šilumos, kiek nustatyta, arba katilo išėjime yra mažiau šilumos, 
// negu boilerio viršuje, tai siurblys išjungiamas
     if ((BoilerisV >= config.Bo_OFF_T || Katilas <= BoilerisV + 8) && (Boilerio_Siurblio_busena == true)) { 
       digitalWrite(BoilerRELAYPIN, Isjungta); 
       Boilerio_Siurblio_busena = false;
#ifdef DEBUGboileris
Serial.print("\nBoilerio siublys ISUNGTAS ***_OFF_***__PRIEZASTIS:");
Serial.print("\nB isjungimo temperatura- ");Serial.print(config.Bo_OFF_T);Serial.print("°C");
Serial.print("\nB ijungimo temperatura- ");Serial.print(config.Bo_ON_T);Serial.print("°C");
Serial.print("\nI isejime temperatura- ");Serial.print(Katilas);Serial.print("°C");
#endif      
   }     
}
//**************************************************************************************************************
void Boilerio_termostatas(){
    #ifdef DEBUGboileris 
    if (config.Bo_Termostatas == true) Serial.print("\nTermostatas: IJUNGTAS *** ON ***");
    else Serial.print("\nTermostatas: ISJUNGTAS *** OFF ***");
    #endif 
// Jei boilerio viršus šaltesnis negu nustatyta įjungimo temperatūra, arba boilerio viršus šaltesnis
// už boilerio išjungimo temperatūrą, ir įjungtas termostatas, tai jungiamas elektrinis boilerio sildymas
if ((BoilerisV < config.Bo_ON_T ) && (config.Bo_Termostatas == true)){
       digitalWrite(BoilerThermostatRELAYPIN, Ijungta); 
       Boilerio_Termostato_busena = true; // zyme, kad termostatas dabar aktyvus
#ifdef DEBUGboileris
Serial.print("\nBoilerio sildymas elektra I_J_U_N_G_T_A_S ***_ON_ *** iki- "); 
Serial.print(config.Bo_OFF_T);Serial.print("°C");
#endif 
   }else{
       digitalWrite(BoilerThermostatRELAYPIN, Isjungta); 
       Boilerio_Termostato_busena = false; // zyme, kad termostatas dabar neaktyvus    
#ifdef DEBUGboileris
Serial.print("\nBoilerio sildymas elektra I_S_J_U_N_G_T_A_S ***_OFF_ ***");
#endif    }
}
// Jei boilerio virsuje yra tiek šilumos, kiek nustatyta,  tai termostatas išjungia boilerio sildymą elektra
     if ((BoilerisV >= config.Bo_OFF_T)  && (Boilerio_Termostato_busena == true)) { 
       digitalWrite(BoilerThermostatRELAYPIN, Isjungta);
       Boilerio_Termostato_busena = false;
#ifdef DEBUGboileris
Serial.print("\nBoilerio sildymas elektra I*S*J*U*N*G*T*A*S ***_OFF_ *** nuo- ");
Serial.print(BoilerisV);Serial.print("°C");
#endif      
   }     
}
