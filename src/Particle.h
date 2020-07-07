#pragma once

#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "vector3.h"
#include "box.h"
#include "ray.h"

class ParticleForceField;

class Particle {
public:
	Particle();

	ofVec3f position;
	ofVec3f velocity;
	ofVec3f acceleration;
	ofVec3f forces;
	ofVec3f offset;
	float	damping;
	float   mass;
	float   lifespan;
	float   radius;
	float   birthtime;
	void    integrate();
	void    draw();
	float   age();        // sec
	ofColor color;

};

class ModelParticle : public Particle {
public:
	ModelParticle();
	void update();
	void draw();
	void setModel(string);
	void drawBox(const Box &);
	void drawRay(const Ray &);
	Box meshBounds(const ofMesh &);
	ofxAssimpModelLoader model;
	ofMesh mesh;
	Box boundingBox;
	Ray rayYMinus;
	Ray rayZMinus;
	Ray rayZPlus;
	Ray rayXMinus;
	Ray rayXPlus;
	ofVec3f intersectionYMinus;
	ofVec3f intersectionXMinus;
	ofVec3f intersectionXPlus;
	ofVec3f intersectionZMinus;
	ofVec3f intersectionZPlus;

};


