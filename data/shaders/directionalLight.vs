
uniform mat4 modelViewProj; 
uniform mat4 modelViewMat;


attribute vec4 vPosition;
attribute vec2 vTexcoord;
attribute vec3 vNormal;

varying vec2 uv; 
varying vec4 normal;
varying vec4 vPos;

void main() 
{ 
	gl_Position =	modelViewProj * vPosition; 
	normal		=	modelViewMat * vec4( vNormal, 0 );
	
	vPos	=	modelViewMat * vPosition;
	uv		=	vTexcoord; 
} 