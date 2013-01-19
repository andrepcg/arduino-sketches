What is this?
=============
This is a personal project to monitor my house's power consumption. It uses a photoresistor to detect power meter LED pulses.

Layout
======
1x ATTiny85 as remote sensor with TX unit
1x Arduino UNO as base with RX unit

How it works
============
The remote sensor collects the LED pulses and sends the count in 10 minutes interval.
The base receives the counts and sends the data to a php script, the script processes the data to get the power usage (in kWh) then sends to the unplu.gg website