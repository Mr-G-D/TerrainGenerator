
#include <Eigen/Core>

const int width = 100;  // Terrain width (in cells)
const int height = 100; // Terrain height (in cells)

inline float flying = 0;  // Animation variable

const double snow_level = 5.0;

const double PI = 3.14159265358979323846;

// Define wave parameters
const double wave_amplitude = 1.0; // Amplitude of the waves
const double wave_frequency = 5.0;  // Frequency of the waves
const double wave_speed = 5.0;      // Speed of the wave propagation
const double water_level = 2.0;     // Base water level

// Constants
const int GRID_SIZE = 50;
const double WATER_HEIGHT_OFFSET = 0.2; // Water level slightly above terrain
const double WAVE_AMPLITUDE = 0.1;
const double WAVE_SPEED = 2.0 * PI / 10.0; // Wave speed factor