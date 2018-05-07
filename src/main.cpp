#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofGLWindowSettings s;
	s.setGLVersion(4, 1);
	ofCreateWindow(s);

	ofRunApp(new ofApp());
}
