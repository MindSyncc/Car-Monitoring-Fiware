<h1>Car Monitoring IoT Project</h1>  
This project is part of the Edge Computing Sprint, focusing on creating an IoT-based car monitoring system. It involves monitoring the speed and overtaking of two cars in a simulated environment. Using an ultrasonic sensor, potentiometers, and the ESP32 microcontroller, this system provides real-time data on car speed and overtakes, communicated via the MQTT protocol and managed through the Fiware platform.

What is IoT?  
IoT, or the Internet of Things, refers to the interconnection of physical devices through a network, allowing them to collect and exchange data without human intervention. The technology is significant in automating processes, enhancing efficiency, and collecting valuable real-time data. In this project, IoT enables the remote monitoring of vehicle speeds and overtaking events using sensors, microcontrollers, and MQTT communication.

<h2>Project Architecture</h2>  
  
The IoT architecture for this project consists of the following components:

<h3>Physical Devices:</h3>  

* Ultrasonic sensor for distance measurement
* Two potentiometers to simulate car speed control
* LCD Display to show both car speeds
* ESP32 Devkit V1 as the microcontroller
<img src="https://i.imgur.com/Q0DWD6b.png"/>  
<h3>Communication Protocol:</h3>

The MQTT protocol is used for sending sensor data to a central server. MQTT is an efficient, lightweight protocol suitable for IoT devices with limited bandwidth.
Fiware Platform:
<div>
<img src="https://encrypted-tbn0.gstatic.com/images?q=tbn:ANd9GcRGWJVn9EgMFSXLLpaoL_v59oHNyJnrUcJEjQ&s" width="300"/>
</div>

**Fiware** is used to manage real-time data flow, logging the car's information. Services include:
* Mosquitto (MQTT Broker)
* Orion Context Broker (for context management)
* STH-Comet (for storing historical data)
How It Works
Car Speed Simulation
Two potentiometers simulate the speed of two cars. The values from these potentiometers are read by the ESP32 and displayed on an LCD. The ESP32 also uses an ultrasonic sensor to measure the distance between cars, which helps determine overtakes.

<h3>Overtaking Logic</h3>
The system checks for an overtaking scenario when one car’s speed is at least 30% greater than the other car's speed, and the distance between the cars is less than 400 cm. If these conditions are met, the overtaking event is logged.

<h3>Data Flow</h3>
<p>The ESP32 collects speed and overtaking data and sends it to the Fiware platform using the MQTT protocol.</p>
<p>The Orion Context Broker registers the devices and stores their states.</p>
STH-Comet records historical speed and overtaking data in MongoDB, which can be visualized in Google Colab via graphs.
Platform Setup
Simulator - Wokwi
The project uses the Wokwi simulator to test hardware interactions, such as sensor readings and potentiometer-based speed control.

<h3>Fiware Setup</h3>
* IoT Agent MQTT: Handles MQTT communication between the ESP32 and Fiware.
* Orion Context Broker: Manages real-time data and device registration.
* STH-Comet: Stores historical data on MongoDB for future analysis.
* Instructions
<h3>Wowki Setup</h3>
Load the ESP32 code into Wokwi.
Simulate sensor and potentiometer interactions.
Verify that speed and overtaking data is displayed on the LCD.

<h3>Postman Collection</h3>
In Postman, the proyect has the following requisitions: 

<h4>Folder 1: IoT Agent MQTT</h4>

1. Health Check
2. Provisioning a Service Group for MQTT
3. Provisioning Car Monitoring
4. Registering Car Monitoring Commands
5. List All Devices Provisioned
6. Result of Both Cars' Speed
7. Result of Both Cars' Overtakes
8. Delete Car Monitoring in IoT Agent
9. Delete Car Monitoring in Orion Context Broker

<h4>Folder 2: STH-Comet</h4>

1. Health Check
2. Subscribe Car Data
3. Request Car Speed History
   
<h4>Folder 3: Orion Context Broker</h4>

1. Version
2. Create
3. Get
4. Delete

<h4>Visualizing data on Google Colab</h4>
To display the speed charts of both cars as well as the number of overtakes made by each, you can run the two code blocks in Google Colab. The first block will show two speed curves, one for the first car and another for the second. The second block will display a bar chart with the number of overtakes made by car 1 and car 2.

<h3>And then, how to create my own device?</h3>
By modifying the id "fiware_carros_monitor" to the one of your preference in both the Wokwi simulator and the Postman Collection you can create your own device and collect your own data, not worrying about doing all the process of creating a fiware service on a cloud virtual machine, just change it in all the fields that id appears in and everything should work perfectly.

<hr>
This project is still in development, stay tuned for new updates and happy coding 😀!
