#ifndef MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA
#define MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA

#include <cmath>
#include <cassert>
#include <cstdlib>

#include "vec3.hpp"
#include "vec4.hpp"
#include <iostream>

/** Mat44f: 4x4 matrix with floats
 *
 * See vec2f.hpp for discussion. Similar to the implementation, the Mat44f is
 * intentionally kept simple and somewhat bare bones.
 *
 * The matrix is stored in row-major order (careful when passing it to OpenGL).
 *
 * The overloaded operator () allows access to individual elements. Example:
 *    Mat44f m = ...;
 *    float m12 = m(1,2);
 *    m(0,3) = 3.f;
 *
 * The matrix is arranged as:
 *
 *   ⎛ 0,0  0,1  0,2  0,3 ⎞
 *   ⎜ 1,0  1,1  1,2  1,3 ⎟
 *   ⎜ 2,0  2,1  2,2  2,3 ⎟
 *   ⎝ 3,0  3,1  3,2  3,3 ⎠
 */
struct Mat44f
{
	float v[16];

	constexpr
	float& operator() (std::size_t aI, std::size_t aJ) noexcept
	{
		assert( aI < 4 && aJ < 4 );
		return v[aI*4 + aJ];
	}
	constexpr
	float const& operator() (std::size_t aI, std::size_t aJ) const noexcept
	{
		assert( aI < 4 && aJ < 4 );
		return v[aI*4 + aJ];
	}
};

// Identity matrix
constexpr Mat44f kIdentity44f = { {
	1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f
} };

// Common operators for Mat44f.
// Note that you will need to implement these yourself.
constexpr
bool operator==(Mat44f const& aLeft, Mat44f const& aRight) noexcept{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (aLeft(i, j) != aRight(i, j)) {
				return false;
			}
		}
	}
	return true;
}



constexpr
Mat44f operator*( Mat44f const& aLeft, Mat44f const& aRight ) noexcept
{


	Mat44f ret = { {0.f, 0.f, 0.f, 0.f,
					0.f, 0.f, 0.f, 0.f,
					0.f, 0.f, 0.f, 0.f,
					0.f, 0.f, 0.f, 0.f} };

	ret(0, 0) = aLeft(0, 0) * aRight(0, 0) + aLeft(0, 1) * aRight(1, 0) + aLeft(0, 2) * aRight(2, 0) + aLeft(0, 3) * aRight(3, 0);
	ret(0, 1) = aLeft(0, 0) * aRight(0, 1) + aLeft(0, 1) * aRight(1, 1) + aLeft(0, 2) * aRight(2, 1) + aLeft(0, 3) * aRight(3, 1);
	ret(0, 2) = aLeft(0, 0) * aRight(0, 2) + aLeft(0, 1) * aRight(1, 2) + aLeft(0, 2) * aRight(2, 2) + aLeft(0, 3) * aRight(3, 2);
	ret(0, 3) = aLeft(0, 0) * aRight(0, 3) + aLeft(0, 1) * aRight(1, 3) + aLeft(0, 2) * aRight(2, 3) + aLeft(0, 3) * aRight(3, 3);

	ret(1, 0) = aLeft(1, 0) * aRight(0, 0) + aLeft(1, 1) * aRight(1, 0) + aLeft(1, 2) * aRight(2, 0) + aLeft(1, 3) * aRight(3, 0);
	ret(1, 1) = aLeft(1, 0) * aRight(0, 1) + aLeft(1, 1) * aRight(1, 1) + aLeft(1, 2) * aRight(2, 1) + aLeft(1, 3) * aRight(3, 1);
	ret(1, 2) = aLeft(1, 0) * aRight(0, 2) + aLeft(1, 1) * aRight(1, 2) + aLeft(1, 2) * aRight(2, 2) + aLeft(1, 3) * aRight(3, 2);
	ret(1, 3) = aLeft(1, 0) * aRight(0, 3) + aLeft(1, 1) * aRight(1, 3) + aLeft(1, 2) * aRight(2, 3) + aLeft(1, 3) * aRight(3, 3);

	ret(2, 0) = aLeft(2, 0) * aRight(0, 0) + aLeft(2, 1) * aRight(1, 0) + aLeft(2, 2) * aRight(2, 0) + aLeft(2, 3) * aRight(3, 0);
	ret(2, 1) = aLeft(2, 0) * aRight(0, 1) + aLeft(2, 1) * aRight(1, 1) + aLeft(2, 2) * aRight(2, 1) + aLeft(2, 3) * aRight(3, 1);
	ret(2, 2) = aLeft(2, 0) * aRight(0, 2) + aLeft(2, 1) * aRight(1, 2) + aLeft(2, 2) * aRight(2, 2) + aLeft(2, 3) * aRight(3, 2);
	ret(2, 3) = aLeft(2, 0) * aRight(0, 3) + aLeft(2, 1) * aRight(1, 3) + aLeft(2, 2) * aRight(2, 3) + aLeft(2, 3) * aRight(3, 3);

	ret(3, 0) = aLeft(3, 0) * aRight(0, 0) + aLeft(3, 1) * aRight(1, 0) + aLeft(3, 2) * aRight(2, 0) + aLeft(3, 3) * aRight(3, 0);
	ret(3, 1) = aLeft(3, 0) * aRight(0, 1) + aLeft(3, 1) * aRight(1, 1) + aLeft(3, 2) * aRight(2, 1) + aLeft(3, 3) * aRight(3, 1);
	ret(3, 2) = aLeft(3, 0) * aRight(0, 2) + aLeft(3, 1) * aRight(1, 2) + aLeft(3, 2) * aRight(2, 2) + aLeft(3, 3) * aRight(3, 2);
	ret(3, 3) = aLeft(3, 0) * aRight(0, 3) + aLeft(3, 1) * aRight(1, 3) + aLeft(3, 2) * aRight(2, 3) + aLeft(3, 3) * aRight(3, 3);

	return ret;

	
}

