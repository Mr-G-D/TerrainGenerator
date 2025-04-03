#include <iostream>
#include <vector>
#include <cmath>
#include <Eigen/Core>
#include <igl/opengl/glfw/Viewer.h>

#include "constants.h"

extern int p[512];  // Declare the global array p
extern void init_permutation(int seed);  // Declare the function
extern float perlin_noise(float x, float y, int seed);  // Declare the function

// Create terrain mesh (vertices and faces)
void create_terrain_mesh(const std::vector<std::vector<float>>& terrain, int width, int height, Eigen::MatrixXd& V, Eigen::MatrixXi& F) {
    V.resize(width * height, 3);
    F.resize((width - 1) * (height - 1) * 2, 3);  // Two triangles per quad (triangle strip)

    // Fill vertices
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            V(y * width + x, 0) = x * 20;  // x-coordinate
            V(y * width + x, 1) = y * 20;  // y-coordinate
            V(y * width + x, 2) = terrain[x][y];  // z-coordinate (height)
        }
    }

    // Fill faces (two triangles per quad)
    int f = 0;
    for (int y = 0; y < height - 1; ++y) {
        for (int x = 0; x < width - 1; ++x) {
            int v0 = y * width + x;
            int v1 = y * width + (x + 1);
            int v2 = (y + 1) * width + x;
            int v3 = (y + 1) * width + (x + 1);

            // First triangle
            F.row(f++) << v0, v1, v2;
            // Second triangle
            F.row(f++) << v1, v3, v2;
        }
    }
}

// Generate terrain with Perlin noise (3D grid)
void generate_terrain(std::vector<std::vector<float>>& terrain, int width, int height, float scale, float& flying) {
    init_permutation(42);  // Seed for Perlin noise
    float flyingOffset = flying;

    for (int y = 0; y < height; ++y) {
        float xoff = 0;
        for (int x = 0; x < width; ++x) {
            terrain[x][y] = perlin_noise(xoff, flyingOffset, 0) * 200;  // Increase height scaling for more variation
            xoff += 0.1f;  // Adjust step size for x-axis noise
        }
        flyingOffset += 0.1f;  // Adjust step size for y-axis noise
    }

    flying -= 0.1f;  // Animate the terrain by adjusting flying offset
}

Eigen::MatrixXd generate_color_based_on_height(const Eigen::MatrixXd& V, double water_level, double transition_width = 10.0) {
    Eigen::MatrixXd C(V.rows(), 3); // RGB colors

    // Height normalization
    double min_height = V.col(2).minCoeff();
    double max_height = V.col(2).maxCoeff();

    // Define colors
    Eigen::RowVector3d lowland_brown(0.6, 0.4, 0.2);
    Eigen::RowVector3d mid_green(0.2, 0.6, 0.2);
    Eigen::RowVector3d highland_green(0.0, 0.8, 0.0);
    Eigen::RowVector3d snow_white(1.0, 1.0, 1.0);

    for (int i = 0; i < V.rows(); ++i) {
        double height = V(i, 2);
        double normalized_height = (height - min_height) / (max_height - min_height);

        if (height < water_level + transition_width) {
            double blend = (height - water_level) / transition_width;
            blend = std::clamp(blend, 0.0, 1.0);
            C.row(i) = blend * mid_green + (1 - blend) * Eigen::RowVector3d(0.0, 0.5, 1.0);  // Blends green and shallow blue
        }
        else if (normalized_height < 0.6) {
            C.row(i) = mid_green;
        }
        else if (normalized_height < 0.85) {
            C.row(i) = highland_green;
        }
        else {
            C.row(i) = snow_white;  // Peaks get snow
        }
    }
    return C;
}


// Function to create a flat water surface
void generate_water_surface(Eigen::MatrixXd& water_V, Eigen::MatrixXi& water_F, const Eigen::MatrixXd& terrain_V, double water_level) {
    // Create a simple grid matching the terrain size
    int grid_size = 80; // Adjust for better resolution
    water_V.resize(grid_size * grid_size, 3);
    water_F.resize((grid_size - 1) * (grid_size - 1) * 2, 3);

    double minX = terrain_V.col(0).minCoeff();
    double maxX = terrain_V.col(0).maxCoeff();
    double minY = terrain_V.col(1).minCoeff();
    double maxY = terrain_V.col(1).maxCoeff();

    // Generate grid vertices
    int index = 0;
    for (int i = 0; i < grid_size; ++i) {
        for (int j = 0; j < grid_size; ++j) {
            double x = minX + (maxX - minX) * i / (grid_size - 1);
            double y = minY + (maxY - minY) * j / (grid_size - 1);
            water_V.row(index++) = Eigen::RowVector3d(x, y, water_level);
        }
    }

    // Generate faces
    index = 0;
    for (int i = 0; i < grid_size - 1; ++i) {
        for (int j = 0; j < grid_size - 1; ++j) {
            int v1 = i * grid_size + j;
            int v2 = i * grid_size + (j + 1);
            int v3 = (i + 1) * grid_size + j;
            int v4 = (i + 1) * grid_size + (j + 1);

            // Triangle 1
            water_F.row(index++) = Eigen::RowVector3i(v1, v2, v3);
            // Triangle 2
            water_F.row(index++) = Eigen::RowVector3i(v2, v4, v3);
        }
    }


}

// Function to update water height for animation
void update_water_animation(Eigen::MatrixXd& water_V, Eigen::MatrixXi& water_F, double time, igl::opengl::glfw::Viewer& viewer) {
    // Update vertex positions to create a wave effect
    Eigen::MatrixXd new_V = water_V; // Copy original vertices
    for (int i = 0; i < new_V.rows(); ++i) {
        // Create a wave effect based on the x-coordinate
        new_V(i, 2) = 0.1 * std::sin(2 * PI * (new_V(i, 0) + time)); // Wave effect
    }

    viewer.data(1).set_mesh(new_V, water_F);
}

