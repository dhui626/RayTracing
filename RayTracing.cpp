
#include <iostream>
#include <fstream>
#include <Eigen/core>
#include <chrono>   

#include "ray.h"
#include "hittable_list.h"
#include "ball.h"
#include "camera.h"
#include "material.h"
#include "rtweekend.h"

#define PRECISION 1E-3

using namespace std;
using namespace Eigen;
using namespace chrono;

Vector3f ray_color(const ray& r, const hittable& world, int depth) {

    if (depth <= 0) return Vector3f(0, 0, 0);   //光线反射次数过多

    hit_record rec;
    if (world.hit(r, PRECISION, infinity, rec)) {
        ray scattered;
        Vector3f attenuation;

        if (rec.mat_ptr->scatter(r, attenuation, scattered, rec))
            return attenuation.array() * ray_color(scattered, world, depth - 1).array();
        return Vector3f(0, 0, 0);
    }

    Vector3f dir = r.direction();
    dir /= dir.norm();
    auto t = 0.5 * (dir.y() + 1.0);
    return (1.0 - t) * Vector3f(1.0, 1.0, 1.0) + t * Vector3f(0.5, 0.7, 1.0);
}

void write_color(std::ostream& out, Vector3f pixel_color) {

    out << static_cast<int>(255.999 * pixel_color.x()) << ' '
        << static_cast<int>(255.999 * pixel_color.y()) << ' '
        << static_cast<int>(255.999 * pixel_color.z()) << '\n';

}

hittable_list random_scene() {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(Vector3f(0.5, 0.5, 0.5));
    world.add(make_shared<ball>(Vector3f(0, -1000, 0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            Vector3f center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - Vector3f(4, 0.2, 0)).norm() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    Vector3f albedo = Vector3f::Random() / 2 + 0.5 * Vector3f(1, 1, 1);
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<ball>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95) {
                    // metal
                    Vector3f albedo = Vector3f::Random() / 4 + 0.75 * Vector3f(1, 1, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<ball>(center, 0.2, sphere_material));
                }
                else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<ball>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<ball>(Vector3f(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(Vector3f(0.4, 0.2, 0.1));
    world.add(make_shared<ball>(Vector3f(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(Vector3f(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<ball>(Vector3f(4, 1, 0), 1.0, material3));

    return world;
}

int main() {

    // Image

    const auto aspect_ratio = 3.0 / 2.0;
    const int image_width = 1200;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    // World
    /*hittable_list world;

    auto material_ground = make_shared<lambertian>(Vector3f(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(Vector3f(0.1, 0.2, 0.5));
    auto material_left   = make_shared<dielectric>(1.5);
    auto material_right  = make_shared<metal>(Vector3f(0.8, 0.6, 0.2), 0.0);

    world.add(make_shared<ball>(Vector3f(0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<ball>(Vector3f(0.0, 0.0, -1.0), 0.5, material_center));
    world.add(make_shared<ball>(Vector3f(-1.0, 0.0, -1.0), 0.5, material_left));
    world.add(make_shared<ball>(Vector3f(-1.0, 0.0, -1.0), -0.45, material_left));
    world.add(make_shared<ball>(Vector3f(1.0, 0.0, -1.0), 0.5, material_right));*/

    auto world = random_scene();
    
    // Camera
    Vector3f lookfrom(13, 2, 3);
    Vector3f lookat(0, 0, 0);
    Vector3f vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture, dist_to_focus);

    // Render

    ofstream ppmFile;
    ppmFile.open("ppmFile.ppm");

    ppmFile << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    auto start = system_clock::now();
    for (int j = image_height - 1; j >= 0; j--) {
        
        auto end = system_clock::now();
        auto duration = duration_cast<microseconds>(end - start) / (image_height - j) * j;

        std::cerr << "\rScanlines Processing: " << image_height - j << " / " << image_height 
            << "\tRemaining " << double(duration.count()) * microseconds::period::num / microseconds::period::den << " seconds."<< std::flush;

        for (int i = 0; i < image_width; i++) {

            Vector3f pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }
            pixel_color /= samples_per_pixel;       // 对每条光线得到的颜色值取平均

            // Gamma校正
            auto r = sqrt(pixel_color.x());
            auto g = sqrt(pixel_color.y());
            auto b = sqrt(pixel_color.z());
            pixel_color = Vector3f(r, g, b);

            write_color(ppmFile, pixel_color);
        }
        
    }

    std::cerr << "\nDone.\n" << std::flush;
    auto end = system_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout << "Spent " << double(duration.count()) * microseconds::period::num / microseconds::period::den << " seconds." << endl;
    
}

/* To Do
    添加方体
    GPU并行计算
*/