#include "struct_vec.h"
#include "serch_light_random_d.h"
#ifndef INTERSECTIONPOINT_H  // 二重読み込み防止（おまじない）
#define INTERSECTIONPOINT_H


typedef struct{
    Vec next_d; // 反射したレイの方向
    Vec next_o; // 反射地点
    Vec color; // その地点で計算された色
} SurfaceResult;

// 交点をわずかに浮かせることで自分自身に当たるのを防ぐための関数, pが元の交点
static inline Vec offset_pos(Vec p, Vec n){
    return add(p, mul(n, 0.001));
}

static inline SurfaceResult intersect_point_sphere(Vec o, Vec d, double t, Sphere s, Vec obj_col){
    SurfaceResult res;
    res.next_o = add(o, mul(d, t)); // 交点を求める
    Vec normal = normalize(sub(res.next_o, s.o)); // 反射地点における法線
    
    res.next_o = offset_pos(res.next_o, normal); // 座標を浮かせる

    res.next_d = generate_random_d(res.next_o, normal); // 次の反射方向を作成
    res.color = search_light(normal, res.next_o, obj_col); // 直接光を計算
    return res;
}

static inline SurfaceResult intersect_point_infplane(Vec o, Vec d, double t, Infplane i, Vec obj_col){
    SurfaceResult res;
    res.next_o = add(o, mul(d, t));
    Vec normal = normalize(i.n);

    if (dot(d, normal) > 0){
        normal = mul(normal, -1.0);
    }

    res.next_o = offset_pos(res.next_o, normal); // 交点を浮かせる

    res.next_d = generate_random_d(res.next_o, normal);
    res.color = search_light(normal, res.next_o, obj_col);
    return res;
}

static inline SurfaceResult intersect_point_limitplane(Vec o, Vec d, double t, Limitplane l, Vec obj_col){
    SurfaceResult res;
    res.next_o = add(o, mul(d, t));
    Vec normal = normalize(l.n);

    if (dot(d, normal) > 0){
        normal = mul(normal, -1.0);
    }

    res.next_o = offset_pos(res.next_o, normal);

    res.next_d = generate_random_d(res.next_o, normal);
    res.color = search_light(normal, res.next_o, obj_col);
    return res;
}

static inline SurfaceResult intersect_point_cylinder(Vec o, Vec d, double t, Cylinder c, Vec obj_col){
    SurfaceResult res;
    res.next_o = add(o, mul(d, t));

    Vec cp = sub(res.next_o, c.o); // 底面中心から交点へのベクトル
    double h = dot(cp, c.n); // cpの円柱軸方向の成分

    Vec normal;
    if (h > c.h - 0.0003){
        normal = normalize(c.n); // 上面に当たった
    } else if (h < 0.0003) { // 下面
        normal = normalize(mul(c.n, -1.0));
    } else { // 側面
        normal = normalize(sub(cp, mul(c.n, h)));
    }

    res.next_o = offset_pos(res.next_o, normal);

    res.next_d = generate_random_d(res.next_o, normal);
    res.color = search_light(normal, res.next_o, obj_col);
    return res;
}

static inline SurfaceResult intersect_point(Vec o, Vec d, double t, Object obj) {

    SurfaceResult result;

    // switchはifみたいなもの。ここでは、ifよりみやすいので使う。
    switch (obj.type){
        case SPHERE:
            result =  intersect_point_sphere(o, d, t, obj.s, obj.col);
            break;
        case INFPLANE:
            result = intersect_point_infplane(o, d, t, obj.i, obj.col);
            break;
        case LIMITPLANE:
            result = intersect_point_limitplane(o, d, t, obj.l, obj.col);
            break;
        case CYLINDER:
            result = intersect_point_cylinder(o, d, t, obj.c, obj.col);
            break;
    }
    return result;
}

static inline SurfaceResult intersect_metal_sphere(Vec o, Vec d, double t, Sphere s, Vec obj_col){
    SurfaceResult res;
    res.next_o = add(o, mul(d, t)); // 交点を求める
    Vec normal = normalize(sub(res.next_o, s.o)); // 反射地点における法線
    
    if (dot(d, normal) > 0){
        normal = mul(normal, -1.0);
    }

    res.next_o = offset_pos(res.next_o, normal); // 座標を浮かせる

    res.next_d = sub(d, mul(normal, 2.0 * dot(d, normal))); // 次の反射方向を作成
    res.color = (Vec){0, 0, 0}; // 直接光を計算
    return res;
}

