#include "WPILib.h"
#include "RobotDrive.h"
#include "qcelite.h"

/*
 * FRC Code for FIRST Stronghold 2016 by QC Elite, Team 648
 */

class Robot: public SampleRobot {

	// Define Drive Motor Channels
	// These correspond to the PWM channels for the drive motor controllers
	const static int 	MOTOR_FRONT_LEFT		= 0;
	const static int 	MOTOR_REAR_LEFT			= 1;
	const static int 	MOTOR_FRONT_RIGHT		= 2;
	const static int 	MOTOR_REAR_RIGHT		= 3;

	// Other Motor Channels
	const static int 	MOTOR_ROLLER 			= 8;   // BaneBots Motor to control intake axle
	const static int 	MOTOR_GRABBER 			= 6;   // AndyMark Motor to change angle of intake arm
	const static int	MOTOR_WINCH				= 7;   // CIM Motor to pull hook
	const static int	MOTOR_HOOK_ANGLE		= 4;   // Window Motor(?) to control climb arm angle
	const static int	MOTOR_HOOK_SLIDE		= 5;   // Window Motor(?) to control climb arm telescoping action

	// Define Joystick Channels
	const static int 	JOYSTICK_DRIVER			= 0;
	const static int 	JOYSTICK_OPERATOR		= 1;
	const static int	JOYSTICK_PANEL			= 2;
	
	// IDs for grabber Position
	const static int	GRABBER_AUTO_LOWBAR		= 10;
	const static int	GRABBER_MANUAL			= -1;
	const static int 	GRABBER_FULLY_RETRACTED = 0;
	const static int	GRABBER_VERTICAL		= 1;
	const static int 	GRABBER_SHOOT_LOAD 		= 2;
	const static int 	GRABBER_GRAB 			= 3;
	const static int 	GRABBER_PORTCULLIS_HIGH = 4;
	const static int 	GRABBER_PORTCULLIS_LOW 	= 5;
	const static int 	GRABBER_CHEVAL 			= 6;

	// Constants for limits and encoders. All encoder numbers are made up. Don't trust them.
	const static int 	LIMIT_GRABBER_RETRACTED = 0;
	const static int 	LIMIT_GRABBER_EXTENDED 	= 1;   // If I remember correctly this was never actually added. Don't count on it.

	const static int	ENCODER_POS_AUTO_LOWBAR = 1050;
	
	const static int	ENCODER_POS_VERTICAL	= 1000;
	const static int 	ENCODER_POS_SHOOT 		= 1000;
	const static int 	ENCODER_POS_GRAB 		= 1950;
	const static int 	ENCODER_POS_PORTHIGH 	= 1200;
	const static int 	ENCODER_POS_PORTLOW 	= 2050;
	const static int 	ENCODER_POS_CHEVAL		= 2050;



	RobotDrive     		robotDrive;			// Robot drive system
	Joystick        	joystick_driver;		// Joystick to control steering and drive
	Joystick        	joystick_operator;		// Joystick to control arms and winches
	Joystick			joystick_panel;
	
	DigitalInput		digitalbit1;			// digital bit 1 from option switch
	DigitalInput		digitalbit2;			// digital bit 2 from option switch
	DigitalInput		digitalbit4;			// digital bit 4 from option switch
	DigitalInput		digitalbit8;			// digital bit 8 from option switch
	
	DigitalInput    	grabberInnerLimit;
	DigitalInput    	grabberOuterLimit;
	
	Victor          	rollerMotor;			// Corresponds with MOTOR_ROLLER
	Victor          	grabberMotor;			// Corresponds with MOTOR_GRABBER
	Victor			winchMotor;			// Corresponds with MOTOR_WINCH
	Victor			hookAngle;			// Corresponds with MOTOR_HOOK_ANGLE
	Victor			hookSlide;			// Corresponds with MOTOR_HOOK_SLIDE

	Encoder			armPosition;			// Encoder on grabberMotor/MOTOR_GRABBER

	int			imode_select;			// Option switch result in base 10
	int			arm_target;			// Target arm position

	preset current;
	CustomPanelOutput panel;

public:
	Robot() :
		/*
		 * Here we are initializing the input and output channels we assigned earlier
		 * They must be intialized in the same order as they are declared in the above list
		 */
		robotDrive(MOTOR_FRONT_LEFT, MOTOR_REAR_LEFT, MOTOR_FRONT_RIGHT, MOTOR_REAR_RIGHT),
		joystick_driver(JOYSTICK_DRIVER),						
		joystick_operator(JOYSTICK_OPERATOR),			
		joystick_panel(JOYSTICK_PANEL),
		digitalbit1(1),
		digitalbit2(2),
		digitalbit4(3),
		digitalbit8(4),
		grabberInnerLimit(0),
		grabberOuterLimit(8),
		rollerMotor(MOTOR_ROLLER),
		grabberMotor(MOTOR_GRABBER),
		winchMotor(MOTOR_WINCH),
		hookAngle(MOTOR_HOOK_ANGLE),
		hookSlide(MOTOR_HOOK_SLIDE),
		armPosition(5,6,false,Encoder::EncodingType::k4X),
		panel(&joystick_panel)
	
