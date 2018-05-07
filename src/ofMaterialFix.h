#pragma once

#include "ofMaterial.h"

class ofMaterialFix : public ofMaterial {
public:
	ofMaterialFix();
	virtual ~ofMaterialFix() {};

	// apply the material
	void begin() const;
	void end() const;

protected:
	void initShaders(ofGLProgrammableRenderer & renderer) const;
	const ofShader & getShader(int textureTarget, ofGLProgrammableRenderer & renderer) const;

	static ofShader shaderNoTexture;
	static ofShader shaderTexture2D;
	static ofShader shaderTextureRect;
	static bool shadersInitialized;
	static size_t shaderLights;
	static string vertexShader;
	static string fragmentShaderFixed;
};