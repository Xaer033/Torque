


#include <fstream>
#include <sstream>

#include "s3e.h"
#include "IwDebug.h"
#include "IwGL.h"
#include "s3ePointer.h"

#include "GeometryBuffer3d.h"
#include "Vertex.h"
#include "Vector.h"
#include "Shader.h"
#include "Texture.h"
#include "Matrix.h"
#include "Transform.h"
#include "Camera.h"
#include "ObjFormat.h"
#include "GmbFormat.h"
#include "DebugText.h"
#include "Time.h"
#include "GameObject.h"
#include "RenderState.h"
#include "Scene.h"
#include "MeshRenderer.h"
#include "Material.h"



#include <spine/spine.h>
#include <spine/extension.h>

void _spAtlasPage_createTexture(spAtlasPage* self, const char* path) 
{
	/*CIwTexture *texture = new CIwTexture();
	texture->LoadFromFile(path);
	texture->SetClamping(true);
	texture->SetMipMapping(false);
	texture->SetFiltering(true);
	texture->Upload();

	self->rendererObject = texture;
	self->width = texture->GetWidth();
	self->height = texture->GetHeight();*/
}

void _spAtlasPage_disposeTexture(spAtlasPage* self) 
{/*
	if (self->rendererObject)
		delete static_cast<CIwTexture*>(self->rendererObject);*/
}

char* _spUtil_readFile(const char* path, int* length)
{
	/*s3eFile *f = s3eFileOpen(path, "rb");
	if (f)
	{
		*length = s3eFileGetSize(f);
		char *buffer = (char *)malloc(*length);
		*length = s3eFileRead(buffer, 1, *length, f);
		return buffer;
	}
*/

	return  NULL;
}


std::string loadFile( const char * filename )
{
	std::ifstream in( filename, std::ios::in | std::ios::binary );
	
	std::string contents;
	
	if( in )
	{
		in.seekg( 0, std::ios::end );
		contents.resize( in.tellg() );
		in.seekg( 0, std::ios::beg );
		in.read( &contents[0], contents.size() );
		in.close();
	}

    return contents;
}
	

void makeSuperSquare( GG::GeometryBuffer3d & buffer )
{
	float size = 20;
	buffer.setVertexProperties( GG::TEXCOORDS | GG::NORMALS | GG::TANGENTS | GG::BITANGENTS );

	buffer.pushNormal( Vector3( 0, 0, -1 ) );


	
	buffer.pushTexCoord( Vector2( 0, 1 ) );
	buffer.addPosition( Vector3( -size,  size, 0 ) );

	
	buffer.pushTexCoord( Vector2( 1, 1 ) );
	buffer.addPosition( Vector3( size,  size, 0 ) );

	buffer.pushTexCoord( Vector2( 1, 0 ) );
	buffer.addPosition( Vector3( size, -size, 0 ) );
	
	buffer.addPosition( Vector3( size, -size, 0 ) );
	
	buffer.pushTexCoord( Vector2( 0, 0 ) );
	buffer.addPosition( Vector3( -size, -size, 0 ) );
	
	buffer.pushTexCoord( Vector2( 0, 1 ) );
	buffer.addPosition( Vector3( -size,  size, 0 ) );
	


	buffer.build();
}




//
// Initialize the shader and program object
//
bool initShader( GG::Shader ** shader, const std::string & shaderName )
{

	if( *shader )
		delete *shader;

	*shader = new GG::Shader();


	(*shader)->addDefinition( "#define USE_NORMALMAP \n" );

	std::string directory = "shaders/";

	std::string vShaderStr	= loadFile( (directory + shaderName + std::string(".vs")).c_str() );
	std::string fShaderStr	= loadFile( (directory + shaderName + std::string(".ps")).c_str()  );

	
	(*shader)->bindAttribute( GG::PositionTag,	"vPosition" );
	(*shader)->bindAttribute( GG::TexcoordTag,	"vTexcoord" );
	(*shader)->bindAttribute( GG::NormalTag,	"vNormal"	);
	(*shader)->bindAttribute( GG::TangentTag,	"vTangent"	);
	(*shader)->bindAttribute( GG::BitangentTag,	"vBitangent" );

	(*shader)->compile( vShaderStr.c_str(), fShaderStr.c_str() );


	return true;
}


void handleInput( GG::Transform & model )
{
	//if( s3eKeyboardGetState( s3eKeyLeft ) & S3E_KEY_STATE_DOWN )
		model.rotate( -20.0f * GG::Time::getDeltaTime() , Vector3(0, 1, 0) );
	
	if( s3eKeyboardGetState( s3eKeyRight ) & S3E_KEY_STATE_DOWN )
		model.rotate( -1, Vector3(0, 1, 0) );


	if( s3eKeyboardGetState( s3eKeyA ) & S3E_KEY_STATE_DOWN )
	{
		Vector3 right = model.left() * 3.0f;
		model.translate( right );
	}
	
	if( s3eKeyboardGetState( s3eKeyD ) & S3E_KEY_STATE_DOWN )
	{
		Vector3 right = model.right() * 3.0f;
		model.translate( right );
	}
}


