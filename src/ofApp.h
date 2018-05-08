#pragma once

#define ENABLE_FIXED_MODE //please comment out this line if you want to check default ofxAssimpModelLoader

#include "ofMain.h"

#ifdef ENABLE_FIXED_MODE
#include "ofxAssimpModelLoaderFix.h"
#else
#include "ofxAssimpModelLoader.h"
#endif

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
		
    protected:
		
#ifdef ENABLE_FIXED_MODE
		ofxAssimpModelLoaderFix sibenik;
#else
		ofxAssimpModelLoader sibenik;
#endif
};
