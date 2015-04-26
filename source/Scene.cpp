/*
	Module: SceneRenderer
	Description: Render the visible gameObjects in the scene
	Author: Julian Williams
*/

#include "Scene.h"

#include <iostream>
#include <vector>
#include <hash_map>
#include <sstream>
#include <algorithm>

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <IwTypes.h>

#include "GameObject.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "RenderState.h"
#include "Time.h"
#include "FrameBufferObject.h"
#include "GeometryBuffer3d.h"
#include "RenderFactory.h"

namespace GG
{
	typedef std::hash_map< uint, GameObject *> GameObjectMap;
	typedef std::vector< Camera *>			CameraList;
	typedef std::vector< Light * >			LightList;
	typedef std::vector< MeshRenderer * >	MeshRenderList;

	class SceneImpl
	{
	public:

		RenderState		_renderState;

		GameObjectMap	_gameObjectMap;
		CameraList		_cameraList;
		LightList		_lightList;

		MeshRenderList	_meshRendererList;
		
		std::stringstream _nameCache;

		uint _gameObjectIndex;
		uint _cameraNameIndex;
		uint _lightIndex;

		uint _renderWidth;
		uint _renderHeight;

		uint _deviceWidth;
		uint _deviceHeight;


		FrameBufferObject	_frameBufferObject;
		GeometryBuffer3d	_fullScreenQuad;
		Shader *			_fullScreenShader;

	public:

		SceneImpl()
		{
			_gameObjectIndex	= 0;
			_cameraNameIndex	= 0;
			_lightIndex			= 0;

			_deviceWidth	= _renderWidth	= IwGLGetInt( IW_GL_WIDTH );
			_deviceHeight	= _renderHeight	= IwGLGetInt( IW_GL_HEIGHT );

			_fullScreenShader	= NULL;

			//glEnable( GL_CULL_FACE );
			//glCullFace( GL_BACK );

			_makeFullScreenQuad();

			_frameBufferObject.build( _renderWidth, _renderHeight );
		}

		~SceneImpl()
		{
		}


		void setFullScreenShader( Shader & shader )
		{
			_fullScreenShader = &shader;
		}

		GameObject * addGameObject( const char * name, const Vector3 & position, const glm::quat & rotation )
		{
			if( name == NULL )
			{
				
				_nameCache.str("gameObject_");
				_nameCache << _gameObjectIndex;

				name = _nameCache.str().c_str();
			}

			GameObject * newObject = new GameObject( _gameObjectIndex, name );
			newObject->transform.setPosition( position );

			_gameObjectMap[ _gameObjectIndex ] = newObject;
			
			_gameObjectIndex++;

			return newObject;
		}

		Camera * addCamera( const char * name )
		{
			if( name == NULL )
			{
				_nameCache.str("camera_");
				_nameCache << _cameraNameIndex;

				name = _nameCache.str().c_str();
			}

			_cameraList.push_back( new Camera( name ) );
			_cameraNameIndex++;

			return _cameraList.back();
		}


		Light * addLight( const char * name, LightType lightType, const Vector3 & position )
		{
			if( name == NULL )
			{
				_nameCache.str("light_");
				_nameCache << _lightIndex;

				name = _nameCache.str().c_str();
			}

			Light * light = NULL;

			switch( lightType )
			{
			case LT_POINT:
			{
				light = new PointLight();
				PointLight * p = (PointLight *)light;
				p->position = Vector4( position, 1 );
				break;
			}
			case LT_DIRECTIONAL:
			{
				light = new DirectionLight();
				DirectionLight * d = (DirectionLight *)light;
				d->position = Vector4( position, 0 );
				break;
			}
			case LT_SPOT:
				light = new SpotLight();
				break;
			}

			light->Id	= _lightIndex;
			light->type	= lightType;

			_lightList.push_back( light );

			_lightIndex++;

			return light;
		}



		void update()
		{
			GameObjectMap::iterator it = _gameObjectMap.begin();
			while( it != _gameObjectMap.end() )
			{
				if( it->second->doRemove() )
				{
					_gameObjectMap.erase( it );
				}
				else
				{
					it->second->update( GG::Time::getDeltaTime() );
					++it;
				}
			}
			
		}


		void setRenderSize( int width, int height )
		{
			_renderWidth	= width;
			_renderHeight	= height;

			//_renderState.setRenderSize( width, height );
			_frameBufferObject.build( width, height );
		}


