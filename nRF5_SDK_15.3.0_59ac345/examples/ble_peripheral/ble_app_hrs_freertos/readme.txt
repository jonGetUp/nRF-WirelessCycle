Heart Rate Sensor Demo

NB!
This firmware contains an nRF SoftDevice, restricted by the license included in this archive: "license.txt".


Once programmed, the nRF52DK will act as a Heart Rate Sensor and send heart rate measurements
to connected heart rate monitors. Use the Heart Rate Monitor (HRS) in the nRF Toolbox app to
connect to the nRF52DK and display these measurements.

1. Check that LED 1 on the nRF52DK is blinking. This indicates advertising. If it's not blinking,
   you may have to wake it up from sleep by pressing button 1. Other reasons could be that you
   forgot to turn the nRF52DK on, or the programming of the nRF52DK failed.

2. In the nRF Toolbox app, open the Heart Rate Monitor and press the "connect" button. The nRF52DK
   should appear in the list of devices as "Nordic_HRS".

4. Check that LED 1 on the nRF52DK is lit to indicate a connection (instead of blinking).
   When a connection has successfully been established, heart rate measurements will appear on the
   phone.

5. Press and hold button 1 for a few seconds to disconnect from the phone. This takes us back to the
   advertising state. Check that LED 1 on the nRF52DK is blinking to indicate advertising.

6. While advertising, press button 1 to go to sleep mode. Check that LED 1 on the nRF52DK is off.
   This indicates sleep mode. Sleep Mode is automatically entered if the Heart Rate Sensor has
   advertised for 3 minutes without finding a connection.
   
7. While in sleep mode, press button 1 to wake up.
   Check that LED 1 on the nRF52DK is blinking to indicate advertising.

Note: When switching between demos, the name ("Nordic_HRS"/"Nordic_Prox") will sometimes be wrong
      in iOS because it still remembers the previous name for the connection.