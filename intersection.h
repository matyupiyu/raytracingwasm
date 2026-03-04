#include "struct_vec.h"
#ifndef INTERSECTION_H // 二重読み込み防止（おまじない）
#define INTERSECTION_H

// 球体の判別
static inline double intersect_sphere(Vec cam_o, Vec ray_d, Sphere s) {

Vec co = sub(cam_o, s.o);
    
double a = dot(ray_d, ray_d); //係数aを計算する
double b = 2.0 * dot(co, ray_d); // 係数bを計算する
double c = dot(co, co) - (s.r * s.r);  // 係数cを計算する
double D = b*b - 4.0 * a * c; // 判別式を計算する(aは単位ベクトルなので1)

    if (D > 1e-9) {
        // もし当たったら二次方程式の結果t(距離)を返す
        double t = (-b - sqrt(D)) / (2.0 * a);
        return t;
    } else {
    // もし外れたら-1.0を返す
    return -1.0;
    }
}

// 無限平面の判別
static inline double intersect_infplane(Vec cam_o, Vec ray_d, Infplane i) {
    
    // レイと法線ベクトルの内積
    double Dn = dot(ray_d, i.n);
    
    // fabsは絶対値
    // レイと法線ベクトルの内積が0に近い⇒
    // 平面とレイが平行で交わらない
    if (fabs(Dn) > 1e-6) {
        double t = dot(i.n, (sub(i.o, cam_o))) / Dn; 
        if (t > 0) {
            return t;
        }
    }
return -1.0;
}

// 有限平面の判別

static inline double intersect_limitplane(Vec cam_o, Vec ray_d, Limitplane l) {
    double Dn = dot(ray_d, l.n);
    if (fabs(Dn) > 1e-6) {
        double t = dot(l.n, (sub(l.o, cam_o))) / Dn;
        if (t > 0) {

            // 交点の座標ベクトルを求める  
            Vec P = add(cam_o, mul(ray_d, t));
            // 交点から中心へのベクトル
            Vec OP = sub(P, l.o);

            // 横方向の単位ベクトルを独自の座標軸として作る
            double M = 1 / len(l.w_vec);          
            Vec WIDE = mul(l.w_vec, M);

            // 縦方向の単位ベクトルを独自の座標軸として作る
            // 縦方向と垂直なnと横方向のベクトルを外積することで求まる
            double N = 1 / len(l.n);
            Vec O = mul(l.n, N);
            Vec HEIGHT = cross(WIDE, O);

            // 上で求めた独自座標系におけるPの座標を求める
            double WIDE_P = dot(WIDE, OP);
            double HEIGHT_P = dot(HEIGHT, OP);

            if (fabs(WIDE_P) <= l.width / 2.0 && fabs(HEIGHT_P) <= l.height / 2.0) {
                return t;
            }
        }
    }
    return -1.0;
}
// 円柱の判別
static inline double intersect_cylinder(Vec cam_o, Vec ray_d, Cylinder c) {

    Vec co = sub(cam_o, c.o);
    double min_t = 1e30;

    // 円柱の筒の当たり判定
    double a = dot(ray_d, ray_d) - dot(ray_d, c.n) * dot(ray_d, c.n);
    double b = 2 * (dot(ray_d, co) - dot(ray_d, c.n) * dot(co, c.n) );
    // Cylinder cのcと区別するために大文字のCにした 
    double C = dot(co, co) - dot(co, c.n) * dot(co, c.n) - c.r * c.r;
    double D = b*b - 4.0 * a * C;
    // 下底から軸沿いに図った高さ
    double t_side = (-b - sqrt(D)) / (2.0 * a);
    // カメラから円柱状のPへのベクトルOPを定義
    Vec op = add(cam_o, mul(ray_d, t_side)); 
    // CPベクトルの軸方向成分の長さを計算
    double height = dot(sub(op, c.o), c.n); 
    
    // 無限円柱に交わり、かつ、有限円柱の高さの制限以内で交わること
    if (D > 1e-9 && 0 <= height && height <= c.h) {
        min_t = t_side;
    }

    // 円柱の下底の当たり判定
    double t_bottom = dot(sub(c.o, cam_o), c.n) / dot(ray_d, c.n);
    Vec P_bottom = add(cam_o, mul(ray_d ,t_bottom)); // ベクトルP=O+tDをP_bottomに代入
    double pc_bottom = dot(sub(P_bottom, c.o), sub(P_bottom, c.o)); // ベクトルPCの長さを定義
    
    double Dn_bottom = dot(ray_d, c.n);

    // もし、平面と光線が平行でなく、円の範囲から逸脱せず、t>0で、t_bottomが最小距離より短いとき、
    if (fabs(Dn_bottom) > 1e-6 && 0 <= pc_bottom && pc_bottom <= c.r * c.r && t_bottom > 0 && t_bottom < min_t) {
        min_t = t_bottom;
    }
    

    // 円柱の上底の当たり判定
    Vec nh =  mul(c.n, c.h);
    Vec top_center = add(c.o, nh);
    double t_top = dot(sub(top_center, cam_o), c.n) / dot(ray_d, c.n);
    Vec P = add(cam_o, mul(ray_d ,t_top)); // ベクトルP=O+tDをPに代入
    double pc_top = dot(sub(P, add((c.o), nh)), sub(P, add((c.o), nh))); // ベクトルPCの長さを定義
    
    double Dn_top = dot(ray_d, c.n);

    // もし、平面と光線が平行でなく、円の範囲から逸脱せず、t>0で、t_topが最小距離より短いとき、
    if (fabs(Dn_top) > 1e-6 && 0 <= pc_top && pc_top <= c.r * c.r && t_top > 0 && t_top < min_t) {
        min_t = t_top;
    }
    if (min_t == 1e30) {
        return -1.0;
    } else {
        return min_t;
    }
}

// staticは、この関数(の変数名)の有効範囲をそのファイル内だけに制限する
// inlineは、関数を呼び出すのではなく、呼び出し元にその中身を埋め込む
// mainから受け取った値は、この関数限りのo,d,objで受け取る

static inline Hitrecord intersect_function(Vec o, Vec d, Object obj, int id) { 

    Hitrecord result;
    result.scene_id = -1;
    result.t = -1.0;

    double t = -1.0;
    // switchはifみたいなもの。ここでは、ifよりみやすいので使う。
    switch (obj.type){
        case SPHERE:
            t =  intersect_sphere(o, d, obj.s);
            break;
        case INFPLANE:
            t = intersect_infplane( o, d, obj.i);
            break;
        case LIMITPLANE:
            t = intersect_limitplane(o, d, obj.l);
            break;
        case CYLINDER:
            t = intersect_cylinder(o, d, obj.c);
            break;
        default: t = -1.0;
            break;
    }
    if (t > 1e-6){
        result.t = t;
        result.scene_id = id;
    }
    return result;
}



#endif