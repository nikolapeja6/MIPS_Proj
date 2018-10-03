# MIPS Proj

This project is a simple home alarm system equipped with a motion detector and RFID authentication and was done for the MIPS course at the [School of Electrical Engineering][school] of the [University of Belgrade][university].

When the system is armed, in case it detects motion in the vicinity of the device a countdown is started signaled by periodic beeps. During this time the user is expected to authenticate himself by placing his RFID tag on the reader, thus disarming the system. If the user does not do this, an alarm consisting of two interchanging tones is sounded.

The core of the Home Alarm system is the STM32 microcontroller on the [MikroElektronica][mikroe]'s [Clicker 2 board][clicker2]. The system also consists of the RFID-RC522 module, a motion detector taken from a Paradox Motion Detector and a Piezo Buzzer.

During development, code for interacting with the RFID-RC522 module from an Arduino was used, with minor ajdustments made in order to translate it for the Clicker 2 board. The original code for Arduino can be found [here][original_rfid_code].

A demo of the system can be found below.

[![Demo video](https://i.ytimg.com/vi/hSoqx0P2P3c/1.jpg?time=1538526714243)](https://youtu.be/hSoqx0P2P3c)

[school]: https://www.etf.bg.ac.rs/en
[university]: http://www.bg.ac.rs/en/
[mikroe]: https://www.mikroe.com/
[clicker2]: https://www.mikroe.com/clicker-2-stm32f4
[original_rfid_code]: http://wiki.keyestudio.com/index.php/Ks0067_keyestudio_RC522_RFID_Module_for_Arduino
