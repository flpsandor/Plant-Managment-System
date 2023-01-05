# Plant growth system
Arduino controlled plant growth management system with more individual soil and air, temperature and humidity control.

## Main idea
Checking soil and air, moisture, and temperature and controlling ventilation, heating, and watering. We control the moisture in the air by activating or turning off the fans or creating a mist effect if the values are too low. The air temperature is regulated by two sources. Two small PTC heaters to control temperature to a small degree, and a large 1kw blowing radiator. Soil temperature is controlled by heating cable, and soil humidity by small water nozzles. The light is controlled by the RTC module.

![Screenshot 2022-08-23 131313](https://user-images.githubusercontent.com/111133064/188284658-a9c3b89f-8e13-41a5-824c-6c36ba73c85d.png)

## Hardware used
- Arduino Uno clone.
- Analog moisture sensor. It reads the moisture level of the pot depending on the amount of water present on it.
- DHT11. Digital sensor to measure air temp and air humidity.
- One Wire temperature sensor for measure soil temperature.
- Custom RGB UV Light
- Two 12V water pump. One for direct soil watering. One for creating water mist.
- Two fans in the opposite direction for creating draft and controling hummidity.
- One heating cable for soil heating.
- One old hairdryer (with some sawing) for heating air.
- RTC for controling light

## Required libraries
- DHT.h
- OneWire.h
- DallasTemperature.h
- Wire.h
