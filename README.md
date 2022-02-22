# msp430LaunchPad-UART
Sample code on MSP-EXP430G2 with MSP4302553

<pre>
3 type UART: Choose as you like. Plus, allowed to use 2 channel UARTs with 1)+3).(Choose only one at the sample program)<br>
  Type1) Use H/W USCI: P1.1(Rx)/P1.2(Tx)<br>
  Type2) Use Timer0_A: P1.1(Tx)/P1.2(Rx)<br>
  Type3) Use Timer1_A: P2.3(Tx)/P2.4(Rx)<br>
</pre>

<pre>
How to choose one at the program code.<br>
  Type1) Use H/W USCI:  #define UART    1 &#009; // @general.h, 0:TimerA_UART, 1:USCI_UART<br>
  Type2) Use Timer0_A:  #define UART    0 &#009; // @general.h, 0:TimerA_UART, 1:USCI_UART<br>
                        #define TimerA  0 &#009; // @timerA.h<br>
  Type3) Use Timer1_A:  #define UART    0 &#009; // @general.h, 0:TimerA_UART, 1:USCI_UART<br>
                        #define TimerA  1 &#009; // @timerA.h<br>
</pre>

<pre>
J3(J2) jumper<br>
  Type1) J3: 2-4 short, 1-3 short<br>
  Type2) J3: 1-2 short, 3-4 short<br>
  Type3) J2.11(P2.3)-J3.1 short, J2.12(P2.4)-J3.4 short  &#009; // Remove 2 jumper pins<br>
</pre>

<pre>
Note<br>
  A) MSP430 Application UART driver does not change the uart baudrate on MSP430-EXP430G2LaunchPad MSP430F16x and 9600bps is fixed.<br>
    So baudrate must be 9600bps(See usci.h and timerA.h)<br>
    #define BAUDRATE &#009; 9600UL &#009; // usci.h<br>
    #define TA_BAUDRATE &#009; 9600UL &#009; // timerA.h<br>   
  B) If you like to use USB-COM(USB serial converter) instead, connect appropriately. Allowed to choose the baudrate whatever you like. <br>
    EX. Type3) Timer1_A -> P2.3---Rx@USB-Serial, P2.4---Tx@USB-Serial (Cross)<br>
</pre>
    
