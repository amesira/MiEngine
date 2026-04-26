# MiEngine 全体構成メモ

## 概要

- `MiEngine` は、`WinMain` から起動される自作ゲームエンジン本体です。
- 構成としては、ざっくり次の 4 層に分かれています。
  - `Engine`
    - エンジン本体
  - `Game`
    - エンジンを使うゲーム側
  - `Shader`
    - HLSL シェーダ群
  - `asset`
    - モデル、テクスチャなどのリソース

このメモでは主に `Engine` 側の構成だけをまとめます。

---

## 起動から更新までの流れ

### 1. `main.cpp`

- Windows アプリのエントリポイントです。
- ウィンドウを作成し、`MiEngine` を初期化します。
- メッセージループの中で `RunOneFrame()` を毎フレーム呼びます。

### 2. `Engine/engine.h`, `Engine/engine.cpp`

- `MiEngine` クラスがエンジン本体です。
- 主な役割は以下です。
  - `Direct3D` 初期化
  - `ShaderManager` / `ResourceManager` 初期化
  - `GameWorld` 初期化
  - `ImGui` ベースのエディタ UI 初期化
  - 毎フレームの `Update()` / `Render()` 実行

### 3. `Engine/game_world.h`, `Engine/game_world.cpp`

- 実際のゲーム世界更新を担当する中心クラスです。
- 主な中身は以下です。
  - `SceneManager`
  - `PhysicsProcessor`
  - `AnimationProcessor`
  - `BehaviorProcessor`
  - `CameraProcessor`
  - `RenderProcessor`
  - `RenderView` 配列

---

## Engine 直下の主要ファイル

### `engine.*`

- エンジン本体クラス
- 初期化、終了処理、1フレーム実行を担当

### `game_world.*`

- シーンと各 Processor を束ねる更新ハブ

### `scene_manager.*`

- 現在シーンの生成、破棄、切り替えを担当

### `render_view.h`

- カメラごとの描画先情報をまとめる構造体
- 例
  - view / projection 行列
  - eyePosition
  - color / depth buffer
  - post effect 用バッファ

### `resource_manager.*`

- リソース管理の窓口
- テクスチャ、モデル、マテリアルなどの管理と連携する土台

### `shader_manager.*`

- シェーダの読み込みと定数バッファ管理を担当
- `TransformBuffer`、`CameraBuffer` などの共通 CB をまとめて扱う

### `engine_service_locator.h`

- エンジンの主要マネージャや共通機能へアクセスするための窓口
- 各 Processor / Pass / Component からの参照を簡単にしている

---

## フォルダごとの役割

## `Engine/Core`

- エンジンの最小単位や基底クラス群
- 主に以下のようなものが入っています。
  - `GameObject`
  - `Component`
  - `Processor`
  - `Pass`
  - `SceneBase`
  - `SceneInterface`

ここは「エンジンの文法」を作っている層です。

---

## `Engine/Framework`

- 実際のゲーム機能を積み上げる中核層です。
- 大きく次の 2 つに分かれています。

### `Framework/Component`

- 各 GameObject に付くデータの集まり
- 例
  - `TransformComponent`
  - `ModelComponent`
  - `CameraComponent`
  - `LightComponent`
  - `RigidbodyComponent`
  - `ColliderComponent`
  - `AnimationComponent`
  - `TextComponent`
  - `ImageComponent`
  - `DecalComponent`
  - `JointComponent`

### `Framework/Processor`

- Component をまとめて処理する層
- 主なもの
  - `PhysicsProcessor`
  - `BehaviorProcessor`
  - `AnimationProcessor`
  - `CameraProcessor`
  - `RenderProcessor`

さらに `Processor` 配下には細分化された pass 群があります。

#### `PhysicsPass`

- 物理更新の細分化
- 例
  - `IntegratePass`
  - `CollisionPass`
  - `ResolvePass`
  - `ConstraintPass`

#### `RenderPass`

- 描画更新の細分化
- 例
  - `LightingPass`
  - `ShadowMapPass`
  - `OpaqueRenderPass`
  - `DecalRenderPass`
  - `UIRenderPass`
  - `SkyboxPass`

この `Framework` 層が、いわゆる「ゲームエンジンらしい処理本体」です。

---

## `Engine/System`

- DirectX や各種 GPU/入力リソースに近い低レイヤです。
- 大きく次の 2 系統があります。

### `System/Device`

- デバイス寄りの低レイヤ処理
- 例
  - `direct3d`
  - `keyboard`
  - `mouse`
  - `audio`
  - `fps`

### `System/Graphics`

- 描画リソース寄りの管理
- 例
  - `texture_resource`
  - `material_resource`
  - `model_resource`
  - 各種 repository / GPU リソース

ここは「OS / DirectX に近い層」と「描画資産管理層」をまとめた場所です。

---

## `Engine/Editor`

- ImGui ベースの簡易エディタ機能です。
- 主なウィンドウ
  - `SceneView`
  - `Hierarchy`
  - `Inspector`
  - `ToolBar`
  - `DebugView`

- 関連クラス
  - `MiImguiManager`
  - `EditorContext`

この層によって、ゲーム実行だけでなくエディタ的な確認やデバッグができます。

---

## 更新順のざっくりした考え方

現在の全体像はかなりざっくり言うと次の流れです。

1. `MiEngine` が毎フレーム更新を開始
2. `GameWorld` が現在シーンを更新
3. `PhysicsProcessor`
4. `AnimationProcessor`
5. `BehaviorProcessor`
6. `CameraProcessor`
7. `RenderProcessor`
8. `ImGui` ベースの Editor 描画

描画側はさらに `RenderProcessor` の中で RenderPass 単位に分かれています。

---

## 現在の設計の特徴

- `GameObject + Component + Processor` の分離がはっきりしている
- 描画と物理を pass 単位で細かく分離している
- `System` と `Framework` の層が分かれている
- エディタ UI まで含めてエンジン化しようとしている
- `Game` 側はあくまで `Engine` の上に乗る利用者として置かれている

---

## 一言で言うと

MiEngine は、

- `Core`
  - 基底ルール
- `Framework`
  - ゲーム機能本体
- `System`
  - DirectX / リソース低レイヤ
- `Editor`
  - デバッグ・編集 UI

の 4 本柱でできている構成です。

ゲーム本体よりも先に、「ゲームを動かすための土台」を作ろうとしている設計になっています。