	{
		panel = CustomPanelOutput(&joystick_panel);
		panel.updateLighting();
		
		robotDrive.SetExpiration(0.1);
		robotDrive.SetInvertedMotor(RobotDrive::kFrontLeftMotor, true);	// invert the left side motors
		robotDrive.SetInvertedMotor(RobotDrive::kRearLeftMotor, true);	// you may need to change or remove this to match your robot
		robotDrive.SetInvertedMotor(RobotDrive::kFrontRightMotor, true);// invert the Right side motors
		robotDrive.SetInvertedMotor(RobotDrive::kRearRightMotor, true);	// you may need to change or remove this to match your robot

		imode_select = 0;
		arm_target = 0;
	}

	/*
	 * This function is called when our robot turns on. 
	 * Anything that only needs to run once should go in here, such as configuring the camera.
	 */
	void	RobotInit(void)	{
		
		printf("RobotInit Started\n");

		imode_select = 0;
		arm_target = 0;

		get_mode(); // Find autonomous preset

		printf("RobotInit Completed, imode_select = %4d", imode_select);
		
		{
			CameraServer::GetInstance()->SetQuality(50);
			CameraServer::GetInstance()->StartAutomaticCapture("cam0");
		}
		
		// Don't mess with these, encoder settings
		armPosition.SetMaxPeriod(.1);
		armPosition.SetMinRate(10);
		armPosition.SetDistancePerPulse(5);
		armPosition.SetSamplesToAverage(7);
		armPosition.Reset();

	}

	/**
	 * Drive Autonomous mode
	 */
	void Autonomous(void) {
		printf("Autonomous mode\n");

		robotDrive.SetSafetyEnabled(false);
		
		// Get the autonomous mode again, just to be safe. 
		get_mode();
		printf("\n switch in autonomous, imode_select = %d\n",imode_select);

		switch (imode_select) {
		case	0:
			/*
			 *  Autonomous Mode 0 is always to do nothing.
			 *  This way if we have an issue with the option switch,
			 *  the robot doesn't behave unexpectedly.
			 */
			printf("Autonomous mode 0: Do Nothing\n");
			break;
		case	1:
			printf("Autonomous mode 1: Low Bar");
			setGrabberPosition(GRABBER_AUTO_LOWBAR);
			for(int i = 0; i < 3/0.1; i++){
				setGrabberPosition(arm_target);
				Wait(0.1);
			}

			definedDrive(.5, 3);
			break;
		case	2:
			printf("Autonomous mode 2: Everything except low bar");
			setGrabberPosition(GRABBER_FULLY_RETRACTED);
			for(int i = 0; i < 3/0.1; i++){
				setGrabberPosition(arm_target);
				Wait(0.1);
			}

			definedDrive(.5, 3);
			break;
		case	3:
			printf("Autonomous mode 3: Drive at 70 for 4 seconds");
			setGrabberPosition(GRABBER_AUTO_LOWBAR);

			for(int i = 0; i < 3/0.1; i++){
				setGrabberPosition(arm_target);
				Wait(0.1);
			}

			definedDrive(.55, 3.5);
			break;
		default:
			printf("Err: Autonomous mode invalid. Doing Nothing \n");
		}

	}

