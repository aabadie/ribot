### RiBot : The RIOT powered Bot

RiBot is simple DIY teleoperated robot. All design files and source code is
free:
* Source code of the different software parts (firmware, controls) are under
  the [LGPLv3](https://www.gnu.org/licenses/lgpl-3.0.en.html)
* The design files (mecanic parts and setup documentation are under Creative
  Commons [CC BY-NC 3.0](https://creativecommons.org/licenses/by-nc/3.0/)

This project aims at showing the possibility provided by the RT OS
[RIOT](http://www.riot-os.org) for simple robots.


![Image of RiBot](https://github.com/aabadie/ribot/blob/master/docs/images/ribot.png)


### Provided source codes

1. Firmware with Bluetooth communication

The source code of this firmware is located in the [bt-fw](bt-fw) directory.

Note that the firmware is designed to work on a
[Nucleo-f446](http://www.st.com/en/evaluation-tools/nucleo-f446re.html) board
with [RIOT](http://github.com/RIOT-OS/RIOT.git).
It can be easily adapted to any board supported by RIOT with support for GPIO (2
minimum), PWM (2 minimum) and UART (2 minimum).

To build and flash it, you need:
* To clone RIOT source code:
```
$ mkdir ~/ribot-project && cd ~/ribot-project
$ git clone https://github.com/RIOT-OS/RIOT.git
```
* To clone this repository:
```
$ git clone https://github.com/aabadie/ribot.git
```
* To plug the board on your computer (only Linux and OSX are supported)
* To issue the following commands:
```
$ cd bt-fw
$ make RIOTBASE=~/ribot-projectRIOT BOARD=nucleo-f446 flash
```
Your board is ready!

2. Control application (Qt)

The source code of this applicatin is located in the `bt-qt-control` directory.
You need Qt >= 5.7 to build it.

* Configure a Qt development environment. [Qt website](http://www.qt.io) is a good start.
  You have to use QtCreator for building this application.
* Load the `RiBot.pro` file in QtCreator.
* Build and Run using the Desktop Kit


