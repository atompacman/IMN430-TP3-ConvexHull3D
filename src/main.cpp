#include <fstream>
#include <vector>
#include "ConvexHull3D.h"
#include "Point.h"

std::vector<Point> readVertexFile(const char* i_Filepath)
{
	// Open file
	std::ifstream file(i_Filepath);

	// Check that it opened correctly
	if (file.bad()) {
		std::cerr << "Invalid file \"" << i_Filepath << "\"" << std::endl;
	}

	// Read points
	double x, y, z;
	std::vector<Point> pts;
	while (!file.eof()) {
		file >> x;
		file >> y;
		file >> z;
		pts.emplace_back(x, y, z);
	}

	return pts;
}

int main(int argc, const char** argv)
{
	// Check that there's a vertex list file path in arguments
	if (argc != 2) {
		std::cerr << "Expected a vertex list file path in arguments" << std::endl;
	}

	// Read vertex in file
	std::vector<Point> pts(readVertexFile(argv[1]));

	// Compute convex hull
	compute3DConvexHull(pts);
}