#include <IwGL.h>
#include <IwUtil.h>
#include <IwRandom.h>
#include <s3e.h>
#include <algorithm>

#include <s3ePointer.h>
#include <stdio.h>

#include <fstream>
#include <sstream>
#include <string>

#include "Texture.h"
#include "Material.h"
#include "GeometryBuffer3d.h"
#include "StreamVertexBuffer.h"
#include "RenderState.h"
#include "Vector.h"
#include "Shader.h"
#include "Camera.h"




#include <spine/spine.h>
#include <spine/extension.h>


const int kMaxCharacters = 800;

void _spAtlasPage_createTexture (spAtlasPage* self, const char* path) 
{
    GG::Texture * texture = new GG::Texture();
    texture->loadFromFile( path );

    self->rendererObject = (void*)texture;
    self->width     = texture->getWidth();
    self->height    = texture->getHeight();
}

void _spAtlasPage_disposeTexture (spAtlasPage* self)
{
	if (self->rendererObject)
	{ 
		delete( static_cast< GG::Texture * >( self->rendererObject ) );
	}
}

char* _spUtil_readFile( const char* path, int* length )
{
	s3eFile *f = s3eFileOpen( path, "rb" );
	if( f )
	{
		*length = s3eFileGetSize( f );
		char *	buffer = (char *)malloc( *length );
		*length = s3eFileRead( buffer, 1, *length, f );
		return buffer;
	}

  return  NULL;
}


spAtlas* atlas = NULL;
spSkeletonData* skeletonData = NULL;
spSkeleton* skeleton = NULL;
spAnimationState* state = NULL;
spAnimationStateData* stateData = NULL;
spSkeletonBounds* bounds = NULL;
spSlot* headSlot = NULL;        // spineboy's head
uint64 lastFrameTime = 0;


GG::RenderState renderState;


GG::Camera camera;
GG::IVertexBuffer * geometry = NULL;
float * worldVertex;

Vector4 funColor;
GG::Shader * unlitShader = NULL;
Vector3 startPosition;

std::vector< GG::Transform > transformList( kMaxCharacters );

void SpineBoy()
{
    // Load atlas, skeleton, and animations.
    atlas = spAtlas_createFromFile("spine/spineboy.atlas", 0);
    //IwTrace(SPINE, ("First region name: %s, x: %d, y: %d\n", atlas->regions->name, atlas->regions->x, atlas->regions->y));
   // IwTrace(SPINE, ("First page name: %s, size: %d, %d\n", atlas->pages->name, atlas->pages->width, atlas->pages->height));

    spSkeletonJson* json = spSkeletonJson_create(atlas);
    json->scale = 0.4f;
    skeletonData = spSkeletonJson_readSkeletonDataFile(json, "spine/spineboy.json");
    if (!skeletonData) IwAssertMsg(SPINE, false, ("Error: %s\n", json->error));
    spSkeletonJson_dispose(json);
    IwTrace(SPINE, ("Default skin name: %s\n", skeletonData->defaultSkin->name));

    bounds = spSkeletonBounds_create();

    // Configure mixing.
    stateData = spAnimationStateData_create(skeletonData);
    spAnimationStateData_setMixByName(stateData, "walk", "jump", 0.2f);
    spAnimationStateData_setMixByName(stateData, "jump", "run", 0.2f);

    skeleton = spSkeleton_create(skeletonData);
    state = spAnimationState_create(stateData);

    skeleton->flipX = false;
    skeleton->flipY = false;
    spSkeleton_setToSetupPose(skeleton);

    skeleton->x = 150;
    skeleton->y = 400;
    spSkeleton_updateWorldTransform(skeleton);

    headSlot = spSkeleton_findSlot(skeleton, "head");

    spAnimationState_setAnimationByName(state, 0, "walk", true);
    spAnimationState_addAnimationByName(state, 0, "jump", false, 3);
    spAnimationState_addAnimationByName(state, 0, "run", true, 0);
}

