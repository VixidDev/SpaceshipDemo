#include <catch2/catch_amalgamated.hpp>

#include "../vmlib/mat44.hpp"
#include "../vmlib/vec4.hpp"


TEST_CASE("Matrix Multiplication", "[mat44]") {

	Mat44f matrix1 = { {5.f, 2.f, 6.f, 1.f,
						0.f, 6.f, 2.f, 0.f,
						3.f, 8.f, 1.f, 4.f,
						1.f, 8.f, 5.f, 6.f } };

	Mat44f matrix2 = { {7.f, 5.f, 8.f, 0.f,
						1.f, 8.f, 2.f, 6.f,
						9.f, 4.f, 3.f, 8.f,
						5.f, 3.f, 7.f, 9.f } };

	Mat44f resultMatrix = { {96.f, 68.f, 69.f, 69.f,
							24.f, 56.f, 18.f, 52.f,
							58.f, 95.f, 71.f, 92.f,
							90.f, 107.f, 81.f, 142.f } };

	Mat44f testMatrix = matrix1 * matrix2;

	REQUIRE(testMatrix == resultMatrix);
}

TEST_CASE("Matrix Vector Multiplication", "[mat44]") {

	Mat44f matrix1 = { {5.f, 2.f, 6.f, 1.f,
						0.f, 6.f, 2.f, 0.f,
						3.f, 8.f, 1.f, 4.f,
						1.f, 8.f, 5.f, 6.f } };

	Vec4f vector1 = { 1.f, 2.f, 3.f, 4.f };

	Vec4f resultVector = { 31.f, 18.f, 38.f, 56.f };

	Vec4f testVector = matrix1 * vector1;

	REQUIRE(testVector == resultVector);
}

TEST_CASE("Rotation X", "[mat44]") {

 	Mat44f result = { {
		1.f, 0.f,		0.f,		0.f,
		0.f, cosf(1.f), -sinf(1.f), 0.f,
		0.f, sinf(1.f), cosf(1.f),  0.f, 
		0.f, 0.f,		0.f,		1.f}};

	Mat44f test = make_rotation_x(1.f);

	REQUIRE(result == test);
}

TEST_CASE("Rotation Y", "[mat44]") {

	Mat44f result = { { cosf(1.f),  0.f, sinf(1.f), 0.f,
						0.f,	    1.f, 0.f,       0.f,
						-sinf(1.f), 0.f, cosf(1.f), 0.f,
						0.f,        0.f, 0.f,      1.f
		} };

	Mat44f test = make_rotation_y(1.f);

	REQUIRE(result == test);
}

TEST_CASE("Rotation Z", "[mat44]") {

	Mat44f result = { { cosf(1.f), -sinf(1.f), 0.f, 0.f,
						sinf(1.f),  cosf(1.f), 0.f, 0.f,
						0.f,		0.f,       1.f, 0.f,
						0.f,        0.f,       0.f, 1.f
		} };

	Mat44f test = make_rotation_z(1.f);

	REQUIRE(result == test);
}

TEST_CASE("Translation", "[mat44]") {
	Mat44f result = { { 1.f, 0.f, 0.f, 10.f,
						0.f, 1.f, 0.f, 10.f,
						0.f, 0.f, 1.f, 10.f,
						0.f, 0.f, 0.f, 1.f
		} };
	Vec3f vector = { 10.f, 10.f, 10.f};
	Mat44f test = make_translation(vector);
	REQUIRE(result == test);
}

TEST_CASE("Scaling", "[mat44]") {
	Mat44f result = { {5.f, 0.f,  0.f,  0.f,
					   0.f, 10.f, 0.f,  0.f,
					   0.f, 0.f,  15.f, 0.f,
					   0.f, 0.f,  0.f,  1.f
		} };
	Mat44f test = make_scaling(5.f, 10.f, 15.f);
	REQUIRE(result == test);
}

TEST_CASE("Perspective Projection") {
	float pi = atan(1.f) * 4;
	float fov = pi / 2.f;
	float aspect = 16.f / 9.f;
	float near = 0.1f;
	float far = 100.f;
	Mat44f result = { { (9.f/16.f), 0.f, 0.f,		 0.f,
						0.f,		1.f, 0.f,		 0.f,
						0.f,		0.f, -1.002002f, -0.2002002f,
						0.f,		0.f, -1.f,		 0.f
} };
	Mat44f test = make_perspective_projection(fov, aspect, near, far);
	REQUIRE(result == test);

}


