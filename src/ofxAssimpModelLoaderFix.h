#pragma once
//#include "ofxAssimpModelLoader.h"
#include "ofMain.h"
#include "ofxAssimpMeshHelperFix.h"
#include "ofxAssimpAnimation.h"
#include "ofxAssimpTexture.h"

//class ofxAssimpModelLoaderFix: public ofxAssimpModelLoader{
//public: 
//	ofxAssimpMeshHelperFix & getMeshHelper(int meshIndex);
//	void draw(ofPolyRenderMode renderType);
//
//protected:
//	void updateMeshes(aiNode * node, ofMatrix4x4 parentMatrix);
//	// Initial VBO creation, etc
//	void loadGLResources();
//	void getBoundingBoxForNode(const ofxAssimpMeshHelperFix & mesh, aiVector3D* min, aiVector3D* max);
//	vector<ofxAssimpMeshHelperFix> modelMeshes;
//
//};

class ofxAssimpModelLoaderFix {

public:
	~ofxAssimpModelLoaderFix();
	ofxAssimpModelLoaderFix();

	bool loadModel(string modelName, bool optimize = false);
	bool loadModel(ofBuffer & buffer, bool optimize = false, const char * extension = "");
	void createEmptyModel();
	void createLightsFromAiModel();
	void optimizeScene();

	void update();

	bool hasAnimations();
	unsigned int getAnimationCount();
	ofxAssimpAnimation & getAnimation(int animationIndex);
	void playAllAnimations();
	void stopAllAnimations();
	void resetAllAnimations();
	void setPausedForAllAnimations(bool pause);
	void setLoopStateForAllAnimations(ofLoopType state);
	void setPositionForAllAnimations(float position);
	OF_DEPRECATED_MSG("Use ofxAssimpAnimation instead", void setAnimation(int animationIndex));
	OF_DEPRECATED_MSG("Use ofxAssimpAnimation instead", void setNormalizedTime(float time));
	OF_DEPRECATED_MSG("Use ofxAssimpAnimation instead", void setTime(float time));
	OF_DEPRECATED_MSG("Use ofxAssimpAnimation instead", float getDuration(int animationIndex));

	bool hasMeshes();
	unsigned int getMeshCount();
	ofxAssimpMeshHelperFix & getMeshHelper(int meshIndex);

	void clear();

	void setScale(float x, float y, float z);
	void setPosition(float x, float y, float z);
	void setRotation(int which, float angle, float rot_x, float rot_y, float r_z);

	// Scale the model to the screen automatically.
	void setScaleNormalization(bool normalize);
	void setNormalizationFactor(float factor);

	vector<string> getMeshNames();
	int getNumMeshes();

	ofMesh getMesh(string name);
	ofMesh getMesh(int num);

	ofMesh getCurrentAnimatedMesh(string name);
	ofMesh getCurrentAnimatedMesh(int num);

	ofMaterialFix getMaterialForMesh(string name);
	ofMaterialFix getMaterialForMesh(int num);

	ofTexture getTextureForMesh(string name);
	ofTexture getTextureForMesh(int num);


	void drawWireframe();
	void drawFaces();
	void drawVertices();

	void enableTextures();
	void disableTextures();
	void enableNormals();
	void enableMaterials();
	void disableNormals();
	void enableColors();
	void disableColors();
	void disableMaterials();

	void draw(ofPolyRenderMode renderType);

	ofPoint getPosition();
	ofPoint getSceneCenter();
	float getNormalizedScale();
	ofPoint getScale();
	ofMatrix4x4 getModelMatrix();

	ofPoint getSceneMin(bool bScaled = false);
	ofPoint	getSceneMax(bool bScaled = false);

	int	getNumRotations();	// returns the no. of applied rotations
	ofPoint	getRotationAxis(int which); // gets each rotation axis
	float getRotationAngle(int which); //gets each rotation angle

	void calculateDimensions();

	const aiScene * getAssimpScene();

protected:
	void updateAnimations();
	void updateMeshes(aiNode * node, ofMatrix4x4 parentMatrix);
	void updateBones();
	void updateModelMatrix();

	// ai scene setup
	unsigned int initImportProperties(bool optimize);
	bool processScene();

	// Initial VBO creation, etc
	void loadGLResources();

	// updates the *actual GL resources* for the current animation
	void updateGLResources();

	void getBoundingBoxWithMinVector(aiVector3D* min, aiVector3D* max);
	void getBoundingBoxForNode(const ofxAssimpMeshHelperFix & mesh, aiVector3D* min, aiVector3D* max);

	ofFile file;

	aiVector3D scene_min, scene_max, scene_center;

	bool normalizeScale;
	double normalizedScale;

	vector<float> rotAngle;
	vector<ofPoint> rotAxis;
	ofPoint scale;
	ofPoint pos;
	ofMatrix4x4 modelMatrix;

	vector<ofLight> lights;
	vector<ofxAssimpTexture> textures;
	vector<ofxAssimpMeshHelperFix> modelMeshes;
	vector<ofxAssimpAnimation> animations;
	int currentAnimation; // DEPRECATED - to be removed with deprecated animation functions.

	bool bUsingTextures;
	bool bUsingNormals;
	bool bUsingColors;
	bool bUsingMaterials;
	float normalizeFactor;

	// the main Asset Import scene that does the magic.
	shared_ptr<const aiScene> scene;
	shared_ptr<aiPropertyStore> store;
};