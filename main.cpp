#include <iostream>
#include <vector>
#include <cmath>
#include <Eigen/Core>
#include <igl/opengl/glfw/Viewer.h>

#include "src/constants.h"

extern void develop_terrain(Eigen::MatrixXd& terrain_V, Eigen::MatrixXi& terrain_F, igl::opengl::glfw::Viewer& viewer);
extern void develop_water(Eigen::MatrixXd& water_V, Eigen::MatrixXi& water_F, Eigen::MatrixXd& terrain_V, igl::opengl::glfw::Viewer& viewer);
extern void update_water_animation(Eigen::MatrixXd& water_V, Eigen::MatrixXi& water_F, double time, igl::opengl::glfw::Viewer& viewer);



int main() {

    Eigen::MatrixXd terrain_V;  // Vertex positions (x, y, z)
    Eigen::MatrixXi terrain_F;  // Face indices (connect the vertices into triangles)

    Eigen::MatrixXd water_V;  // Vertex positions (x, y, z)
    Eigen::MatrixXi water_F;  // Face indices (connect the vertices into triangles)



    igl::opengl::glfw::Viewer viewer;  // Initialize the libigl viewer

    develop_terrain(terrain_V, terrain_F, viewer);

    develop_water(water_V, water_F, terrain_V, viewer);

    double time = 0.0;
    viewer.callback_pre_draw = [&](igl::opengl::glfw::Viewer& viewer) {
        time += 0.05; // Increment time
    
        // Update vertex positions to cbreate a wave effect for the second mesh
        Eigen::MatrixXd new_V = water_V; // Copy original vertices
        for (int i = 0; i < new_V.rows(); ++i) {
            // Create a wave effect based on the x-coordinate
            new_V(i, 2) = 1 + 10 * std::sin(2 * PI * (new_V(i, 0) + time)); // Wave effect
        }
    
        // Update the mesh in the viewer
        viewer.data(1).set_mesh(new_V, water_F);
        return false; // Return false to continue the animation
    };

    // Enable continuous animation
    viewer.core().is_animating = true; // Set to true for continuous updates

    // Launch the viewer
    viewer.launch();


    return 0;
}


