"""
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
"""

# This file contains derived code from Dexter Industries.
"""
MIT License

Copyright (c) 2017 Dexter Industries

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""


import subprocess
import serial
import kociemba  # Rubik's cube solver

PROCEED = b"\x00"
CAP_FACE = b"\x00"
REL_CW = b"\x01"
REL_CCW = b"\x02"
REL_2 = b"\x03"
HOLD_CW = b"\x04"
HOLD_CCW = b"\x05"
HOLD_2 = b"\x06"
FLIP = b"\x07"
RELEASE = b"\x08"
HOLD = b"\t"
DONE = b"\n"

try:
    ser = serial.Serial("/dev/ttyACM0", 9600)
except:
    ser = serial.Serial("/dev/ttyACM1", 9600)
DIR = "/tmp/QBer"
orient = [0, 1, 2]  # [UP, FRONT, RIGHT]


def cap_face(face, deg="0"):
    """
    Captures and stores an image of the the Rubik's cube face pointed at the camera based on the given parameters.

    :param face: a character representing the face of the Rubik's cube being captured
    :param deg: a string whole number of clockwise degrees to rotate the image
    """
    filename = DIR + "/{}-face.png".format(face)
    if deg == "90" or deg == "270":
        subprocess.run(
            [
                "raspistill",
                "-t",
                "1000",
                "-w",
                "1944",
                "-h",
                "2592",
                "-rot",
                deg,
                "-o",
                filename,
            ]
        )
    else:
        subprocess.run(
            [
                "raspistill",
                "-t",
                "1000",
                "-rot",
                deg,
                "-o",
                filename,
            ]
        )


def get_cube():
    """
    Uses the camera to capture images of the cube and returns it.

    :return: a string representing the configuration of the cube based on its colours.
    """
    while True:
        while True:
            ser.write(PROCEED)
            # up
            if ser.read() == CAP_FACE:
                cap_face("U")
                print("Captured U")
            # front
            if ser.read() == CAP_FACE:
                cap_face("F")
                print("Captured F")
            # down
            if ser.read() == CAP_FACE:
                cap_face("D")
                print("Captured D")
            # back
            if ser.read() == CAP_FACE:
                cap_face("B", "180")
                print("Captured B")
            # right
            if ser.read() == CAP_FACE:
                cap_face("R", "90")
                print("Captured R")
            # left
            if ser.read() == CAP_FACE:
                cap_face("L", "90")
                print("Captured L")

            RGBVals = subprocess.run(
                ["rubiks-cube-tracker.py", "--directory", DIR],
                capture_output=True,
                text=True,
            )

            if RGBVals.returncode == 0:  # all cube colours detected
                RGBVals = RGBVals.stdout
                break

        cube = subprocess.run(
            ["rubiks-color-resolver.py", "--rgb", RGBVals],
            capture_output=True,
            text=True,
        )

        if cube.returncode == 0:  # detected cube is solvable
            print(cube.stderr)
            cube = cube.stdout
            break

    # change cube orientation
    global orient
    orient = [5, 4, 3]
    return cube


def opp_face(face):
    """
    Returns the opposite face of a given face of a Rubik's cube.

    :param face: an integer from 0 to 5 representing the face facing the given face.
    :return: an integer from 0 to 5 representing the face facing the given face.
    """
    return (face + 3) % 6


def write(step):
    """
    Converts a standard Rubik's cube move to moves executable by the robot and writes it to the Arduino board.

    :param step: a string representing a Rubik's cube move in standard notation
    """
    # adjust orientation of cube
    if "U" in step:
        face = 0
    elif "F" in step:
        face = 1
    elif "R" in step:
        face = 2
    elif "D" in step:
        face = 3
    elif "B" in step:
        face = 4
    elif "L" in step:
        face = 5

    up, front, right = orient[0], orient[1], orient[2]

    if face == up:
        # flip twice
        ser.write(FLIP)
        ser.write(FLIP)

        orient[0] = opp_face(orient[0])
        orient[1] = opp_face(orient[1])

    elif face == front:
        # rotate twice
        ser.write(REL_2)
        orient[1] = opp_face(orient[1])
        orient[2] = opp_face(orient[2])

        # flip
        ser.write(FLIP)
        tmp = orient[0]
        orient[0] = orient[1]
        orient[1] = opp_face(tmp)

    elif face == right:
        # rotate counterclockwise
        ser.write(REL_CCW)
        tmp = orient[2]
        orient[2] = orient[1]
        orient[1] = opp_face(tmp)

        # flip
        ser.write(FLIP)
        tmp = orient[0]
        orient[0] = orient[1]
        orient[1] = opp_face(tmp)

    elif face == opp_face(up):  # target is down
        pass

    elif face == opp_face(front):  # target is back
        # flip
        ser.write(FLIP)
        tmp = orient[0]
        orient[0] = orient[1]
        orient[1] = opp_face(tmp)

    elif face == opp_face(right):  # target is left
        # rotate clockwise
        ser.write(REL_CW)
        tmp = orient[1]
        orient[1] = orient[2]
        orient[2] = opp_face(tmp)

        # flip
        ser.write(FLIP)
        tmp = orient[0]
        orient[0] = orient[1]
        orient[1] = opp_face(tmp)

    # rotate bottom face
    if "'" in step:
        ser.write(HOLD_CW)
    elif "2" in step:
        ser.write(HOLD_2)
    else:
        ser.write(HOLD_CCW)


if __name__ == "__main__":
    while True:
        if ser.read() == PROCEED:
            try:
                cube = get_cube()
                soln = kociemba.solve(cube)
                ser.write(DONE)
                print(soln)
                for step in soln.split():
                    write(step)
                ser.write(DONE)
            except Exception as e:
                print(str(e))
