# RayStudio

ブラウザで動くパストレーシング型レイトレーサーです。
C言語で書いたレンダラーをWebAssembly(WASM)にコンパイルし、HTMLのUIからシーンを編集してレンダリングできます。

## デモ

> レンダリング例

![render example](image10.png)
![render example1](image1.png)

## 特徴

- C言語製パストレーサーをWASMで動作
- 球・無限平面・有限平面・円柱・光源を配置可能
- SOLID / METAL マテリアル対応
- TOP / FRONT / SIDE の3方向ビューでオブジェクトを確認
- シーンをJSONファイルで保存・読み込み
- オブジェクトのON/OFF、複製、ドラッグ並び替え
- レンダリング結果のズーム・PNG保存
- Ctrl+Zか元に戻すボタンで戻す

## 使い方（プログラムを知らない方）

1. [こちら](https://matyupiyu.github.io/RayStudio/) からサイトを開く
2. 左のリストでオブジェクトを選んでプロパティを編集
3. ONにしたオブジェクトだけがレンダリングされる
4. 設定タブでサンプル数・解像度を調整
5. 「レンダリング」ボタンを押して結果を確認
6. 💾ボタンでPNG保存

## ビルド方法（開発者向け）

### 必要なもの

- [Emscripten](https://emscripten.org/) 3.0以上

### コンパイル

```powershell
emcc main.c -o renderer.js \
  -s EXPORTED_FUNCTIONS="['_render','_get_buffer_size','_set_object','_set_object_count','_set_camera']" \
  -s EXPORTED_RUNTIME_METHODS="['ccall','cwrap','HEAPU8']" \
  -s ALLOW_MEMORY_GROWTH=1 \
  -O2
```

または付属の `compile.ps1` を実行：

```powershell
powershell -ExecutionPolicy Bypass -File compile.ps1
```

### ローカルで動かす

```bash
python -m http.server 8080
```

ブラウザで `http://localhost:8080` を開く。

## ファイル構成

```
index.html          # UI
renderer.js         # WASMラッパー（emccが生成）
renderer.wasm       # コンパイル済みレンダラー
main.c              # レンダラー本体（WASM用エントリーポイント）
struct_vec.h        # ベクトル・構造体定義
intersection.h      # 交差判定
intersectionpoint.h # 交点処理
serch_light_random_d.h # 直接光・ランダム反射
compile.ps1         # コンパイルスクリプト
```

## 技術的な仕組み

- パストレーシングで各ピクセルにレイを飛ばしてサンプリング
- 直接光サンプリング（影判定付き）
- ランダム半球サンプリングによる間接光
- METALマテリアルは鏡面反射、SOLIDはランバート反射

## ライセンス

MIT