static inline SurfaceResult intersect_metal_infplane(Vec o, Vec d, double t, Infplane i, Vec obj_col){
    SurfaceResult res;
    res.next_o = add(o, mul(d, t));
    Vec normal = normalize(i.n);

    if (dot(d, normal) > 0){
        normal = mul(normal, -1.0);
    }

    res.next_o = offset_pos(res.next_o, normal); // 交点を浮かせる

    res.next_d = sub(d, mul(normal, 2.0 * dot(d, normal)));
    res.color = (Vec){0, 0, 0};
    return res;
}

static inline SurfaceResult intersect_metal_limitplane(Vec o, Vec d, double t, Limitplane l, Vec obj_col){
    SurfaceResult res;
    res.next_o = add(o, mul(d, t));
    Vec normal = normalize(l.n);

    if (dot(d, normal) > 0){
        normal = mul(normal, -1.0);
    }

    res.next_o = offset_pos(res.next_o, normal);

    res.next_d = sub(d, mul(normal, 2.0 * dot(d, normal)));
    res.color = (Vec){0, 0, 0};
    return res;
}

static inline SurfaceResult intersect_metal(Vec o, Vec d, double t, Object obj) {

    SurfaceResult result;

    // switchはifみたいなもの。ここでは、ifよりみやすいので使う。
    switch (obj.type){
        case SPHERE:
            result =  intersect_metal_sphere(o, d, t, obj.s, obj.col);
            break;
        case INFPLANE:
            result = intersect_metal_infplane(o, d, t, obj.i, obj.col);
            break;
        case LIMITPLANE:
            result = intersect_metal_limitplane(o, d, t, obj.l, obj.col);
            break;
    }
    return result;
}
static inline SurfaceResult intersect_glass_sphere(Vec o, Vec d, double t, Object obj){
    SurfaceResult res;
    Sphere s = obj.s;

    Vec first_o = add(o, mul(d, t)); // 入口の交点を計算

    Vec normal = normalize(sub(first_o, s.o)); // 入口の法線（外向き）

    double cos_1 = -dot(d, normal); // 入射角のcos
    double sin_1 = sqrt(1.0 - cos_1 * cos_1);

    double sin_2 = sin_1 * 1.0 / obj.refractive_index; // スネルの法則
    double cos_2 = sqrt(1.0 - sin_2 * sin_2);

    Vec first_d = add(mul(d, 1.0 / obj.refractive_index),
                      mul(normal, (1.0 / obj.refractive_index) * cos_1 - cos_2)); // 屈折ベクトル
    first_d = normalize(first_d);

    Vec first_o_offset = offset_pos(first_o, mul(normal, -1)); // 球内側に浮かす

    Hitrecord hit = intersect_function(first_o_offset, first_d, obj, 0); // 出口を探す

    if (hit.scene_id == -1 || hit.t <= 0) {
        res.color = (Vec){0, 0, 0};
        return res;
    }

    Vec second_o = add(first_o_offset, mul(first_d, hit.t)); // 出口の交点

    Vec exit_normal = normalize(sub(second_o, s.o)); // 出口の法線（外向き）

    // 出口では内部レイとexit_normalが同じ向きなのでdotが正
    double cos_3 = dot(first_d, exit_normal);
    double sin_3 = sqrt(1.0 - cos_3 * cos_3);

    double sin_4 = sin_3 * obj.refractive_index / 1.0; // スネルの法則（逆方向）
    double cos_4 = sqrt(1.0 - sin_4 * sin_4);

    if (sin_4 > 1.0) { // 全反射
        res.color = (Vec){0, 0, 0};
        return res;
    }

    double n_ratio = obj.refractive_index / 1.0;
    Vec n_exit = mul(exit_normal, -1.0); // 屈折式には内向き法線を使う
    Vec second_d = add(mul(first_d, n_ratio),
                   mul(n_exit, n_ratio * cos_3 - cos_4));
    second_d = normalize(second_d);

    res.next_o = offset_pos(second_o, exit_normal); // 球の外側に浮かす
    res.next_d = second_d;
    res.color = (Vec){0, 0, 0}; // ガラス自体は色を持たない
    return res;
}

static inline SurfaceResult intersect_glass(Vec o, Vec d, double t, Object obj) {
    SurfaceResult result;
    switch (obj.type){
        case SPHERE:
            result = intersect_glass_sphere(o, d, t, obj);
            break;
        default:
            result.color = (Vec){0, 0, 0};
            break;
    }
    return result;
}

#endif