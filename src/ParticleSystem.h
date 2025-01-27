#pragma once
//  Kevin M. Smith - CS 134 SJSU

#include "ofMain.h"
#include "Particle.h"


//  Pure Virtual Function Class - must be subclassed to create new forces.
//
class ParticleForce {
protected:
public:
	bool applyOnce = false;
	bool applied = false;
	virtual void updateForce(Particle *) = 0;
};

class ParticleSystem {
public:
	void add(const Particle &);
	void addForce(ParticleForce *);
	void removeForces() { forces.clear(); }
	void remove(int);
	void update();
	void setLifespan(float);
	void reset();
	int removeNear(const ofVec3f & point, float dist);
	void draw();
	vector<Particle> particles;
	vector<ParticleForce *> forces;
};

class ModelParticleSystem : public ParticleSystem {
public:
	void add(const ModelParticle &);
	void update();
	void draw();
	vector<ModelParticle> modelParticles;
};


// Some convenient built-in forces
//
class GravityForce: public ParticleForce {
	ofVec3f gravity;
public:
	void set(const ofVec3f &g) { gravity = g; }
	ofVec3f getGravity() { return gravity; }
	GravityForce(const ofVec3f & gravity);
	GravityForce() {}
	void updateForce(Particle *);
};

class TurbulenceForce : public ParticleForce {
	ofVec3f tmin, tmax;
public:
	void set(const ofVec3f &min, const ofVec3f &max) { tmin = min; tmax = max; }
	TurbulenceForce(const ofVec3f & min, const ofVec3f &max);
	TurbulenceForce() { tmin.set(0, 0, 0); tmax.set(0, 0, 0); }
	void updateForce(Particle *);
};

class ImpulseRadialForce : public ParticleForce {
	float magnitude = 1.0;
	float height = .2;
public:
	void set(float mag) { magnitude = mag; }
	void setHeight(float h) { height = h; }
	ImpulseRadialForce(float magnitude);
	ImpulseRadialForce() {}
	void updateForce(Particle *);
};

class CyclicForce : public ParticleForce {
	float magnitude = 1.0;
public:
	void set(float mag) { magnitude = mag; }
	CyclicForce(float magnitude);  
	CyclicForce() {}
	void updateForce(Particle *);
};

class JumpImpulseForce : public ParticleForce {
	float magnitude = 1.0;
	ofVec3f dir = ofVec3f(0, 1, 0);
public:
	void setMag(float mag) { magnitude = mag; }
	float getMag() { return magnitude; }
	void setDir(ofVec3f);
	ofVec3f getDir() { return dir; }
	JumpImpulseForce(float magnitude);
	JumpImpulseForce() {};
	void updateForce(Particle*);
};

class MoveImpulseForce : public ParticleForce {
	float magnitude = 1.0;
	ofVec3f dir = ofVec3f(0, 1, 0);
public:
	void setMag(float mag) { magnitude = mag; }
	float getMag() { return magnitude; }
	void setDir(ofVec3f);
	ofVec3f getDir() { return dir; }
	MoveImpulseForce(float magnitude);
	MoveImpulseForce() {};
	void updateForce(Particle*);
};