#pragma once

#include "simple_mesh.hpp"

#include "../vmlib/mat44.hpp"

TexturelessSimpleMeshData makeCylinder(std::size_t subDivs = 16, Vec3f color = { 1.f, 1.f, 1.f }, Mat44f preTransform = kIdentity44f, bool isCapped = true);
TexturelessSimpleMeshData makeCone(std::size_t subDivs, Vec3f color, Mat44f preTransform = kIdentity44f, bool isCapped = true);
TexturelessSimpleMeshData makePyramid(Vec3f color, Mat44f preTransform = kIdentity44f);