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
	// Prepare layers for drawing.
	BatchAllLayers();

	// Draw all gameEntities on every eleven layers. (0-10)
	for (int i = 0; i < 10; i++)
	{
		// Disable blend and set depthMask true for opaqueGameEntities.
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);

		for (int j = 0; j < opaqueLayerBatchVector[i].size(); j++)
		{
			RenderSystem::GetInstance()->Draw(&opaqueLayerBatchVector[i].at(j));
		}

		// Enable blend and set depthMask false for translucentGameEntities.
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);

		for (int j = 0; j < translucentLayerBatchVector[i].size(); j++)
		{
			RenderSystem::GetInstance()->Draw(&translucentLayerBatchVector[i].at(j));
		}
		// Clear gameEntities from current layer.
		opaqueLayerBatchVector[i].clear();
		translucentLayerBatchVector[i].clear();
	}
	// Clear vectors for gameEntities added during draw cycle.
	opaqueGameEntityVector.clear();
	translucentGameEntityVector.clear();
}

void SpriteBatch::DrawOld()
{
	//for (int i = 0; i < layers.size(); i++)
	{
	//Change the Z buffer to i;

	BatchOpaqueComponents(); //with depth i

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	for (int i = 0; i < batchVector.size(); i++)
		RenderSystem::GetInstance()->Draw(&batchVector[i]);

	batchVector.clear();
	BatchTranslucentComponents(); //with depth i


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	for (int i = 0; i < batchVector.size(); i++)
		RenderSystem::GetInstance()->Draw(&batchVector[i]);


	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);

	opaqueGameEntityVector.clear();
	translucentGameEntityVector.clear();
	batchVector.clear();
	}
}

