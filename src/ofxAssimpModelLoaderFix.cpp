/*
* Modified ofxAssimpModelLoader by hikatech @hikatech.com
*
* loadGLResources()
* * add [texture.setTextureWrap(wrapMode, wrapMode);] to enable wrap mode, GL_REPEAT.
*/

#include "ofxAssimpModelLoaderFix.h"
#include "ofxAssimpUtils.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/config.h>

ofxAssimpModelLoaderFix::ofxAssimpModelLoaderFix() {
}

ofxAssimpModelLoaderFix::~ofxAssimpModelLoaderFix() {
}

//------------------------------------------
bool ofxAssimpModelLoaderFix::loadModel(string modelName, bool optimize) {

	file.open(modelName, ofFile::ReadOnly, true); // Since it may be a binary file we should read it in binary -Ed
	if (!file.exists()) {
		ofLogVerbose("ofxAssimpModelLoaderFix") << "loadModel(): model does not exist: \"" << modelName << "\"";
		return false;
	}

	ofLogVerbose("ofxAssimpModelLoaderFix") << "loadModel(): loading \"" << file.getFileName()
		<< "\" from \"" << file.getEnclosingDirectory() << "\"";

	if (scene.get() != nullptr) {
		clear();
		// we reset the shared_ptr explicitly here, to force the old 
		// aiScene to be deleted **before** a new aiScene is created.
		scene.reset();
	}

	// sets various properties & flags to a default preference
	unsigned int flags = initImportProperties(optimize);

	// loads scene from file
	scene = shared_ptr<const aiScene>(aiImportFileExWithProperties(file.getAbsolutePath().c_str(), flags, NULL, store.get()), aiReleaseImport);

	bool bOk = processScene();
	return bOk;
}


bool ofxAssimpModelLoaderFix::loadModel(ofBuffer & buffer, bool optimize, const char * extension) {

	ofLogVerbose("ofxAssimpModelLoaderFix") << "loadModel(): loading from memory buffer \"." << extension << "\"";

	if (scene.get() != nullptr) {
		clear();
		// we reset the shared_ptr explicitly here, to force the old 
		// aiScene to be deleted **before** a new aiScene is created.
		scene.reset();
	}

	// sets various properties & flags to a default preference
	unsigned int flags = initImportProperties(optimize);

	// loads scene from memory buffer - note this will not work for multipart files (obj, md3, etc)
	scene = shared_ptr<const aiScene>(aiImportFileFromMemoryWithProperties(buffer.getData(), buffer.size(), flags, extension, store.get()), aiReleaseImport);

	bool bOk = processScene();
	return bOk;
}


bool ofxAssimpModelLoaderFix::processScene() {

	normalizeFactor = ofGetWidth() / 2.0;

	if (scene) {
		loadGLResources();
		update();
		calculateDimensions();

		if (getAnimationCount())
			ofLogVerbose("ofxAssimpModelLoaderFix") << "loadModel(): scene has " << getAnimationCount() << "animations";
		else {
			ofLogVerbose("ofxAssimpModelLoaderFix") << "loadMode(): no animations";
		}

		return true;
	}
	else {
		ofLogError("ofxAssimpModelLoaderFix") << "loadModel(): " + (string)aiGetErrorString();
		clear();
		return false;
	}

	return false;
}


