# SmartHome_Micro-bit
Build a smart system control applicants in the house.
The code mainly involves using FSM to optimize the opeartion of the system.
---------------Living Room
- Equipment: LCD Adafruit, PIC sensor, Buzzer.
- Functions: 
      + Initialize the 4-digit key pass for the user to unlock the lock.
          + Have the PIR sensor works as the security lock. 
          + Alert the user if any danger was found(run out of time or wrong pass key).
---------------Kitchen + Garden
-Equiment: LCD Adafruit, DHT11 sensor, YL69 Soil Moisture sensor, 
           LightIntensity sensor, MQ2 Gas sensor, Buzzer, DC Fan,
           DC Pump, Bluetooth HC05.
-Functions:
          + Measure the temperature and humidity in the kitchen.
          + Send the date to the user via a controller mobile app using bluetooth.
          + Activate/Disactivate the fan correspond to the environment.
          + Measure the soil humidity and light intensity in the garden.
          + Send the date to the user via a controller mobile app using bluetooth.
          + Activate/Disactivate light system and DC pump correspond to environment.
---------------Bedroom
-Equipment: LCD Adafruit, RealTime clock, LightIntensity sensor, Buzzer.
-Functions:
          + Automatically turn on/off light according to timeline: morning, noon, evening.
          + RealTime clock: display current date and time.
          + Alarm mode.
