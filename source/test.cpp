#include <IwGL.h>
#include <IwUtil.h>
#include <s3e.h>

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

void _spAtlasPage_createTexture (spAtlasPage* self, const char* path) 
{
    GG::Texture *texture = new GG::Texture();
    texture->loadFromFile( path );

    self->rendererObject = (void*)texture;
    self->width     = texture->getWidth();
    self->height    = texture->getHeight();
}

void _spAtlasPage_disposeTexture (spAtlasPage* self)
{
  if (self->rendererObject)
    delete( static_cast< GG::Texture * >( self->rendererObject ) );

}

char* _spUtil_readFile( const char* path, int* length )
{
  s3eFile *f = s3eFileOpen(path, "rb");
  if (f)
  {
    *length = s3eFileGetSize(f);
    char *buffer = (char *)malloc(*length);
    *length = s3eFileRead(buffer, 1, *length, f);
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

GG::Camera camera;
GG::IVertexBuffer * geometry = NULL;

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
    json->scale = 1.0f;
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

    skeleton->x = 150;
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

    if( *shader )
        delete *shader;

    *shader = new GG::Shader();


    ( *shader )->addDefinition( "#define USE_NORMALMAP \n" );

    std::string directory = "shaders/";

    std::string vShaderStr = loadFile( ( directory + shaderName + std::string( ".vs" ) ).c_str() );
    std::string fShaderStr = loadFile( ( directory + shaderName + std::string( ".ps" ) ).c_str() );


    ( *shader )->bindAttribute( GG::PositionTag,    "vPosition"     );
    ( *shader )->bindAttribute( GG::TexcoordTag,    "vTexcoord"     );
    ( *shader )->bindAttribute( GG::NormalTag,      "vNormal"       );
    ( *shader )->bindAttribute( GG::TangentTag,     "vTangent"      );
    ( *shader )->bindAttribute( GG::BitangentTag,   "vBitangent"    );
    ( *shader )->bindAttribute( GG::ColorTag,       "vColor"        );

    ( *shader )->compile( vShaderStr.c_str(), fShaderStr.c_str() );


    return true;
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
    
    camera.setPerspective( 45, ( float )width, ( float )height, 0.01f, 1000 );

    camera.setViewport( 0, 0, 1, 1 );
    camera.setClearColor( Vector4( 0.3f, 0.6f, 0.4f, 1.0f ) );
    camera.setClearMode( GG::CM_COLOR | GG::CM_DEPTH );
    camera.transform.setPosition( Vector3( 0, 0, 500) );
    camera.transform.lookAt( Vector3( 0 ) );

    geometry = new GG::StreamVertexBuffer();


 // SpineBoy();
  Goblins();

  lastFrameTime = s3eTimerGetMs();
}

void release()
{
  if (bounds) spSkeletonBounds_dispose(bounds);
  if (stateData) spAnimationStateData_dispose(stateData);
  spAnimationState_dispose(state);
  spSkeleton_dispose(skeleton);
  spSkeletonData_dispose(skeletonData);
  spAtlas_dispose(atlas);

  if( geometry )
      delete( geometry );

  //IwGxTerminate();
}

bool update() {
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
//  IwGxSetColClear(0, 0, 0, 0);
 /// Clear the screen
  //IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);
  // Draw screen space in the back so 3D sprites are visible
  //IwGxSetScreenSpaceSlot(-1);
    GG::RenderState renderState;

    renderState.setClearColor( camera.getClearColor() );
    renderState.clear( camera.getClearMode() );

  GG::Texture * lastTexture = NULL;
  GG::BlendMode lastBlend   = GG::BlendMode::BM_ALPHA;

  geometry->clearVertices();
  geometry->setVertexProperties( GG::VertexProperty::TEXCOORDS | GG::VertexProperty::COLORS );

  for (int i = 0; i < skeleton->slotsCount; ++i)
  {

    spSlot* slot = skeleton->slots[i];
    spAttachment* attachment = slot->attachment;
    if (!attachment) continue;

    GG::Texture * texture = NULL;
	GG::BlendMode blend = ( slot->data->blendMode == SP_BLEND_MODE_ADDITIVE ) ? 
                                GG::BlendMode::BM_ADDITIVE : GG::BlendMode::BM_ALPHA;

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

        float worldVertices[ 8 ];
        spRegionAttachment* regionAttachment = ( spRegionAttachment* )attachment;
        texture = static_cast< GG::Texture* >( ( ( spAtlasRegion* )regionAttachment->rendererObject )->page->rendererObject );
        spRegionAttachment_computeWorldVertices( regionAttachment, slot->bone, worldVertices );

        float r = ( skeleton->r * slot->r );
        float g = ( skeleton->g * slot->g );
        float b = ( skeleton->b * slot->b );
        float a = ( skeleton->a * slot->a );

        geometry->pushColor( Vector4( r, g, b, a ) );

       
        geometry->pushTexCoord( Vector2(regionAttachment->uvs[ 0 ], regionAttachment->uvs[ 1 ] ) );
        geometry->addPosition( Vector3( worldVertices[ 0 ], worldVertices[ 1 ], 0.0f )  );

        geometry->pushTexCoord( Vector2( regionAttachment->uvs[ 2 ], regionAttachment->uvs[ 3 ] ) );
        geometry->addPosition( Vector3( worldVertices[ 2 ], worldVertices[ 3 ], 0.0f ) );

        geometry->pushTexCoord( Vector2( regionAttachment->uvs[ 4 ], regionAttachment->uvs[ 5 ] ) );
        geometry->addPosition( Vector3( worldVertices[ 4 ], worldVertices[ 5 ], 0.0f ) );
       
        geometry->addPosition( Vector3( worldVertices[ 4 ], worldVertices[ 5 ], 0.0f ) );

        geometry->pushTexCoord( Vector2( regionAttachment->uvs[ 6 ], regionAttachment->uvs[ 7] ) );
        geometry->addPosition( Vector3( worldVertices[ 6 ], worldVertices[ 7 ], 0.0f )  );

        geometry->pushTexCoord( Vector2( regionAttachment->uvs[ 0 ], regionAttachment->uvs[ 1 ] ) );
        geometry->addPosition( Vector3( worldVertices[ 0 ], worldVertices[ 1 ], 0.0f ) );

        
    }
    else if (attachment->type == SP_ATTACHMENT_MESH)
    {
        spMeshAttachment *mesh = (spMeshAttachment*)attachment;

        num_vertices = mesh->trianglesCount;

        
        float *worldVertices = new float[mesh->verticesCount];
        texture = static_cast<GG::Texture*>(((spAtlasRegion*)mesh->rendererObject)->page->rendererObject);

        spMeshAttachment_computeWorldVertices(mesh, slot, worldVertices);

        float r = ( skeleton->r * slot->r );
        float g = ( skeleton->g * slot->g );
        float b = ( skeleton->b * slot->b );
        float a = ( skeleton->a * slot->a );

        geometry->pushColor( Vector4( r, g, b, a ) );

        for (int i = 0; i < mesh->trianglesCount; ++i)
        {
            int index = mesh->triangles[i] << 1;

            geometry->pushTexCoord( Vector2( mesh->uvs[ index ], mesh->uvs[ index + 1 ] ) );
            geometry->addPosition( Vector3( worldVertices[ index ], worldVertices[ index + 1 ], 0.0f ) );
        }

        delete[] worldVertices;
    }
    else if (attachment->type == SP_ATTACHMENT_SKINNED_MESH)
    {
        spSkinnedMeshAttachment* mesh = (spSkinnedMeshAttachment*)attachment;
       
        num_vertices = mesh->trianglesCount;

        float *worldVertices = new float[mesh->uvsCount];
        texture = static_cast<GG::Texture*>(((spAtlasRegion*)mesh->rendererObject)->page->rendererObject);

        spSkinnedMeshAttachment_computeWorldVertices(mesh, slot, worldVertices);

        float r = ( skeleton->r * slot->r );
        float g = ( skeleton->g * slot->g );
        float b = ( skeleton->b * slot->b );
        float a = ( skeleton->a * slot->a );

        geometry->pushColor( Vector4( r, g, b, a ) );

        for( int i = 0; i < mesh->trianglesCount; ++i )
        {
            int index = mesh->triangles[ i ] << 1;

            geometry->pushTexCoord( Vector2( mesh->uvs[ index ], mesh->uvs[ index + 1 ] ) );
            geometry->addPosition( Vector3( worldVertices[ index ], worldVertices[ index + 1 ], 0.0f ) );
        }

        delete[] worldVertices;
    }

    IwAssert(SPINE, texture);
    if (!texture) return;


    // primitive caching of the material texture
    if( lastTexture != texture || lastBlend != blend )
    {
      
        renderState.bindTexture2d( 0, *texture );
        renderState.setDepthTesting( false );

        renderState.setBlendmode( blend );
        renderState.setCullMode( GG::CullMode::CULL_NONE );

        lastTexture = texture;
        lastBlend = blend;
    }

    
  }

  static GG::Shader * shader = NULL;

  if( !shader )
  {
      shader = new GG::Shader();

      if( !initShader( &shader, "unlit" ) )
      {
          delete( shader );
          return;
      }
  }

    geometry->build( GG::D_DYNAMIC );
  

  //t.setPosition( Vector3( -200, -200, 0 ) );
 // t.rotate( 100.0f , t.up );
 // renderState.bindGeometry3d( *geometry );
  renderState.bindVertexBuffer( *geometry );
  renderState.bindShader( *shader );

  shader->setParameterSampler( "mainTex", 0 );
  shader->setParameter( "modelViewProj", camera.getModelViewProjection( transform ) );

  renderState.renderBoundGeometry( GG::DrawMode::TRIANGLE_LIST );
 // geometry->render( GG::TRIANGLE_LIST );





  // Call IwGL swap instead of egl directly
  IwGLSwapBuffers();

  // Sleep for 0ms to allow the OS to process events etc.
  s3eDeviceYield( 0 );
}

S3E_MAIN_DECL void IwMain() {

  init();
  GG::Transform t;

  // Main Game Loop
  while (!s3eDeviceCheckQuitRequest())
  {
      GG::Time::update();

    // Yield to the operating system
    s3eDeviceYield(0);

    // Prevent back-light to off state
    s3eDeviceBacklightOn();

    // Update the game
    if (!update())
      break;
    t.setPosition( Vector3( -200, -200, 0) );
  //  t.rotate( 100.0f * GG::Time::getDeltaTime(), Vector3( 0, 0, 1 ) );
    // Draw the scene
    draw( t );
  }

  release();

}
