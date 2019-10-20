#ifndef PAGE_EXAMPLE_H
#define PAGE_EXAMPLE_H
//
//   The EXAMPLE PAGE
//
const char PAGE_EXAMPLE[] PROGMEM = R"=====(
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<div class="smartphone">
  <div class="content">
<!-- <center><span class="textas">Katilinės valdiklis</span></center><hr> -->
<table border="0"  cellspacing="0" cellpadding="3" style="width:300px" >
<th colspan="2">
<a href="/admin.html" class="myButton">Valdiklio konfigūravimas</a>
<br>
<b>Dabartinė ūkio būsena</b>
</th><tr><td>
<div align="right">Kolektorius :</div>
<div align="right">Oras lauke:</div>
<div align="right">Kolektoriaus siurblio būsena :</div>
<div align="right">Apsauga (0- išjungta):</div>
<div align="right">Nuorinimas (0- išjungta):</div>
<br>
<div align="right">BoilerisV t :</div>
<div align="right">BoilerisA t :</div>
<div align="right">Termostatas :</div>
<br>
<div align="right">Akumuliacinės viršus t :</div>
<div align="right">Akumuliacinės apačia t :</div>
<div align="right">Ak. talpos siurblys :</div>
<div align="right">Ak. talpos rankinis valdymas :</div>
<br>
<div align="right">Oras kambaryje :</div>
<div align="right">Radijatorių t :</div>
<div align="right">PVožtuvo rankinis valdymas :</div>
<br>
<div align="right">Katilas:</div>
</td><td> 
<div><span id="K_t"></span>&deg; C</div>
<div><span id="OL_t"></span>&deg; C</div> 
<div><span id="rele"></span></div> 
<div><span id="apsauga"></span></div> 
<div><span id="nuorinimas"></span></div>
<br>
<div><span id="BV_t"></span>&deg; C</div>
<div><span id="BA_t"></span>&deg; C</div>
<div><span id="Bo_rankinis"></span></div>
<br>
<div><span id="AV_t"></span>&deg; C</div>
<div><span id="AA_t"></span>&deg; C</div>
<div><span id="AK_siurblys"></span></div>
<div><span id="AK_rankinis"></span></div>
<br>
<div><span id="OK_t"></span>&deg; C</div>
<div><span id="radijatoriu_t"></span>&deg; C</div>
<div><span id="pv_rankinis_ijungimas"></span></div>
<br>
<div><span id="KKK_t"></span>&deg; C</div>
</td></tr>
<th colspan=2><a href="/"  class="myButton">Atnaujinti</a></th>
</table>
<hr>
<span class="textas">Dabar : <span id="x_ntp"></span><br>
<!-- Valdiklis veikia <span id="x_ntp2"></span> nuo <span id="x_ntp2"></span><br></span> -->



<p><small><a href=https://saulevire.lt>SauleVire.lt</a> © 2019</small></p>
</div>
</div></center>
<!-- <div id="K_t">Here comes the Dynamic Data in </div> -->
<!-- added a DIV, where the dynamic data goes to -->
   <script>                
    window.onload = function ()
    { load("style.css","css", function() 
      { load("microajax.js","js", function() 
        { setValues("/admin/filldynamicdata");  //-- this function calls the function on the ESP      
        });
      });
    }
    function load(e,t,n){
      if("js"==t){var a=document.createElement("script");
    a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}
    else if("css"==t){var a=document.createElement("link");
    a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}
   </script>

)=====";
#endif


void filldynamicdata()
{        
  String values ="";
  values += "K_t|" + (String)Kolektorius +  "|div\n";
  values += "OL_t|" + (String)OrasL +  "|div\n";
  values += "rele|" + (String)CollectorRelayState +  "|div\n";
  values += "apsauga|" + (String)config.k_uzsalimas +  "|div\n";
  values += "nuorinimas|" + (String)config.k_nuorinimas +  "|div\n";
  
  values += "BV_t|" + (String)BoilerisV +  "|div\n";
  values += "BA_t|" + (String)BoilerisA +  "|div\n";
  values += "Bo_rankinis|" + ThermostatRelayState +  "|div\n";

  values += "AV_t|" + (String)AkumuliacineV +  "|div\n";
  values += "AA_t|" + (String)AkumuliacineA +  "|div\n";
  values += "AK_siurblys|" + (String)AkumuliacinesT_Siurblio_busena +  "|div\n";
  values += "AK_rankinis|" + (String)config.At_Rankinis_ijungimas +  "|div\n";

  values += "OK_t|" + (String)OrasK +  "|div\n";
  values += "radijatoriu_t|" + (String)PVoztuvas +  "|div\n";
  values += "pv_rankinis_ijungimas|" + (String)config.PV_rankinis_ijungimas +  "|div\n";

  values += "KKK_t|" + (String)Katilas +  "|div\n";
  
  values += "x_ntp|" + (String)DateTime.year + "." + (String)DateTime.month + "." + (String)DateTime.day + " " + (String)DateTime.hour + ":" + (String)DateTime.minute + ":" + (String)DateTime.second + " |div\n";
  values += "x_ntp2|" + (String)(NTP.getTimeDateString (NTP.getFirstSync ()).c_str ())+ " |div\n";
  values += "x_ntp1|" + (String)(NTP.getUptimeString ())+ " |div\n";
  server.send ( 200, "text/plain", values); 
}

void processIndex()
{        
    if (server.args() > 0 )  // Are there any POST/GET Fields ? 
    {
       for ( uint8_t i = 0; i < server.args(); i++ ) {  // Iterate through the fields
            if (server.argName(i) == "firstname") 
            {
                 // Your processing for the transmitted form-variable 
                 String fName = server.arg(i);
            }
        }
    }
    server.send ( 200, "text/html", PAGE_EXAMPLE  ); 
}
