
uniform mat4 modelViewProj; 

attribute vec4 vPosition;
attribute vec2 vTexcoord;

varying vec2 uv; 

void main() 
{ 
	gl_Position =	modelViewProj * vPosition; 
	uv = vTexcoord; 
} 