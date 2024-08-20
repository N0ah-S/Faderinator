# Faderinator
This is a university project using the [RP2040 Launchpad](https://github.com/Terstegge/rp2040-launchpad-PCB), an RP2040-based microcontroller board developed by our professor,
and [YAHAL](https://git.fh-aachen.de/Terstegge/YAHAL), a hardware abstraction layer that works with the aforementioned launchpad. It has (currently) two modules: the microcontroller 
controlling a bunch of motor faders and sending the data to a computer, and a C# (no GUI yet) program that talks with the MCU and deligates the inputs to plugins, that are either 
directly part of this module *or with named pipes*[^1].


<p align="center">
  <img src="https://github.com/user-attachments/assets/1270a1a2-de1b-4265-8cb3-c40974285f4b"  width=50%>
</p>

## Setup
WIP

## Wiring Diagram
WIP



The old state/the state when this project was submitted can be found [here](https://github.com/N0ah-S/Faderinator/tree/submission-state)

[^1]: italic -> work (soon) in progress
