#include "WPILib.h"
#include "RobotDrive.h"
#include "qcelite.h"

const int LED_CLIMBER_POSITION_ZERO = 0;
const int LED_CLIMBER_POSITION_ONE = 1;
const int LED_CLIMBER_POSITION_TWO = 2;

const int LED_GRABBER_POSITION_ZERO = 3;
const int LED_GRABBER_POSITION_ONE = 4;
const int LED_GRABBER_POSITION_TWO = 5;
const int LED_GRABBER_POSITION_THREE = 6;

const int ENCODER_DIVIDER_01 = 100;
const int ENCODER_DIVIDER_12 = 1000;
const int ENCODER_DIVIDER_23 = 1200;

CustomPanelOutput::CustomPanelOutput(){
	climberPosition = grabberPosition=0;
}

CustomPanelOutput::CustomPanelOutput(Joystick *j){
  climberPosition = grabberPosition = 0;
  launchpad = j;
}

void CustomPanelOutput::setClimberPosition(int climb){
  if(climb >= -1 && climb <= 2){
    climberPosition = climb;
  }
  updateLighting();
}

void CustomPanelOutput::setGrabberPosition(int grab){
  if(grab >= -1 && grab <= 3){
    grabberPosition = grab;
  }
  updateLighting();
}

void CustomPanelOutput::blackout(){
  launchpad->SetOutput(LED_GRABBER_POSITION_ZERO, false);
  launchpad->SetOutput(LED_GRABBER_POSITION_ONE, false);
  launchpad->SetOutput(LED_GRABBER_POSITION_TWO, false);
  launchpad->SetOutput(LED_GRABBER_POSITION_THREE, false);
  
  launchpad->SetOutput(LED_CLIMBER_POSITION_ZERO, false);
  launchpad->SetOutput(LED_CLIMBER_POSITION_ONE, false);
  launchpad->SetOutput(LED_CLIMBER_POSITION_TWO, false);
}

void CustomPanelOutput::updateLighting(){
  
  blackout();
  
  switch(grabberPosition){
    case 0: launchpad->SetOutput(LED_GRABBER_POSITION_ZERO, true); break;
    case 1: launchpad->SetOutput(LED_GRABBER_POSITION_ONE, true); break;
    case 2: launchpad->SetOutput(LED_GRABBER_POSITION_TWO, true); break;
    case 3: launchpad->SetOutput(LED_GRABBER_POSITION_THREE, true); break;
    default: 
      if(grabberPosition > 0){
        if(grabberPosition < ENCODER_DIVIDER_01)
          launchpad->SetOutput(LED_GRABBER_POSITION_ZERO, true);
        else if(grabberPosition < ENCODER_DIVIDER_12)
          launchpad->SetOutput(LED_GRABBER_POSITION_ONE, true);
        else if(grabberPosition < ENCODER_DIVIDER_23)
          launchpad->SetOutput(LED_GRABBER_POSITION_TWO, true);
        else
          launchpad->SetOutput(LED_GRABBER_POSITION_THREE, true);
      }
  }
  
  switch(climberPosition){
    case 0: launchpad->SetOutput(LED_CLIMBER_POSITION_ZERO, true); break;
    case 1: launchpad->SetOutput(LED_CLIMBER_POSITION_ONE, true); break;
    case 2: launchpad->SetOutput(LED_CLIMBER_POSITION_TWO, true); break;
    default: break;
  }
}

