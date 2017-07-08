#pragma once

// User-defined Headers
#include "Vertex.h"

// System Headers
#include <utility>
#include <vector>

// Define PI for local use
#define M_PI 3.1415926535897932384626433832795
#define M_2PI M_PI * 2

namespace Engine {

	void CreateSphere(float radius, float slices, float stacks, std::vector<Vertex> * vertices, std::vector<uint16_t> * indices) {

		for (int i = 0; i <= stacks; ++i) {

			// V texture coordinate
			double V = i / static_cast<double>(stacks);
			double phi = V * M_PI;

			for (int j = 0; j <= slices; ++j)
			{
				// U texture coordinate
				double U = j / static_cast<double>(slices);
				double theta = U * M_2PI;

				double X = cos(theta) * sin(phi);
				double Y = cos(phi);
				double Z = sin(theta) * sin(phi);

				// Add this vertex (with white color)
				vertices->push_back({ 
					{X * radius, Y * radius, Z * radius }, // Vertex Position
					{1, 1, 1}, // Vertex Color
					glm::vec2(U, V) * glm::vec2(-1, 1) // Texture Coordinates
					});
			}
		}

		for (uint16_t i = 0; i < slices * stacks + slices; ++i) {
			indices->push_back(i);
			indices->push_back(static_cast<uint16_t>(i + slices + 1));
			indices->push_back(static_cast<uint16_t>(i + slices));

			indices->push_back(static_cast<uint16_t>(i + slices + 1));
			indices->push_back(static_cast<uint16_t>(i));
			indices->push_back(static_cast<uint16_t>(i + 1));
		}
	}
}