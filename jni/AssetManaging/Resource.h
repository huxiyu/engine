#pragma once

#ifndef _WIN32

#include "../defines.h"

#include <android_native_app_glue.h>

class EventLoop;

namespace star
{
	class Resource
	{
	public:
		Resource(android_app* pApplication,tstring pPath);

		tstring getPath();

		status open();
		void close();
		status read(void* pBuffer, size_t pCount);

		off_t getLength();
		const void* bufferize();

	private:
		tstring mPath;
		AAssetManager* mAssetManager;
		AAsset* mAsset;
	};
}

#endif
