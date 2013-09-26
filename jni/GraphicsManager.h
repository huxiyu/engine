#pragma once

#include "defines.h"

#ifdef _WIN32
#include "libs/OpenGL/GLEW/include/GL/glew.h"
#else
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <EGL/egl.h>
#include <android_native_app_glue.h>
#endif

namespace star
{
	class GraphicsManager
	{
	public:
		~GraphicsManager(){}

		static GraphicsManager* GetInstance();

#ifdef _WIN32
		void Initialize();
#else
		void Initialize(const android_app* pApplication);
		void Destroy();
#endif

		void StartDraw();
		void StopDraw();
		void Setup();

	private:

		GraphicsManager();


		static GraphicsManager* mGraphicsManager;

		int32 mScreenWidth;
		int32 mScreenHeight;

#ifndef _WIN32

        int32_t mWidth, mHeight;
        EGLDisplay mDisplay;
        EGLSurface mSurface;
        EGLContext mContext;

#endif

		//disabling default copy constructor
		GraphicsManager(const GraphicsManager& yRef);
		GraphicsManager& operator=(const GraphicsManager& yRef);
	};


}
