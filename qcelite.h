/*
 * preset.h
 *
 *  Created on: Feb 4, 2016
 *      Author: skillina
 */

#ifndef SRC_QCELITE_H_
#define SRC_QCELITE_H_



struct preset{
	char*	name;
	int		grabberPosition;
	int 	climbingPosition;
	int		grabberManualPosition;
	bool 	roller;
	bool 	winch;
};

preset startPosition;
preset lowBar;

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

#endif /* SRC_QCELITE_H_ */
