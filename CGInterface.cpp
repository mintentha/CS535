
//#define GEOM_SHADER

#include "CGInterface.h"
#include "v3.h"
#include "scene.h"

#include <iostream>

using namespace std;

CGInterface::CGInterface() {};

void CGInterface::PerSessionInit() {

	glEnable(GL_DEPTH_TEST);

	CGprofile latestVertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
#ifdef GEOM_SHADER
	CGprofile latestGeometryProfile = cgGLGetLatestProfile(CG_GL_GEOMETRY);
#endif
	CGprofile latestPixelProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);

#ifdef GEOM_SHADER
	if (latestGeometryProfile == CG_PROFILE_UNKNOWN) {
		cerr << "ERROR: geometry profile is not available" << endl;
		exit(0);
	}

	cgGLSetOptimalOptions(latestGeometryProfile);
	CGerror Error = cgGetError();
	if (Error) {
		cerr << "CG ERROR: " << cgGetErrorString(Error) << endl;
	}

	cout << "Info: Latest GP Profile Supported: " << cgGetProfileString(latestGeometryProfile) << endl;

	geometryCGprofile = latestGeometryProfile;
#endif

	cout << "Info: Latest VP Profile Supported: " << cgGetProfileString(latestVertexProfile) << endl;
	cout << "Info: Latest FP Profile Supported: " << cgGetProfileString(latestPixelProfile) << endl;

	vertexCGprofile = latestVertexProfile;
	pixelCGprofile = latestPixelProfile;
	cgContext = cgCreateContext();


}

bool ShaderOneInterface::PerSessionInit(CGInterface* cgi) {

#ifdef GEOM_SHADER
	geometryProgram = cgCreateProgramFromFile(cgi->cgContext, CG_SOURCE,
		"CG/shaderOne.cg", cgi->geometryCGprofile, "GeometryMain", NULL);
	if (geometryProgram == NULL) {
		CGerror Error = cgGetError();
		cerr << "Shader One Geometry Program COMPILE ERROR: " << cgGetErrorString(Error) << endl;
		cerr << cgGetLastListing(cgi->cgContext) << endl << endl;
		return false;
	}
#endif

	vertexProgram = cgCreateProgramFromFile(cgi->cgContext, CG_SOURCE,
		"CG/shaderOne.cg", cgi->vertexCGprofile, "VertexMain", NULL);
	if (vertexProgram == NULL) {
		CGerror Error = cgGetError();
		cerr << "Shader One Vertex Program COMPILE ERROR: " << cgGetErrorString(Error) << endl;
		cerr << cgGetLastListing(cgi->cgContext) << endl << endl;
		return false;
	}

	fragmentProgram = cgCreateProgramFromFile(cgi->cgContext, CG_SOURCE,
		"CG/shaderOne.cg", cgi->pixelCGprofile, "FragmentMain", NULL);
	if (fragmentProgram == NULL) {
		CGerror Error = cgGetError();
		cerr << "Shader One Fragment Program COMPILE ERROR: " << cgGetErrorString(Error) << endl;
		cerr << cgGetLastListing(cgi->cgContext) << endl << endl;
		return false;
	}

	// load programs
#ifdef GEOM_SHADER
	cgGLLoadProgram(geometryProgram);
#endif
	cgGLLoadProgram(vertexProgram);
	cgGLLoadProgram(fragmentProgram);

	// build some parameters by name such that we can set them later...
	vertexModelViewProj = cgGetNamedParameter(vertexProgram, "modelViewProj");
	vertexObjectCenter = cgGetNamedParameter(vertexProgram, "objectCenter");
	vertexMorphAnimation = cgGetNamedParameter(vertexProgram, "morphAnimation");
	fragmentEye = cgGetNamedParameter(fragmentProgram, "eye");
	shouldTexture = cgGetNamedParameter(fragmentProgram, "shouldTexture");
	texture = cgGetNamedParameter(fragmentProgram, "texture");
#ifdef GEOM_SHADER
	geometryModelViewProj = cgGetNamedParameter(geometryProgram, "modelViewProj");
#endif
	return true;

}

void ShaderOneInterface::PerFrameInit() {

	// set intrinsics
	scene->ppc->SetIntrinsicsHW();
	// set extrinsics
	scene->ppc->SetExtrinsicsHW();
	//set parameters

	cgGLSetStateMatrixParameter(vertexModelViewProj,
		CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);

#ifdef GEOM_SHADER
	cgGLSetStateMatrixParameter(
		geometryModelViewProj,
		CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
#endif

	V3 eye = scene->ppc->C;
	eye = V3(1.0f, 1.0f, 0.0f);
	cgSetParameter3fv(fragmentEye, (const float*)&eye);
	cgSetParameter1f(shouldTexture, textureID != 0);
	cgGLSetTextureParameter(texture, textureID);

	cgSetParameter3fv(vertexObjectCenter, (const float*)&scene->tms[0].GetCenter());
	cgSetParameter1f(vertexMorphAnimation, scene->morphAnimation);

	float radius = 20.0f;
	//  cgSetParameter1f(vertexMorphRadius, radius);
	V3 center = scene->tms[0].GetCenter();
	//  cgSetParameter3fv(vertexMorphCenter, (const float*)&center);
	//cgSetParameter3fv(fragmentEye, (float*)&(scene->ppc->C));

	BindPrograms();

}

void ShaderOneInterface::BindPrograms() {

#ifdef GEOM_SHADER
	cgGLBindProgram(geometryProgram);
#endif
	cgGLBindProgram(vertexProgram);
	cgGLBindProgram(fragmentProgram);

}

void CGInterface::DisableProfiles() {

	cgGLDisableProfile(vertexCGprofile);
#ifdef GEOM_SHADER
	cgGLDisableProfile(geometryCGprofile);
#endif
	cgGLDisableProfile(pixelCGprofile);

}

void CGInterface::EnableProfiles() {

	cgGLEnableProfile(vertexCGprofile);
#ifdef GEOM_SHADER
	cgGLEnableProfile(geometryCGprofile);
#endif
	cgGLEnableProfile(pixelCGprofile);

}

void ShaderOneInterface::bindTexture(unsigned int textureID) {
	//std::cout << textureID << std::endl;
	this->textureID = textureID;
	cgSetParameter1f(shouldTexture, textureID != 0);
	cgGLSetTextureParameter(texture, textureID);
	//std::cout << this->textureID << std::endl;
}

void ShaderOneInterface::unbindTexture() {
	textureID = 0;
	cgSetParameter1f(shouldTexture, textureID != 0);
	cgGLSetTextureParameter(texture, textureID);
}