constexpr
Vec4f operator*( Mat44f const& aLeft, Vec4f const& aRight ) noexcept
{
	//Vec4f vec;
	return Vec4f{
		aLeft(0, 0) * aRight.x + aLeft(0, 1) * aRight.y + aLeft(0, 2) * aRight.z + aLeft(0, 3) * aRight.w,
		aLeft(1, 0) * aRight.x + aLeft(1, 1) * aRight.y + aLeft(1, 2) * aRight.z + aLeft(1, 3) * aRight.w,
		aLeft(2, 0) * aRight.x + aLeft(2, 1) * aRight.y + aLeft(2, 2) * aRight.z + aLeft(2, 3) * aRight.w,
		aLeft(3, 0) * aRight.x + aLeft(3, 1) * aRight.y + aLeft(3, 2) * aRight.z + aLeft(3, 3) * aRight.w
	};
}

// Functions:

Mat44f invert( Mat44f const& aM ) noexcept;

// Rotation method to rotate a matrix by an angle on a certain axis. Requires angle to be passed in as radians
// From https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
inline
Mat44f rotate(float angle, Vec3f axis) noexcept
{
	Mat44f ret = kIdentity44f;

	float cos = std::cos(angle);
	float sin = std::sin(angle);

	ret(0, 0) = cos + (axis.x * axis.x) * (1.f - cos);
	ret(0, 1) = axis.x * axis.y * (1.f - cos) - axis.z * sin;
	ret(0, 2) = axis.x * axis.z * (1.f - cos) + axis.y * sin;
	ret(1, 0) = axis.y * axis.x * (1.f - cos) + axis.z * sin;
	ret(1, 1) = cos + (axis.y * axis.y) * (1.f - cos);
	ret(1, 2) = axis.y * axis.z * (1.f - cos) - axis.x * sin;
	ret(2, 0) = axis.z * axis.x * (1.f - cos) - axis.y * sin;
	ret(2, 1) = axis.z * axis.y * (1.f - cos) + axis.x * sin;
	ret(2, 2) = cos + (axis.z * axis.z) * (1.f - cos);

	return ret;
}

