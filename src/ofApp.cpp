#include "ofApp.h"


//--------------------------------------------------------------
// CS134 Midterm 1
// Anhduy Khong
// Create a particle that has forces applied on to it to simulate jump and gravity
// Impulse force and gravity force used for vertical movement
// Velocity modification (no forces) used for horizontal movement
// Collision detection applied for "boxes" that can stop movement in both x and y axis
//--------------------------------------------------------------
void ofApp::setup(){
	//Setup Lights
	ofEnableSmoothing();
	ofEnableDepthTest();
	keyLight.setup();
	keyLight.enable();
	keyLight.setAreaLight(1, 1);
	keyLight.setAmbientColor(ofFloatColor(0.1, 0.1, 0.1));
	keyLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	keyLight.setSpecularColor(ofFloatColor(1, 1, 1));

	keyLight.setDiffuseColor(ofFloatColor(1, 1, 1));
	keyLight.rotate(45, ofVec3f(0, 1, 0));
	keyLight.rotate(-15, ofVec3f(1, 0, 0));
	keyLight.setPosition(5, 5, 5);
	keyLight.setPosition(50, 400, 50);

	//Initialize utility flags
	bModelLoaded = false;
	bWireFrame = false;
	displayOctree = false;
	displayLeafNodes = false;
	leftKeyDown = false;
	rightKeyDown = false;
	spaceKeyDown = false;
	jumping = false;
	started = false;
	state = IDLE;
	score = 0;
	scoreAddition = 0;

	intersectPointIndex = { 2062,2156,2167,2158,15723,15724,15727,15728,15731,15732,15917,
		15918,15921,15922,15925,15926,33937,33938,33939,33940,33941,33942,33943,33944,33945,
		33946,33947,33948,44837,44850,44851,44864,44889,45030,45031,45056,45161,45162,45163,
		46470,46473,46474,46475,46476,46478,46479,46515,46517,46518,46519,46520,46521,46522,
		46523,46524,46547,58645,58648,58649,58652,58653,58656,58657,58660,58661,58664,58665,
		58668,58669,58672,58673 };

	// texture loading
	//
	ofDisableArbTex();     // disable rectangular textures

	// load textures
	//
	if (!ofLoadImage(particleTex, "images/dot.png")) {
		cout << "Particle Texture File: images/dot.png not found" << endl;
		ofExit();
	}

	// Load shaders
#ifdef TARGET_OPENGLES
	shader.load("shaders_gles/shader");
#else
	shader.load("shaders/shader");
#endif

	//Initialize particle
	modelParticle = new ModelParticle();
	modelParticle->lifespan = -1;
	modelParticle->setModel("geo/HouseExport2.obj");
	modelParticle->mass = 2;
	modelParticle->position = ofVec3f(1, 50, 1);

	// Setup cameras
	freeCam.setDistance(10);
	freeCam.setNearClip(.1);
	freeCam.setFov(100);   // approx equivalent to 28mm in 35mm format
	freeCam.setGlobalPosition(modelParticle->position + ofVec3f(50, 0, 0));
	freeCam.lookAt(ofVec3f(0,0,0));

	mainCam.setGlobalPosition(modelParticle->position + ofVec3f(50, 0, 0));
	mainCam.setFov(100);
	mainCam.lookAt(modelParticle->position - modelParticle->offset);
	currentCam = &mainCam;

	sideCam1.setGlobalPosition(modelParticle->position + ofVec3f(0, 0, 50));
	sideCam1.lookAt(modelParticle->position);
	sideCam1.setFov(100);

	sideCam2.setGlobalPosition(modelParticle->position + ofVec3f(0, 0, -50));
	sideCam2.lookAt(modelParticle->position);
	sideCam2.setFov(100);

	frontCam.setGlobalPosition(modelParticle->position + ofVec3f(50, 0, 0));
	frontCam.lookAt(modelParticle->position);
	frontCam.setFov(100);

	backCam.setGlobalPosition(modelParticle->position + ofVec3f(-50, 0, 0));
	backCam.lookAt(modelParticle->position);
	backCam.setFov(100);

	topCam.setGlobalPosition(modelParticle->position + ofVec3f(0, 50, 0));
	topCam.lookAt(modelParticle->position);
	topCam.setFov(100);

	botCam.setGlobalPosition(modelParticle->position + ofVec3f(0, -50, 0));
	botCam.lookAt(modelParticle->position);
	botCam.setFov(100);


	//Initialize particle system and add the particle to it
	modelParticleSystem = new ModelParticleSystem();
	modelParticleSystem->add(*modelParticle);
	modelParticle = &(modelParticleSystem->modelParticles[0]); //Set pointer of particle to the address of the particle inside the system 

	//Initialize forces, do not add forces to system until necessary
	gravityForce = new GravityForce(ofVec3f(0, -5, 0));
	jumpImpulseForce = new JumpImpulseForce(100);
	turbulenceForce = new TurbulenceForce(ofVec3f(-10,-1,-10), ofVec3f(10,1,10));
	moveImpulseForce = new MoveImpulseForce(100);

	//Setup terrain
	string filepath = "geo/upterrainv5.obj";
	if (terrain.loadModel(filepath)) {
		terrain.setScaleNormalization(false);
		//terrain.setScale(10, 10, 10);
		//model.setRotation(1, 180, 0, 0, 1);
		terrain.setPosition(0, 0, 0);
		bModelLoaded = true;
	}
	else cout << "Error: Can't load model: " << filepath << endl;

	terrainTree.create(terrain.getMesh(0), 8);
	intersectNodeYMinus = testOctreeIntersect(modelParticle->rayYMinus);
	modelParticle->intersectionYMinus = ofVec3f(intersectNodeYMinus.box.center().x(), intersectNodeYMinus.box.center().y(), intersectNodeYMinus.box.center().z()); 
	distanceFromGround = modelParticle->position.y - modelParticle->offset.y - modelParticle->intersectionYMinus.y;
	landingPoint1Tree.create(terrain.getMesh(1), 6);
	landingPoint2Tree.create(terrain.getMesh(2), 6);
	landingPoint3Tree.create(terrain.getMesh(3), 6);

	//Setup Emitter
	turbForce = new TurbulenceForce(ofVec3f(-20, -20, -20), ofVec3f(20, 20, 20));
	gravForce = new GravityForce(ofVec3f(0, -5, 0));
	radialForce = new ImpulseRadialForce(1000);
	cyclicForce = new CyclicForce(100);

	emitter.sys->addForce(turbForce);
	emitter.sys->addForce(gravForce);
	//emitter.sys->addForce(radialForce);
	//emitter.sys->addForce(cyclicForce);

	emitter.setVelocity(ofVec3f(0, 0, 0));
	emitter.setOneShot(false);
	emitter.setEmitterType(RadialEmitter);
	emitter.setGroupSize(1);
	emitter.setRandomLife(true);
	emitter.setLifespanRange(ofVec2f(5, 8));
	emitter.setVelocity(ofVec3f(ofRandom(-10, 10), ofRandom(0, 10), ofRandom(-10, 10)));
	emitter.setRate(2);
	emitter.start();


	// Initialize sound
	player.load("sfx/married_life.wav");
	player.play();

	//Sliders setup
	gui.setup();
	//gui.add(gravity.setup("Gravity", gravityForce->getGravity().y, 100, 2000));
	//gui.add(direction.setup("Jump Direction", jumpImpulseForce->getDir(), ofVec3f(-1,-1,-1), ofVec3f(1,1,1)));
	//gui.add(jumpMag.setup("Jump Mag", jumpImpulseForce->getMag(), 50000, 300000));
}

