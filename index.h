const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">
	<link rel=\"icon\" href=\"data:,\">
	<style>
    html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}    
    text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}

    .button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;
    .button2 { background-color: #555555; }

    .slidecontainer { width: 100%; }
    
    .slider {
      -webkit-appearance: none;
      width: 100%;
      height: 25px;
      background: #d3d3d3;
      outline: none;
      opacity: 0.7;
      -webkit-transition: .2s;
      transition: opacity .2s;
    }
    
    .slider:hover { opacity: 1; }
    
    .slider::-webkit-slider-thumb {
      -webkit-appearance: none;
      appearance: none;
      width: 25px;
      height: 25px;
      background: #4CAF50;
      cursor: pointer;
    }
    
    .slider::-moz-range-thumb {
      width: 25px;
      height: 25px;
      background: #4CAF50;
      cursor: pointer;
    }  
</style>
</head>

<body>

<div id="demo">
<h1>The Living Room Blinds are <span id="TILTState">OPEN</span><br></h1>
	<button type="button" onclick="sendData(180)">Tilt Up</button><BR>
	<button type="button" onclick="sendData(90)">Tilt Open</button><BR>
	<button type="button" onclick="sendData(0)">Tilt Down</button><BR>
</div>

<div class="slidecontainer">
  <p>Tilt slider A: <span id="sA"></span></p>
  <input type="range" min="1" max="180" value="90" class="slider" id="TiltA">
  <p>Tilt slider B: <span id="sB"></span></p>
  <input type="range" min="1" max="180" value="90" class="slider" id="TiltB">
  <p>Tilt slider C: <span id="sC"></span></p>
  <input type="range" min="1" max="180" value="90" class="slider" id="TiltC">    
</div>


<script>
  function sendData(val) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("TILTState").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "setTILT?TILTstate="+val, true);
    xhttp.send();
  }


  var sliderA = document.getElementById("TiltA");
  var sliderB = document.getElementById("TiltB");
  var sliderC = document.getElementById("TiltC");
  
  var outputA = document.getElementById("sA");
  outputA.innerHTML = sliderA.value;  
  sliderA.onchange = function() {
    sendCustom(this.id, this.value);    
    outputA.innerHTML = this.value;
  }
  var outputB = document.getElementById("sB");
  outputB.innerHTML = sliderB.value;  
  sliderB.onchange = function() {
    sendCustom(this.id, this.value);   
    outputB.innerHTML = this.value;
  }
  var outputC = document.getElementById("sC");
  outputC.innerHTML = sliderC.value;  
  sliderC.onchange = function() {
    sendCustom(this.id, this.value);   
    outputC.innerHTML = this.value;
  }

  function sendCustom(slide, val) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById(slide).innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "customTILT?Tnum="+slide+"&Tstate="+val, true);
    xhttp.send();  
  }

</script>
<br>
</body>
</html>
)=====";