//-------------------------------------------
void ofxAssimpModelLoaderFix::loadGLResources() {

	ofLogVerbose("ofxAssimpModelLoaderFix") << "loadGLResources(): starting";

	// create new mesh helpers for each mesh, will populate their data later.
	modelMeshes.resize(scene->mNumMeshes, ofxAssimpMeshHelper());

	// create OpenGL buffers and populate them based on each meshes pertinant info.
	for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
		ofLogVerbose("ofxAssimpModelLoaderFix") << "loadGLResources(): loading mesh " << i;
		// current mesh we are introspecting
		aiMesh* mesh = scene->mMeshes[i];

		// the current meshHelper we will be populating data into.
		ofxAssimpMeshHelper & meshHelper = modelMeshes[i];
		//ofxAssimpMeshHelperFix meshHelper;

		//meshHelper.texture = NULL;

		// Handle material info
		aiMaterial* mtl = scene->mMaterials[mesh->mMaterialIndex];
		aiColor4D dcolor, scolor, acolor, ecolor;

		if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &dcolor)) {
			meshHelper.material.setDiffuseColor(aiColorToOfColor(dcolor));
		}

		if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &scolor)) {
			meshHelper.material.setSpecularColor(aiColorToOfColor(scolor));
		}

		if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &acolor)) {
			meshHelper.material.setAmbientColor(aiColorToOfColor(acolor));
		}

		if (AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &ecolor)) {
			meshHelper.material.setEmissiveColor(aiColorToOfColor(ecolor));
		}

		float shininess;
		if (AI_SUCCESS == aiGetMaterialFloat(mtl, AI_MATKEY_SHININESS, &shininess)) {
			meshHelper.material.setShininess(shininess);
		}

		int blendMode;
		if (AI_SUCCESS == aiGetMaterialInteger(mtl, AI_MATKEY_BLEND_FUNC, &blendMode)) {
			if (blendMode == aiBlendMode_Default) {
				meshHelper.blendMode = OF_BLENDMODE_ALPHA;
			}
			else {
				meshHelper.blendMode = OF_BLENDMODE_ADD;
			}
		}

		// Culling
		unsigned int max = 1;
		int two_sided;
		if ((AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_TWOSIDED, &two_sided, &max)) && two_sided)
			meshHelper.twoSided = true;
		else
			meshHelper.twoSided = false;

		// Load Textures
		int texIndex = 0;
		aiString texPath;
		// TODO: handle other aiTextureTypes
		aiTextureMapMode mapMode;// added
		if (AI_SUCCESS == mtl->GetTexture(aiTextureType_DIFFUSE, texIndex, &texPath, 0, 0, 0, 0, &mapMode)) { // modified but mapMode returned garbage value...
			ofLogVerbose("ofxAssimpModelLoaderFix") << "loadGLResource(): loading image from \"" << texPath.data << "\"";
			string modelFolder = file.getEnclosingDirectory();
			string relTexPath = ofFilePath::getEnclosingDirectory(texPath.data, false);
			string texFile = ofFilePath::getFileName(texPath.data);
			string realPath = ofFilePath::join(ofFilePath::join(modelFolder, relTexPath), texFile);

			if (ofFile::doesFileExist(realPath) == false) {
				ofLogError("ofxAssimpModelLoaderFix") << "loadGLResource(): texture doesn't exist: \""
					<< file.getFileName() + "\" in \"" << realPath << "\"";
			}

			ofxAssimpTexture assimpTexture;
			bool bTextureAlreadyExists = false;
			for (int j = 0; j<textures.size(); j++) {
				assimpTexture = textures[j];
				if (assimpTexture.getTexturePath() == realPath) {
					bTextureAlreadyExists = true;
					break;
				}
			}
			if (bTextureAlreadyExists) {
				meshHelper.assimpTexture = assimpTexture;
				ofLogVerbose("ofxAssimpModelLoaderFix") << "loadGLResource(): texture already loaded: \""
					<< file.getFileName() + "\" from \"" << realPath << "\"";
			}
			else {
				ofTexture texture;
				
				//set wrapmode (added part begin)
				GLint wrapMode = GL_REPEAT;
				switch (mapMode) {
				case aiTextureMapMode_Wrap:
					wrapMode = GL_REPEAT;
					break;
				case aiTextureMapMode_Clamp:
					wrapMode = GL_CLAMP_TO_EDGE;
					break;
				case aiTextureMapMode_Decal:
					wrapMode = GL_CLAMP_TO_BORDER;
					break;
				case aiTextureMapMode_Mirror:
					wrapMode = GL_MIRRORED_REPEAT;
					break;
				}
				texture.setTextureWrap(wrapMode, wrapMode);
				// (added part end)

				bool bTextureLoadedOk = ofLoadImage(texture, realPath);
				if (bTextureLoadedOk) {
					textures.push_back(ofxAssimpTexture(texture, realPath));
					assimpTexture = textures.back();
					meshHelper.assimpTexture = assimpTexture;
					ofLogVerbose("ofxAssimpModelLoaderFix") << "loadGLResource(): texture loaded, dimensions: "
						<< texture.getWidth() << "x" << texture.getHeight();
				}
				else {
					ofLogError("ofxAssimpModelLoaderFix") << "loadGLResource(): couldn't load texture: \""
						<< file.getFileName() + "\" from \"" << realPath << "\"";
				}
			}
		}

		meshHelper.mesh = mesh;
		aiMeshToOfMesh(mesh, meshHelper.cachedMesh, &meshHelper);
		meshHelper.cachedMesh.setMode(OF_PRIMITIVE_TRIANGLES);
		meshHelper.validCache = true;
		meshHelper.hasChanged = false;

		int numOfAnimations = scene->mNumAnimations;
		for (int i = 0; i<numOfAnimations; i++) {
			aiAnimation * animation = scene->mAnimations[i];
			animations.push_back(ofxAssimpAnimation(scene, animation));
		}

		if (hasAnimations()) {
			meshHelper.animatedPos.resize(mesh->mNumVertices);
			if (mesh->HasNormals()) {
				meshHelper.animatedNorm.resize(mesh->mNumVertices);
			}
		}


		int usage;
		if (getAnimationCount()) {
#ifndef TARGET_OPENGLES
			if (!ofIsGLProgrammableRenderer()) {
				usage = GL_STATIC_DRAW;
			}
			else {
				usage = GL_STREAM_DRAW;
			}
#else
			usage = GL_DYNAMIC_DRAW;
#endif
		}
		else {
			usage = GL_STATIC_DRAW;

		}

		meshHelper.vbo.setVertexData(&mesh->mVertices[0].x, 3, mesh->mNumVertices, usage, sizeof(aiVector3D));
		if (mesh->HasVertexColors(0)) {
			meshHelper.vbo.setColorData(&mesh->mColors[0][0].r, mesh->mNumVertices, GL_STATIC_DRAW, sizeof(aiColor4D));
		}
		if (mesh->HasNormals()) {
			meshHelper.vbo.setNormalData(&mesh->mNormals[0].x, mesh->mNumVertices, usage, sizeof(aiVector3D));
		}
		if (meshHelper.cachedMesh.hasTexCoords()) {
			meshHelper.vbo.setTexCoordData(meshHelper.cachedMesh.getTexCoordsPointer()[0].getPtr(), mesh->mNumVertices, GL_STATIC_DRAW, sizeof(ofVec2f));
		}

		meshHelper.indices.resize(mesh->mNumFaces * 3);
		int j = 0;
		for (unsigned int x = 0; x < mesh->mNumFaces; ++x) {
			for (unsigned int a = 0; a < mesh->mFaces[x].mNumIndices; ++a) {
				meshHelper.indices[j++] = mesh->mFaces[x].mIndices[a];
			}
		}

		meshHelper.vbo.setIndexData(&meshHelper.indices[0], meshHelper.indices.size(), GL_STATIC_DRAW);

		//modelMeshes.push_back(meshHelper);
	}



	ofLogVerbose("ofxAssimpModelLoaderFix") << "loadGLResource(): finished";
}