//--------------------------------------------------------------
void ofApp::update(){
	if (started) {
		modelParticleSystem->update();
		intersectNodeYMinus = testOctreeIntersect(modelParticle->rayYMinus);
		//intersectNodeXMinus = testOctreeIntersect(modelParticle->rayXMinus);
		/*intersectNodeXPlus = testOctreeIntersect(modelParticle->rayXPlus);
		intersectNodeZMinus = testOctreeIntersect(modelParticle->rayZMinus);
		intersectNodeZPlus = testOctreeIntersect(modelParticle->rayZPlus);*/
		modelParticle->intersectionYMinus = ofVec3f(intersectNodeYMinus.box.center().x(), intersectNodeYMinus.box.center().y(), intersectNodeYMinus.box.center().z());
		//modelParticle->intersectionXMinus = ofVec3f(intersectNodeXMinus.box.center().x(), intersectNodeXMinus.box.center().y(), intersectNodeXMinus.box.center().z());
		/*modelParticle->intersectionXPlus = ofVec3f(intersectNodeXPlus.box.center().x(), intersectNodeXPlus.box.center().y(), intersectNodeXPlus.box.center().z());
		modelParticle->intersectionZMinus = ofVec3f(intersectNodeZMinus.box.center().x(), intersectNodeZMinus.box.center().y(), intersectNodeZMinus.box.center().z());
		modelParticle->intersectionZPlus = ofVec3f(intersectNodeZPlus.box.center().x(), intersectNodeZPlus.box.center().y(), intersectNodeZPlus.box.center().z());*/
		testCollision();
		distanceFromGround = modelParticle->position.y - modelParticle->offset.y - modelParticle->intersectionYMinus.y;
	}
	emitter.setPosition(modelParticle->position + ofVec3f(0, 10, 0));
	emitter.update();
	mainCam.setGlobalPosition(modelParticle->position + ofVec3f(50, 0, 0));
	sideCam1.setGlobalPosition(modelParticle->position + ofVec3f(0, 0, 50));
	sideCam2.setGlobalPosition(modelParticle->position + ofVec3f(0, 0, -50));
	frontCam.setGlobalPosition(modelParticle->position + ofVec3f(50, 0, 0));
	backCam.setGlobalPosition(modelParticle->position + ofVec3f(-50, 0, 0));
	topCam.setGlobalPosition(modelParticle->position + ofVec3f(0, 50, 0));
	botCam.setGlobalPosition(modelParticle->position + ofVec3f(0, -50, 0));
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofSetBackgroundColor(ofColor::black);
	currentCam->begin();
	ofPushMatrix();
	ofRotateDeg(90, 0, 0, 1);
	ofSetLineWidth(1);
	ofSetColor(ofColor::dimGrey);
	ofDrawGridPlane();
	ofPopMatrix();
	if (displayOctree) {
		terrainTree.draw(terrainTree.root, 6, 0);
	}
	else if (displayLeafNodes) {
		terrainTree.drawLeafNodes(terrainTree.root);
	}
	terrain.drawFaces();
	modelParticleSystem->draw();
	currentCam->end();


	loadVbo();
	glDepthMask(GL_FALSE);

	ofSetColor(255, 100, 90);

	// this makes everything look glowy :)
	//
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofEnablePointSprites();

	shader.begin();
	currentCam->begin();
	particleTex.bind();
	vbo.draw(GL_POINTS, 0, (int)emitter.sys->particles.size());
	particleTex.unbind();
	currentCam->end();
	shader.end();
	gui.draw();

	ofDisablePointSprites();
	ofDisableBlendMode();
	ofEnableAlphaBlending();

	// set back the depth mask
	//
	glDepthMask(GL_TRUE);

	string str;
	str += "Frame Rate: " + std::to_string(ofGetFrameRate());
	ofSetColor(ofColor::green);
	ofDrawBitmapString(str, ofGetWindowWidth() - 170, 15);
	ofSetColor(ofColor::white);
	str = "Distance From Ground: " + std::to_string(distanceFromGround);
	ofDrawBitmapString(str, 0, ofGetWindowHeight());
	str = "Velocity Y: " + std::to_string(modelParticle->velocity.y);
	ofDrawBitmapString(str, 0, ofGetWindowHeight() - 10);
	str = "Score: " + std::to_string(score);
	ofDrawBitmapString(str, ofGetWindowWidth() - 100, ofGetWindowHeight());
	ofSetColor(255, 255, 255, 255);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {
	case OF_KEY_UP: {
		started = true;
		leftKeyDown = true;
		modelParticleSystem->removeForces();
		modelParticleSystem->reset();
		moveImpulseForce->setDir(ofVec3f(-1, 0, 0));
		moveImpulseForce->setMag(100);
		addAllForcesToModel();
		break;
	}
	case OF_KEY_DOWN: {
		started = true;
		leftKeyDown = true;
		modelParticleSystem->removeForces();
		modelParticleSystem->reset();
		moveImpulseForce->setDir(ofVec3f(1, 0, 0));
		moveImpulseForce->setMag(100);
		addAllForcesToModel();
		break;
	}
	case OF_KEY_LEFT: {
		started = true;
			leftKeyDown = true;
			modelParticleSystem->removeForces();
			modelParticleSystem->reset();
			moveImpulseForce->setDir(ofVec3f(0, 0, 1));
			moveImpulseForce->setMag(100);
			addAllForcesToModel();
		break;
	}
	case OF_KEY_RIGHT: {
		started = true;
			rightKeyDown = true;
			modelParticleSystem->removeForces();
			modelParticleSystem->reset();
			moveImpulseForce->setDir(ofVec3f(0, 0, -1));
			moveImpulseForce->setMag(100);
			addAllForcesToModel();
		break;
	}
	case ' ': {
		started = true;
			modelParticleSystem->removeForces();
			modelParticleSystem->reset();
			moveImpulseForce->setDir(ofVec3f(0, 1, 0));
			moveImpulseForce->setMag(100);
			addAllForcesToModel();
		break;
	}
	case OF_KEY_F1:
		currentCam = &mainCam;
		break;
	case OF_KEY_F2:
		currentCam = &freeCam;
		break;
	case OF_KEY_F3:
		currentCam = &sideCam1;
		break;
	case OF_KEY_F4:
		currentCam = &sideCam2;
		break;
	case OF_KEY_F5:
		currentCam = &frontCam;
		break;
	case OF_KEY_F6:
		currentCam = &backCam;
		break;
	case OF_KEY_F7:
		currentCam = &topCam;
		break;
	case OF_KEY_F8:
		currentCam = &botCam;
		break;
	case OF_KEY_BACKSPACE: {
		modelParticleSystem->removeForces();
		modelParticle->velocity = ofVec3f(0, 0, 0);
		break;
	}
	case 'B':
	case 'b': {
		if (displayLeafNodes) {
			displayLeafNodes = false;
		}
		displayOctree = !displayOctree;
		break;
	}
	case 'H':
	case 'h': {
		if (displayOctree) {
			displayOctree = false;
		}
		displayLeafNodes = !displayLeafNodes;
		break;
	}
	case 'R':
	case 'r': {
		modelParticleSystem->removeForces();
		modelParticleSystem->reset();
		modelParticle->velocity = ofVec3f(0, 0, 0);
		modelParticle->position = ofVec3f(1, 50, 1);
		score = 0;
		scoreAddition = 0;
		break;
	}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	switch (key) {
	case OF_KEY_LEFT: {
		leftKeyDown = false;
		break;
	}
	case OF_KEY_RIGHT: {
		rightKeyDown = false;
		break;
	}
	case ' ': {
		spaceKeyDown = false;
		break;
	}
	case 'W':
	case 'w':
		bWireFrame = !bWireFrame;
		break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float position[] =
	{ 5.0, 5.0, 5.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
}

void ofApp::loadVbo() {
	if (emitter.sys->particles.size() < 1) return;

	vector<ofVec3f> sizes;
	vector<ofVec3f> points;
	for (int i = 0; i < emitter.sys->particles.size(); i++) {
		points.push_back(emitter.sys->particles[i].position);
		sizes.push_back(ofVec3f(20));
	}
	// upload the data to the vbo
	//
	int total = (int)points.size();
	vbo.clear();
	vbo.setVertexData(&points[0], total, GL_STATIC_DRAW);
	vbo.setNormalData(&sizes[0], total, GL_STATIC_DRAW);
}

TreeNode ofApp::testOctreeIntersect(Ray ray) {
	TreeNode intersectNode;
	if (landingPoint1Tree.intersect(ray, landingPoint1Tree.root, intersectNode, ofVec3f(ray.origin.x(), ray.origin.y(), ray.origin.z()))) {
		scoreAddition = 30;
	}
	else if (landingPoint2Tree.intersect(ray, landingPoint2Tree.root, intersectNode, ofVec3f(ray.origin.x(), ray.origin.y(), ray.origin.z()))) {
		scoreAddition = 20;
	}
	else if (landingPoint3Tree.intersect(ray, landingPoint3Tree.root, intersectNode, ofVec3f(ray.origin.x(), ray.origin.y(), ray.origin.z()))) {
		scoreAddition = 10;
	}
	else {
		terrainTree.intersect(ray, terrainTree.root, intersectNode, ofVec3f(ray.origin.x(), ray.origin.y(), ray.origin.z()));
	}
	return intersectNode;
}

void ofApp::testCollision() {
	/*if (modelParticle->position.x - modelParticle->boundingBox.max().x() - modelParticle->intersectionXPlus.x <= 0) {
		modelParticleSystem->removeForces();
		modelParticleSystem->reset();
		moveImpulseForce->setDir(modelParticle->velocity.cross(ofVec3f(0, 1, 0)).getNormalized());
		addAllForcesToModel();
	}*/
	/*if (modelParticle->position.x - modelParticle->boundingBox.min().x() - modelParticle->intersectionXMinus.x <= 0) {
		modelParticleSystem->removeForces();
		modelParticleSystem->reset();
		moveImpulseForce->setDir(modelParticle->velocity.cross(ofVec3f(0, 1, 0)).getNormalized());
		addAllForcesToModel();
	}*/
	if (modelParticle->position.y - modelParticle->offset.y - modelParticle->intersectionYMinus.y <= 0) {
		if (modelParticle->velocity.y >= -3) {
			modelParticleSystem->removeForces();
			modelParticleSystem->reset();
			modelParticle->velocity = ofVec3f(0, 0, 0);
			modelParticle->position = ofVec3f(modelParticle->position.x, modelParticle->intersectionYMinus.y + modelParticle->offset.y, modelParticle->position.z);
			score += scoreAddition;
			/*bool found = false;
			int i = 0;
			while (!found && i < intersectPointIndex.size()) {
				int j = 0;
				while (!found && j < intersectNodeYMinus.points.size()) {
					if (intersectPointIndex[i] == intersectNodeYMinus.points[j]) {
						found = true;
						score += 10;
					}
					j++;
				}
				i++;
			}
			cout << found << endl;*/
			started = false;
		}
		else {
			modelParticleSystem->removeForces();
			modelParticleSystem->reset();
			//moveImpulseForce->setDir(modelParticle->velocity.cross(ofVec3f(1, 0, 0)).getNormalized());
			moveImpulseForce->setDir(ofVec3f(0, 1, 0));
			moveImpulseForce->setMag(2000);
			addAllForcesToModel();
			modelParticle->position = ofVec3f(modelParticle->position.x, modelParticle->intersectionYMinus.y + modelParticle->offset.y, modelParticle->position.z);
		}
	}
}

void ofApp::addAllForcesToModel() {
	modelParticleSystem->addForce(gravityForce);
	modelParticleSystem->addForce(turbulenceForce);
	modelParticleSystem->addForce(moveImpulseForce);
}