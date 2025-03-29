#include <iostream>
#include <vector>
#include <cmath>
#include <Eigen/Core>
#include <igl/opengl/glfw/Viewer.h>

extern void generate_terrain(std::vector<std::vector<float>>& terrain, int width, int height, float scale, float& flying);
extern void create_terrain_mesh(const std::vector<std::vector<float>>& terrain, int width, int height, Eigen::MatrixXd& V, Eigen::MatrixXi& F);
extern Eigen::MatrixXd generate_visualization(const Eigen::MatrixXd& V, double water_level, double time);

int main() {
    const int width = 100;  // Terrain width (in cells)
    const int height = 100; // Terrain height (in cells)
    std::vector<std::vector<float>> terrain(width, std::vector<float>(height));

    Eigen::MatrixXd V;  // Vertex positions (x, y, z)
    Eigen::MatrixXi F;  // Face indices (connect the vertices into triangles)

    float flying = 0;  // Animation variable

    // Generate the terrain based on Perlin noise
    generate_terrain(terrain, width, height, 20.0f, flying);
    create_terrain_mesh(terrain, width, height, V, F);

    // Generate Mesh Color 

    //double water_level = 3.0;
    //double snow_level = 5.0;
    //Eigen::MatrixXd color = generate_visualization(V, water_level);

    // Initialize the libigl viewer
    igl::opengl::glfw::Viewer viewer;

    // Set the mesh and color in the viewer
    // Water level
    double water_level = 0.5;
    double time = 0.0;

    // Callback for animation (runs every frame)
    viewer.callback_pre_draw = [&](igl::opengl::glfw::Viewer&) -> bool {
        time += 1; // Increase time for animation
        Eigen::MatrixXd color = generate_visualization(V, water_level, time);
        viewer.data().set_colors(color); // Update vertex colors dynamically
        return false; // Continue rendering
        };
    viewer.data().set_mesh(V, F);/*
    viewer.data().set_colors(color);*/

    // Launch the viewer
    viewer.launch();

    return 0;
}
