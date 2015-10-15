#include "SpriteBatch.h"

#include <core\Passert.h>
#include <core\Log.h>
#include <core\Memory.h>

#include "RenderSystem.h"
#include "Drawable.h"
#include "Color.h"
#include "Rectangle.h"

#include <scene\Texture.h>
#include <scene\Transformable.h>
#include <scene\TextureCoordinates.h>


using namespace pm;
using namespace std;

SpriteBatch* SpriteBatch::instance = nullptr;

SpriteBatch* SpriteBatch::GetInstance()
{
	if (instance == nullptr)
		instance = NEW SpriteBatch();
	return instance;
}

void SpriteBatch::DestroyInstance()
{
	delete instance;
	instance = nullptr;
}

void SpriteBatch::Draw()
{
	BatchComponents();

	//glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	//glBlendFunc();
	RenderSystem::GetInstance()->Draw(&batchVector[0]);
	
	//glDisable(GL_DEPTH_TEST);
	//glDisable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	RenderSystem::GetInstance()->Draw(&batchVector[1]);
	glDepthMask(GL_TRUE);

	gameEntityVector.clear();
	opaqueGameEntityVector.clear();
	batchVector.clear();
}

void SpriteBatch::AddGameEntity(GameEntity* gameEntity)
{
	gameEntityVector.push_back(gameEntity);
}

void SpriteBatch::AddOpaqueGameEntity(GameEntity* gameEntity)
{
	opaqueGameEntityVector.push_back(gameEntity);
}

void SpriteBatch::AddOpaqueGameEntity(std::vector<GameEntity*> entityVector)
{
	for (int i = 0; i < entityVector.size(); i++)
	{
		opaqueGameEntityVector.push_back(entityVector.at(i));
	}
}

void SpriteBatch::AddText(Text* textEntity)
{
	for (int i = 0; i < textEntity->GetTextVector().size(); i++)
		opaqueGameEntityVector.push_back(textEntity->GetTextVector().at(i));
}

bool SpriteBatch::IsDrawable(GameEntity* gameEntity)
{
	if (gameEntity->GetComponent<Drawable>() == nullptr)
		return false;
	else if (gameEntity->GetComponent<Rectangle>() == nullptr)
		return false;
	else
		return gameEntity->GetComponent<Drawable>()->GetDrawState();
}

void SpriteBatch::BatchComponents()
{
	for (int i = 0; i < gameEntityVector.size(); i++)
	{
		std::vector<GLfloat> tempVertexData;
		std::vector<GLushort> tempIndexData;
		glm::mat4 tempTransformMatrix = glm::mat4();
		GLuint tempTextureIndex;
		bool newBatch = true;

		if (IsDrawable(gameEntityVector[i]))
		{
			ParseData(gameEntityVector[i], &tempVertexData, &tempIndexData, &tempTransformMatrix, &tempTextureIndex);

			for (unsigned k = 0; k < batchVector.size(); k++)
			{
				// If there is texture with same index as new one, add data to batch.
				if (batchVector[k].textureIndex == tempTextureIndex)
				{
					batchVector[k].AddData(tempVertexData, tempIndexData, tempTransformMatrix);
					newBatch = false;
					break;
				}
			}
			// If no batches with same texture were found, create new batch and add data to it.
			if (newBatch)
				batchVector.push_back(Batch(tempVertexData, tempIndexData, tempTransformMatrix, tempTextureIndex));
		}
	}

	// Somebody has to make this good.
	for (int i = 0; i < opaqueGameEntityVector.size(); i++)
	{
		std::vector<GLfloat> tempVertexData;
		std::vector<GLushort> tempIndexData;
		glm::mat4 tempTransformMatrix = glm::mat4();
		GLuint tempTextureIndex;
		bool newBatch = true;

		if (IsDrawable(opaqueGameEntityVector[i]))
		{
			ParseData(opaqueGameEntityVector[i], &tempVertexData, &tempIndexData, &tempTransformMatrix, &tempTextureIndex);

			for (unsigned k = 0; k < batchVector.size(); k++)
			{
				// If there is texture with same index as new one, add data to batch.
				if (batchVector[k].textureIndex == tempTextureIndex)
				{
					batchVector[k].AddData(tempVertexData, tempIndexData, tempTransformMatrix);
					newBatch = false;
					break;
				}
			}
			// If no batches with same texture were found, create new batch and add data to it.
			if (newBatch)
				batchVector.push_back(Batch(tempVertexData, tempIndexData, tempTransformMatrix, tempTextureIndex));
		}
	}
}

