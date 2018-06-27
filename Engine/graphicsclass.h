#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_
#pragma once

#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "lightshaderclass.h"
#include "lineshaderclass.h"
#include "lightclass.h"
#include "inputclass.h"


const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(InputClass* _input);
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(float delta_time);

private:
	bool Render(float rotation, float delta_time);

private:
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	ModelClass* m_Model;
	LightShaderClass* m_LightShader;
	LineShaderClass* m_LineShader;
	LightClass* m_Light;
	InputClass* m_input;
};

#endif