void SpriteBatch::AddTranslucentGameEntity(GameEntity* gameEntity)
{
	translucentGameEntityVector.push_back(gameEntity);
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
		translucentGameEntityVector.push_back(textEntity->GetTextVector().at(i));
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

void SpriteBatch::CreateLayers()
{
	// Store each gameEntity on a layer according to it's depth.

	for (int i = 0; i < opaqueGameEntityVector.size(); i++)
	{
		int depth = opaqueGameEntityVector[i]->GetComponent<Transformable>()->GetDepth();
		Layers[depth].translucentGO.push_back(opaqueGameEntityVector[i]);
	}

	for (int i = 0;i < translucentGameEntityVector.size(); i++)
	{
		int depth = translucentGameEntityVector[i]->GetComponent<Transformable>()->GetDepth();
		Layers[depth].translucentGO.push_back(translucentGameEntityVector[i]);
	}
}

void SpriteBatch::BatchLayerComponents(int layer, bool type)
{
	// Vectors to make code bit cleaner.
	std::vector<GameEntity*>* lp;
	std::vector<Batch>* blp;

	// Select if batched GameEntities will be opaque or translucent.
	if (type)
	{
		lp = &Layers[layer].opaqueGO;
		blp = &opaqueLayerBatchVector[layer];
	}
	else
	{
		lp = &Layers[layer].translucentGO;
		blp = &translucentLayerBatchVector[layer];
	}

	// Less vectors to be called, cleaning a bit.
	std::vector<GameEntity*>& gameEntityVector = *lp;
	std::vector<Batch>& batchVector = *blp;


	// Go throught all layers to find data to be used on current batch
	for (int i = 0; i < gameEntityVector.size(); i++)
	{
		// Create temporary storages for data gathered from current gameEntity.
		std::vector<GLfloat> tempVertexData;
		std::vector<GLushort> tempIndexData;
		glm::mat4 tempTransformMatrix = glm::mat4();
		GLuint tempTextureIndex;
		// Creates new batch if there is no batch created yet for the texture.
		bool newBatch = true;

		if (IsDrawable(gameEntityVector[i]))
		{
			// Gather data to from current gameEntity to other parameters.
			ParseData(gameEntityVector[i], &tempVertexData, &tempIndexData, &tempTransformMatrix, &tempTextureIndex);

			for (unsigned k = 0; k < batchVector.size(); k++)
			{
				// If there is texture with same index as new one, add data to batch.
				if (batchVector[k].textureIndex == tempTextureIndex)
				{
					// Add found data to batch with same texture.
					batchVector[k].AddData(tempVertexData, tempIndexData, tempTransformMatrix);
					newBatch = false;
					break;
				}
			}
			// If no batches with same texture were found, create new batch and add found data to it.
			if (newBatch)
				batchVector.push_back(Batch(tempVertexData, tempIndexData, tempTransformMatrix, tempTextureIndex));
		}
	}
}

void SpriteBatch::BatchAllLayers()
{
	// Create layers for this batch.
	CreateLayers();

	for (int i = 0; i < Layers.size(); i++)
	{
		// Batch opaqueGO
		BatchLayerComponents(i, true);
		// Batch TranslucentGO
		BatchLayerComponents(i, false);
	}
	for (int i = 0;i < Layers.size(); i++)
	{
		// clear layers created in CreateLayers();
		Layers[i].opaqueGO.clear();
		Layers[i].translucentGO.clear();
	}
}

void SpriteBatch::BatchOpaqueComponents()
{
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

void SpriteBatch::BatchTranslucentComponents()
{
	// Somebody has to make this good.
	for (int i = 0; i < translucentGameEntityVector.size(); i++)
	{
		std::vector<GLfloat> tempVertexData;
		std::vector<GLushort> tempIndexData;
		glm::mat4 tempTransformMatrix = glm::mat4();
		GLuint tempTextureIndex;
		bool newBatch = true; 

		if (IsDrawable(translucentGameEntityVector[i]))
		{
			ParseData(translucentGameEntityVector[i], &tempVertexData, &tempIndexData, &tempTransformMatrix, &tempTextureIndex);

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
	int depth = 0;
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

		// DEBUG_WARNING(("Gathering data from GameEntity without TEXTURE."));
	}
	else
	{
		*textureIndex = gameEntity->GetComponent<Texture>()->GetId();

		//if (gameEntity->GetComponent<TextureCoordinates>() == nullptr)
		//{
		//	for (int i = 0; i < 2; i++)
		//		for (int j = 0; j < 2; j++)
		//		{
		//			vertexTexPos.push_back(i);
		//			vertexTexPos.push_back(j);
		//		}
		//}

		if (gameEntity->GetComponent<TextureCoordinates>() == nullptr)
		{
			glm::fvec2 tempTextureSize = gameEntity->GetComponent<Texture>()->GetTextureSize();
			glm::fvec2 tempVec = gameEntity->GetComponent<Texture>()->GetTrueSize();

			vertexTexPos.push_back(0.0f);
			vertexTexPos.push_back(0.0f);

			vertexTexPos.push_back(0.0f);
			vertexTexPos.push_back((tempVec.y / tempTextureSize.y));

			vertexTexPos.push_back((tempVec.x / tempTextureSize.x));
			vertexTexPos.push_back(0.0f);

			vertexTexPos.push_back((tempVec.x / tempTextureSize.x));
			vertexTexPos.push_back((tempVec.y / tempTextureSize.y));
		}
		else
		{
			glm::uvec2 tempTextureSize = gameEntity->GetComponent<Texture>()->GetTextureSize();
			std::vector<GLfloat> tempVec = gameEntity->GetComponent<TextureCoordinates>()->GetTextureCoordinates();

			vertexTexPos.push_back(tempVec[0] / tempTextureSize.x);
			vertexTexPos.push_back((tempVec[3] / tempTextureSize.y));

			vertexTexPos.push_back(tempVec[0] / tempTextureSize.x);
			vertexTexPos.push_back((tempVec[1] / tempTextureSize.y));

			vertexTexPos.push_back(tempVec[2] / tempTextureSize.x);
			vertexTexPos.push_back((tempVec[3] / tempTextureSize.y));

			vertexTexPos.push_back(tempVec[2] / tempTextureSize.x);
			vertexTexPos.push_back((tempVec[1] / tempTextureSize.y));
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
		vertexData->push_back(0);

		vertexData->push_back(vertexColor.x);
		vertexData->push_back(vertexColor.y);
		vertexData->push_back(vertexColor.z);
		vertexData->push_back(vertexColor.w);

		vertexData->push_back(vertexTexPos[i * 2]);
		vertexData->push_back(vertexTexPos[i * 2 + 1]);
	}
}