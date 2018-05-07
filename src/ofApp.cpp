#include "ofApp.h"

vector<ofLight> lights;

//--------------------------------------------------------------
void ofApp::setup(){

	//light stuff
	ofEnableDepthTest();
	ofSetSmoothLighting(true);
	lights.resize(6);
	for (auto l : lights) {
		l.setDiffuseColor(ofColor(32, 32, 32));
		l.setSpecularColor(ofColor(0, 0, 0));
		l.setAmbientColor(ofColor(10, 10, 10));
		l.setDirectional();
	}
	lights[0].setOrientation(ofVec3f(0, 90, 45));
	lights[1].setOrientation(ofVec3f(0, -90, -45));
	lights[2].setOrientation(ofVec3f(90, 90, -45));
	lights[3].setOrientation(ofVec3f(0, -90, 45));
	lights[4].setOrientation(ofVec3f(90, -90, 45));
	lights[5].setOrientation(ofVec3f(-90, -90, 45));


	//load model
	sibenik.setScaleNormalization(false);
	sibenik.setScale(100, 100, 100);
	//sibenik.loadModel("sibenik/sibenik.3DS", true);
	sibenik.loadModel("sibenikOBJ/sibenik.obj", true);
	sibenik.setPosition(ofGetWidth() / 2, ofGetHeight() / 2, 0);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	// enable lighting //
	ofEnableLighting();
	for (auto l : lights)l.enable();

	//tumble the world with the mouse
	ofPushMatrix();

		//draw in middle of the screen
		ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2, 0);
		//tumble according to mouse
		ofRotate(-mouseY, 1, 0, 0);
		ofRotate(mouseX, 0, 1, 0);
		ofTranslate(-ofGetWidth() / 2, -ofGetHeight() / 2, 0);

		ofSetColor(255, 255, 255, 255);
		sibenik.draw(OF_MESH_FILL);

	ofPopMatrix();

	// turn off lighting //
	for (auto l : lights)l.disable();
	ofDisableLighting();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

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
