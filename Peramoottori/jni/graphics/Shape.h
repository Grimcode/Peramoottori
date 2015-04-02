#ifndef SHAPE_H
#define SHAPE_H

#include <scene\Component.h>
#include <scene\Transformable.h>
#include <glm\common.hpp>
#include <vector>

static const double PI = 3.141;

namespace pm
{
	class Shape : public Component
	{
	public:

		Shape() : Component(){};

		virtual void SetOrigin(glm::vec2 newOrigin)
		{
			origin = newOrigin;
		}
		virtual void SetOrigin(float x, float y)
		{
			origin = { x, y };
		}


	protected:


		std::vector<float> vertices;
		std::vector<unsigned int> indices;

		glm::vec2 origin = { 0.0f, 0.0f };
		
		float colorRed, colorGreen, colorBlue;

	};
}

#endif