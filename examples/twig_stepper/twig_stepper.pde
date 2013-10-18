/*
 * TwigMotor library
 * Author: Mario Lukas
 * Last change: 2011-09-07
 *
 * A stepper motor example, steps forward and backward
 *
 * http://www.mariolukas.de
 *
 *
 * Updated for Arduino 1.0 by Dieter Vandoren
 * Date: 2012-01-06
 *
 * http://dietervandoren.net
 *
 */

#include <Wire.h>
#include <TwigMotor.h>


// change this to the number of steps on your motor
#define STEPS 48
// change this to the shield address
#define ADDR  0x28
// create an instance of the TwigMotor class, specifying
// number of steps and I2C address, here 0x28 motor shield 
// standard address is used
// change address by calling changeAddr(new_address,save_option);
// save_options:
// - SAVEADDR   
// - NOTSAVEADDR
TwigMotor stepper(STEPS);

void setup()
{
  // set the speed of the motor to 100 RPMs
  stepper.setSpeed(ADDR, 100);
}

void loop()
{

  // move 100 steps forward 
  stepper.step(ADDR,100,FORWARD);
  delay(1000);
  // move 100 steps backward
  stepper.step(ADDR,100,BACKWARD);
  // release stepper
  stepper.release(ADDR);  
  delay(1000);
 
}
