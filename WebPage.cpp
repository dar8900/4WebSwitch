// #include "4WebSwitch.h"

// #ifdef NODE_MCU

// #include "Web.h"
// #include <Arduino.h>


// const char* Header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";

// String WebPage_1  = R"=====(

// <!DOCTYPE html>
// <html>
 // <head>
 // <meta name='viewport' content='width=device-width, initial-scale=1.0'/>
 // <meta charset='utf-8'>
 // <style>
  // body {
		// font-size:140%; background-color: #e6f0ff;
  // } 
  // h1 {
		// color: #ffbf80;
		// font-family: "Verdana";
		// text-align:center;
		// text-shadow: 1px 1px 2px  #660000, 0 0 25px black, 0 0 5px black;
		// text-transform:uppercase;
  // } 
  // table#measure{
		// color: black;
		// font-family: "Verdana";
		// font-size: 35px;
		// text-align: center;
		// width: 100%
  // }
  // table#led_table{
		// border-spacing: 30px;
		// width:100%;
		// text-align:center;
		// border-radius: 15px 50px 30px;
		// background-color: #b3ffb3;
		// box-shadow: 10px 10px 5px grey;
  // }
  // input{
		// display: inline-block;
        // padding: 15px 25px;
        // cursor: pointer;
        // text-align: center;
        // text-decoration: none;
        // outline: none;
        // border: 1px;
		// color:	#e6f0ff;
		// font-size: 30px; 
		// background-color:green;
		// text-transform: uppercase;
		// border-radius: 25px;
		// border-style: outset;
		// box-shadow: 0 8px 16px 0 rgba(0,0,0,0.2), 0 6px 20px 0 rgba(0,0,0,0.19);
	// }
	// div#main{
		// background-color: #e6f0ff;
	// }
   // div#table {
		// border: 2px;
		// border-style: outset;
		// border-radius: 15px 50px 30px;
		
		// background-color:#ffff99;
   // }
   	// div#table_measure {
		// border: 2px;
		// border-style: outset;
		// border-radius: 15px 50px 30px;
		// text-align:center;
		// background-color:#ffff99;
   // }
 // </style>
 
// <script>
	// function InitFunc()
	// {
		// var i;
		// var LedName, ButtonElement;
		// var PresaNameOn = "Accendi presa ";
		// var PresaNameOff = "Spegni presa ";
		// for (i = 1; i < 5; i++)
		// {
			// LedName = "Rele"+i;
			// PresaNameOn = "Accendi presa " + i;
			// PresaNameOff = "Spegni presa " + i;
			// ButtonElement = document.getElementById(LedName);
			// if(ButtonElement.value == PresaNameOn)
			// {
				// ButtonElement.style.background = "#b30000";				
			// }
			// else
			// {
				// ButtonElement.style.background = "#73e600";
			// }
		// }
	// }

	// function switchLED(wichled) 
	// {
		// var LedName = "Rele" + wichled;
		// var AjaxLedCommandON = 'RELEON_' + wichled;
		// var AjaxLedCommandOFF = 'RELEOFF_' + wichled;
		// var PresaNameOn = "Accendi presa " + wichled;
		// var PresaNameOff = "Spegni presa " + wichled;
		// var button_text = document.getElementById(LedName);
		
		// if (button_text.value==PresaNameOn)
		// {
			// button_text.value = PresaNameOff;
			// button_text.style.background = "#73e600";
			// button_text.style.color = "black";
			// ajaxLoad(AjaxLedCommandON); 
		// }
		// else
		// {
			// button_text.value = PresaNameOn;
			// button_text.style.background = "#b30000";
			// button_text.style.color = "#e6f0ff";
			// ajaxLoad(AjaxLedCommandOFF);
		// }
	// }

	// var ajaxRequest = null;
		// if (window.XMLHttpRequest)  { ajaxRequest = new XMLHttpRequest(); }
		// else                        { ajaxRequest = new ActiveXObject("Microsoft.XMLHTTP"); }


	// function ajaxLoad(ajaxURL)
	// {
		// if(!ajaxRequest){ alert("AJAX is not supported."); return; }

		// ajaxRequest.open("GET",ajaxURL,true);
		// ajaxRequest.onreadystatechange = function()
		// {
			// if(ajaxRequest.readyState == 4 && ajaxRequest.status==200)
			// {
			  // var ajaxResult = ajaxRequest.responseText;
			  // var i;
			  // for(i = 1; i < 5; i++)
			  // {
					// if( ajaxURL == ("RELE" + i + "STATUS") && (ajaxResult == "STATUS_ON" || ajaxResult == "STATUS_OFF"))   
					// {
						// var button_text = document.getElementById("Rele" + i);
						// if(ajaxResult == "STATUS_ON")
						// {
							// button_text.value = "Spegni presa " + i;
							// button_text.style.background = "#73e600";
							// button_text.style.color = "black";						
						// }
						// else
						// {
							// button_text.value = "Accendi presa " + i;
							// button_text.style.background = "#b30000";
							// button_text.style.color = "#e6f0ff";					
						// }
					// }
			  // }
			// }
		// }
		// ajaxRequest.send();
	// }
 
