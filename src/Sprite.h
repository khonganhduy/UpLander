#pragma once
//----------------------------------------------------------------------------------
//
// Basic Sprite Class
//
//
//  Kevin M. Smith - CS 134 SJSU

#include "ofMain.h"

// This is a base object that all drawable object inherit from
// It is possible this will be replaced by ofNode when we move to 3D
//
class BaseObject {
public:
	BaseObject();
	ofVec2f trans, scale;
	float	rot;
	bool	bSelected;
	void setPosition(ofVec3f);
};

//  General Sprite class  (similar to a Particle)
//
class Sprite : public BaseObject {
public:
	Sprite();
	void draw();
	float age();
	void setImage(ofImage);
	//float speed;    //   in pixels/sec
	ofVec3f velocity; // in pixels/sec
	ofImage image;
	float birthtime; // elapsed time in ms
	float lifespan;  //  time in ms
	//string name;
	bool haveImage;
	float width, height;
};

class AnimatableSprite : public Sprite
{
public:
	AnimatableSprite();
	~AnimatableSprite();
	void loadSpriteSheet(string);
	void update();
	void startAnim();
	void stopAnim();
	void advanceFrame();
	void resetHOff();
	void resetTrackerValues();
	void draw();
	void setAnim(float, float, int, int, float, float, int, float);
protected:
	float hoff, voff, msOfSpriteFrame;
	int ntiles_x, ntiles_y, nframes, frame, row, col, hOffsetMultiplier;
	bool bAnimRunning;
	float lastTimeRec;
};

//  Manages all Sprites in a system.  You can create multiple systems
//

class SpriteSystem {
public:
	//void add(Sprite);
	void add(AnimatableSprite);
	void remove(int);
	void update();
	void removeNear(ofVec3f point, float dist);
	void draw();
	//vector<Sprite> sprites;
	vector<AnimatableSprite> sprites;

};
