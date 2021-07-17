
#include <iostream>
#include <fstream>
#include <Eigen/core>

#include "ray.h"
#include "hittable_list.h"
#include "ball.h"
#include "camera.h"

#define PRECISION 1E-3

using namespace std;
using namespace Eigen;

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}
inline double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}
inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}
inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

Vector3f ray_color(const ray& r, const hittable& world) {

    hit_record rec;
    if (world.hit(r, 0, infinity, rec)) {
        return 0.5 * (rec.n + Vector3f(1, 1, 1));
    }

    Vector3f dir = r.direction();
    dir /= dir.norm();
    auto t = 0.5 * (dir.y() + 1.0);
    return t * Vector3f(1.0, 1.0, 1.0) + (1.0 - t) * Vector3f(0.5, 0.7, 1.0);
}

void write_color(std::ostream& out, Vector3f pixel_color) {

    out << static_cast<int>(255.999 * pixel_color.x()) << ' '
        << static_cast<int>(255.999 * pixel_color.y()) << ' '
        << static_cast<int>(255.999 * pixel_color.z()) << '\n';

}

int main() {

    // Image

    const auto aspect_ratio = 16.0 / 9.0;    //宽高比
    const int image_width = 320;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;

    // World
    hittable_list world;
    world.add(make_shared<ball>(Vector3f(0, 0, -1), 0.5));
    world.add(make_shared<ball>(Vector3f(0, -100.5, -1), 100));

    // Camera
    camera cam;

    // Render

    ofstream ppmFile;
    ppmFile.open("ppmFile.ppm");

    ppmFile << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; j--) {
        std::cerr << "\rScanlines processing: " << image_height - j << " / " << image_height << std::flush;

        for (int i = 0; i < image_width; i++) {

            Vector3f pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world);
            }
            pixel_color /= samples_per_pixel;       // 对每条光线得到的颜色值取平均

            write_color(ppmFile, pixel_color);
        }
        
    }

    std::cerr << "\nDone.\n" << std::flush;

    
}