// </script>
 
 // <title>4 Switch</title>
// </head>

// <script type="text/javascript"> 
   // setInterval(function(){ 
	// ajaxLoad("RELE1STATUS"); 

   // },2000) 
 // </script> 
 
// <script type="text/javascript"> 
   // setInterval(function(){ 
	// ajaxLoad("RELE2STATUS"); 

   // },3000) 
 // </script> 
 
// <script type="text/javascript"> 
   // setInterval(function(){ 
	// ajaxLoad("RELE3STATUS"); 

   // },4000) 
 // </script> 
 
// <script type="text/javascript"> 
   // setInterval(function(){ 
	// ajaxLoad("RELE4STATUS"); 

   // },5000) 
 // </script> 

// <body onload = "InitFunc()">
 // <div id='main'>
  // <h1>Controllo prese</h1>
  // <div id="table">
  // <table id="led_table" >
  // <tr>
// )=====";

// String ReleTable_1 = R"=====(
     // <th><input type="button" id = "Rele1" onclick="switchLED(1)" value="Accendi presa 1"/> </th>
// )=====";

// String TableSep_1 = R"=====(  </tr> 
// <tr>
// )====="; 

// String ReleTable_2 = R"=====(
    // <td><input  type="button" id = "Rele2" onclick="switchLED(2)" value="Accendi presa 2"/> </td>
// )=====";

// String TableSep_2 = R"=====(  </tr> 
// <tr>
// )====="; 

// String ReleTable_3 = R"=====(
    // <td><input  type="button" id = "Rele3" onclick="switchLED(3)" value="Accendi presa 3"/> </td>
// )=====";

// String TableSep_3 = R"=====(  </tr> 
// <tr>
// )====="; 

// String ReleTable_4 = R"=====(
    // <td><input  type="button" id = "Rele4" onclick="switchLED(4)" value="Accendi presa 4"/></td>
// )=====";

// String MeasureLines = R"=====(   
 // </tr>
// </table>
// </div>
 // </div>
 // <br>
// <br>

// <div id="table_measure">
// <p style = "text-align:center; font-size: 40px; font-family: Verdana; "> <b>MISURE</b></p>

// <table id = "measure">
// <th colspan = "2"> Energia misurata </th>
// <tr>
// <td id = "energy_measure" style="text-align:right">0.000</td> <td style="text-align:left"> kWh </td>
// </tr>

// <th colspan = "2"> Potenza misurata </th>
// <tr>
 // <td id = "power_measure" style="text-align:right"> 0.000 </td> <td style="text-align:left"> kW </td>
// </tr>

// <th colspan = "2"> Corrente misurata </th>
// <tr>
// <td id = "current_measure" style="text-align:right"> 0.000 </td> <td style="text-align:left"> A </td>
// </tr>
// </table>
// </div>

 // </div>
// </body>
// </html>
// )=====";

// #endif