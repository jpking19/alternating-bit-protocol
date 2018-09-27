#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project2.h"

/* ***************************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for unidirectional or bidirectional
   data transfer protocols from A to B and B to A.
   Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets may be delivered out of order.

   Compile as gcc -g project2.c student2.c -o p2
**********************************************************************/

// Helper routine prototypes
int flip (int num);
int generate_checksum(struct pkt packet);

// Global vairables shared between A and B
#define TIMEOUT 50 // How long to wait for an ACK

// Global variables for A subroutines
int IS_WAITING = 0; // 1 if A is waiting for an ACK
int A_seq_expect = 0; // SEQ A is expecting
struct pkt A_prev_packet; // The last packet that A has sent to layer 3
struct msg A_prev_message; // The last message (use for retransmission)
struct msg msgQueue[500]; // Stores the messages that will be sent by A_output()
int queueIndex; // Index of the next packet to be sent out
int queueMax; // Upper bound of the queue (used to check if all queued packets sent)
int checkCalc[20]; // Numbers used to calculate checksum

// Global variables for B subroutines
int B_seq_expect = 0; // 0 if we are good to go, 1 if we are waiting for an ACK
struct pkt B_prev_packet; // The last packet that B has sent to layer 3
struct msg B_prev_message; // The last message sent to layer 5 from B

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
/*
 * The routines you will write are detailed below. As noted above,
 * such procedures in real-life would be part of the operating system,
 * and would be called by other procedures in the operating system.
 * All these routines are in layer 4.
 */

/*
 * A_output(message), where message is a structure of type msg, containing
 * data to be sent to the B-side. This routine will be called whenever the
 * upper layer at the sending side (A) has a message to send. It is the job
 * of your protocol to insure that the data in such a message is delivered
 * in-order, and correctly, to the receiving side upper layer.
 */
void A_output(struct msg message) {
  // Check to see if A is waiting for an ACK
  if (IS_WAITING) {
    printf("\nA_output: Waiting for ACK. Add message to queue.\n");
    msgQueue[queueMax] = message; // Store message in the queue
    queueMax = (queueMax + 1) % 500; // Increase queue size (wraps around with overflow)
    return;
  }

  printf("\nA_output: Sending next message in queue.\n");

  // Build the packet from the given message
  A_prev_packet.seqnum = A_seq_expect;
  A_prev_packet.acknum = 0; // Not used

  // Deal with corruption
  int i;
  for (i = 0; i < MESSAGE_LENGTH; i++) {
      A_prev_packet.payload[i] = message.data[i];
      A_prev_message.data[i] = message.data[i];
  }

  A_prev_packet.checksum = generate_checksum(A_prev_packet);

  // Send the packet to layer 3 and start the timer
  tolayer3(AEntity, A_prev_packet);
  startTimer(AEntity, TIMEOUT);
  IS_WAITING = 1; // A is now waiting for an ACK
}

/*
 * Just like A_output, but residing on the B side.  USED only when the
 * implementation is bi-directional.
 */
void B_output(struct msg message)  {
  // Do not need to implement this routine
}

/*
 * A_input(packet), where packet is a structure of type pkt. This routine
 * will be called whenever a packet sent from the B-side (i.e., as a result
 * of a tolayer3() being done by a B-side procedure) arrives at the A-side.
 * packet is the (possibly corrupted) packet sent from the B-side.
 */
void A_input(struct pkt packet) {
  // Check for corruption
  if (packet.checksum != generate_checksum(packet)) {
    printf("\nA_input: Packet corrupted. Drop Packet.\n");
    return;
  }
  // Check to ensure the packet is the next one in the sequence
  if (packet.acknum != A_seq_expect) {
    printf("\nB_input: Not the expected sequence num. Send NAK.\n");
    return;
  }
  // Checks if ACK is correct
  if (packet.acknum == A_seq_expect) {
    printf("\nA_input: Expected ACK.\n");
    // Nothing in the queue
    if (queueIndex == queueMax) {
      stopTimer(AEntity);
      IS_WAITING = 0; // Get ready for next message from layer 5
      A_seq_expect = flip(A_seq_expect);
      return;
    }
    // Something in the queue or overflow of queue
    else if (queueMax > 0 || (queueMax == 0 && queueIndex != 0)) {
      stopTimer(AEntity);
      IS_WAITING = 0;
      A_output(msgQueue[queueIndex]); // Run output on next message in queue
      queueIndex = (queueIndex + 1) % 500;
      A_seq_expect = flip(A_seq_expect);
    }
  }
}

