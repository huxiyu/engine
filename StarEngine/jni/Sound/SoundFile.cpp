#include "SoundFile.h"
#include "SoundService.h"
#include "../Logger.h"
#include "../AssetManaging/Resource.h"
#include "../Helpers/Helpers.h"

#ifndef _WIN32
#include "../StarEngine.h"
#endif


namespace star
{
	SoundFile::SoundFile(const tstring& path):
		mLoopTimes(0),
		bStopped(false),
		bQueuedPlay(false),
#ifdef _WIN32
		mMusic(nullptr)
#else
		mPlayerObj(NULL),
		mPlayer(NULL),
		mPlayerSeek(NULL)
#endif
	{
#ifdef _WIN32
		if(mMusic == NULL)
		{
			std::string sound_path = string_cast<std::string>(path);
			mMusic = Mix_LoadMUS(sound_path.c_str());
			if(!mMusic)
			{
				star::Logger::GetInstance()->Log(star::LogLevel::Info, _T("Audio :Could not load song, reason : ")+string_cast<tstring>(Mix_GetError()));
			}
			
		}
#else
		SLEngineItf mEngine = SoundService::GetInstance()->GetEngine();
		SLresult lRes;
		Resource lResource(star::StarEngine::GetInstance()->GetAndroidApp(), path);
		ResourceDescriptor lDescriptor = lResource.DeScript();
		if(lDescriptor.mDescriptor < 0)
		{
			star::Logger::GetInstance()->Log(star::LogLevel::Error, _T("Audio : Could not open file"));
			return;
		}
		SLDataLocator_AndroidFD lDataLocatorIn;
		lDataLocatorIn.locatorType = SL_DATALOCATOR_ANDROIDFD;
		lDataLocatorIn.fd = lDescriptor.mDescriptor;
		lDataLocatorIn.offset = lDescriptor.mStart;
		lDataLocatorIn.length = lDescriptor.mLength;

		SLDataFormat_MIME lDataFormat;
		lDataFormat.formatType = SL_DATAFORMAT_MIME;
		lDataFormat.mimeType = NULL;
		lDataFormat.containerType = SL_CONTAINERTYPE_UNSPECIFIED;

		SLDataSource lDataSource;
		lDataSource.pLocator = &lDataLocatorIn;
		lDataSource.pFormat = &lDataFormat;

		SLDataLocator_OutputMix lDataLocatorOut;
		lDataLocatorOut.locatorType = SL_DATALOCATOR_OUTPUTMIX;
		lDataLocatorOut.outputMix = SoundService::GetInstance()->GetOutputMixObject();

		SLDataSink lDataSink;
		lDataSink.pLocator=&lDataLocatorOut;
		lDataSink.pFormat= NULL;


		const SLuint32 lPlayerIIDCount = 2;
		const SLInterfaceID lPlayerIIDs[] ={ SL_IID_PLAY, SL_IID_SEEK };
		const SLboolean lPlayerReqs[] ={ SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
		lRes = (*mEngine)->CreateAudioPlayer(mEngine,&mPlayerObj, &lDataSource, &lDataSink,lPlayerIIDCount, lPlayerIIDs, lPlayerReqs);
		if (lRes != SL_RESULT_SUCCESS)
		{
			star::Logger::GetInstance()->Log(star::LogLevel::Error, _T("AudioFile : Can't create audio player"));
			Stop();
			return;
		}

		lRes = (*mPlayerObj)->Realize(mPlayerObj,SL_BOOLEAN_FALSE);
		if (lRes != SL_RESULT_SUCCESS)
		{
			star::Logger::GetInstance()->Log(star::LogLevel::Error, _T("AudioFile : Can't realise audio player"));
			Stop();
			return;
		}

		lRes = (*mPlayerObj)->GetInterface(mPlayerObj,SL_IID_PLAY, &mPlayer);
		if (lRes != SL_RESULT_SUCCESS)
		{
			star::Logger::GetInstance()->Log(star::LogLevel::Error, _T("AudioFile : Can't get audio play interface"));
			Stop();
			return;
		}

		lRes = (*mPlayerObj)->GetInterface(mPlayerObj,SL_IID_SEEK, &mPlayerSeek);
		if (lRes != SL_RESULT_SUCCESS)
		{
			star::Logger::GetInstance()->Log(star::LogLevel::Error, _T("AudioFile : Can't get audio seek interface"));
			Stop();
			return;
		}

		if((*mPlayer)->SetCallbackEventsMask(mPlayer,SL_PLAYSTATE_STOPPED)!=SL_RESULT_SUCCESS)
		{
			star::Logger::GetInstance()->Log(star::LogLevel::Error, _T("AudioFile : Can't set callback flags"));
		}
		if((*mPlayer)->RegisterCallback(mPlayer,MusicStoppedCallback,this)!=SL_RESULT_SUCCESS)
		{
			star::Logger::GetInstance()->Log(star::LogLevel::Error, _T("AudioFile : Can't set callback"));
		}
#endif
	}

	SoundFile::~SoundFile()
	{
#ifdef _WIN32
		// [COMMENT] always use { and } for if statements!!!!
		if(mMusic != nullptr)
			Mix_FreeMusic(mMusic);
		// [COMMENT] this line is useless when it is already nullptr,
		// so better to include it in the body of the if statement!
		mMusic=nullptr;
#else
		// [COMMENT] use nullptr!
		if(mPlayer != NULL)
		{
			SLuint32 lPlayerState;
			(*mPlayerObj)->GetState(mPlayerObj, &lPlayerState);
			if(lPlayerState == SL_OBJECT_STATE_REALIZED)
			{
				(*mPlayer)->SetPlayState(mPlayer,SL_PLAYSTATE_PAUSED);
				(*mPlayerObj)->Destroy(mPlayerObj);
				// [COMMENT] use nullptr!
				mPlayerObj = NULL;
				// [COMMENT] use nullptr!
				mPlayer = NULL;
				// [COMMENT] use nullptr!
				mPlayerSeek = NULL;
				star::Logger::GetInstance()->Log(star::LogLevel::Error, _T("AudioFile : Soundfile Destroyed"));
			}
		}
#endif
	}

	// [COMMENT] Put default parameters in the header file,
	// not in the implementation file!!!
	void SoundFile::Play(int32 looptimes=0)
	{
		mLoopTimes = looptimes;
		star::Logger::GetInstance()->Log(star::LogLevel::Info, _T("AudioFile : Playing File , Looptimes = ")+ star::string_cast<tstring>(mLoopTimes));
#ifdef _WIN32
		// [COMMENT] use nullptr!
		Mix_HookMusicFinished(NULL);
		Mix_PlayMusic(mMusic, mLoopTimes);
#else
		SLresult lRes;
		if(mLoopTimes==-1)
		{
			lRes = (*mPlayerSeek)->SetLoop(mPlayerSeek,SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);
			if (lRes != SL_RESULT_SUCCESS)
			{
				star::Logger::GetInstance()->Log(star::LogLevel::Error, _T("Audio : Can't set audio loop"));
				Stop();
				return;
			}
		}

		lRes = (*mPlayer)->SetPlayState(mPlayer,SL_PLAYSTATE_PLAYING);
		if (lRes != SL_RESULT_SUCCESS)
		{
			star::Logger::GetInstance()->Log(star::LogLevel::Error, _T("Audio : Can't play audio"));
			Stop();
			return;
		};
#endif
	}
	
	// [COMMENT] Put default parameters in the header file,
	// not in the implementation file!!!
	void SoundFile::PlayQueued(int32 looptimes =0)
	{
		mLoopTimes = looptimes;
		bQueuedPlay = true;

#ifdef _WIN32
		Mix_HookMusicFinished(MusicStoppedCallback);
		Mix_PlayMusic(mMusic,mLoopTimes);
#else
		Play(mLoopTimes);
#endif
	}

	void SoundFile::Stop()
	{		
#ifdef _WIN32
		Mix_PauseMusic();
		Mix_RewindMusic();
#else	
		(*mPlayer)->SetPlayState(mPlayer,SL_PLAYSTATE_STOPPED);
#endif
	}

	void SoundFile::Pause()
	{
#ifdef _WIN32
		Mix_PauseMusic();
		Mix_RewindMusic();
#else
		(*mPlayer)->SetPlayState(mPlayer,SL_PLAYSTATE_PAUSED);
#endif
	}

	void SoundFile::Resume()
	{
#ifdef _WIN32
		Mix_PauseMusic();
		Mix_RewindMusic();
#else
		SLresult lres=(*mPlayer)->GetPlayState(mPlayer,&lres);
		if(lres==SL_PLAYSTATE_PAUSED)
		(*mPlayer)->SetPlayState(mPlayer,SL_PLAYSTATE_PLAYING);
#endif
	}


#ifdef _WIN32
	void SoundFile::MusicStoppedCallback()
	{
		star::SoundService::GetInstance()->NextSongInQueue();
	}

#else
	void SoundFile::MusicStoppedCallback(SLPlayItf caller,void *pContext,SLuint32 event)
	{
		SoundFile* file = reinterpret_cast<SoundFile*>(pContext);

		star::Logger::GetInstance()->Log(star::LogLevel::Info, _T("AudioFile : Callback Entered, Looptimes = ")+star::string_cast<tstring>(file->mLoopTimes));
		if(file->mLoopTimes==0)
		{
			SLPlayItf pPlay = file->mPlayer;
			(*pPlay)->SetPlayState(pPlay,SL_PLAYSTATE_STOPPED);
			file->bStopped=true;
			if(file->bQueuedPlay)
				star::SoundService::GetInstance()->NextSongInQueue();
		}
		else
		{
			SLPlayItf pPlay = file->mPlayer;
			(*pPlay)->SetPlayState(pPlay,SL_PLAYSTATE_STOPPED);
			file->mLoopTimes-=1;
			file->Play(file->mLoopTimes);
		}
	}
#endif
}
