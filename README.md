# Wireless-Sensor-Networks--Assignment
## Main Idea
<p align = justify> The main idea of this project is to implement a transceiver system used in a school classroom. The system will consist of a class beacon node and multiple user nodes that belong to each student individually. <br>
The beacon is placed in the center of the classroom and tuned in a specific frequency. The beacon receives the heartbeats of the students within the classroom when in receiver mode and sends off data concerning the class conditions (temperature, humidity, airquality) to all students when in transmitter mode. <br>
On the other hand, the each student's node receives the class conditions data when functioning in receiver mode and transmits the student's heart data when in transmitter mode, to monitor their health. </p>

## Hardware
- At least two Arduino Boards, one that mimics the beacon system and the other that mimics the equipment worn by the student.
- At least two NRF24L01 transceiver antennas, one for each Arduino Board.
- A DHT temperature and humidity sensor.
- An MQ-135 air quality sensor.
- A heart rate pulse sensor compatible with the Arduino platform.
