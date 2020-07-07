#pragma once
#include "ofMain.h"

//  Kevin M. Smith - CS 134 SJSU
//

//  Base class for any object that needs a transform.
//
class TransformObject {
protected:
	TransformObject();
	ofVec3f scale;
	float	rotation;
	bool	bSelected;
public:
	ofVec3f position;
	void setPosition(const ofVec3f &);
};