void Goblins () {
    // Load atlas, skeleton, and animations.
    atlas = spAtlas_createFromFile("spine/goblins-ffd.atlas", 0);
    IwTrace(SPINE, ("First region name: %s, x: %d, y: %d\n", atlas->regions->name, atlas->regions->x, atlas->regions->y));
    IwTrace(SPINE, ("First page name: %s, size: %d, %d\n", atlas->pages->name, atlas->pages->width, atlas->pages->height));

    spSkeletonJson* json = spSkeletonJson_create(atlas);
    json->scale = 0.01f;
    skeletonData = spSkeletonJson_readSkeletonDataFile(json, "spine/goblins-ffd.json");
    if (!skeletonData) IwAssertMsg(SPINE, false, ("Error: %s\n", json->error));
    spAnimation* walkAnimation = spSkeletonData_findAnimation(skeletonData, "walk");
    spSkeletonJson_dispose(json);

    skeleton = spSkeleton_create(skeletonData);
    state = spAnimationState_create(NULL);

    skeleton->flipX = false;
    skeleton->flipY = false;
    spSkeleton_setSkinByName(skeleton, "goblin");
    spSkeleton_setSlotsToSetupPose(skeleton);
    //Skeleton_setAttachment(skeleton, "left hand item", "dagger");

	skeleton->x = 0;
    skeleton->y = 0;
    spSkeleton_updateWorldTransform(skeleton);

    spAnimationState_setAnimation(state, 0, walkAnimation, true);
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
        in.read( &contents[ 0 ], contents.size() );
        in.close();
    }

    return contents;
}

bool initShader( GG::Shader ** shader, const std::string & shaderName )
{
	if( *shader != NULL )
	{
		delete( *shader );
	}

    *shader = new GG::Shader();


   // ( *shader )->addDefinition( "#define USE_NORMALMAP \n" );

    const std::string directory = "shaders/";

    std::string vShaderStr = loadFile( ( directory + shaderName + std::string( ".vs" ) ).c_str() );
    std::string fShaderStr = loadFile( ( directory + shaderName + std::string( ".ps" ) ).c_str() );


    ( *shader )->bindAttribute( GG::PositionTag,    "vPosition"     );
    ( *shader )->bindAttribute( GG::TexcoordTag,    "vTexcoord"     );
    //( *shader )->bindAttribute( GG::NormalTag,      "vNormal"       );
   //( *shader )->bindAttribute( GG::TangentTag,     "vTangent"      );
    //( *shader )->bindAttribute( GG::BitangentTag,   "vBitangent"    );
    ( *shader )->bindAttribute( GG::ColorTag,       "vColor"        );

    ( *shader )->compile( vShaderStr.c_str(), fShaderStr.c_str() );

    return true;
}

bool sortTransform( const GG::Transform & a, const GG::Transform & b )
{
	float	d1 = glm::distance( a.getPosition( GG::WORLD ), camera.transform.getPosition( GG::WORLD ) );
	float	d2 = glm::distance( b.getPosition( GG::WORLD ), camera.transform.getPosition( GG::WORLD ) );

	return d1 > d2;
}

void setupTransforms()
{
	float xPos = -20.0f;
	float zPos = 0;

	for( int i = 0; i < kMaxCharacters; ++i )
	{
		GG::Transform t;

		if( i % 100 == 0 )
		{
			xPos += 5.0f;
			zPos = 0;
		}

		zPos -= 1.0f * 1.0f;

		Vector3 position = Vector3( xPos, 0, zPos );
		t.setPosition( position );
		transformList.push_back( t );
	}

	std::sort( transformList.begin(), transformList.end(), sortTransform );
	
}

