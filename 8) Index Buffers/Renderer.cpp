#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent) {
	//create starting ico
	ico = Mesh::GenerateIcosphere(6);
	//store created ico
	icoStore4 = ico;

	camera = new  Camera();
	//start camera position
	camera->SetPosition(Vector3(0, 0, 120));
	
	//the shader for which creates lod based on distance from camera
	constantShader = new  Shader(SHADERDIR"planetVert.glsl", SHADERDIR"planetFrag.glsl", "", SHADERDIR"constantTcs.glsl", SHADERDIR"planetTes.glsl");
	if (!constantShader->LinkProgram()) {
		return;
	}

	//the shader for which creates lod based on distance from camera
	distanceShader = new  Shader(SHADERDIR"planetVert.glsl", SHADERDIR"planetFrag.glsl", "", SHADERDIR"distanceTcs.glsl", SHADERDIR"planetTes.glsl");
	if (!distanceShader->LinkProgram()) {
		return;
	}

	//the shader for which creates lod based on distance from mouse location
	gazeShader = new  Shader(SHADERDIR"planetVert.glsl", SHADERDIR"planetFrag.glsl", "", SHADERDIR"gazeTcs.glsl", SHADERDIR"planetTes.glsl");
	if (!gazeShader->LinkProgram()) {
		return;
	}

	//the shader for which creates lod based on distance from mouse location
	distanceGazeCombinedShader = new  Shader(SHADERDIR"planetVert.glsl", SHADERDIR"planetFrag.glsl", "", SHADERDIR"combinedTcs.glsl", SHADERDIR"planetTes.glsl");
	if (!distanceGazeCombinedShader->LinkProgram()) {
		return;
	}

	//set default shader
	currentShader = constantShader;


	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, (float)width/(float)height, 45.0f);
	glEnable(GL_DEPTH_TEST);
	//enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	init = true;
}

Renderer ::~Renderer(void) {
	if (icoStore0 != NULL) {
		delete icoStore0;
	}
	if (icoStore1 != NULL) {
		delete icoStore1;
	}
	if (icoStore2 != NULL) {
		delete icoStore2;
	}
	if (icoStore3 != NULL) {
		delete icoStore3;
	}
	if (icoStore4 != NULL) {
		delete icoStore4;
	}
	if (icoStore5 != NULL) {
		delete icoStore5;
	}
	if (icoStore6 != NULL) {
		delete icoStore6;
	}

	delete  camera;
}


void  Renderer::UpdateScene(float  msec) {
	camera->UpdateCamera(msec);
	
	viewMatrix = camera->BuildViewMatrix();

	//bind the camera position and seed to shaders uniform
	cameraPos = &camera->GetPosition();
	glUniform3f(glGetUniformLocation(currentShader->GetProgram(), "cameraPos"), cameraPos->x, cameraPos->y, cameraPos->z);
	
	//bind the location the user is looking at
	GazePos = &Window::GetMouse()->GetAbsolutePosition();
	//note y inverted here as 0 is at top of screen when getting mouse
	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "gazePos"), GazePos->x, -GazePos->y);
	//bind the window size
	glUniform2f(glGetUniformLocation(currentShader->GetProgram(), "screenSize"), width, height);
	

	//seed for terrain
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "seed"), seed);

	//x and z to change between wire/not wire
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_Z)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_X)) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	//change shader on t
	//1 to change seed
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_T)) {
		if (!toggleShader) {
			switch (currentShaderNum) {
			case 0:
				currentShader = distanceShader;
				currentShaderNum++;
				break;

			case 1:
				currentShader = gazeShader;
				currentShaderNum++;
				break;
			case 2:
				currentShader = distanceGazeCombinedShader;
				currentShaderNum++;
				break;
			case 3:
				currentShader = constantShader;
				currentShaderNum = 0;
				break;


			}

			toggleShader = true;
		}
	}
	else {
		toggleShader = false;
	}

	//change seed
	//1 to change seed
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_1)) {
		if (!genSeed) {
			if (seed < 250) {
				seed = seed + 1;
			}
			else {
				seed = 0;
			}
			genSeed = true;
		}
	}
	else {
		genSeed = false;
	}


	


	//change number of subdivisons in ico mesh by distance of camera from center of it, currently performs terrible change so only recreate mesh when passing thresholds

	float dist = camera->GetPosition().Length();

	if (dist >= 8000) {
		if (subdivisions != 0) {
			subdivisions = 0;
			distanceChanged = true;
		}
	}
	else if (dist < 8000 && dist >= 7000) {
		if (subdivisions != 1) {
			subdivisions = 1;
			distanceChanged = true;
		}
	}
	else if (dist < 7000 && dist >= 6000) {
		if (subdivisions != 2) {
			subdivisions = 2;
			distanceChanged = true;
		}
	}
	else if (dist < 6000 && dist >= 4000) {
		if (subdivisions != 3) {
			subdivisions = 3;
			distanceChanged = true;
		}
	}
	else if (dist < 4000 && dist >= 1500) {
		if (subdivisions != 4) {
			subdivisions = 4;
			distanceChanged = true;
		}
	}
	
	else if (dist < 1500 && dist >= 750) {
		if (subdivisions != 5) {
			subdivisions = 5;
			distanceChanged = true;
		}
	}
	else if (dist < 750) {
		if (subdivisions != 6) {
			subdivisions = 6;
			distanceChanged = true;
		}
	}
	
	//if distanced changed past threshold
	//if (distanceChanged) {
	//	//pick correct number of subdivsions
	//	switch (subdivisions) {
	//	case 0:
	//		//if icohasnt been created in past with this number of subdivisons create and store then swap to be used
	//		if (icoStore0 == NULL) {
	//			icoStore0 = Mesh::GenerateIcosphere(0);
	//		}
	//		ico = icoStore0;
	//		break;
	//
	//	case 1:
	//		if (icoStore1 == NULL) {
	//			icoStore1 = Mesh::GenerateIcosphere(1);
	//		}
	//		ico = icoStore1;
	//		break;
	//
	//	case 2:
	//		if (icoStore2 == NULL) {
	//			icoStore2 = Mesh::GenerateIcosphere(2);
	//		}
	//		ico = icoStore2;
	//		break;
	//	case 3:
	//		if (icoStore3 == NULL) {
	//			icoStore3 = Mesh::GenerateIcosphere(3);
	//		}
	//		ico = icoStore3;
	//		break;
	//	case 4:
	//		if (icoStore4 == NULL) {
	//			icoStore4 = Mesh::GenerateIcosphere(4);
	//		}
	//		ico = icoStore4;
	//		break;
	//	case 5:
	//		if (icoStore5 == NULL) {
	//			icoStore5 = Mesh::GenerateIcosphere(5);
	//		}
	//		ico = icoStore5;
	//		break;
	//	case 6:
	//		if (icoStore6 == NULL) {
	//			icoStore6 = Mesh::GenerateIcosphere(6);
	//		}
	//		ico = icoStore6;
	//		break;
	//	}
	//
	//
	//
	//}
	//distanceChanged = false;


}

//to display text on screen(fps counter, number of triangles and vertices
void Renderer::PrintFps() {
	cout << (1000 / timer.GetTimedMS()) << endl;
}

void  Renderer::RenderScene() {
	PrintFps();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(currentShader->GetProgram());
	modelMatrix = Matrix4::Scale(Vector3(100, 100, 100));
	UpdateShaderMatrices();


	//draw the planet mesh
	
	ico->Draw();
	
	glUseProgram(0);
	SwapBuffers();
}
