var url = "ws://192.168.0.184:81/";
// Connect to WebSocket server
var websocket= new WebSocket(url);

//COLOR PICKER
var colorWheel = new iro.ColorPicker("#picker", {

    width: 200,
  color: "rgb(255, 0, 0)",
  borderWidth: 1,
  borderColor: "#fff"  
});

var values = document.getElementById("values");

function init() {
    wsConnect(url);
}

// Call the init function as soon as the page loads
window.addEventListener("load", init, false);

function wsConnect(url) {    
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

function cambiarEstadoLed() {
    doSend("toggleLED");
    doSend("getLEDState");
}

colorWheel.on(["color:init", "color:change"], function(color){
    values.innerHTML = ["rgb: " + color.rgbString].join("<br>");
    if (websocket.readyState) {
        doSend(color.rgbString);
    }
});