void init() 
{
    s3eSurfaceSetup( S3E_SURFACE_PIXEL_TYPE_RGB888 );

    //Initialise graphics system(s)
    if( !IwGLInit() )
    {
        IwTrace( GL, ( "IwGLInit failed" ) );
        return;
    }

    IwGLSwapBuffers();
    s3eDeviceYield( 0 );

    // Get dimensions from IwGL
    int width   = IwGLGetInt( IW_GL_WIDTH );
    int height  = IwGLGetInt( IW_GL_HEIGHT );
    
    camera.setPerspective( 60, ( float )width, ( float )height, 0.01f, 1000 );

    camera.setViewport( 0, 0, 1, 1 );
    camera.setClearColor( Vector4( 0.3f, 0.6f, 0.4f, 1.0f ) );
    camera.setClearMode( GG::CM_COLOR | GG::CM_DEPTH );

	startPosition = Vector3( 0, 4, 15 );
    camera.transform.setPosition( startPosition );
    camera.transform.lookAt( Vector3( 0 ) );

    geometry	= new GG::StreamVertexBuffer();
    worldVertex = new float[ 1000 ];

     
    initShader( &unlitShader, "unlit" );
   
	setupTransforms();

 // SpineBoy();
  Goblins();

  lastFrameTime = s3eTimerGetMs();
}

void release()
{
  if (bounds)		spSkeletonBounds_dispose(bounds);
  if (stateData)	spAnimationStateData_dispose(stateData);

  spAnimationState_dispose(state);
  spSkeleton_dispose(skeleton);
  spSkeletonData_dispose(skeletonData);
  spAtlas_dispose(atlas);

  if( geometry )	delete( geometry );

  if( worldVertex ) delete[]( worldVertex );

}

bool update() 
{
  float dt = (float)(s3eTimerGetMs() - lastFrameTime) / 1000.f;
  lastFrameTime = s3eTimerGetMs();

  if (bounds)
  {
      spSkeletonBounds_update(bounds, skeleton, true);

      if (s3ePointerGetState(S3E_POINTER_BUTTON_SELECT) & S3E_POINTER_STATE_PRESSED)
      {

      }

      int pointerx = s3ePointerGetX();
      int pointery = s3ePointerGetY();

      IwTrace(SPINE, ("pointer: %d, %d", pointerx, pointery));
      if (spSkeletonBounds_containsPoint(bounds, (float)pointerx, (float)pointery))
      {
          IwAssert(SPINE, headSlot);
          headSlot->g = 0;
          headSlot->b = 0;
      }
      else
      {
          headSlot->g = 1.0f;
          headSlot->b = 1.0f;
      }
  }

  spSkeleton_update(skeleton, dt);
  spAnimationState_update(state, dt/* * timeScale*/);
  spAnimationState_apply(state, skeleton);
  spSkeleton_updateWorldTransform(skeleton);

  return true;
}


void makeSuperSquare( GG::GeometryBuffer3d & buffer )
{
    float size = 20;
    buffer.setVertexProperties( GG::TEXCOORDS | GG::NORMALS | GG::TANGENTS | GG::BITANGENTS );

    buffer.pushNormal( Vector3( 0, 0, -1 ) );



    buffer.pushTexCoord( Vector2( 0, 1 ) );
    buffer.addPosition( Vector3( -size, size, 0 ) );


    buffer.pushTexCoord( Vector2( 1, 1 ) );
    buffer.addPosition( Vector3( size, size, 0 ) );

    buffer.pushTexCoord( Vector2( 1, 0 ) );
    buffer.addPosition( Vector3( size, -size, 0 ) );

    buffer.addPosition( Vector3( size, -size, 0 ) );

    buffer.pushTexCoord( Vector2( 0, 0 ) );
    buffer.addPosition( Vector3( -size, -size, 0 ) );

    buffer.pushTexCoord( Vector2( 0, 1 ) );
    buffer.addPosition( Vector3( -size, size, 0 ) );



    buffer.build();
}


