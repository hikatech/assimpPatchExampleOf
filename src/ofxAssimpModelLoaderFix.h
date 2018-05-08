/*
* Modified ofxAssimpModelLoader by hikatech @hikatech.com
*
* loadGLResources()
* * added [texture.setTextureWrap(wrapMode, wrapMode);] to enable wrap mode, GL_REPEAT.
*/

#pragma once

#include "ofxAssimpModelLoader.h"

class ofxAssimpModelLoaderFix:public ofxAssimpModelLoader {

public:
	~ofxAssimpModelLoaderFix();
	ofxAssimpModelLoaderFix();

	bool loadModel(string modelName, bool optimize = false);
	bool loadModel(ofBuffer & buffer, bool optimize = false, const char * extension = "");

protected:

	// ai scene setup
	bool processScene();

	// Initial VBO creation, etc
	void loadGLResources();
};