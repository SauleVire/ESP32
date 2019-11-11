
void Akumuliacine_talpa () {
              #ifdef DEBUG_akumuliacine
              Serial.print("\nAT ijungimo temperatura- ");Serial.print(config.At_ON_T);Serial.print("°C");
              Serial.print("\nAT isjungimo temperatura- ");Serial.print(config.At_OFF_T);Serial.print("°C");
              Serial.print("\nAT siurblio būsena- ");Serial.print(AkumuliacinesT_Siurblio_busena);
              if (AkumuliacinesT_Siurblio_busena == true) Serial.print("\nAT siublio busena: IJUNGTAS *** ON ***");
              else Serial.print("\nAT siublio busena: ISJUNGTAS *** OFF ***");
              Serial.print("\nKatilo temperatura- ");Serial.print(Katilas);Serial.print("°C\n\n");
              #endif
// jei AT išjungimo temperatūra nustatoma mažesnėė, negu įjungimo, tai ji keičiama į didesnę
if (config.At_OFF_T < config.At_ON_T) 
    {config.At_OFF_T = config.At_ON_T + 1;
    WriteConfigHeatStorageTank();
    Serial.print("\n Pakeista AT ON temperatūraDEBUG_akumuliacine\n");
    }

  // jei katilo temperatūra yra didesnė, negu nustatyta akumuliacinės talpos šildymo įjungimo temperatūra,
  // tai įjungiamas akumuliacinės talpos siurblys
 if ((Katilas >= config.At_ON_T) && (AkumuliacinesT_Siurblio_busena == false))  {
   digitalWrite(HeatTanktRELAYpin, Ijungta); // Įjungiamas akumuliacinės talpos siurblys
   AkumuliacinesT_Siurblio_busena = true;
#ifdef DEBUG_akumuliacine
Serial.println("\n Akumuliacines talpos siublys IJUNGTAS *** ON ***");
#endif
 }
              // jei katilo išėjime yra mažiau šilumos, negu nustatyta akumuliacinės talpos šildymo išjungimo temperatūra,
              // tai išjungiamas akumuliacinės talpos siurblys
              if ((Katilas <= config.At_OFF_T) && (AkumuliacinesT_Siurblio_busena == true)){
              digitalWrite(HeatTanktRELAYpin, Isjungta); // Išjungiamas akumuliacinės talpos siurblys
              AkumuliacinesT_Siurblio_busena = false;
              #ifdef DEBUG_akumuliacine
              Serial.println("\n Akumuliacines talpos siublys ISJUNGTAS *** OFF ***");
              #endif
              } 
}
/*
// Praėjus nustatytam laikui ir jei tenkinama sąlyga išjungimui pagal temperatūras
// įrašoma žymė, kad baigėsi rankinio siurblio įjungimo veikimas
void Akumuliacine_talpa_rankinis (){
  if ((millis() < AT_rankinio_ijungimo_laikas + AT_rankinio_ijungimo_trukme ) && (KI <= At_OFF_T)){
  } else {AT_rankinis_ijungimas = false;}
}
*/