void draw( GG::Transform transform ) 
{
  //  GG::Texture * lastTexture = NULL;
   // GG::BlendMode lastBlend   = GG::BlendMode::BM_ALPHA;

    
    for (int i = 0; i < skeleton->slotsCount; ++i)
    {

      spSlot* slot = skeleton->slots[i];
      spAttachment* attachment = slot->attachment;
      if (!attachment) continue;

      GG::Texture * texture = NULL;
	  GG::BlendMode blend = ( slot->data->blendMode == SP_BLEND_MODE_ADDITIVE ) ? 
                                GG::BM_ADDITIVE : GG::BM_ALPHA;


      renderState.setBlendmode( blend );

    int num_vertices = 0;

  
 //   IwGxPrimType primType = IW_GX_QUAD_LIST;

  //  CIwFVec2 *uvs = NULL;
    //CIwSVec2 *verts = NULL;
    //CIwColour *colors = NULL;

    if( attachment->type == SP_ATTACHMENT_REGION )
    {
        num_vertices = 4;
        // uvs = IW_GX_ALLOC(CIwFVec2, num_vertices);
        // verts = IW_GX_ALLOC(CIwSVec2, num_vertices);
        //  colors = IW_GX_ALLOC(CIwColour, num_vertices);

        spRegionAttachment* regionAttachment = ( spRegionAttachment* )attachment;
        texture = static_cast< GG::Texture* >( ( ( spAtlasRegion* )regionAttachment->rendererObject )->page->rendererObject );

        renderState.bindTexture2d( 0, *texture );
        spRegionAttachment_computeWorldVertices( regionAttachment, slot->bone, worldVertex );

        float r = ( skeleton->r * slot->r );
        float g = ( skeleton->g * slot->g );
        float b = ( skeleton->b * slot->b );
        float a = ( skeleton->a * slot->a );

        geometry->pushColor( Vector4( r, g, b, a ) * funColor );

       
        geometry->pushTexCoord( Vector2(regionAttachment->uvs[ 0 ], regionAttachment->uvs[ 1 ] ) );
        geometry->addPosition( transform.transformPoint(  Vector3( worldVertex[ 0 ], worldVertex[ 1 ], 0.0f ) ));

        geometry->pushTexCoord( Vector2( regionAttachment->uvs[ 2 ], regionAttachment->uvs[ 3 ] ) );
        geometry->addPosition( transform.transformPoint(  Vector3( worldVertex[ 2 ], worldVertex[ 3 ], 0.0f ) ));

        geometry->pushTexCoord( Vector2( regionAttachment->uvs[ 4 ], regionAttachment->uvs[ 5 ] ) );
        geometry->addPosition( transform.transformPoint(  Vector3( worldVertex[ 4 ], worldVertex[ 5 ], 0.0f ) ));
       
        geometry->addPosition( transform.transformPoint( Vector3( worldVertex[ 4 ], worldVertex[ 5 ], 0.0f ) ));

        geometry->pushTexCoord( Vector2( regionAttachment->uvs[ 6 ], regionAttachment->uvs[ 7] ) );
        geometry->addPosition( transform.transformPoint(  Vector3( worldVertex[ 6 ], worldVertex[ 7 ], 0.0f ) ));

        geometry->pushTexCoord( Vector2( regionAttachment->uvs[ 0 ], regionAttachment->uvs[ 1 ] ) );
        geometry->addPosition( transform.transformPoint(  Vector3( worldVertex[ 0 ], worldVertex[ 1 ], 0.0f ) ));

        
    }
    else if (attachment->type == SP_ATTACHMENT_MESH)
    {
        spMeshAttachment *mesh = (spMeshAttachment*)attachment;

        num_vertices = mesh->trianglesCount;

        
        texture = static_cast<GG::Texture*>(((spAtlasRegion*)mesh->rendererObject)->page->rendererObject);

        renderState.bindTexture2d( 0, *texture );
        spMeshAttachment_computeWorldVertices( mesh, slot, worldVertex );

        float r = ( skeleton->r * slot->r );
        float g = ( skeleton->g * slot->g );
        float b = ( skeleton->b * slot->b );
        float a = ( skeleton->a * slot->a );

        geometry->pushColor( Vector4( r, g, b, a )  * funColor );

        for (int i = 0; i < mesh->trianglesCount; ++i)
        {
            int index = mesh->triangles[i] << 1;

            geometry->pushTexCoord( Vector2( mesh->uvs[ index ], mesh->uvs[ index + 1 ] ) );
            geometry->addPosition( transform.transformPoint( Vector3( worldVertex[ index ], worldVertex[ index + 1 ], 0.0f ) ) );
        }
    }
    else if (attachment->type == SP_ATTACHMENT_SKINNED_MESH)
    {
        spSkinnedMeshAttachment* mesh = (spSkinnedMeshAttachment*)attachment;
       
        num_vertices = mesh->trianglesCount;

        texture = static_cast<GG::Texture*>(((spAtlasRegion*)mesh->rendererObject)->page->rendererObject);

        spSkinnedMeshAttachment_computeWorldVertices( mesh, slot, worldVertex );

        float r = ( skeleton->r * slot->r );
        float g = ( skeleton->g * slot->g );
        float b = ( skeleton->b * slot->b );
        float a = ( skeleton->a * slot->a );

        geometry->pushColor( Vector4( r, g, b, a )  * funColor );

        for( int i = 0; i < mesh->trianglesCount; ++i )
        {
            int index = mesh->triangles[ i ] << 1;

            geometry->pushTexCoord( Vector2( mesh->uvs[ index ], mesh->uvs[ index + 1 ] ) );
            geometry->addPosition( transform.transformPoint( ( Vector3( worldVertex[ index ], worldVertex[ index + 1 ], 0.0f ) ) ));
        }

    }

    IwAssert(SPINE, texture);
  }
}