void SpriteBatch::ParseData(GameEntity* gameEntity,
	std::vector<GLfloat>* vertexData,
	std::vector<GLushort>* indexData,
	glm::mat4* transformMatrix,
	GLuint* textureIndex)
{
	std::vector<GLfloat> vertexPos;
	GLfloat depth = 0;
	std::vector<GLfloat> vertexTexPos;
	glm::vec4 vertexColor;

	/// SHAPE ///
	if (gameEntity->GetComponent<Rectangle>() == nullptr)
	{
		// Should be edited back to shape class.
		DEBUG_WARNING(("GameEntity in SpriteBatch has no SHAPE."));
		return;
	}
	else
	{
		vertexPos = gameEntity->GetComponent<Rectangle>()->GetVertices();
		*indexData = gameEntity->GetComponent<Rectangle>()->GetIndices();
		//ASSERT(vertexPos.empty());
		//ASSERT(indices.empty());
	}

	/// TRANSFORMABLE ///
	if (gameEntity->GetComponent<Transformable>() == nullptr)
	{
		DEBUG_WARNING(("Gathering data from GameEntity without TRANSFORMABLE."));
		*transformMatrix = glm::mat4(1);
	}
	else
	{
		*transformMatrix = gameEntity->GetComponent<Transformable>()->GetTransformMatrix(); // Do transform magic.
		depth = gameEntity->GetComponent<Transformable>()->GetDepth();
	}

	/// TEXTURE ///
	if (gameEntity->GetComponent<Texture>() == nullptr)
	{
		*textureIndex = -1;
		for (int i = 0; i < 8; i++)
			vertexTexPos.push_back(0);

		DEBUG_WARNING(("Gathering data from GameEntity without TEXTURE."));
	}
	else
	{
		*textureIndex = gameEntity->GetComponent<Texture>()->GetId();

		if (gameEntity->GetComponent<TextureCoordinates>() == nullptr)
		{
			for (int i = 0; i < 2; i++)
				for (int j = 0; j < 2; j++)
				{
					vertexTexPos.push_back(i);
					vertexTexPos.push_back(j);
				}
		}
		else
		{
			glm::uvec2 tempTextureSize = gameEntity->GetComponent<Texture>()->GetTextureSize();;
			std::vector<GLfloat> tempVec = gameEntity->GetComponent<TextureCoordinates>()->GetTextureCoordinates();

			vertexTexPos.push_back(tempVec[0] / tempTextureSize.x);
			vertexTexPos.push_back(1 - (tempVec[3] / tempTextureSize.y));

			vertexTexPos.push_back(tempVec[0] / tempTextureSize.x);
			vertexTexPos.push_back(1 - (tempVec[1] / tempTextureSize.y));

			vertexTexPos.push_back(tempVec[2] / tempTextureSize.x);
			vertexTexPos.push_back(1 - (tempVec[3] / tempTextureSize.y));

			vertexTexPos.push_back(tempVec[2] / tempTextureSize.x);
			vertexTexPos.push_back(1 - (tempVec[1] / tempTextureSize.y));
		}
	}

	/// COLOR ///
	if (gameEntity->GetComponent<Color>() == nullptr)
	{
		vertexColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	else
	{
		vertexColor = gameEntity->GetComponent<Color>()->GetColor();
	}

	for (int i = 0; i < 4; i++)
	{
		vertexData->push_back(vertexPos[i * 2]);
		vertexData->push_back(vertexPos[i * 2 + 1]);
		vertexData->push_back(depth);

		vertexData->push_back(vertexColor.x);
		vertexData->push_back(vertexColor.y);
		vertexData->push_back(vertexColor.z);
		vertexData->push_back(vertexColor.w);

		vertexData->push_back(vertexTexPos[i * 2]);
		vertexData->push_back(vertexTexPos[i * 2 + 1]);
	}
}