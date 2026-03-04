#include <math.h>
#ifndef STRUCT_VEC_H  // 二重読み込み防止（おまじない）
#define STRUCT_VEC_H

// 構造体の宣言（プロトタイプ宣言） 
// 基本となる構造体の定義
typedef struct { double x, y, z;} Vec; // 位置ベクトルの導入
typedef enum { SOLID, METAL, GLASS} Material;
typedef struct{
    double t;
    int scene_id;
} Hitrecord;

typedef struct {
    Vec o;
} Light;

// 物体やカメラの構造体の定義
// ある球体の構造体(中心の位置ベクトル、半径、色)
typedef struct { 
    Vec o;  // 球体の中心の座標
    double r; //球体の半径
} Sphere;

// 無限平面と構造体
typedef struct {
    Vec o; // 平面の通る任意の一点
    Vec n; // 平面の向き(法線ベクトル)
} Infplane;

// 有限平面の構造体
typedef struct {
    Vec o;  // 中心点
    Vec n;  // 平面の向き
    Vec w_vec; // 横方向の方向ベクトル,nとの内積が0になるようにする
    double width, height; // 横と縦の長さ
} Limitplane; 


// 円柱の構造体
typedef struct {
    Vec o; // 底面の中心
    Vec n; // 底面の向き(法線ベクトル)
    double r;  // 半径
    double h;  // 高さ
} Cylinder;

// カメラの構造体(一ベクトル、解像度)
typedef struct {
    Vec o;           // カメラの位置ベクトル
    Vec n;           // カメラの方向ベクトル
    int width, height; // カメラの解像度
    Vec screen;      // スクリーンの位置ベクトル
}Camera;

// 形状を判別するためのラベル(数字(scene)に名前)
typedef enum {
    SPHERE,
    INFPLANE,
    LIMITPLANE,
    CYLINDER,
    LIGHT
} Shapetype;

// すべての物体に対応した箱
typedef struct {
    Shapetype type;
    Vec col;
    Material mat;
    union{
        Sphere s;
        Infplane i;
        Limitplane l;
        Cylinder c;
        Light li;
    };
} Object;


// ベクトルの基本演算を定義
static inline Vec add(Vec a, Vec b) { return (Vec){a.x + b.x, a.y + b.y, a.z + b.z};}   // 足し算
static inline Vec sub(Vec a, Vec b) { return (Vec){a.x - b.x, a.y - b.y, a.z - b.z};}  // 引き算
static inline double dot(Vec a, Vec b) { return a.x * b.x + a.y * b.y + a.z * b.z;}  // ベクトルの掛け算
static inline double len(Vec a) { return sqrt(dot(a,a));} // ベクトルの長さ
// ベクトルの外積
static inline Vec cross(Vec a, Vec b) {
    return (Vec) {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}
static inline Vec mul(Vec a, double k) { return (Vec){a.x * k, a.y * k, a.z * k};} // ベクトルのスカラー倍
static inline Vec normalize(Vec a) { 
    double l = len(a);
    // aがoベクトルか、非常に小さい場合, 0ベクトルを返す(0ベクトルで割ることを避ける)
    if (l < 1e-9) {return (Vec){0, 0, 0};  } else {       
    return (mul(a, 1.0/l));}; // 単位ベクトル(向きを変えずに長さを1にする)
}

static inline Vec hadamard(Vec a, Vec b) { return (Vec){a.x * b.x, a.y * b.y, a.z * b.z};} // 色の乗算で必要なアダマール積

#endif