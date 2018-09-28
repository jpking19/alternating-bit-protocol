CS 3516 Project 2 by Jason King (jpking@wpi.edu)

In this project, I wrote the sending and receiving transport-level code for implementing a simple reliable data transfer protocol, i.e., the Alternating-Bit-Protocol. My code (seven routines for both the sending and receiving entity) is executed in a simulated hardware/software environment.

To compile, run:
make all

To start the simulation, run:
./p2

The program will then prompt you for several parameters (number of messages, packet loss probability, packet corruption probability, packet out-of-order probability, average number of time units between messages from layer 5, trace level, randomization, and bidirectional) that will specify values regarding the simulated environment. I did not implement a bidirectional option.

Suggested parameters are (1000, .5, .5, .5, 1000, 1, 1, 0) and (1000, .1, .1, .1, 1000, 1, 1, 0)
