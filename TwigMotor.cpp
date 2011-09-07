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


 */
#include "WProgram.h"
#include "TwigMotor.h"
#include "../Wire/Wire.h"


/*
 *   constructor for four-pin version
 *   Sets which wires should control the motor.
 */

TwigMotor::TwigMotor(int number_of_steps, unsigned char addr)
{
  this->step_number = 0;                       // which step the motor is on
  this->speed = 0;                             // the motor speed, in revolutions per minute
  this->direction = 0;                         // motor direction
  this->last_step_time = 0;                    // time stamp in ms of the last step taken
  this->number_of_steps = number_of_steps;     // total number of steps for this motor
  this->address = addr;  
  
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
  Wire.send(CHANNELSET);
  Wire.send(value);
  Wire.send(0);
  Wire.endTransmission();
}

/* 
   changes I2C shield address
*/
void TwigMotor::changeAddr(unsigned char new_addr,unsigned char save_or_not){
  Wire.beginTransmission(this->address);
  Wire.send(CHANGEADDR);
  Wire.send(new_addr);
  Wire.send(save_or_not);
  Wire.endTransmission();
  this->address = new_addr;
  delayMicroseconds(100);
}

void TwigMotor::fre_pre(unsigned char pres){
  Wire.beginTransmission(this->address); 
  Wire.send(SETFREQ);
  Wire.send(pres);
  Wire.send(0);
  Wire.endTransmission();
}

/* 
  sets motor pwm
*/
void TwigMotor::setPWM(unsigned char pwm_value){
  Wire.beginTransmission(this->address); 
  Wire.send(SETPWM);
  Wire.send(pwm_value);
  Wire.send(pwm_value);
  Wire.endTransmission();
}	

/*
  Sets the speed in revs per minute

*/
void TwigMotor::setSpeed(long whatSpeed)
{
  this->step_delay = 60L * 1000L / this->number_of_steps / whatSpeed;

}

/*
  Release Motor
*/ 

void TwigMotor::release(void){
     setPWM(0);
     setChannel(0b00000000);
}

/*
  Moves the motor steps_to_move steps.  If the number is negative, 
   the motor moves in the reverse direction.
 */
void TwigMotor::step(int steps_to_move, int direction)
{ 
  
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
  
}

/*
 * Moves the motor forward or backwards.
 */
void TwigMotor::stepMotor(int thisStep)
{
   switch (thisStep) 
   {
      case 0:    // 1010
	setChannel(0b00001010);		
     break;
      case 1:    // 0110
	setChannel(0b00000110);
     break;
      case 2:    //0101
	setChannel(0b00000101);
     break;
      case 3:    //1001
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