inline
Mat44f transpose( Mat44f const& aM ) noexcept
{
	Mat44f ret;
	for( std::size_t i = 0; i < 4; ++i )
	{
		for( std::size_t j = 0; j < 4; ++j )
			ret(j,i) = aM(i,j);
	}
	return ret;
}

inline
Mat44f make_rotation_x( float aAngle ) noexcept
{
	float cosTheta = cosf(aAngle);
	float sinTheta = sinf(aAngle);

	return {{ 1.f, 0.f,       0.f,      0.f,
			  0.f, cosTheta, -sinTheta, 0.f,
			  0.f, sinTheta,  cosTheta, 0.f,
			  0.f, 0.f,       0.f,      1.f
		}};
}

inline
Mat44f make_rotation_y( float aAngle ) noexcept
{
	float cosTheta = cosf(aAngle);
	float sinTheta = sinf(aAngle);
	return {{ cosTheta, 0.f, sinTheta, 0.f,
			  0.f,1.f,  0.f,           0.f,
			 -sinTheta, 0.f, cosTheta, 0.f,
			  0.f,      0.f, 0.f,      1.f
		}};
}

inline
Mat44f make_rotation_z( float aAngle ) noexcept
{
	float cosTheta = cosf(aAngle);
	float sinTheta = sinf(aAngle);
	return {{ cosTheta, -sinTheta, 0.f, 0.f,
			  sinTheta,  cosTheta, 0.f, 0.f,
			  0.f,		 0.f,      1.f, 0.f,
			  0.f,       0.f,      0.f, 1.f
		}};
}

inline
Mat44f make_translation( Vec3f aTranslation ) noexcept
{
	return {{ 1.f, 0.f, 0.f, aTranslation.x,
			  0.f, 1.f, 0.f, aTranslation.y,
			  0.f, 0.f, 1.f, aTranslation.z,
			  0.f, 0.f, 0.f, 1.f
		}};
}

inline
Mat44f make_scaling( float aSX, float aSY, float aSZ ) noexcept
{
	return {{aSX, 0.f, 0.f, 0.f,
			0.f,  aSY, 0.f, 0.f,
			0.f,  0.f, aSZ, 0.f,
			0.f,  0.f, 0.f, 1.f		
		}};
}


inline
Mat44f make_perspective_projection( float aFovInRadians, float aAspect, float aNear, float aFar ) noexcept
{
	float tanHalfFov = tanf(aFovInRadians / 2.f);
	float sx = 1 / (aAspect * tanHalfFov);
	float sy = 1 / tanHalfFov;
	float a = -((aFar + aNear) / (aFar - aNear));
	float b = -2.f * ((aFar * aNear) / (aFar - aNear));

	return {{ sx,  0.f,  0.f, 0.f,
			  0.f, sy,   0.f, 0.f,
			  0.f, 0.f,  a,   b,
			  0.f, 0.f, -1.f, 0.f 
		}};
}

// Orthographic projection matrix creation for text UI elements
inline
Mat44f make_orthographic_projection(float left, float right, float bottom, float top) noexcept
{
	Mat44f ret = kIdentity44f;

	ret(0, 0) = 2.f / (right - left);
	ret(1, 1) = 2.f / (top - bottom);
	ret(2, 2) = -1.f;
	ret(0, 3) = -(right + left) / (right - left);
	ret(1, 3) = -(top - bottom) / (top + bottom);

	return ret;
}

// lookAt matrix creation
inline
Mat44f look_at(Vec3f cameraPosition, Vec3f pointToLookAt, Vec3f up) noexcept
{
	Vec3f forward = normalize(pointToLookAt - cameraPosition);
	Vec3f right =   normalize(cross(forward, up));
	Vec3f newUp =   cross(right, forward);

	return Mat44f{{  right.x,	 right.y,	 right.z,	-dot(right, cameraPosition),
					 newUp.x,	 newUp.y,	 newUp.z,	-dot(newUp, cameraPosition),
					-forward.x, -forward.y, -forward.z,  dot(forward, cameraPosition),
					 0.f,		 0.f,		 0.f,		 1.f }};
}


#endif // MAT44_HPP_E7187A26_469E_48AD_A3D2_63150F05A4CA
