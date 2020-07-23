Proximity Demo

NB!
This firmware contains an nRF SoftDevice, restricted by the license included in this archive: "license.txt".


Once programmed, the nRF52DK will act as a Proximity Reporter and a Find Me Locator.
A Proximity Reporter can trigger an alert on a Proximity Monitor.
A Find Me locator can trigger an alert on a Find Me Target.
Using the Proximity app in the nRF Toolbox, our phone will be a Proximity Monitor and Find Me
Target.

1. Check that LED 1 on the nRF52DK is blinking. This indicates advertising. If it's not blinking,
   you may have to wake it up from sleep by pressing button 1. Other reasons could be that you
   forgot to turn the nRF52DK on, or the programming of the nRF52DK failed.

2. In the nRF Toolbox app, open Proximity app and press the "connect" button. The nRF52DK
   should appear in the list of devices as "Nordic_Prox". Press on the name to connect.

4. Check that LED 1 on the nRF52DK is lit to indicate a connection (instead of blinking).
   In the nRF Toolbox app, the padlock symbol will be unlocked when we are in a connection.


5. In the nRF Toolbox app, press "Find Me" and observe that LED 3 on the nRF52DK is blinking.
   This indicates an alert. Press "Find Me" again and observe that LED 3 turns off.

6. On the nRF52, press button 1. Observe that an alert triggered on the phone.
   Press button 1 again to dismiss the alert.

7. Separate the devices (for example by switching off the nRF52 or by walking far enough away)
   and observe that a Link Loss is signaled by the padlock symbol being locked. It will unlock
   again once the connection has been reestablished.

6. on the nRF52DK Press and hold button 1 for a few seconds to disconnect from the phone.
   This takes us back to the advertising state. Check that LED 1 on the nRF52DK is blinking to
   indicate advertising.

7. While advertising, press button 1 to go to sleep mode. Check that LED 1 on the nRF52DK is off.
   This indicates sleep mode. Sleep Mode is automatically entered if the Proximity Demo has
   advertised for 3 minutes without finding a connection.
   
8. While in sleep mode, press button 1 to wake up.
   Check that LED 1 on the nRF52DK is blinking to indicate advertising.

Note: When switching between demos, the name ("Nordic_HRS"/"Nordic_Prox") will sometimes be wrong
      in iOS because it still remembers the previous name for the connection.