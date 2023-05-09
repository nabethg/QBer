# Project Proposal: QBer — Rubik's cubing robot


## Summary
We propose to make a Rubik’s Cube solving robot for our final project which will be able to manipulate and solve a Rubik’s Cube. The robot will have two modes:

1.	**Autonomous mode**: The robot detects the pattern on the cube using a camera, then computes a solution and manipulates the cube appropriately to solve it. 

2.	**Manual mode**: The robot will be able to be manually controlled via Bluetooth using a GUI on a phone.


## Sensor Inputs
-	Raspberry Pi Camera to capture visual images of the cube and detect the pattern on the cube

-	HC-05 Bluetooth module to read Bluetooth signals from a phone for manual control of the robot


## Power
-	The Raspberry Pi will be powered via the USB-C port using a USB power supply connected to a wall outlet.

-	The Arduino Uno will receive power from Raspberry Pi via the USB port and from a 9V battery via the barrel connector.


## Raspberry Pi Connections
-	Raspberry Pi Camera via the built-in camera connector to read the image from the camera

-	Arduino Uno via USB, to power the Arduino and send/receive data via Serial


## Arduino Uno Connections
-	Three servo motors: one continuous servo for rotating the cube, and two standard servos for flipping and holding the cube

-	HC-05 Bluetooth Module to receive Bluetooth signals from a phone, for manual control

-	A 1602 LCD for displaying the status of the robot and the total time it took to solve the cube

-	Raspberry Pi via USB, to power the Arduino and send/receive data via Serial


## Autonomous Mode
The Raspberry Pi will read the image from the camera to determine the pattern on the cube, then compute the steps to solve the cube and send the steps via Serial to the Arduino.

The Arduino will be responsible for moving the servo motors according to the commands sent from the Raspberry Pi. The servo motors will be controlled by the Arduino using the Servo library. One of the motors will be a continuous servo attached to a square platform, where the cube will sit, and it will be responsible for rotating the cube. The other two servos will be attached to an arm, and they will work together to flip the cube or hold onto the top two layers of the cube. If the first servo is rotated while the arm is lowered, the arm will hold the top two layers of the cube in place while the platform rotates, thereby rotating the bottom face of the cube. This allows the robot to turn any of the six faces of the cube since it can flip and rotate any of the faces to the bottom and then rotate the bottom face. This design also minimizes the number of servo motors required to solve the cube, making it more practical to build with the available resources.


## Manual Mode 
In addition to being connected to the Raspberry Pi and three servos, the Arduino will also be connected to an HC-05 Bluetooth module via Software Serial. The Bluetooth module will be able to read Bluetooth signals from a phone. The RemoteXY app will be used to control the switch between separate operating modes - manual and autonomous, and will also be used to move the robot manually using a GUI.


## Reference Designs
[BricKuber Project – A Raspberry Pi Rubiks Cube Solving Robot](https://www.dexterindustries.com/projects/brickuber-project-raspberry-pi-rubiks-cube-solving-robot-project/)

[CUBOTone: a Rubik Cube Solver Robot, With Raspberry Pi and Picamera](https://www.instructables.com/Rubik-Cube-Solver-Robot-With-Raspberry-Pi-and-Pica/)

[NXT LEGO Rubik's Cube Solver](https://grabcad.com/library/nxt-lego-rubik-s-cube-solver-1)
