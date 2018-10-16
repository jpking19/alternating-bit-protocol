# Transport Layer Alternating-Bit-Protocol

Jason King

Implementation of the sending and receiving transport-level code for implementing a simple reliable data transfer protocol, i.e., the Alternating-Bit-Protocol. My code (seven routines for both the sending and receiving entity) is executed in a simulated hardware/software environment. The environment was provided by the Computer Networks professor as a starting point for this project.

## Run the Simulation

First, "make all"

Then, "./p2"

The program will then prompt you for several parameters (number of messages, packet loss probability, packet corruption probability, packet out-of-order probability, average number of time units between messages from layer 5, trace level, randomization, and bidirectional) that will specify values in the simulated environment. I did not implement a bidirectional option.

Suggested parameters are (1000, .5, .5, .5, 1000, 1, 1, 0) and (1000, .1, .1, .1, 1000, 1, 1, 0).
