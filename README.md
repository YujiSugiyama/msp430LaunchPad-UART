# msp430LaunchPad-UART
Sample code on MSP-EXP430G2 with MSP4302553

3 type UART: Choose as you like. Plus, allowed to use 2 channel UARTs with 1)+3).(Choose only one at the sample program)
  1) Use H/W USCI: P1.1(Rx)/P1.2(Tx)
  2) Use Timer0_A: P1.1(Tx)/P1.2(Rx)
  3) Use Timer1_A: P2.3(Tx)/P2.4(Rx)
  
How to choose one at the program code.
  1) Use H/W USCI:  #define UART    1   // @general.h, 0:TimerA_UART, 1:USCI_UART
  2) Use Timer0_A:  #define UART    0   // @general.h, 0:TimerA_UART, 1:USCI_UART
                    #define TimerA  0   // @timerA.h
  3) Use Timer1_A:  #define UART    0   // @general.h, 0:TimerA_UART, 1:USCI_UART
                    #define TimerA  1   // @timerA.h

J3(J2) jumper
  1) J3: 2-4 short, 1-3 short
  2) J3: 1-2 short, 3-4 short
  3) J2.11(P2.3)-J3.1 short, J2.12(P2.4)-J3.4 short // Remove 2 jumper pins

Note
  1) MSP430 Application UART driver does not change the uart baudrate on MSP430-EXP430G2LaunchPad MSP430F16x and 9600bps is fixed.
    So baudrate must be 9600bps(See usci.h and timerA.h)
    #define BAUDRATE		    9600UL  // usci.h
    #define TA_BAUDRATE			9600UL  // timerA.h
  2) If you like to use USB-COM(USB serial converter) instead, connect appropriately. Allowed to choose the baudrate whatever you like. 
    EX. case 3) Timer1_A -> P2.3---Rx@USB-Serial, P2.4---Tx@USB-Serial (Cross)
