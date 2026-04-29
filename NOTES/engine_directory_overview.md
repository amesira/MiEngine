# Engine配下の構成メモ

このメモは `Source/Engine` 配下の大まかな責務を整理するためのものです。
細かい実装手順ではなく、「新しい機能やファイルをどこへ置くか」を判断しやすくするための概要です。

## 全体方針

`Engine` 配下は、ゲーム固有の処理ではなく、ゲームを動かすための基盤を置く場所です。

現在はおおまかに次の役割へ分けています。

```txt
Source/Engine
  Core        基礎クラス、最低限の抽象
  Framework   ComponentやProcessorなど、ゲーム実行の仕組み
  System      DirectX、入力、音、描画リソースなど低レイヤ
  Manager     エンジン全体で共有する管理クラス
  Settings    シーンや描画全体に関わる設定値
  Editor      ImGuiベースのエディタUI
```

`engine.*` と `game_world.*` は、これらを束ねる上位の実行ハブとして `Engine` 直下に置いています。

## Engine直下

### `engine.*`

`MiEngine` 本体です。

主に以下を担当します。

- Direct3Dや各Managerの初期化、終了処理
- `GameWorld` の更新と描画
- ImGuiエディタの更新と描画
- エンジン全体のサービスへの入口

アプリケーションとしてのエンジン本体に近いものはここに集まります。

### `game_world.*`

現在のゲーム世界を更新する中心です。

主に以下を持ちます。

- `SceneManager`
- `PhysicsProcessor`
- `AnimationProcessor`
- `BehaviorProcessor`
- `CameraProcessor`
- `RenderProcessor`
- `RenderView`

シーンと各Processorを束ね、1フレームごとの更新順を管理する役割です。

### `render_view.h`

カメラごとの描画先やビュー情報をまとめる構造体です。

カメラ、描画ターゲット、ポストエフェクト用バッファなど、「この視点で描画するために必要な状態」を持ちます。

## `Core`

エンジンの基礎になる最小単位を置きます。

例:

- `GameObject`
- `Component`
- `ComponentPool`
- `Processor`
- `Pass`
- `SceneBase`
- `SceneInterface`

ここは特定の機能に寄りすぎない、エンジン全体の土台です。
描画、物理、エディタなどの具体的な処理は基本的にここへ置きません。

## `Framework`

ゲーム実行時の仕組みを置きます。

`Core` が基礎ルールなら、`Framework` はそのルールの上で動く実体です。

### `Framework/Component`

`GameObject` に付くデータや振る舞いの単位です。

例:

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

複数のオブジェクトに付くもの、TransformやGameObjectの存在と強く結びつくものはここに置きます。

### `Framework/Processor`

Componentを集めて処理する層です。

例:

- `PhysicsProcessor`
- `AnimationProcessor`
- `BehaviorProcessor`
- `CameraProcessor`
- `RenderProcessor`

Processorの中でさらに処理を細分化したい場合は、Passとして分けます。

### `Framework/Processor/RenderPass`

描画処理の各段階を置きます。

例:

- `LightingPass`
- `ShadowMapPass`
- `OpaqueRenderPass`
- `DecalRenderPass`
- `SkyboxPass`
- `UIRenderPass`
- `PostEffectPass`

ここには「処理そのもの」を置きます。
シーン全体で保存したい設定値は、基本的に `Settings` 側に置き、Passはその設定を読んでGPUへ渡す役割にします。

## `System`

OS、DirectX、GPUリソース、入力、音などに近い低レイヤを置きます。

### `System/Device`

デバイスやプラットフォーム寄りの処理です。

例:

- `direct3d`
- `keyboard`
- `mouse`
- `audio`
- `mi_fps`

### `System/Graphics`

描画リソースの実体やRepositoryを置きます。

例:

- `TextureRepository`
- `ModelRepository`
- `MaterialRepository`
- `TextureResource`
- `ModelResource`
- `MaterialResource`

モデル、テクスチャ、マテリアルなど、GPUやアセットに近い描画資源を扱う場所です。

## `Manager`

エンジン全体で共有する管理クラスを置きます。

現在の例:

- `ResourceManager`
- `SceneManager`
- `ShaderManager`

Managerは、特定のSceneやGameObjectに属さず、エンジン側から広く参照される管理役です。

目安:

- 複数のRepositoryを束ねる
- シーンの生成、破棄、切り替えを管理する
- シェーダーや共通定数バッファを管理する
- `MiEngine` や `GameWorld` が所有し、他の処理から参照する

逆に、毎フレームの具体的な処理手順は `Processor` や `Pass` に置きます。
個々のオブジェクトに付くデータは `Component` に置きます。

## `Settings`

シーン全体、描画全体、エンジン全体に関わる設定値を置きます。

現在の例:

- `SceneRenderSettings`

`SceneRenderSettings` は、リムライト、半球ライト、ポストプロセスなど、シーン全体の見え方に関わる値をまとめる想定です。

目安:

- シーンに1つあればよい
- GameObjectとして存在する必要がない
- ComponentPoolで複数管理する意味が薄い
- エディタ上で一箇所から調整したい
- `LightingPass` や `PostEffectPass` など複数の描画処理から参照される

例:

- `RimLightSettings`
- `HemisphereLightSettings`
- `PostProcessSettings`
- `BloomSettings`
- `ColorGradingSettings`
- `FogSettings`

設定値の所有は `Settings` 側、実際の描画処理は `RenderPass` 側、GPU用構造体は必要に応じてPass側、という分担にすると見通しがよくなります。

## `Editor`

ImGuiベースのエディタUIを置きます。

例:

- `MiImguiManager`
- `EditorContext`
- `SceneViewWindow`
- `HierarchyViewWindow`
- `InspectorViewWindow`
- `DebugViewWindow`
- `ToolBarWindow`

エディタは、ComponentやSettingsを編集する入口です。

シーン全体の描画設定を編集する場合は、Inspectorとは別タブで `SceneRenderSettings` 用のビューを用意する方針です。

## 配置判断の目安

新しい機能を追加するときは、まず次の基準で置き場所を決めます。

| 置き場所 | 判断基準 |
| --- | --- |
| `Core` | エンジン全体の基礎ルール、抽象、最小単位 |
| `Framework/Component` | GameObjectに付く、複数存在する、Transformと結びつく |
| `Framework/Processor` | Component群を集めて毎フレーム処理する |
| `Framework/Processor/*Pass` | Processor内の処理段階として分けたい |
| `System` | DirectX、入力、音、GPUリソースなど低レイヤ |
| `Manager` | エンジン全体で共有される管理役 |
| `Settings` | シーン全体や描画全体に効く調整値 |
| `Editor` | ImGuiによる編集、確認、デバッグUI |

## ライトと描画設定の考え方

ライトやポストエフェクトは、性質によって置き場所を分けます。

```txt
DirectionalLight
  GameObject + LightComponent
  シーン上の存在として扱い、向きや影と結びつける

PointLight / SpotLight
  GameObject + LightComponent
  複数配置でき、Transformを持つ

RimLight / HemisphereLight
  SceneRenderSettings
  シーン全体の見え方として扱う

PostProcess / Bloom / ColorGrading
  SceneRenderSettings
  PostEffectPassが設定を読んで処理する
```

「シーン上の物体」ならComponent、「シーンの見え方」ならSettings、という分け方を基本にします。

