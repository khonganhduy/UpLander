#pragma once

#include "ofMain.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "ParticleSystem.h"
#include "ofxGui.h"
#include "ofxAssimpModelLoader.h"
#include "Octree.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void initLightingAndMaterials();
		void loadVbo();
		TreeNode testOctreeIntersect(Ray);
		TreeNode testOctreeIntersectYMinus();
		TreeNode testOctreeIntersectXMinus();
		TreeNode testOctreeIntersectXPlus();
		TreeNode testOctreeIntersectZMinus();
		TreeNode testOctreeIntersectZPlus();

		void testCollision();
		void addAllForcesToModel();

		ofCamera *currentCam;
		ofCamera mainCam;
		ofCamera sideCam1;
		ofCamera sideCam2;
		ofCamera frontCam;
		ofCamera backCam;
		ofCamera topCam;
		ofCamera botCam;
		ofCamera trackingCam;
		ofEasyCam freeCam;

		ofLight keyLight;
		//ofLight 
		ModelParticle* modelParticle;
		ofxAssimpModelLoader terrain;
		ParticleEmitter emitter;
		ParticleSystem* particleSystem;
		ModelParticleSystem* modelParticleSystem;
		GravityForce *gravityForce;
		JumpImpulseForce *jumpImpulseForce;
		TurbulenceForce *turbulenceForce;
		MoveImpulseForce *moveImpulseForce;

		TurbulenceForce *turbForce;
		GravityForce *gravForce;
		ImpulseRadialForce *radialForce;
		CyclicForce *cyclicForce;

		Octree terrainTree;
		Octree landingPoint1Tree;
		Octree landingPoint2Tree;
		Octree landingPoint3Tree;
		TreeNode intersectNodeYMinus;
		TreeNode intersectNodeXMinus;
		TreeNode intersectNodeXPlus;
		TreeNode intersectNodeZMinus;
		TreeNode intersectNodeZPlus;
		vector<int> intersectPointIndex;

		ofxPanel gui;
		ofxFloatSlider gravity;
		ofxFloatSlider jumpMag;

		bool started;
		bool bModelLoaded;
		bool bWireFrame;
		bool displayOctree;
		bool displayLeafNodes;
		bool leftKeyDown;
		bool rightKeyDown;
		bool spaceKeyDown;
		bool jumping;
		float botBound = ofGetWindowHeight();
		float topBound = 0;
		float rightBound = ofGetWindowWidth();
		float leftBound = 0;
		int distanceFromGround;
		int score;
		int scoreAddition;

		enum playerState {LEFT,RIGHT, IDLE};
		playerState state;
		//ofImage image;
		ofSoundPlayer player;

		// textures
//
		ofTexture  particleTex;

		// shaders
		//
		ofVbo vbo;
		ofShader shader;
};
