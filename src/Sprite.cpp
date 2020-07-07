#include "Sprite.h"
//----------------------------------------------------------------------------------
//
// Sprite example source file provided by Kevin Smith
//
//
//  Kevin M. Smith - CS 134 SJSU
//--------------------------------------------------------------
BaseObject::BaseObject() {
	trans = ofVec3f(0,0,0);
	scale = ofVec3f(1, 1, 1);
	rot = 0;
}

void BaseObject::setPosition(ofVec3f pos) {
	trans = pos;
}

//
// Basic Sprite Object
//
Sprite::Sprite() {
	lifespan = -1;      // lifespan of -1 => immortal 
	birthtime = 0;
	bSelected = false;
	haveImage = false;
	//name = "UnamedSprite";
	width = 20;
	height = 20;
}

// Return a sprite's age in milliseconds
//
float Sprite::age() {
	return (ofGetElapsedTimeMillis() - birthtime);
}

//  Set an image for the sprite. If you don't set one, a rectangle
//  gets drawn.
//
void Sprite::setImage(ofImage img) {
	image = img;
	haveImage = true;
	width = image.getWidth();
	height = image.getHeight();
}


//  Render the sprite
//
void Sprite::draw() {

	ofSetColor(255, 255, 255, 255);

	// draw image centered and add in translation amount
	//
	if (haveImage) {
		image.draw(-width / 2.0 + trans.x, -height / 2.0 + trans.y);

	}
	else {
		// in case no image is supplied, draw something.
		// 
		ofNoFill();
		ofSetColor(255, 0, 0);
		ofDrawRectangle(-width / 2.0 + trans.x, -height / 2.0 + trans.y, width, height);
		ofSetColor(255, 255, 255, 255);
	}
}

AnimatableSprite::AnimatableSprite() :Sprite()
{
	bAnimRunning = false;
}


AnimatableSprite::~AnimatableSprite()
{
}

void AnimatableSprite::loadSpriteSheet(string filePath) {
	if (!image.load(filePath)) {
		cout << "Can't load image file: " << filePath << endl;
		ofExit();
	}
}

void AnimatableSprite::startAnim() {
	frame = 0;
	bAnimRunning = true;
	lastTimeRec = ofGetSystemTimeMillis();
}

void AnimatableSprite::stopAnim() {
	bAnimRunning = false;
}

void AnimatableSprite::update() {
	if (!bAnimRunning) return;
	float curTime = ofGetSystemTimeMillis();
	if ((curTime - lastTimeRec) > msOfSpriteFrame) {
		advanceFrame();
		lastTimeRec = curTime;
	}
}


void AnimatableSprite::advanceFrame() {
	if (frame == (nframes - 1)) {
		resetTrackerValues();// cycle back to first frame
	}
	else {
		frame++;
		if (col == ntiles_x - 1) {
			col = 0;
			resetHOff();
		}
		else {
			col++;
			hOffsetMultiplier++;
		}
		//	row = frame / ntiles_x;
	}
}

void AnimatableSprite::resetHOff() {
	hOffsetMultiplier = 0;
}

void AnimatableSprite::resetTrackerValues() {
	resetHOff();
	col = 0;
	row = 0;
	frame = 0;
}

void AnimatableSprite::draw() {
	image.drawSubsection(trans.x - (width / 2), trans.y - (height / 2), width, height, col*width + hoff * hOffsetMultiplier, row*height + voff);
}

void AnimatableSprite::setAnim(float aWidth, float aHeight, int ntilesx, int ntilesy, float v_off, float h_off, int n_frames, float ms_OfSpriteFrame) {
	width = aWidth;
	height = aHeight;
	ntiles_x = ntilesx;
	ntiles_y = ntilesy;
	voff = v_off;
	hoff = h_off;
	nframes = n_frames;
	msOfSpriteFrame = ms_OfSpriteFrame;
	resetTrackerValues();
}

//  Add a Sprite to the Sprite System
//
/*
void SpriteSystem::add(Sprite s) {
	sprites.push_back(s);
}*/

void SpriteSystem::add(AnimatableSprite s) {
	sprites.push_back(s);
}

// Remove a sprite from the sprite system. Note that this function is not currently
// used. The typical case is that sprites automatically get removed when the reach
// their lifespan.
//
void SpriteSystem::remove(int i) {
	sprites.erase(sprites.begin() + i);
}

// remove all sprites within a given dist of point
//
void SpriteSystem::removeNear(ofVec3f point, float dist) {
	//vector<Sprite>::iterator s = sprites.begin();
	//vector<Sprite>::iterator tmp;
	vector<AnimatableSprite>::iterator s = sprites.begin();
	vector<AnimatableSprite>::iterator tmp;

	while (s != sprites.end()) {
		ofVec3f v = s->trans - point;
		if (v.length() < dist) {
			tmp = sprites.erase(s);
			s = tmp;
		}
		else s++;
	}
}


//  Update the SpriteSystem by checking which sprites have exceeded their
//  lifespan (and deleting).  Also the sprite is moved to it's next
//  location based on velocity and direction.
//
void SpriteSystem::update() {

	if (sprites.size() == 0) return;
	//vector<Sprite>::iterator s = sprites.begin();
	//vector<Sprite>::iterator tmp;
	vector<AnimatableSprite>::iterator s = sprites.begin();
	vector<AnimatableSprite>::iterator tmp;

	// check which sprites have exceed their lifespan and delete
	// from list.  When deleting multiple objects from a vector while
	// traversing at the same time, use an iterator.
	//
	while (s != sprites.end()) {
		if (s->lifespan != -1 && s->age() > s->lifespan) {
			tmp = sprites.erase(s);
			s = tmp;
		}
		else s++;
	}

	//  Move sprite
	//
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].trans += sprites[i].velocity / ofGetFrameRate();
	}
}

//  Render all the sprites
//
void SpriteSystem::draw() {
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].draw();
	}
}
