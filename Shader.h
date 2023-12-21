#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include <GL\glew.h>

#include "CommonValues.h"

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"


class Shader
{
	public:
		Shader();

		void CreateFromString(const char* vertexCode, const char* fragmentCode);
		void CreateFromFiles(const char* vertexLocation, const char* fragmentLocation);

		std::string ReadFile(const char* fileLocation);

		GLuint GetProjectionLocation() const;
		GLuint GetModelLocation() const;
		GLuint GetViewLocation() const;
		GLuint GetAmbientIntensityLocation() const;
		GLuint GetAmbientColourLocation() const;
		GLuint GetDiffuseIntensityLocation() const;
		GLuint GetDirectionLocation() const;
		GLuint GetSpecularIntensityLocation() const;
		GLuint GetShininessLocation() const;
		GLuint GetEyePositionLocation() const;

		void SetDirectionalLight(DirectionalLight * dLight) const;
		void SetPointLights(PointLight * pLight, unsigned int lightCount);
		void SetSpotLights(SpotLight* sLight, unsigned int lightCount);

		void UseShader();
		void ClearShader();

		~Shader();

	private:

		int pointLightCount; // We will have multiple point lights
		int spotLightCount;

		GLuint shaderID, uniformProjection, uniformModel, uniformView, uniformEyePosition;
		GLuint uniformSpecularIntensity, uniformShininess;

		struct {
			GLuint uniformColour;
			GLuint uniformAmbientIntensity;
			GLuint uniformDiffuseIntensity;

			GLuint uniformDirection;
		} uniformDirectionalLight;

		GLuint uniformPointLightCount;

		struct {
			GLuint uniformColour;
			GLuint uniformAmbientIntensity;
			GLuint uniformDiffuseIntensity;

			GLuint uniformPosition;
			GLuint uniformConstant;
			GLuint uniformLinear;
			GLuint uniformExponent;
		} uniformPointLight[MAX_POINT_LIGHTS];

		void CompileShader(const char* vertexCode, const char* fragmentCode);
		void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);

		GLuint uniformSpotLightCount;

		struct {
			GLuint uniformColour;
			GLuint uniformAmbientIntensity;
			GLuint uniformDiffuseIntensity;

			GLuint uniformPosition;
			GLuint uniformConstant;
			GLuint uniformLinear;
			GLuint uniformExponent;

			GLuint uniformDirection;
			GLuint uniformEdge;

		} uniformSpotLight[MAX_SPOT_LIGHTS];
};

