#ifndef VECTOR2_H
#define VECTOR2_H
#include <core\Log.h>

namespace pm
{
	/**
	*Engines own Vector2 that can be used to make fast 2 place vectors 
	*/
	template<typename T>
	class Vector2
	{
		Vector2<T> operator<<(const Vector2<T>& v)
		{
			DEBUG_INFO(("&f, &f", v.x, v.y));
		}

	public:
		/// Constructor for Vector2
		Vector2(T x, T y)
		{
			this->x = x;
			this->y = y;
		};
		~Vector2(){};

		/// overloaded operator +
		Vector2<T> operator + (const Vector2<T>& right)
		{
			x = x + right.x;
			y = y + right.y;
			return *this;
		}
		/// overloaded operator -
		Vector2<T> operator - (const Vector2<T>& right)
		{
			x = x - right.x;
			y = y - right.y;
			return *this;
		}
		/// overloaded operator *=
		Vector2<T> operator *= (Vector2<T>& left)
		{
			x *= left.x;
			y *= left.y;
			return *this;
		}
		/// overloaded operator /=
		Vector2<T> operator /= (Vector2<T>& left)
		{
			x /= left.x;
			y /= left.y;
			return *this;
		}
		/// overloaded operator +=
		Vector2<T> operator += (Vector2<T>& left)
		{
			x += left.x;
			y += left.y;
			return *this;
		}
		/// overloaded operator -=
		Vector2<T> operator -= (Vector2<T>& left)
		{
			x -= left.x;
			y -= left.y;
			return *this;
		}

		T x, y;

	};
}
#endif
