#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <vector>
#include <GLES2\gl2.h>

#include "Batch.h"
#include "Shader.h"
#include "Buffer.h"

namespace pm
{
	/** \brief System that contains necessary data (shader, camera, etc.) for rendering.
	*
	* pm::Application handles the lifeline of %RenderSystem.
	*
	* \sa pm::SpriteBatch contains all necessary functions for rendering GameEntities.
	*
	* \ingroup Graphics
	*/

	class RenderSystem
	{
	public:

		friend class SpriteBatch;
		friend class Application;
		friend class CommandCenter;

		/** \brief Return instance of RenderSystem. 
		* \return Pointer to RenderSystem.
		*/
		static RenderSystem* GetInstance();

		/** \brief Set active pm::Camera for rendering.
		* \param[in] camera Pointer to pm::Camera.
		void SetActiveCamera(Camera* camera) { (this->activeCamera) = camera; } */
		
		/** \brief Return handle to pm::Camera currently being used.
		* \return Pointer to pm::Camera.
		Camera* GetActiveCamera() { return activeCamera; } */

		/** \internal Following four functions set private as not to confuse user.
		* For example RenderSystem and SpriteBatch both have Draw().
		* Also public Initialize function might imply user needs to initialize RenderSystem himself. */
	private:
		void DestroyInstance(); /// Removes current instance.

		void Initialize(); ///< RenderSystem should be initialized when context is ready.
		
		void Draw(Batch* batch); ///< renders one patch.

		bool IsInitialized();

	private:

		RenderSystem() : shaderProgram(), vertexBuffer(), indexBuffer() {};

		void BindBuffers(Batch* batch); ///< Binds buffers before rendering patch.

		void CreateShaders(); // May be changed.

		GLint transformMatrixLocation;
		GLint cameraMatrixLocation;
		GLint projectionLocation;
		
		Shader shaderProgram;

		Buffer vertexBuffer, indexBuffer;

		static RenderSystem* instance;

		static bool initialized;

		~RenderSystem();
	};
}

#endif