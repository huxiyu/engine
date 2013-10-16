#pragma once
#include <vector>
#include "../defines.h"
#include <memory>
#include "../Shaders/Shader.h"
#include "../Components/SpriteComponent.h"

namespace star
{
	struct SpriteInfo
	{
		std::vector<GLfloat> vertices;
		std::vector<GLfloat> uvCoords;
		tstring filePath, spriteName;
		int32 width, height;
		mat4x4 transform;
	};

	class SpriteBatch
	{
	public:
		~SpriteBatch(void);
		static std::shared_ptr<SpriteBatch> GetInstance();

		void Initialize();
		void Flush();
		void AddSpriteToQueue(SpriteComponent* sprite);

	private:
		SpriteBatch(void);
		void Begin();
		void End();
		void CreateSpriteQuad();
		void FlushSprites();
		mat4x4 InverseMatrix(const mat4x4& matrix);

		static std::shared_ptr<SpriteBatch> m_pSpriteBatch;
		static const int BATCHSIZE = 50;

		std::vector<SpriteInfo> m_SpriteQueue;
		std::vector<GLfloat> m_VertexBuffer;
		std::vector<GLfloat> m_UvCoordBuffer;
		std::vector<mat4x4> m_WorldMatBuffer;
		Shader m_Shader;	

		//disabling default copy constructor
		SpriteBatch(const SpriteBatch& yRef);
		SpriteBatch(SpriteBatch&& yRef);
		SpriteBatch& operator=(const SpriteBatch& yRef);
	};
}