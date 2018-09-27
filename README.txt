CS 3516 Project 2 by Jason King (jpking@wpi.edu)

In this project, I wrote the sending and receiving transport-level code for implementing a simple reliable data transfer protocol, i.e., the Alternating-Bit-Protocol. My code (seven routines for both the sending and receiving entity) is executed in a simulated hardware/software environment.

To compile, run:
make all

To start the simulation, run:
./p2

The program will then prompt you for several parameters (number of messages, packet loss probability, packet corruption probability...) that will specify values regarding the simulated environment.

Notes:
- My code is constrained by two values input by the user: number of messages to be sent, and the average time between messages from layer 5. I implemented a message queue to store messages to be sent later, when the sending entity is not waiting for an ACK from a previous sent packet. This queue is an arbitrary size of 500, and will overflow if too many messages are ready for sending. The TA's and I found this an acceptable limitation based on the simulation.
- In order to prevent hitting this limiation, I advice:
  # of messages: <500,
  avg time between messages: >10
- Running on a trace level 3 or greater will display my personal debugging prints
