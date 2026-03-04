#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <emscripten.h>
#include "struct_vec.h"
#include "intersection.h"
#include "intersectionpoint.h"

#define MAX_OBJECTS 64

Object scene[MAX_OBJECTS];
int scene_count = 0;

Camera cam = {
    {0, 0, -1.8},
    {0, 0, 1},
    512, 512,
    {0, 0, 0}
};

static unsigned char* pixel_buf = NULL;

EMSCRIPTEN_KEEPALIVE
void set_object_count(int n) {
    scene_count = n;
}

EMSCRIPTEN_KEEPALIVE
void set_object(int idx, int type, int mat,
                double r, double g, double b,
                double px, double py, double pz,
                double nx, double ny, double nz,
                double radius, double height,
                double wx, double wy, double wz,
                double lwidth, double lheight) {

    if (idx < 0 || idx >= MAX_OBJECTS) return;

    scene[idx].col  = (Vec){r, g, b};
    scene[idx].type = (Shapetype)type;
    scene[idx].mat  = (Material)mat;

    switch ((Shapetype)type) {
        case SPHERE:
            scene[idx].s.o = (Vec){px, py, pz};
            scene[idx].s.r = radius;
            break;
        case INFPLANE:
            scene[idx].i.o = (Vec){px, py, pz};
            scene[idx].i.n = (Vec){nx, ny, nz};
            break;
        case LIMITPLANE:
            scene[idx].l.o      = (Vec){px, py, pz};
            scene[idx].l.n      = (Vec){nx, ny, nz};
            scene[idx].l.w_vec  = (Vec){wx, wy, wz};
            scene[idx].l.width  = lwidth;
            scene[idx].l.height = lheight;
            break;
        case CYLINDER:
            scene[idx].c.o = (Vec){px, py, pz};
            scene[idx].c.n = (Vec){nx, ny, nz};
            scene[idx].c.r = radius;
            scene[idx].c.h = height;
            break;
        case LIGHT:
            scene[idx].li.o = (Vec){px, py, pz};
            break;
    }
}

EMSCRIPTEN_KEEPALIVE
void set_camera(double ox, double oy, double oz,
                double sx, double sy, double sz) {
    cam.o      = (Vec){ox, oy, oz};
    cam.screen = (Vec){sx, sy, sz};
}

EMSCRIPTEN_KEEPALIVE
unsigned char* render(int width, int height, int samples, int reflects, double decay) {

    if (pixel_buf) free(pixel_buf);
    pixel_buf = (unsigned char*)malloc(width * height * 4);
    if (!pixel_buf) return NULL;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            double px = ((double)x / width)  * 2.0 - 1.0;
            double py = -(((double)y / height) * 2.0 - 1.0);

            Vec ray = normalize(sub((Vec){px, py, cam.screen.z}, cam.o));

            Vec sum_color = {0, 0, 0};

            for (int i = 0; i < samples; i++) {
                Vec ray_d = ray;
                Vec ray_o = cam.o;
                Vec throughput = {1.0, 1.0, 1.0};
                Vec path_color = {0, 0, 0};

                for (int j = 0; j <= reflects; j++) {

                    Hitrecord best_hit = {1e30, -1};

                    for (int k = 0; k < scene_count; k++) {
                        Hitrecord h = intersect_function(ray_o, ray_d, scene[k], k);
                        if (h.scene_id != -1 && h.t < best_hit.t) {
                            best_hit = h;
                        }
                    }

                    if (best_hit.scene_id != -1) {
                        Object obj = scene[best_hit.scene_id];

                        if (obj.type == LIGHT) {
                            path_color = add(path_color, hadamard(throughput, obj.col));
                            break;
                        }
                        if (obj.mat == METAL) {
                            SurfaceResult res = intersect_metal(ray_o, ray_d, best_hit.t, obj);
                            throughput = hadamard(throughput, obj.col);
                            path_color = add(path_color, hadamard(throughput, res.color));
                            ray_o = res.next_o;
                            ray_d = res.next_d;
                        } else if (obj.mat == GLASS) {
                            break;
                        } else {
                            SurfaceResult res = intersect_point(ray_o, ray_d, best_hit.t, obj);
                            throughput = hadamard(throughput, obj.col);
                            throughput = mul(throughput, decay);
                            path_color = add(path_color, hadamard(throughput, res.color));
                            ray_o = res.next_o;
                            ray_d = res.next_d;
                        }
                    }
                }
                sum_color = add(sum_color, path_color);
            }

            Vec avg = mul(sum_color, 1.0 / samples);

            int idx = (y * width + x) * 4;
            pixel_buf[idx + 0] = (unsigned char)(fmax(0.0, fmin(1.0, avg.x)) * 255);
            pixel_buf[idx + 1] = (unsigned char)(fmax(0.0, fmin(1.0, avg.y)) * 255);
            pixel_buf[idx + 2] = (unsigned char)(fmax(0.0, fmin(1.0, avg.z)) * 255);
            pixel_buf[idx + 3] = 255;
        }
    }

    return pixel_buf;
}

EMSCRIPTEN_KEEPALIVE
int get_buffer_size(int width, int height) {
    return width * height * 4;
}
