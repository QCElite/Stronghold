/*
 * preset.h
 *
 *  Created on: Feb 4, 2016
 *      Author: skillina
 */

#ifndef SRC_PRESET_H_
#define SRC_PRESET_H_



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


#endif /* SRC_PRESET_H_ */
