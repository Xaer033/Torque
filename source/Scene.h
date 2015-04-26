/*
	Module: Scene Object
	Description:	This manages the scene, so adding Gameobjects,
				lights, cameras to it, and updating there components
				also renders them to screen
	Author: Julian Williams
*/

#ifndef __SCENE_H__
#define __SCENE_H__

#include "Vector.h"
#include "Quaternion.h"

#include "Light.h"


namespace GG
{

	class SceneImpl;
	class GameObject;
	class Camera;
	class Shader;


	class Scene
	{
	public:
		Scene( void );
		virtual ~Scene(void);
		
		void setFullScreenShader( Shader & shader );

		GameObject * addGameObject( const char * name, const Vector3 & position, const Quaternion & rotation );
		Camera * addCamera( const char *name );
		Light * addLight( const char * name, LightType lightType, const Vector3 & position );
	
		void setRenderSize( uint width, uint height );
	
		void update();
	
		void render();
	
	
	private:
		SceneImpl * _impl;
	};

}

#endif
