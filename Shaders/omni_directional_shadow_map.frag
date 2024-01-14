#version 330

in vec4 FragPos;

uniform vec3 lightPos;
uniform float farPlane;

void main()
{
	float distance = length(FragPos.xyz - lightPos);  // Distance between fragment and light

	distance = distance / farPlane;  // calculate how far it is from farPlane (between 0-1 range). Normalize by doing this.

	gl_FragDepth = distance;  // Set fragment depth. Override default value
}