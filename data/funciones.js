var url = "ws://192.168.0.184:81/";

function init() {
    wsConnect(url);
}

function wsConnect(url) {
    // Connect to WebSocket server
    websocket = new WebSocket(url);
    
    // Assign callbacks
    websocket.onopen = function(evt) { onOpen(evt) };
    websocket.onclose = function(evt) { onClose(evt) };
    websocket.onmessage = function(evt) { onMessage(evt) };
    websocket.onerror = function(evt) { onError(evt) };
}

function onOpen(evt) {

    // Log connection state
    console.log("Connected");
    
    // Get the current state of the LED
    doSend("getLEDState");
}

// Called when the WebSocket connection is closed
function onClose(evt) {

    // Log disconnection state
    console.log("Disconnected");
    
    // Try to reconnect after a few seconds
    setTimeout(function() { wsConnect(url) }, 2000);
}

function onMessage(evt) {

    // Print out our received message
    console.log("Received: " + evt.data);
    
    // Update circle graphic with LED state
    switch(evt.data) {
        case "0":
            console.log("LED is off");
            document.getElementById("led").src = "LED_OFF.png"
            break;
        case "1":
            console.log("LED is on");
            document.getElementById("led").src = "LED_ON.png"
            break;
        default:
            break;
    }
}

// Called when a WebSocket error occurs
function onError(evt) {
    console.log("ERROR: " + evt.data);
}

// Sends a message to the server (and prints it to the console)
function doSend(message) {
    console.log("Sending: " + message);
    websocket.send(message);
}

// Call the init function as soon as the page loads
window.addEventListener("load", init, false);

function cambiarEstadoLed() {

    doSend("toggleLED");
    doSend("getLEDState");

}

//COLOR PICKER
var colorWheel = new iro.ColorPicker("#picker", {

    width: 200,
  color: "rgb(255, 0, 0)",
  borderWidth: 1,
  borderColor: "#fff",
    // layout: [
    // { 
    //   component: iro.ui.Wheel,
    //   options: {
    //     width: 200,
    //     wheelLightness: true,
    //     wheelAngle: 0,
    //     wheelDirection: "anticlockwise",
    //     borderWidth: 1,
    //     borderColor: "#fff"
    //   } 
    // }
    // ]
  
});

var values = document.getElementById("values");

// https://iro.js.org/guide.html#color-picker-events
colorWheel.on(["color:init", "color:change"], function(color){
  // Show the current color in different formats
  // Using the selected color: https://iro.js.org/guide.html#selected-color-api
  values.innerHTML = [
    //"hex: " + color.hexString,
    "rgb: " + color.rgbString,
    //"hsl: " + color.hslString,
  ].join("<br>");
});