#include "WPILib.h"
#include "RobotDrive.h"
#include "preset.h"

const int LED_CLIMBER_POSITION_ZERO = 0;
const int LED_CLIMBER_POSITION_ONE = 1;
const int LED_CLIMBER_POSITION_TWO = 2;

const int LED_GRABBER_POSITION_ZERO = 3;
const int LED_GRABBER_POSITION_ONE = 4
const int LED_GRABBER_POSITION_TWO = 5;
const int LED_GRABBER_POSITION_THREE = 6;

const int ENCODER_DIVIDER_01 = 100;
const int ENCODER_DIVIDER_12 = 1000;
const int ENCODER_DIVIDER_23 = 1200;


class CustomPanelOutput{
  public:
    CustomPanel(Joystick *j);
    void setClimberPosition(int climberPosition);
    void setGrabberPosition(int grabberPosition);
    void updateLighting();
  protected:
    Joystick *launchpad;
    int climberPosition;
    int grabberPosition;
};

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
  launchpad->setOutput(LED_GRABBER_POSITION_ZERO, false);
  launchpad->setOutput(LED_GRABBER_POSITION_ONE, false);
  launchpad->setOutput(LED_GRABBER_POSITION_TWO, false);
  launchpad->setOutput(LED_GRABBER_POSITION_THREE, false);
  
  launchpad->setOutput(LED_CLIMBER_POSITION_ZERO, false);
  launchpad->setOutput(LED_CLIMBER_POSITION_ONE, false);
  launchpad->setOutput(LED_CLIMBER_POSITION_TWO, false);
}

void CustomPanelOutput::updateLighting(){
  
  blackout();
  
  switch(grabberPosition){
    case 0: launchpad->setOutput(LED_GRABBER_POSITION_ZERO, true); break;
    case 1: launchpad->setOutput(LED_GRABBER_POSITION_ONE, true); break;
    case 2: launchpad->setOutput(LED_GRABBER_POSITION_TWO, true); break;
    case 3: launchpad->setOutput(LED_GRABBER_POSITION_THREE, true); break;
    default: 
      if(grabberPosition > 0){
        if(grabberPosition < ENCODER_DIVIDER_01)
          launchpad->setOutput(LED_GRABBER_POSITION_ZERO, true);
        else if(grabberPosition < ENCODER_DIVIDER_12)
          launchpad->setOutput(LED_GRABBER_POSITION_ONE, true);
        else if(grabberPosition < ENCODER_DIVIDER_23)
          launchpad->setOutput(LED_GRABBER_POSITION_TWO, true);
        else
          launchpad->setOutput(LED_GRABBER_POSITION_THREE, true);
      }
  }
  
  switch(climberPosition){
    case 0: launchpad->setOutput(LED_CLIMBER_POSITION_ZERO, true); break;
    case 1: launchpad->setOutput(LED_CLIMBER_POSITION_ONE, true); break;
    case 2: launchpad->setOutput(LED_CLIMBER_POSITION_TWO, true); break;
    default: break;
  }
}