	void OperatorControl() {

		robotDrive.SetSafetyEnabled(false);

		int directionMultiplier = 1;

		while (IsOperatorControl() && IsEnabled()) {
			
			// Pressing Button 8 on the Driver Joystick allows easy reverse
			if(joystick_driver.GetRawButton(8)){
				Wait(0.5);
				directionMultiplier = -directionMultiplier;
			}

			float drivex, drivey;
			if(joystick_driver.GetRawButton(11)){
				drivex = joystick_driver.GetX() * 0.65;
				drivey = joystick_driver.GetY();

				if(!joystick_driver.GetRawButton(2)){
					drivey = drivey/2;
				}
			}else{
				drivex = joystick_driver.GetX();
				drivey = joystick_driver.GetY();
			}

			robotDrive.ArcadeDrive(drivey*directionMultiplier, drivex);

			/*
			 *
			 * BEGIN OPERATOR CODE
			 *
			 */
				
				
			// If operator pulls trigger, move arm according to Y-Axis
			if(joystick_operator.GetRawButton(1) && (joystick_operator.GetY() > 0.1 || joystick_operator.GetY() < -0.1)){
				safeGrabberMotorSet(joystick_operator.GetY());
				arm_target = GRABBER_MANUAL;
			}else{
				grabberMotor.Set(0.0);
			}

			// If holding buttons 4 or 5, pull in or spit out balls through roller.
			if(joystick_operator.GetRawButton(4) || (imode_select == 9 && joystick_panel.GetRawButton(5))){
				rollerMotor.Set(1.0);
			}else if(joystick_operator.GetRawButton(5) || (imode_select == 9 && joystick_panel.GetRawButton(6))){
				rollerMotor.Set(-1.0);
			}else{
				rollerMotor.Set(0.0);
			}
			
			// If the operator pushes button 3 or 2 on their joystick, cycle to the next or previous grabber preset
			if(joystick_operator.GetRawButton(3)){
				Wait(0.5);
				nextArmPreset(true);
			}else if(joystick_operator.GetRawButton(2)){
				Wait(0.5);
				nextArmPreset(false);
			}

			// If the operator pushes button 6 or 7, raise or lower the hook angle
			if(joystick_operator.GetRawButton(6)){
				hookAngle.Set(1.0);
			}else if(joystick_operator.GetRawButton(7) || (imode_select == 9 && joystick_panel.GetRawButton(1))){
				hookAngle.Set(-1.0);
			}else{
				hookAngle.Set(0.0);
			}
			
			// If the operator pushes button 10 or 11, raise or lower the hook slide
			if(joystick_operator.GetRawButton(11) || (imode_select == 9 && joystick_panel.GetRawButton(2))){
				hookSlide.Set(1.0);
			}else if(joystick_operator.GetRawButton(12) || (imode_select == 9 && joystick_panel.GetRawButton(1))){
				hookSlide.Set(-1.0);
			}else{
				hookSlide.Set(0.0);
			}
			
			// If the operator pushes button 8 AND 9 (at the same time for safety), wind the winch
			if((joystick_operator.GetRawButton(8) && joystick_operator.GetRawButton(9)) || (imode_select == 9 && joystick_panel.GetRawButton(3))){
				winchMotor.Set(-0.5);
			}else{
				winchMotor.Set(0.0);
			}
			
			
			setGrabberPosition(arm_target);   // Every cycle, update the grabber position
			// printf("\nEncoder Values: %i", armPosition.Get());
			// printf("\nArm Target: %i", arm_target);

			Wait(0.005); // wait 5ms to avoid hogging CPU cycles
		}
	}

	// This is for the safety of the robot, since during a definedDrive() the robot will not respond to input
	// If motors were left running, they would be unstoppable and could burn out or damage something
	// Magic smoke bad. Pause good.
	void pauseAllMotors(){
		rollerMotor.Set(0.0);
		grabberMotor.Set(0.0);
	}
	
	// Move to the next arm preset, but only if it is a valid selection.
	void nextArmPreset(bool forward){
		if(forward && arm_target < GRABBER_CHEVAL){
			arm_target += 1;
		}else if(arm_target > 0){
			arm_target -= 1;
		}
	}
	
	// Drive straight at a given speed for a given length of time.
	void definedDrive(float power, float seconds){
		robotDrive.ArcadeDrive(-power, 0.0, false);
		for(int i = 0; i < seconds/0.1; i++){
			setGrabberPosition(arm_target);
			Wait(0.1);
		}
		robotDrive.ArcadeDrive(0.0, 0.0, false);
	}
	
	// Spin at a given speed for a given length of time. We think this may be contributing to brown-out issues.
	void definedRotate(float power, float seconds){
		robotDrive.ArcadeDrive(0.0, power, false);
		for(int i = 0; i < seconds/0.1; i++){
			setGrabberPosition(arm_target);
			Wait(0.1);
		}
		robotDrive.ArcadeDrive(0.0, 0.0, false);
	}

	
	// Change the current preset configuration
	void applyNewPreset(preset p){
		current = p;
	}

	// Apply the current preset
	void applyCurrentPreset(){
		setGrabberPosition(current.grabberPosition);
		setClimberPosition(current.climbingPosition);
	}
	

	void setClimberPosition(int pos){
		// This has not yet been implemented, awaiting further details about
		// Climbing system and its motor layout
	}

	// Set the grabber to run until hitting a limit switch
	void runGrabberTowardsLimit(int lim){
		switch(lim){
			case LIMIT_GRABBER_RETRACTED:
				if(!grabberInnerLimit.Get()){
					safeGrabberMotorSet(0.5);
				}else{
					armPosition.Reset();
					safeGrabberMotorSet(0.0);
				}
				break;
			case LIMIT_GRABBER_EXTENDED:
				if(grabberOuterLimit.Get()){
					safeGrabberMotorSet(-0.5);
				}else{
				  grabberMotor.Set(0.0);
				}
				break;
			default:
				grabberMotor.Set(0.0);
		}
	}
	
