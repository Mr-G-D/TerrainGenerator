
#include <Eigen/Core>
#include <igl/opengl/glfw/Viewer.h>

#include "../constants.h"

extern void generate_terrain(std::vector<std::vector<float>>& terrain, int width, int height, float scale, float& flying);
extern void create_terrain_mesh(const std::vector<std::vector<float>>& terrain, int width, int height, Eigen::MatrixXd& V, Eigen::MatrixXi& F);
Eigen::MatrixXd generate_color_based_on_height(const Eigen::MatrixXd& V, double water_level, double transition_width = 10.0);


void develop_terrain(Eigen::MatrixXd& terrain_V, Eigen::MatrixXi& terrain_F, igl::opengl::glfw::Viewer& viewer) {

	std::vector<std::vector<float>> terrain(width, std::vector<float>(height));

	// Generate the terrain based on Perlin noise
	generate_terrain(terrain, width, height, 20.0f, flying);
	create_terrain_mesh(terrain, width, height, terrain_V, terrain_F);

	Eigen::MatrixXd color = generate_color_based_on_height(terrain_V, water_level);    // Generate Mesh Color 

	viewer.data().set_mesh(terrain_V, terrain_F);
	viewer.data().show_lines = false;
	viewer.data().set_colors(color);
}

