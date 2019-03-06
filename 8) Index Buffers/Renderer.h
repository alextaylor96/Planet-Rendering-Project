#pragma  once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/camera.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Noise.h"

class  Renderer : public  OGLRenderer {

	public:

		Renderer(Window &parent);
		virtual ~Renderer(void);
		virtual  void  PrintFps();
		virtual  void  RenderScene();
		virtual  void  UpdateScene(float  msec);


	protected:
		//currently used ico
		Mesh* ico;
	
		//stored icos initialized as null because not created
		Mesh* icoStore0 = NULL;
		Mesh* icoStore2 = NULL;
		Mesh* icoStore1 = NULL;
		Mesh* icoStore3 = NULL;
		Mesh* icoStore4 = NULL;
		Mesh* icoStore5 = NULL;
		Mesh* icoStore6 = NULL;
	
		Camera*      camera;
		Vector3* cameraPos;
		Vector2* GazePos;
		Vector2* screenSize;

		int subdivisions = 3;
		bool distanceChanged;
		int seed = 1;
		bool genSeed = false;
		bool toggleShader = false;
		int currentShaderNum = 0;

		//timer for fps
		GameTimer timer;
	
}; 
