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
#endif