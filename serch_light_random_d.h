#include <stdlib.h>
#include "struct_vec.h"
#ifndef SERCH_LIGHT_RANDOM_D_H
#define SERCH_LIGHT_RANDOM_D_H

// main.c で定義されたシーンを参照
extern Object scene[];
extern int scene_count;

static inline Vec generate_random_d(Vec o, Vec normal){
    normal = normalize(normal);
    Vec dx = normal;
    Vec dy;
    if ((normal.x) > -0.9 && (normal.x) < 0.9){
        Vec x = {1.0, 0.0, 0.0};
        dy = normalize(cross(normal, x));
    } else {
        Vec x = {0.0, 1.0, 0.0};
        dy = normalize(cross(normal, x));
    }
    Vec dz = normalize(cross(dx, dy));
    Vec xyz = {0, 0, 0};
    while(1){
        double ux = 2 * ((double)rand() / RAND_MAX) - 1;
        double uy = 2 * ((double)rand() / RAND_MAX) - 1;
        double uz = 2 * ((double)rand() / RAND_MAX) - 1;
        if (ux < 0) ux = -ux;
        Vec r = add(mul(dx, ux), add(mul(dy, uy), mul(dz, uz)));
        if (len(r) > 1.0) continue;
        xyz = normalize(r);
        break;
    }
    return xyz;
}

static inline Vec search_light(Vec normal, Vec o, Vec obj_col){
    Vec direct_lightning = {0.0, 0.0, 0.0};
    for (int i = 0; i < scene_count; i++){
        if (scene[i].type == LIGHT){
            Vec light_pos = scene[i].li.o;
            Vec light_d = sub(light_pos, o);
            double distance = len(light_d);
            Vec light_n = normalize(light_d);
            double cos_theta = dot(normal, light_n);
            if (cos_theta <= 0) continue;
            int shadowed = 0;
            for (int j = 0; j < scene_count; j++){
                if (i == j || scene[j].type == LIGHT) continue;
                Hitrecord h = intersect_function(o, light_n, scene[j], j);
                if (h.t > 0.001 && h.t < distance){ shadowed = 1; break; }
            }
            if (!shadowed){
                Vec light_color = hadamard(scene[i].col, obj_col);
                direct_lightning = add(direct_lightning, mul(light_color, cos_theta));
            }
        }
    }
    return direct_lightning;
}

#endif