/*
 * A_timerinterrupt()  This routine will be called when A's timer expires
 * (thus generating a timer interrupt). You'll probably want to use this
 * routine to control the retransmission of packets. See startTimer()
 * and stopTimer() in the writeup for how the timer is started and stopped.
 */
void A_timerinterrupt() {
  printf("\nA timer has been interrupted, A will now resend the packet. \n");
  IS_WAITING = 0; // Allow A to send message
  A_output(A_prev_message); // Resend previous message
}

/* The following routine will be called once (only) before any other    */
/* entity A routines are called. You can use it to do any initialization */
void A_init() {
  IS_WAITING = 0;
  A_seq_expect = 0;
  queueIndex = 0;
  queueMax = 0;

  // Fill checksum array with numbers
  int i;
  for (i = 0; i < MESSAGE_LENGTH; i++) {
    checkCalc[i] = i + 2;
  }
}


/*
 * Note that with simplex transfer from A-to-B, there is no routine  B_output()
 */

/*
 * B_input(packet),where packet is a structure of type pkt. This routine
 * will be called whenever a packet sent from the A-side (i.e., as a result
 * of a tolayer3() being done by a A-side procedure) arrives at the B-side.
 * packet is the (possibly corrupted) packet sent from the A-side.
 */
void B_input(struct pkt packet) {
  // Check for corruption
  if (packet.checksum != generate_checksum(packet)) {
    // Send NAK (ACK with opposite ack number) back to A
    printf("\nB_input: Packet corrupted. Send NAK.\n");
    B_prev_packet.seqnum = 0;
    B_prev_packet.acknum = flip(B_seq_expect);
    B_prev_packet.checksum = generate_checksum(B_prev_packet);
    tolayer3(BEntity, B_prev_packet);
    return;
  }

  // Check to ensure the packet is the next one in the sequence
  if (packet.seqnum != B_seq_expect) {
    // Send NAK (ACK with opposite ack number) back to A
    printf("\nB_input: Not the expected sequence num. Send NAK.\n");
    B_prev_packet.seqnum = 0;
    B_prev_packet.acknum = flip(B_seq_expect);
    B_prev_packet.checksum = generate_checksum(B_prev_packet);
    tolayer3(BEntity, B_prev_packet);
    return;
  }

  // Build ACK packet from given packet
  printf("\nB_input: Sending ACK.\n");
  B_prev_packet.acknum = B_seq_expect;
  B_prev_packet.checksum = generate_checksum(B_prev_packet);
  tolayer3(BEntity, B_prev_packet);

  // Build message from the given payload
  printf("\nB_input: Received message: %s\n", packet.payload);
  memmove(B_prev_message.data, packet.payload, 20);
  tolayer5(BEntity, B_prev_message);

  B_seq_expect = flip(B_seq_expect);
}

/*
 * B_timerinterrupt()  This routine will be called when B's timer expires
 * (thus generating a timer interrupt). You'll probably want to use this
 * routine to control the retransmission of packets. See startTimer()
 * and stopTimer() in the writeup for how the timer is started and stopped.
 */
void B_timerinterrupt() {
  // Do not need to implement this routine
}

/*
 * The following routine will be called once (only) before any other
 * entity B routines are called. You can use it to do any initialization
 */
void B_init() {
  B_seq_expect = 0;
}

// --- Jason's helper routines ---

// Returns 1 if given 0, and vice versa
int flip (int num) {
  return num ? 0 : 1;
}

/*
 * Trivial method for generating a checksum by multiplying everything
 * by the numbers in the checksum array
 */
int generate_checksum(struct pkt packet) {
  int num = 0;
  if (packet.payload[0] != NULL) {
    int i;
    for (i = 0; i < MESSAGE_LENGTH; i++) {
      num += packet.payload[i] * checkCalc[i];
    }
  }
  num += ((packet.seqnum + 2) * checkCalc[0]);
  num += ((packet.acknum + 2) * checkCalc[19]);
  return num;
}