S3E_MAIN_DECL void IwMain()
{

  init();

  // Main Game Loop
  while( !s3eDeviceCheckQuitRequest() )
  {
      //Update the input systems
      s3eKeyboardUpdate();
      s3ePointerUpdate();

      GG::Time::update();

      // Prevent back-light to off state
      s3eDeviceBacklightOn();

      // Update the game
      if( !update() )
      {
          break;
      }
      
      if( s3eKeyboardGetState( s3eKeyR ) & S3E_KEY_STATE_RELEASED )
      {
          initShader( &unlitShader, "unlit" );
      }


      

      renderState.setClearColor( camera.getClearColor() );
      renderState.clear( camera.getClearMode() );


      renderState.bindShader( *unlitShader );

      geometry->clearVertices();
      geometry->setVertexProperties( GG::TEXCOORDS | GG::COLORS );

     
      renderState.setDepthTesting( false );
      renderState.setCullMode( GG::CULL_NONE );
      
	  Vector4 white = Vector4( 1, 1, 1, 1 );
      unlitShader->setParameterSampler( "mainTex", 0 );
      unlitShader->setParameter( "tintColor", white );
      unlitShader->setParameter( "modelViewProj", camera.getProjectionMatrix() * camera.getViewMatrix() );
	  
	  static float mover = 0;

	  mover +=  GG::Time::getDeltaTime();

	  Vector3 newPosition = startPosition + Vector3( cosf( mover ), 0, 0 ) * 10.0f;

	  camera.transform.setPosition( newPosition );
	  camera.transform.lookAt( Vector3( 0 ) );

      for( int i = 0; i < kMaxCharacters; ++i )
      {
            funColor = Vector4( fabs( cos( i )), fabs( sin( i )), fabs( cos( -i )), 1.0f );
			draw( transformList[ i ] );
      }
   
      geometry->build( GG::D_DYNAMIC );
      renderState.bindVertexBuffer( *geometry );

    renderState.renderBoundGeometry( GG::TRIANGLE_LIST );
    // Call IwGL swap instead of egl directly
    IwGLSwapBuffers();

    // Sleep for 0ms to allow the OS to process events etc.
    s3eDeviceYield( 0 );
  }

  release();

}
