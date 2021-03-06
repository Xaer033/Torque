
uniform mat4 modelViewProj; 

attribute vec4 vPosition;
attribute vec2 vTexcoord;
attribute vec4 vColor;

varying vec2 uv; 
varying vec4 vertColor;

void main() 
{ 
	gl_Position =	modelViewProj * vPosition; 

	uv          = vTexcoord; 
    vertColor   = vColor;
} 