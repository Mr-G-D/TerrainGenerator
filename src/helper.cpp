#include <iostream>
#include <vector>
#include <cmath>
#include <Eigen/Core>

extern int p[512];  // Declare the global array p
extern void init_permutation(int seed);  // Declare the function
extern float perlin_noise(float x, float y, int seed);  // Declare the function


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

Eigen::MatrixXd generate_water_visualization(const Eigen::MatrixXd& V, double water_level, double time) {
    Eigen::MatrixXd C(V.rows(), 3); // RGB colors

    // Water colors
    Eigen::RowVector3d water_deep(0.0, 0.1, 0.5);    // Deep water - dark blue
    Eigen::RowVector3d water_shallow(0.0, 0.7, 1.0); // Shallow water - light blue

    double wave_frequency = 0.05; // Controls wave density
    double wave_amplitude = 0.2;  // Controls wave height
    double wave_speed = 2.0;      // Controls wave movement speed

    for (int i = 0; i < V.rows(); ++i) {
        double height = V(i, 2);
        double x = V(i, 0);
        double y = V(i, 1);

        if (height < water_level) {
            // Simulating flowing water using sine waves
            double wave_effect = wave_amplitude * std::sin(wave_frequency * (x + y) + wave_speed * time);
            double adjusted_depth = water_level - height + wave_effect;

            // Normalize depth for color transition
            double depth_factor = adjusted_depth / water_level;
            depth_factor = std::clamp(depth_factor, 0.0, 1.0);

            // Interpolate between deep and shallow water colors
            C.row(i) = (1 - depth_factor) * water_shallow + depth_factor * water_deep;
        }
    }
    return C;
}

Eigen::MatrixXd generate_visualization(const Eigen::MatrixXd& V, double water_level, double time) {
    Eigen::MatrixXd C = generate_color_based_on_height(V, water_level);
    Eigen::MatrixXd water = generate_water_visualization(V, water_level, time);

    for (int i = 0; i < V.rows(); ++i) {
        if (V(i, 2) < water_level) {
            C.row(i) = water.row(i);
        }
    }
    return C;
}

