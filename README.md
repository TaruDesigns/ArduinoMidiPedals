# ArduinoMidiPedals
A simple project to convert standard expression/footswitch pedals into MIDI data using Arduino. This project uses hardware MIDI Output (5 Pin DIN connector). It shouldn't be hard to convert it to USB Midi by using an Arduino Nano instead.
 
One day I might make a proper board for it, with PCB mounted jacks and USB Power.

I've added 3D models for an enclosure. This enclosure was made with [OnShape](https://cad.onshape.com/documents/4eba54e11bb9bf6d27ddf1b2/w/d35c74aab7e3221d58318394/e/4fd7029cd8b945e5e373a2fa?renderMode=0&uiState=6328c16b08d11d2174f0bdc6)

The analog input is filtered and will only send values when the reading has changed.

