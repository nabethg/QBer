/*
  QBer - Rubik's cubing robot
  Copyright © 2023 Nabeth Ghazi

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, version 3.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <LiquidCrystal.h>
#include <Servo.h>

#define PROCEED (byte)0
#define CAP_FACE (byte)0
#define REL_CW (byte)1
#define REL_CCW (byte)2
#define REL_2 (byte)3
#define HOLD_CW (byte)4
#define HOLD_CCW (byte)5
#define HOLD_2 (byte)6
#define FLIP (byte)7
#define REL (byte)8
#define HOLD (byte)9
#define DONE (byte)10

#define BLANK_STRING "                " // blank a line of the LCD
#define BASE_CCW 180
#define BASE_CW 0
#define BASE_REL_ROT_90_MS 350
#define BASE_REL_ROT_180_MS 654
#define BASE_HOLD_ROT_90_MS 360
#define BASE_HOLD_ROT_180_MS 720

#define BASE_MOTOR_PIN 9
#define ARM_MOTOR_1_PIN 10
#define ARM_MOTOR_2_PIN 11

/*
  This source code of graphical user interface has been generated automatically
  by RemoteXY editor.
*/
//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// RemoteXY select connection mode and include library
#define REMOTEXY_MODE__SOFTSERIAL
#include <RemoteXY.h>
#include <SoftwareSerial.h>

// RemoteXY connection settings
#define REMOTEXY_SERIAL_RX 2
#define REMOTEXY_SERIAL_TX 3
#define REMOTEXY_SERIAL_SPEED 9600

// RemoteXY configurate
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] = {
    255, 5, 0, 0, 0, 73, 0, 15, 30, 0, 1, 0, 69, 21, 14, 14,
    2, 31, 70, 76, 73, 80, 0, 1, 0, 62, 35, 14, 14, 2, 31, 226,
    134, 187, 0, 1, 0, 78, 35, 14, 14, 2, 31, 226, 134, 186, 0, 2,
    1, 11, 30, 24, 13, 2, 26, 31, 31, 72, 79, 76, 68, 0, 82, 69,
    76, 0, 1, 0, 43, 9, 14, 14, 2, 31, 83, 79, 76, 86, 69, 0};

// this structure defines all the variables and events of your control
// interface
struct
{
  // input variables
  uint8_t flip;   // =1 if button pressed, else =0
  uint8_t CWRot;  // =1 if button pressed, else =0
  uint8_t CCWRot; // =1 if button pressed, else =0
  uint8_t arm;    // =1 if switch ON and =0 if OFF
  uint8_t solve;  // =1 if button pressed, else =0

  // other variable
  uint8_t connect_flag; // =1 if wire connected, else =0

} RemoteXY;
#pragma pack(pop)

/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////

Servo armMotor1;
Servo armMotor2;
Servo baseMotor;
LiquidCrystal LCD(A0, A1, A2, A3, A4, A5);

/**
 * Moves the arm of the robot a given position by rotating the two servo motors.
 *
 * @param pos a position from 0 to 180 to move the arm to
 */
void moveArm(int pos)
{
  armMotor1.write(pos);
  armMotor2.write(180 - pos);
  delay(1000); // travel time
}

/**
 * Rotates the base holding the cube in a given speed and time while the top two
 * layers of the cube are either or held or released.
 *
 * @param hold  whether or not the top two layers of the cube should be held
 * @param speed the rotational speed of the continuos servo motor
 * @param time  an unsigned long representing the duration of the rotation
 */
void rotate(boolean hold, int speed, unsigned long time)
{
  // make sure cube is released/held
  if (hold)
    perform(HOLD);
  else
    perform(REL);
  // rotate cube in a certain direction indicated by speed
  baseMotor.attach(BASE_MOTOR_PIN);
  baseMotor.write(speed);
  delay(time);
  baseMotor.write(90); // stop
  delay(50);
  baseMotor.detach();
  delay(100);
}

/**
 * Performs a given step to manipulate the cube or control the robot.
 *
 * @param step a byte representing the step to be performed by the robot
 */
