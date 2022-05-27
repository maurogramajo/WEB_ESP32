# WEB_ESP32

Con una interfaz web se controla una tira led RGB.

Para evitar glitches en el manejo de la tira rgb es necesario que entre el ESP32 y esta se coloque un conversor de 3v3 a 5v. Ya que los leds WS2812b funcionan a 5v logicos.
Esto soluciona los problemas de HW que pueden llegar a existir.

# EJEMPLO FUNCIONANDO!!!