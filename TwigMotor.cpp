/*
  TwigMotor.cpp - - Twig I2C Motor Shield  library for Arduino - Version 0.1
 
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
  
  The original Arduino Stepper library was used as Skeleton.
  Drives a unipolar or bipolar stepper motor using  4 wires

  The sequence of control signals for 4 control wires is as follows:

  Step C0 C1 C2 C3
     1  1  0  1  0
     2  0  1  1  0
     3  0  1  0  1
     4  1  0  0  1
  
  *
  * Updated for Arduino 1.0 by Dieter Vandoren
  * Date: 2012-01-06
  *
  * hpp://dietervandoren.net
  *
 
 */
#include "Arduino.h"
#include "TwigMotor.h"
#include "Wire.h"


/*
 *   constructor for four-pin version
 *   Sets which wires should control the motor.
 */

TwigMotor::TwigMotor(int number_of_steps, unsigned char addr)
{
  this->step_number = 0;                       // which step the motor is on
  this->speed = 100;                             // the motor speed, in revolutions per minute
  this->direction = 0;                         // motor direction
  this->last_step_time = 0;                    // time stamp in ms of the last step taken
  this->number_of_steps = number_of_steps;     // total number of steps for this motor
  this->address = addr;  
  
  // pin_count is used by the stepMotor() method:  
  this->pin_count = 4; 
//  Wire.begin();	                               // init I2C communication
//  delayMicroseconds(10); 

}

TwigMotor::TwigMotor(int number_of_steps)
{
  this->step_number = 0;                       // which step the motor is on
  this->speed = 100;                             // the motor speed, in revolutions per minute
  this->direction = 0;                         // motor direction
  this->last_step_time = 0;                    // time stamp in ms of the last step taken
  this->number_of_steps = number_of_steps;     // total number of steps for this motor
  this->address = 0x28;  
  
  // pin_count is used by the stepMotor() method:  
  this->pin_count = 4; 
  Wire.begin();	                               // init I2C communication
  delayMicroseconds(10); 

}

/*
  sets channel, set IN1 - IN4  of H - bridge
*/
void TwigMotor::setChannel(unsigned char value){
  Wire.beginTransmission(this->address);
  Wire.write(CHANNELSET);
  Wire.write(value);
  Wire.write(0);
  Wire.endTransmission();
  delayMicroseconds(100);
}

/* 
   changes I2C shield address
*/
void TwigMotor::changeAddr(unsigned char motor_addr, unsigned char new_addr,unsigned char save_or_not){
  this->address = motor_addr;
  Wire.beginTransmission(this->address);
  Wire.write(CHANGEADDR);
  Wire.write(new_addr);
  Wire.write(save_or_not);
  Wire.endTransmission();
  this->address = new_addr;
  delayMicroseconds(100);
}

void TwigMotor::speedAB(unsigned char spda , unsigned char spdb){
  Wire.beginTransmission(this->address); // transmit to device MOTORSHIELDaddr
  Wire.write(SETPWM);        //set pwm header 
  Wire.write(spda);              // send pwma 
  Wire.write(spdb);              // send pwmb    
  Wire.endTransmission();    // stop transmitting
}
 

void TwigMotor::fre_pre(unsigned char pres){
  Wire.beginTransmission(this->address); 
  Wire.write(SETFREQ);
  Wire.write(pres);
  Wire.write(0);
  Wire.endTransmission();
}

void TwigMotor::motorAndspd( unsigned char motor_s,unsigned char Mstatus, unsigned char spd)
{
  Wire.beginTransmission(this->address); // transmit to device MOTORSHIELDaddr
  Wire.write(motor_s);        // motor select information
  Wire.write(Mstatus);        // motor satus information
  Wire.write(spd);            //  motor speed information
  Wire.endTransmission();    // 
}


/* 
  sets motor pwm
*/
void TwigMotor::setPWM(unsigned char pwm_value){
  Wire.beginTransmission(this->address); 
  Wire.write(SETPWM);
  Wire.write(pwm_value);
  Wire.write(pwm_value);
  Wire.endTransmission();
}	

/*
  Sets the speed in revs per minute

*/
void TwigMotor::setSpeed(unsigned char motor_addr,long whatSpeed)
{
  this->address = motor_addr;
  this->step_delay = 60L * 1000L / this->number_of_steps / whatSpeed;
  this->speed = whatSpeed;
}

/*
  Release Motor
*/ 

void TwigMotor::release(unsigned char motor_addr){
  	
        this->address = motor_addr;
	setPWM(0);	
	setChannel(0b00000000);
}

/*
  Moves the motor steps_to_move steps.  If the number is negative, 
   the motor moves in the reverse direction.
 */
void TwigMotor::step(unsigned char motor_addr, int steps_to_move, int direction)
{ 
  // switch to actual address ... 
  this->address = motor_addr;
  this->direction = direction;
  setPWM(255);	  
 	
  int steps_left = abs(steps_to_move);  // how many steps to take
     
  // decrement the number of steps, moving one step each time:
  while(steps_left > 0) {
  // move only if the appropriate delay has passed:
  if (millis() - this->last_step_time >= this->step_delay) {
      // get the timeStamp of when you stepped:
      this->last_step_time = millis();
      // increment or decrement the step number,
      // depending on direction:
      if (this->direction == 1) {
        this->step_number++;
        if (this->step_number == this->number_of_steps) {
          this->step_number = 0;
        }
      } 
      else { 
        if (this->step_number == 0) {
          this->step_number =  this->number_of_steps;
        }
        this->step_number--;
      }
      // decrement the steps left:
      steps_left--;
      // step the motor to step number 0, 1, 2, or 3:
      stepMotor(this->step_number % 4);
    }
  }
  release(this->address);
  
}

/*
 * Moves the motor forward or backwards.
 */
void TwigMotor::stepMotor(int thisStep)
{
   switch (thisStep) 
   {
      case 0:    // 1010
//	motorAndspd(0xa5,0b10,this->speed);
//	motorAndspd(0xa1,0b10,this->speed);
 	setChannel(0b00001010);		
     break;
      case 1:    // 0110
//	motorAndspd(0xa5,0b01,this->speed);
//	motorAndspd(0xa1,0b10,this->speed);
  	
 	setChannel(0b00000110);
     break;
      case 2:    //0101
//	motorAndspd(0xa5,0b01,this->speed);
// 	motorAndspd(0xa1,0b01,this->speed);
  	
	setChannel(0b00000101);
     break;
      case 3:    //1001
//	motorAndspd(0xa5,0b10,this->speed);
//	motorAndspd(0xa1,0b01,this->speed);
  	
	setChannel(0b00001001);
     break;
    } 
}

/*
  version() returns the version of the library:
*/
int TwigMotor::version(void)
{
  return 1;
}
