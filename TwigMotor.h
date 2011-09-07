/*
  TwigMotor.h - - Twig I2C Motor Shield library for Arduino - Version 0.1

  Author: Mario Lukas
  Last change: 2011-09-07 
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
    
  The original Arduino stepper library was used as skeleton for this library.
  The sequence of controls signals for 4 control wires is as follows
   
  Step C0 C1 C2 C3
     1  1  0  1  0
     2  0  1  1  0
     3  0  1  0  1
     4  1  0  0  1

*/

#ifndef Stepper_h
#define Stepper_h
#define BACKWARD 0
#define FORWARD  1
#define CHANNELSET 0xaa
#define SAVEADDR 'S'
#define NOTSAVEADDR 'N'
#define CHANGEADDR 0x83
#define SETFREQ 0x84
#define SETPWM 0x82

// library interface description
class TwigMotor {
  public:
    // constructors:
    TwigMotor(int number_of_steps, unsigned char addr);

    // speed setter method:
    void setSpeed(long whatSpeed);

    // mover method:
    void step(int number_of_steps, int direction);
    
    // Change Motor I2C  Address
    void changeAddr(unsigned char new_addr, unsigned char save_or_not);	

    // Release
    void release(void);
 
    int version(void);

  private:
	
    void stepMotor(int this_step);
    void setChannel(unsigned char value); 
    void fre_pre(unsigned char pres);   
    void setPWM(unsigned char pwm);	
 
    int direction;             // Direction of rotation
    int speed;                 // Speed in RPMs
    unsigned long step_delay;  // delay between steps, in ms, based on speed
    int number_of_steps;       // total number of steps this motor can take
    int pin_count;             // whether you're driving the motor with 2 or 4 pins
    int step_number;           // which step the motor is on
        
    int address;               // Motor Address   
    long last_step_time;       // time stamp in ms of when the last step was taken
};

#endif

