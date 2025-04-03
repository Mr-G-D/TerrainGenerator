#include <igl/opengl/glfw/Viewer.h>
#include <Eigen/Core>
#include <igl/read_triangle_mesh.h>

#include "../constants.h"

extern void generate_water_surface(Eigen::MatrixXd& water_V, Eigen::MatrixXi& water_F, const Eigen::MatrixXd& terrain_V, double water_level);


void develop_water(Eigen::MatrixXd& water_V, Eigen::MatrixXi& water_F, Eigen::MatrixXd& terrain_V, igl::opengl::glfw::Viewer& viewer) {

	generate_water_surface(water_V, water_F, terrain_V, water_level);

    //create_square_mesh(water_V, water_F, width, 100.0);


    viewer.append_mesh();
    viewer.data(1).set_mesh(water_V, water_F);
    viewer.data(1).set_colors(Eigen::RowVector3d(0.0, 0.5, 1.0)); // Blue tint
    viewer.data(1).invert_normals = true;
    // Make water slightly transparent
    viewer.data(1).show_lines = false;
    viewer.data(1).set_face_based(true);

	
}

