#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "Shape.h"

namespace pm
{
	class Rectangle : public Shape
	{
	public:

		Rectangle() : width(0), height(0) { 
		SetIndices();
		SetVertices();
		};
		Rectangle(float width, float height);
		Rectangle(glm::vec2 size);

		void SetOrigin(glm::vec2 newOrigin);
		void SetOrigin(float newOriginX, float newOriginY);
		void SetSize(glm::vec2 newSize);
		void SetSize(float newSizeX, float newSizeY);
		
	private:
		
		void SetVertices();
		void SetIndices();

		float width, height;

	};
}

#endif