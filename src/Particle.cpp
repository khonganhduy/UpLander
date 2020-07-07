#include "Particle.h"


Particle::Particle() {

	// initialize particle with some reasonable values first;
	//
	velocity.set(0, 0, 0);
	acceleration.set(0, 0, 0);
	position.set(0, 0, 0);
	forces.set(0, 0, 0);
	lifespan = 5;
	birthtime = 0;
	radius = .1;
	damping = .99;
	mass = 1;
}

void Particle::draw() {
	ofSetColor(ofMap(age(), 0, lifespan, 255, 10), 0, 0);
	ofDrawSphere(position, radius);
}

// write your own integrator here.. (hint: it's only 3 lines of code)
//
void Particle::integrate() {

	// check for 0 framerate to avoid divide errors
	//
	float framerate = ofGetFrameRate();
	if (framerate < 1.0) return;

	// interval for this step
	//
	float dt = 1.0 / framerate;

	// update position based on velocity
	//
//	position += (velocity * dt);

	// update acceleration with accumulated paritcles forces
	// remember :  (f = ma) OR (a = 1/m * f)
	//
	ofVec3f accel = acceleration;    // start with any acceleration already on the particle
	accel += (forces * (1.0 / mass));
	velocity += accel * dt;

	// add a little damping for good measure
	//
	velocity *= damping;
	position += (velocity * dt);
	// clear forces on particle (they get re-added each step)
	//
	forces.set(0, 0, 0);
}

//  return age in seconds
//
float Particle::age() {
	return (ofGetElapsedTimeMillis() - birthtime)/1000.0;
}

ModelParticle::ModelParticle() {

}

void ModelParticle::draw() {
	//drawBox(boundingBox);
	drawRay(rayYMinus);
	model.drawFaces();
}

void ModelParticle::update() {
	model.setPosition(position.x - offset.x, position.y - offset.y, position.z - offset.z);
	rayYMinus.origin = Vector3(position.x, position.y, position.z);
	rayXMinus.origin = Vector3(position.x, position.y, position.z);
	rayXPlus.origin = Vector3(position.x, position.y, position.z);
	rayZMinus.origin = Vector3(position.x, position.y, position.z);
	rayZPlus.origin = Vector3(position.x, position.y, position.z);
}

void ModelParticle::setModel(string modelPath) {
	if (model.loadModel(modelPath)) {
		model.setScaleNormalization(false);
		model.setPosition(0, 0, 0);
		//model.setScale(1, 1, 1);
		mesh = model.getMesh(0);
		boundingBox = meshBounds(mesh);
		this->position = ofVec3f(boundingBox.center().x(), boundingBox.center().y(), boundingBox.center().z());
		offset = this->position - model.getPosition();
		rayYMinus = Ray(Vector3(this->position.x, this->position.y, this->position.z), Vector3(0, -1, 0));
		rayXMinus = Ray(Vector3(this->position.x, this->position.y, this->position.z), Vector3(-1, 0, 0));
		rayXPlus = Ray(Vector3(this->position.x, this->position.y, this->position.z), Vector3(1, 0, 0));
		rayZMinus = Ray(Vector3(this->position.x, this->position.y, this->position.z), Vector3(0, 0, -1));
		rayZPlus = Ray(Vector3(this->position.x, this->position.y, this->position.z), Vector3(0, 0, 1));
	}
	else {
		cout << "Cannot load model from: " << modelPath << endl;
		ofExit();
	}
}

Box ModelParticle::meshBounds(const ofMesh & mesh) {
	int n = mesh.getNumVertices();
	ofVec3f v = mesh.getVertex(0);
	ofVec3f max = v;
	ofVec3f min = v;
	for (int i = 1; i < n; i++) {
		ofVec3f v = mesh.getVertex(i);

		if (v.x > max.x) max.x = v.x;
		else if (v.x < min.x) min.x = v.x;

		if (v.y > max.y) max.y = v.y;
		else if (v.y < min.y) min.y = v.y;

		if (v.z > max.z) max.z = v.z;
		else if (v.z < min.z) min.z = v.z;
	}
	return Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
}

void ModelParticle::drawBox(const Box &box) {
	Vector3 min = box.parameters[0];
	Vector3 max = box.parameters[1];
	Vector3 size = max - min;
	Vector3 center = size / 2 + min;
	ofVec3f p = ofVec3f(center.x(), center.y(), center.z());
	float w = size.x();
	float h = size.y();
	float d = size.z();
	ofNoFill();
	ofDrawBox(p, w, h, d);
	ofFill();
}

void ModelParticle::drawRay(const Ray &ray) {
	//ofFill();
	ofSetColor(ofColor::green);
	ofSetLineWidth(2);
	ofDrawLine(ofVec3f(ray.origin.x(), ray.origin.y() - offset.y, ray.origin.z()), ofVec3f(intersectionYMinus.x, intersectionYMinus.y, intersectionYMinus.z));
	ofDrawSphere(intersectionYMinus, 1);
	//ofDrawLine(ofVec3f(ray.origin.x(), ray.origin.y() - offset.y, ray.origin.z()), ofVec3f(intersectionXMinus.x, intersectionXMinus.y, intersectionXMinus.z));
	//ofDrawSphere(intersectionXMinus, 3);
	ofSetColor(255, 255, 255, 255);
	//ofNoFill();
}