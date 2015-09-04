/******************************************************************************/
/* send_receive.c   Dr. Juan Gonzalez Gomez. July 2010                        */
/*----------------------------------------------------------------------------*/
/* Example of Serial communications in Linux in C++                           */
/* Sending and receiving data strings                                         */
/*----------------------------------------------------------------------------*/
/* GPL LICENSE                                                                */
/*----------------------------------------------------------------------------*/
/* This example sends a ASCII string to the serial port. It waits for the     */
/* same string to be echoed by another device (For example a microcontroller  */
/* running an echo-firmware or a wire joining the PC tx and rx pins           */
/* The received string is print on the screen. If no data is received         */
/* during the TIMEOUT time, a timeout message is printed                      */
/*                                                                            */
/* The serial port speed is configured to 9600 baud                           */
/*----------------------------------------------------------------------------*/
/* Example of use:                                                            */
/*                                                                            */
/*   ./send_receive /dev/ttyUSB0                                              */
/*                                                                            */
/*  The serial device name should be passed as a parameter                    */
/*  When executing this example, if the echoed data is received the           */
/*  output will be the following:                                             */
/*                                                                            */
/*    String sent------> ASCII Command test                                   */
/*    String received--> ASCII Command test (18 bytes)                        */
/*                                                                            */
/*  If no data is received, the output will be:                               */
/*    String sent------> ASCII Command test                                   */
/*    String received--> Timeout!                                             */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*  In linux, the serial devices names are:                                   */
/*                                                                            */
/*    /dev/ttyS0  --> First native serial port                                */
/*    /dev/ttyS1  --> Second native serial port                               */
/*    ...                                                                     */
/*    /dev/ttyUSB0  --> First USB-RS232 converter                             */
/*    /dev/ttyUSB1  --> Second USB-RS232 converter                            */
/*    ...                                                                     */
/******************************************************************************/

#include <iostream>
#include <SerialPort.h>

using namespace std;

//-- ASCII string to send through the serial port
const string CMD="ASCII Command test";

int main(int argc, char **argv)
{

  //-- Check if the serial device name is given
  if (argc<2) {
    cerr << "No serial device name is given" << endl;
    return 1;
  }

  cout << "Serial port: " << argv[1] << endl;
  cout << "Opening..." << endl;

  //-- Open the serial port
  //-- Serial port configuration: 9600 baud, 8N1
  SerialPort serial_port(argv[1]);
  try {
    serial_port.Open(SerialPort::BAUD_9600,
                     SerialPort::CHAR_SIZE_8,
                     SerialPort::PARITY_NONE,
                     SerialPort::STOP_BITS_1,
                     SerialPort::FLOW_CONTROL_NONE);
  }
  catch (SerialPort::OpenFailed E) {
    cerr << "Error opening the serial port" << endl;
    return 1;
  }

  //-- Send the command to the serial port
  cout << "String sent------> " << CMD << endl;
  serial_port.Write(CMD);

  cout << "String received: ";
    
  //-- Wait for the received data
  SerialPort::DataBuffer buffer;

  try {
    serial_port.Read(buffer,CMD.size(),500);
  }
  catch (SerialPort::ReadTimeout E) {
    cout << "TIMEOUT!";
    return 1;
  }

  //-- Show the received data
  for(int ii=0; ii < buffer.size(); ii++) {
    cout << buffer[ii];
  }
  cout << endl;

  serial_port.Close();
  return 0;
}
