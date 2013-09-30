#pragma once

#include "defines.h"

#ifdef _WIN32
#include "libs/OpenGL/GLEW/include/GL/glew.h"
#else
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <android_native_app_glue.h>
#endif

namespace star
{
	class GraphicsManager
	{
	public:
		~GraphicsManager() {}

		static GraphicsManager* GetInstance();

#ifdef _WIN32
		void Initialize(int32 screenWidth, int32 screenHeight);
#else
		void Initialize(const android_app* pApplication);
		void Destroy();
#endif


		void StartDraw();
		void StopDraw();
		void Setup();

		int32 GetScreenWidth(){return mScreenWidth;}
		int32 GetScreenHeight(){return mScreenHeight;}

	private:

		GraphicsManager();


		static GraphicsManager* mGraphicsManager;

		int32 mScreenWidth;
		int32 mScreenHeight;

#ifndef _WIN32

        EGLDisplay mDisplay;
        EGLSurface mSurface;
        EGLContext mContext;

#endif

		//disabling default copy constructor
		GraphicsManager(const GraphicsManager& yRef);
		GraphicsManager(GraphicsManager&& yRef);
		GraphicsManager& operator=(const GraphicsManager& yRef);
	};


}