Vector3 mousePosition()
{
	return Vector3( s3ePointerGetX(), s3ePointerGetY(), 0 );
}

// Main entry point for the application
int main()
{
	
	s3eSurfaceSetup( S3E_SURFACE_PIXEL_TYPE_RGB888 );

    //Initialise graphics system(s)
    if( !IwGLInit() )
    {
        IwTrace(GL, ("IwGLInit failed"));
        return 1;
    }

	IwGLSwapBuffers();
    s3eDeviceYield(0);

	
        // Get dimensions from IwGL
    int width	= IwGLGetInt( IW_GL_WIDTH );
    int height	= IwGLGetInt( IW_GL_HEIGHT );

	{

	GG::Scene world;
	world.setRenderSize( width, height );

	GG::RenderState renderState;

	GG::Texture diffuseTex;
	diffuseTex.loadFromFile("textures/bitBox_C.png");

	GG::Texture specularTex;
	specularTex.loadFromFile("textures/bitBox_S.png");

	GG::Texture normalTex;
	normalTex.loadFromFile("textures/bitBox_N.png");


	GG::Shader * lightShader	= NULL;
	GG::Shader * unlitShader	= NULL;
	GG::Shader * screenShader	= NULL;


	if( !initShader( &lightShader, "uberShader" ) )
		return -1;

	

	
	if( !initShader( &screenShader, "fullScreenShader" ) )
		return -1;


	renderState.bindShader( *lightShader );
	lightShader->setParameterSampler( "diffuseTex",		0	);
	lightShader->setParameterSampler( "specularTex",	1	);
	lightShader->setParameterSampler( "normalTex",		2	);

	renderState.bindShader( *screenShader );
	screenShader->setParameterSampler( "mainTex",		0	);



	world.setFullScreenShader( *screenShader );



	GG::Material barrelMat;
	barrelMat.shader		= lightShader;
	barrelMat.usedLayers	= 1;
	barrelMat.textureLayers[0].diffuseName	= &diffuseTex;
	barrelMat.textureLayers[0].specularName	= &specularTex;
	barrelMat.textureLayers[0].normalName	= &normalTex;


	GG::GeometryBuffer3d squareObj;
	makeSuperSquare( squareObj );



	GG::GeometryBuffer3d playerModel;
	playerModel.setVertexProperties( GG::TEXCOORDS | GG::NORMALS | GG::TANGENTS | GG::BITANGENTS );
	//GG::ObjFormat::loadFromFile( "models/player.obj", playerModel );
	GG::GmbFormat::loadFromFile( "models/player.gmb", playerModel );


	GG::GeometryBuffer3d orcBarrelModel;
	orcBarrelModel.setVertexProperties( GG::TEXCOORDS | GG::NORMALS | GG::TANGENTS | GG::BITANGENTS );
	//GG::ObjFormat::loadFromFile( "models/bitBox.obj", orcBarrelModel );
	GG::GmbFormat::loadFromFile( "models/bitBox.gmb", orcBarrelModel, 2.0f );

	
  //  renderState.setClearColor( 0.3f, 0.3f, 0.3f, 1.0f );
	//renderState.setDepthTesting( true );


	
	GG::Camera * miniMapCamera = world.addCamera( "miniMap" );
	miniMapCamera->setDepth( 1 );
	miniMapCamera->setClearMode( GG::CM_DEPTH );
	miniMapCamera->setViewport( 0, 1-0.4f, 0.4f, 0.3f );
	miniMapCamera->setOrthogonal( -20, 20, -18.5f, 18.5f, 0.1f, 100.0f );
	miniMapCamera->transform.setPosition( Vector3(0, 50, 0 ) );
	miniMapCamera->transform.rotate( -90, Vector3(1, 0, 0) );

	GG::Camera * mainCamera = world.addCamera( "main" );
	mainCamera->setDepth( 0 );
	mainCamera->setClearMode(  GG::CM_COLOR | GG::CM_DEPTH  );
	mainCamera->setClearColor( Vector4( 0.6f, 0.5f, 0.273f, 1.0f ) );
	mainCamera->setPerspective( 60.0f , (float)width, (float)height, 0.1f, 100.0f );
	mainCamera->transform.setPosition( Vector3( 0, 15, 15 ) );
	mainCamera->setViewport( 0, 0, 1, 1 );



	GG::GameObject * playerObj	= world.addGameObject( "player_1", Vector3( 0, 0, 0 ), GG::identityQuat );
	GG::MeshRenderer * meshRenderer = (GG::MeshRenderer*)playerObj->addComponent("meshRenderer", GG::MESH_RENDERER );
	meshRenderer->setMesh( orcBarrelModel );
	meshRenderer->setMaterial( barrelMat );
	//playerObj->transform.scale( Vector3( 2, 2, 2 ) );


	GG::GameObject* floorObj	= world.addGameObject( "floor", Vector3( 0, 0, 0 ), GG::identityQuat );
	meshRenderer = (GG::MeshRenderer*)floorObj->addComponent("meshRenderer", GG::MESH_RENDERER );
	meshRenderer->setMesh( squareObj );
	meshRenderer->setMaterial( barrelMat );
	floorObj->transform.rotate( 90, Vector3( 1, 0, 0 ) );


	GG::GameObject * p2Obj	= world.addGameObject( "player_2", Vector3( 10, 0, 0 ), GG::identityQuat );
	meshRenderer = (GG::MeshRenderer*)p2Obj->addComponent("meshRenderer", GG::MESH_RENDERER );
	meshRenderer->setMesh( playerModel );
	meshRenderer->setMaterial( barrelMat );

	p2Obj->transform.setParent( &playerObj->transform );


	GG::DirectionLight * dLight	= (GG::DirectionLight *)world.addLight("dLight", GG::LT_DIRECTIONAL,  Vector3( 1, 1, 5 ) );
	//dLight->radius = 6;
	dLight->color = Vector4( 1, 0.7, 0.5, 1 );
	dLight->intensity = 1.0f;

	GG::PointLight * pLight = (GG::PointLight *)world.addLight("pLight", GG::LT_POINT,  Vector3( 5, 3, -5 ) );
	pLight->radius = 3.0;
	pLight->color = Vector4(0.35, 1, 0.44, 1 );
	pLight->intensity = 3.0f;

	float timer = 0;

	GG::DebugText debugText;
	
	Vector3 oldPos	= mousePosition();

	bool isMoving	= false;
	bool isZooming	= false;

	GG::Transform cameraRotObj;
	cameraRotObj.setPosition( playerObj->transform.getPosition() );
	
	mainCamera->transform.setParent( &cameraRotObj );


    // Loop forever, until the user or the OS performs some action to quit the app
    while( !s3eDeviceCheckQuitRequest() )
    {
        //Update the input systems
        s3eKeyboardUpdate();
        s3ePointerUpdate();


		GG::Time::update();


		float deltaTime = GG::Time::getDeltaTime();
		
		Vector3 newPos	= mousePosition();

		if( s3ePointerGetState( S3E_POINTER_BUTTON_SELECT ) == S3E_POINTER_STATE_DOWN && !isMoving )
		{
			oldPos = mousePosition();
			newPos = oldPos;

			isMoving = true;
		}
		else if( s3ePointerGetState( S3E_POINTER_BUTTON_SELECT ) == S3E_POINTER_STATE_UP )
		{
			isMoving = false;
		}
		

		if( s3ePointerGetState( S3E_POINTER_BUTTON_RIGHTMOUSE ) == S3E_POINTER_STATE_DOWN && !isZooming )
		{
			oldPos = mousePosition();
			newPos = oldPos;

			isZooming = true;
		}
		else if( s3ePointerGetState( S3E_POINTER_BUTTON_RIGHTMOUSE ) == S3E_POINTER_STATE_UP )
		{
			isZooming = false;
		}



		if( isMoving )
		{
			Vector3 delta	= (oldPos - newPos) * 10.0f;
			delta.y *= -1;
		//delta			= (delta.length() < 3.0001f ) ? delta : Vector3();
			oldPos			= newPos;
			cameraRotObj.rotate( delta.x * deltaTime, Vector3( 0, 1, 0 ) );

			mainCamera->transform.translate( Vector3( 0, delta.y * 0.4f * deltaTime, 0 ), GG::WORLD );
		}

		if( isZooming )
		{
			Vector3 delta	= (oldPos - newPos) * 10.0f;

			oldPos			= newPos;
			mainCamera->transform.translate( Vector3( 0, 0, delta.y * deltaTime ), GG::LOCAL );
		}

		if( s3eKeyboardGetState( s3eKeyR ) & S3E_KEY_STATE_PRESSED )
		{
			initShader( &lightShader, "uberShader" );

		}

		handleInput( playerObj->transform );

		timer += deltaTime;


		world.update();


		p2Obj->transform.rotate( 100.0f * deltaTime, Vector3( 0, 1, 0 ) );
		p2Obj->transform.translate( Vector3( sinf( timer ) * 5.0f, 0, 0 ) * deltaTime, GG::WORLD );

		pLight->position = Vector4( p2Obj->transform.getPosition( GG::WORLD ) + Vector3(0, 3, 0), 1 );


		mainCamera->transform.lookAt( playerObj->transform.getPosition() + Vector3( 0, 5, 0 ) );
		

		//Vector3 camPos	= miniMapCamera->transform.getPosition();
		//camPos.x		= playerObj->transform.getPosition().x;
		//camPos.z		= playerObj->transform.getPosition().z;
		//miniMapCamera->transform.setPosition( camPos );

        
		world.render();

		s3eDebugPrintf( 10, 40, true, "DeltaTime: %f", deltaTime );
		
        // Call IwGL swap instead of egl directly
        IwGLSwapBuffers();

        // Sleep for 0ms to allow the OS to process events etc.
        s3eDeviceYield(0);
	}
	}
    //Terminate modules being used
    IwGLTerminate();
    
    // Return
    return 0;
}
