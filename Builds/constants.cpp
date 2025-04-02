
#include <Eigen/Core>

const int width = 100;  // Terrain width (in cells)
const int height = 100; // Terrain height (in cells)

Eigen::MatrixXd V;  // Vertex positions (x, y, z)
Eigen::MatrixXi F;  // Face indices (connect the vertices into triangles)

const float flying = 0;  // Animation variable

const double water_level = 3.0;
const double snow_level = 5.0;

const double time = 0.0;