	// Set the grabber to run until within a reasonable margin of the target position
	void runGrabberTowardsEncoder(int target, int margin){
		if(armPosition.Get() - target > margin){
			safeGrabberMotorSet(0.5);
		}else if(armPosition.Get() - target < -margin){
			safeGrabberMotorSet(-0.5);
		}else{
			grabberMotor.Set(0.0);
		}
	}

	// Translate the arm position to an encoder or limit position
	void setGrabberPosition(int pos){
		arm_target = pos;
		switch(pos){
			case GRABBER_FULLY_RETRACTED:
				runGrabberTowardsLimit(LIMIT_GRABBER_RETRACTED);
				panel.setGrabberPosition(0);
				break;
			case GRABBER_SHOOT_LOAD:
				runGrabberTowardsEncoder(ENCODER_POS_SHOOT, 25);
				panel.setGrabberPosition(2);
				break;
			case GRABBER_VERTICAL:
				runGrabberTowardsEncoder(ENCODER_POS_VERTICAL, 25);
				panel.setGrabberPosition(1);
				break;
			case GRABBER_GRAB:
				runGrabberTowardsEncoder(ENCODER_POS_GRAB, 25);
				panel.setGrabberPosition(3);
				break;
			case GRABBER_PORTCULLIS_HIGH:
				runGrabberTowardsEncoder(ENCODER_POS_PORTHIGH, 25);
				panel.setGrabberPosition(2);
				break;
			case GRABBER_PORTCULLIS_LOW:
				runGrabberTowardsEncoder(ENCODER_POS_PORTLOW, 25);
				panel.setGrabberPosition(3);
				break;
			case GRABBER_CHEVAL:
				runGrabberTowardsEncoder(ENCODER_POS_CHEVAL, 25);
				panel.setGrabberPosition(3);
				break;
			case GRABBER_AUTO_LOWBAR:
				runGrabberTowardsEncoder(ENCODER_POS_AUTO_LOWBAR, 25);
				panel.setGrabberPosition(3);
				break;
			case GRABBER_MANUAL:
				panel.setGrabberPosition(armPosition.Get());
				break;
			default:
				runGrabberTowardsLimit(LIMIT_GRABBER_RETRACTED);
				panel.setGrabberPosition(0);
		}

		if(grabberInnerLimit.Get()){
			printf("Limit is being triggered");
		}

		if(grabberMotor.Get() > 0 && !grabberInnerLimit.Get())
			grabberMotor.Set(0.0);

	}

	bool safeGrabberMotorSet(float value){
		if(value > 0 && !grabberInnerLimit.Get()){
			grabberMotor.Set(0.0);
			return false;
		}else{
			grabberMotor.Set(value);
			return true;
		}
	}



	int subtract_fudge(int itmp1, int itmp2, int itmp3) {
		int itmp4;
		int itmp5;

		itmp4 = itmp1 - itmp2;
		itmp5 = itmp4;
		
		if ( itmp5 < 0) itmp5 = -itmp5;
		if ( itmp5 < itmp3) itmp4 = 0;

		return itmp4;

	}

	void get_mode(void) {

		imode_select = 0;
		if (!digitalbit1.Get()) printf("get_mode = bit1 set\n");
		if (!digitalbit2.Get()) printf("get_mode = bit2 set\n");
		if (!digitalbit4.Get()) printf("get_mode = bit4 set\n");
		if (!digitalbit8.Get()) printf("get_mode = bit8 set\n");

		if (!digitalbit1.Get()) imode_select = imode_select + 1;
		if (!digitalbit2.Get()) imode_select = imode_select + 2;
		if (!digitalbit4.Get()) imode_select = imode_select + 4;
		if (!digitalbit8.Get()) imode_select = imode_select + 8;

		printf("Get_mode completed, imode_select = %d \n",imode_select);
	}

	void Test() {

		printf("\n test mode entered \n");

		robotDrive.SetSafetyEnabled(false);

		get_mode();			//   get option mode

		printf(" test mode, imode_select = %d\n", imode_select);

		switch (imode_select) {
			case	0:
				break;
			case	1:

				break;
			case	2:
			break;
			case	3:
				definedDrive(1.0, 1.0);
				if(grabberInnerLimit.Get())
					printf("triggered");
				printf("Well something should have happened...");
			default:
			printf("test mode, option out of range\n");

		}

		printf("test mode completed\n");

	}

};

START_ROBOT_CLASS(Robot);