void perform(byte step)
{
  switch (step)
  {
  case CAP_FACE:
    perform(REL);
    Serial.write(PROCEED);
    delay(2000);
    break;
  case REL_CW:
    rotate(0, BASE_CW, BASE_REL_ROT_90_MS);
    break;
  case REL_CCW:
    rotate(0, BASE_CCW, BASE_REL_ROT_90_MS);
    break;
  case REL_2:
    rotate(0, BASE_CCW, BASE_REL_ROT_180_MS);
    break;
  case HOLD_CW:
    rotate(1, BASE_CW, BASE_HOLD_ROT_90_MS);
    break;
  case HOLD_CCW:
    rotate(1, BASE_CCW, BASE_HOLD_ROT_90_MS);
    break;
  case HOLD_2:
    rotate(1, BASE_CCW, BASE_HOLD_ROT_180_MS);
    break;
  case FLIP:
    armMotor1.write(180);
    armMotor2.write(0);
    delay(800);
    armMotor1.write(20);
    armMotor2.write(160);
    delay(800);
    break;
  case REL:
    moveArm(0);
    break;
  case HOLD:
    moveArm(90);
    break;
  }
  delay(800);
}

/**
 * Displays two lines of text to the Liquid Crystal Display.
 *
 * @param line1 a string to be displayed on the first line of the LCD
 * @param line2 a string to be displayed on the second line of the LCD
 */
void display(String line1, String line2)
{
  // display first line
  LCD.setCursor(0, 0);
  LCD.print(line1 + BLANK_STRING);
  // display second line
  LCD.setCursor(0, 1);
  LCD.print(line2 + BLANK_STRING);
}

void setup()
{
  // Bluetooth interface
  RemoteXY_Init();

  // hardware serial
  Serial.begin(9600);

  // servo motors
  armMotor1.attach(ARM_MOTOR_1_PIN);
  armMotor2.attach(ARM_MOTOR_2_PIN);

  // LCD
  LCD.begin(16, 2);
  display("Bluetooth Mode", "");
}

void loop()
{
  RemoteXY_Handler();

  // check for mode
  if (RemoteXY.solve)
  { // autonomous mode
    Serial.write(PROCEED);

    // perform the steps to map the cube
    display("Determining Cube", "Configuration");
    byte cmd;
    do
    {
      while (!Serial.available())
      {
      }
      cmd = Serial.read();
      if (cmd == PROCEED)
      {
        const byte getCube[] = {CAP_FACE, FLIP, CAP_FACE, FLIP, CAP_FACE,
                                FLIP, CAP_FACE, REL_CW, FLIP, CAP_FACE,
                                FLIP, FLIP, CAP_FACE};
        for (const byte &step : getCube)
          perform(step);
      }
    } while (cmd != DONE);

    // receive the solution in steps
    display("Computing", "Efficient Solution");
    int size = -1;
    byte soln[100];
    do
    {
      while (!Serial.available())
      {
      }
      size++;
      soln[size] = Serial.read();
    } while (soln[size] != DONE);

    // perform the solution
    display("Solving the", "Rubik's Cube");
    unsigned long elapsedTime = millis();
    for (int i = 0; i < size; i++)
      perform(soln[i]);
    elapsedTime = millis() - elapsedTime;

    display("Rubik's Cube", "Solved in " + String(elapsedTime / 1000) + "s");
    perform(REL);
  }
  else
  { // manual mode
    if (RemoteXY.arm)
    {
      perform(HOLD);
      if (RemoteXY.CWRot)
        perform(HOLD_CW);
      else if (RemoteXY.CCWRot)
        perform(HOLD_CCW);
      else if (RemoteXY.flip)
      {
        perform(FLIP);
        perform(REL);
      }
    }
    else
    {
      perform(REL);
      if (RemoteXY.CWRot)
        perform(REL_CW);
      else if (RemoteXY.CCWRot)
        perform(REL_CCW);
      else if (RemoteXY.flip)
      {
        perform(FLIP);
        perform(REL);
      }
    }
  }
}