		void render()
		{
			if( _fullScreenShader == NULL )
				return;

			_renderState.bindFrameBuffer( _frameBufferObject );
			
			_renderState.setRenderSize( _renderWidth, _renderHeight );

			_renderState.setDepthTesting( true );

			
			_renderState.setCullMode( CULL_BACK );

			_sortCameras();


			//Draw each camera's view
			for( uint i = 0; i < _cameraList.size(); ++i )
			{
				Camera * c = _cameraList[i];

				Vector4 vp = c->getViewport();
				
				
				_renderState.setViewport( vp.x, vp.y, vp.w, vp.z );

				int clearMode = c->getClearMode();


				if( clearMode & CM_COLOR )
					_renderState.setClearColor( c->getClearColor() );

				_renderState.clear( c->getClearMode() );

				///*TODO* Get all visable gameObjects from this camera's
				RenderFactory::RenderMap renderMap = RenderFactory::getInstance().getRenderMap();
				RenderFactory::RenderMap::iterator it = renderMap.begin();

				for(; it != renderMap.end(); ++it )
				{
					MeshRenderer *	mesh		= it->second;
					Material	&	material	= mesh->getMaterial();
					Shader		&	shader		= *material.shader;
					Transform	&	transform	= _gameObjectMap[ it->first ]->transform;

					for( uint o = 0; o < material.usedLayers; ++o)
					{
						_renderState.bindTexture2d( 0, *material.textureLayers[o].diffuseName	);
						_renderState.bindTexture2d( 1, *material.textureLayers[o].specularName	);
						_renderState.bindTexture2d( 2, *material.textureLayers[o].normalName	);
					}

					_renderState.bindShader( shader );

					Matrix4 modelView = c->getViewMatrix() * transform.getMatrix();

					uint lightCount = ( _lightList.size() > 4 ) ? 4 : _lightList.size();
					

					for( uint k = 0; k < 4; ++k )
					{
						Matrix4 & viewMat = c->getViewMatrix();

						if( k >= lightCount )
						{
							_bindLightToShader( viewMat, shader, NULL, k );
							continue;
						}

						if( _lightList[ k ] != NULL )
						{
							_bindLightToShader( viewMat, shader, _lightList[ k ], k );

						}
					}


					Matrix4 modelViewProj = c->getProjectionMatrix() * modelView;
					
					Vector4 camPos = c->getViewMatrix() * Vector4( c->transform.getPosition( GG::WORLD ), 1 );
					shader.setParameter( "cameraPos", camPos );
					shader.setParameter( "modelViewProj", modelViewProj );
					shader.setParameter( "modelViewMat",  modelView );
		
					_renderState.bindGeometry3d( mesh->getMesh() );

					_renderState.renderBoundGeometry( GG::TRIANGLE_LIST );
				}
			}


			_renderState.bindFrameBuffer( 0 );

			_renderState.setRenderSize( _deviceWidth, _deviceHeight );
			_renderState.setViewport( 0, 0, 1, 1 );
			//glViewport( 0, 0, _deviceWidth, _deviceHeight );
			
			_renderState.setCullMode( CULL_NONE );

			_renderState.setDepthTesting( false );

			_renderState.bindShader( *_fullScreenShader );
			_renderState.bindTexture2d( 0, _frameBufferObject.getColorTexture() );
			_renderState.bindGeometry3d( _fullScreenQuad );

			_renderState.renderBoundGeometry( GG::TRIANGLE_LIST );

		}


	private:

		void _makeFullScreenQuad( )
		{
			_fullScreenQuad.setVertexProperties( GG::TEXCOORDS );

			_fullScreenQuad.pushTexCoord(	Vector2( 0, 0 )			);
			_fullScreenQuad.addPosition(	Vector3( -1,  1, 0 )	);
			
			_fullScreenQuad.pushTexCoord(	Vector2( 1, 0 )			);
			_fullScreenQuad.addPosition(	Vector3( 1,  1, 0 )		);

			_fullScreenQuad.pushTexCoord(	Vector2( 1, 1 )			);
			_fullScreenQuad.addPosition(	Vector3( 1, -1, 0 )		);
			
			_fullScreenQuad.addPosition(	Vector3( 1, -1, 0 )		);
			
			_fullScreenQuad.pushTexCoord(	Vector2( 0, 1 )			);
			_fullScreenQuad.addPosition(	Vector3( -1, -1, 0 )	);
			
			_fullScreenQuad.pushTexCoord(	Vector2( 0, 0 )			);
			_fullScreenQuad.addPosition(	Vector3( -1,  1, 0 )	);
			

			_fullScreenQuad.build();
		}



		void _bindLightToShader( Matrix4 & view, Shader & shader, Light * light, uint lightIndex )
		{

			if( light == NULL )
			{
				Vector4 zero = Vector4( 0 );

				_nameCache.str("");
				_nameCache << "lights[" << lightIndex << "].tintColor";
				shader.setParameter( _nameCache.str().c_str(), zero );

				
				_nameCache.str("");
				_nameCache << "lights[" << lightIndex << "].position";
				shader.setParameter( _nameCache.str().c_str(), zero );
				return;
			}

			_nameCache.str("");
			_nameCache << "lights[" << lightIndex << "].tintColor";
			shader.setParameter( _nameCache.str().c_str(), light->color );
			
			_nameCache.str("");
			_nameCache << "lights[" << lightIndex << "].intensity";
			shader.setParameter( _nameCache.str().c_str(), light->intensity );


			Vector4 position = view * light->position;

			_nameCache.str("");
			_nameCache << "lights[" << lightIndex << "].position";
			shader.setParameter( _nameCache.str().c_str(), position );


			if( light->type == LT_POINT )
			{
				PointLight * pointLight = (PointLight *)light;

				
				_nameCache.str("");
				_nameCache << "lights[" << lightIndex << "].radius";
				shader.setParameter(_nameCache.str().c_str(), (float)pointLight->radius );
			}
			else if ( light->type == LT_DIRECTIONAL )
			{
			}
		}

		void _sortCameras()
		{


			std::sort( _cameraList.begin(), _cameraList.end(), Camera::depthCompare );
		}
	};




//************************* Interface *******************************//

	Scene::Scene( )
	{
		_impl = new SceneImpl();
	}


	Scene::~Scene( )
	{
		delete _impl;
	}
	
	void Scene::setFullScreenShader( Shader & shader )
	{
		_impl->setFullScreenShader( shader );
	}

	Camera * Scene::addCamera( const char * name )
	{
		return _impl->addCamera( name );
	}


	GameObject * Scene::addGameObject( const char * name, const Vector3 & position, const Quaternion & rotation )
	{
		return _impl->addGameObject( name, position, rotation );
	}


	Light * Scene::addLight( const char * name, LightType lightType, const Vector3 & position )
	{
		return _impl->addLight( name, lightType, position );
	}
	
	
	void Scene::setRenderSize( uint width, uint height )
	{
		_impl->setRenderSize( width, height );
	}

	void Scene::update()
	{
		_impl->update();
	}
	
	void Scene::render()
	{
		_impl->render();
	}
}
