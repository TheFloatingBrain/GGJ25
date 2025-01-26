#include <Bubbles/Common.hpp>
#undef RAYMATH_DISABLE_CPP_OPERATORS
#include <raymath.h>

#ifndef BUBBLES_VECTOR_UTILITIES_INCLUDE_GUARD
#define BUBBLES_VECTOR_UTILITIES_INCLUDE_GUARD
namespace Bubbles
{
	inline const ::Vector3 b2rv(const btVector3& from) {
		return ::Vector3{from.x(), from.y(), from.z()};
	}

	inline const btVector3 r2bv(const ::Vector3& from) {
		return btVector3(from.x, from.y, from.z);
	}

	#define BUBBLES_DEF_RAY_VECTOR3_OPERATORS(OP_SYMBOL) \
		inline ::Vector3 operator OP_SYMBOL (const float left, const ::Vector3& right) \
		{ \
			return ::Vector3{  \
				left OP_SYMBOL right.x, \
				left OP_SYMBOL right.y, \
				left OP_SYMBOL right.z \
			}; \
		}

	BUBBLES_DEF_RAY_VECTOR3_OPERATORS(+)
	BUBBLES_DEF_RAY_VECTOR3_OPERATORS(-)
	BUBBLES_DEF_RAY_VECTOR3_OPERATORS(*)
	BUBBLES_DEF_RAY_VECTOR3_OPERATORS(/)

	//inline ::Vector3& operator=(::Vector3& to, const btVector3 from)
	//{
	//	to.x = from.x();
	//	to.y = from.y();
	//	to.z = from.z();
	//	return to;
	//}

	//inline btVector3& operator=(btVector3& to, const ::Vector3 from)
	//{
	//	to.x() = from.x;
	//	to.y() = from.y;
	//	to.z() = from.z;
	//	return to;
	//}

	inline ::Vector3 toRotation(const btQuaternion from)
	{
		::Vector3 rotation;
		from.getEulerZYX(rotation.z, rotation.y, rotation.x);
		return rotation;
	}

	inline float directionToYAngle(Vector3 direction) {
		return std::atan2(direction.x, direction.z);
	}
}
#endif // BUBBLES_VECTOR_UTILITIES_INCLUDE_GUARD

