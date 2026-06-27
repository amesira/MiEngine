# MiEngine v2 Direction Memo

## コンセプト

MiEngine v2 は、ゲームエンジンとしての完成度を追うよりも、アクションゲームの見た目・演出・手触りを素早く調整するための軽量な自作ゲーム制作ランタイムとして設計する。

目指す一文:

> MiEngine v2 は、C++ の再コンパイルなしにアクションゲームの演出・見た目・配置を素早く調整できる、軽量な自作ゲーム制作ランタイムにする。

就活作品としては、エンジンそのものを主役にするよりも、次の見せ方を主軸にする。

> ゲームの手触り調整およびグラフィック調整のために、こういう開発環境を作りました。

## 拾うもの

- パーティクル、マテリアル、シェーダーなどの見た目調整をエディタ上で確認できるようにする。
- 数値調整は C++ 直書きから外し、JSON / ImGui / 外部ファイルに逃がす。
- Unity の ScriptableObject に近い、ゲーム調整用のデータファイルを用意する。
- パーティクルは ImGui 上で編集し、`.particle.json` として Save / Load できるようにする。
- マテリアルは通常マテリアルと演出マテリアルの両方を扱うが、先に攻撃・回避・被弾・溜めなどに連動する演出マテリアルを重視する。
- `.mat.json` は名前付きプロパティで触れるようにする。
- シェーダーやマテリアルは、まずボタン式 Reload で十分とする。
- フィールドは Blender で配置した glTF / FBX を読み込む方向にする。
- フィールドには高低差、階段、歩道橋、円柱などがある検証用アリーナを想定する。

## 捨てるもの

- 汎用的な Unity 風エディタ。
- 完全なシーン保存 / 復元。
- ノードベースのマテリアルエディタ。
- エンジン単体としての汎用性。
- すべてのオブジェクト配置を MiEngine 内で完結させる本格レベルエディタ。

## 中心になる考え方

v2 の核は、Tuning Asset と呼べる調整用アセット群にする。

C++ 側は「処理の型」を持つ。JSON 側は「調整値」を持つ。ImGui はその値を触る。

例:

```txt
Assets/
  Tuning/
    Player/
      player_default.player.json
      player_attack.attack.json
      player_dodge.dodge.json

    Particle/
      hit_spark.particle.json
      run_dust.particle.json
      charge_absorb.particle.json

    Material/
      player_hologram.mat.json
      slash_trail.mat.json

    Effect/
      hit_impact.effect.json
      just_dodge.effect.json

    Field/
      arena_01.field.json
```

## Prefab ではなく Factory + TuningAsset

v2 では、最初から完全な Prefab システムを作る必要はない。

代わりに、Factory がオブジェクト構造を作り、TuningAsset が細かい値を流し込む構成にする。

例:

```cpp
PlayerFactory::Create(scene, "Assets/Tuning/Player/player_default.player.json");
```

C++ 側:

- Player に必要な Component を追加する。
- Behavior の種類や基本構造を決める。
- ゲーム固有の処理を書く。

JSON 側:

- 移動速度。
- 攻撃値。
- 当たり判定サイズ。
- カメラやヒットストップなどの演出値。
- 使用する Particle / Material / Effect アセットへの参照。

これにより、Prefab システムほど重くせずに、C++ 再コンパイルなしの調整を実現する。

## Player Tuning Asset の例

```json
{
  "name": "Player",
  "model": "Assets/Model/player.fbx",
  "collider": {
    "type": "capsule",
    "center": [0.0, 1.0, 0.0],
    "radius": 0.45,
    "height": 1.8
  },
  "movement": {
    "walkSpeed": 5.5,
    "dashSpeed": 9.0,
    "acceleration": 18.0,
    "turnSpeed": 12.0
  },
  "combat": {
    "attackPower": 12,
    "hitStop": "Assets/Tuning/Effect/hit_impact.effect.json",
    "hitParticle": "Assets/Tuning/Particle/hit_spark.particle.json"
  }
}
```

## Particle Asset

v2 の最初の成果物は Particle Asset にする。

現在の `ParticleSystemComponent` は `Main / Emission / Shape / SizeOverLifetime / Renderer` に分かれており、アセット化しやすい。

最初の目標:

1. `ParticleSystemComponent` の値を `.particle.json` として保存できる。
2. `.particle.json` を読み込んで ParticleSystem に反映できる。
3. ImGui の専用 Particle Editor Window から編集できる。
4. Play / Stop / Save / Load / Reload ができる。
5. ゲーム画面上で実際のエフェクトとして確認できる。

既存の `RenderEffectFactory` に直書きされているパーティクル値は、`.particle.json` へ移行する候補にする。

候補:

- `hit_spark.particle.json`
- `run_dust.particle.json`
- `charge_absorb.particle.json`
- `explosion.particle.json`

## Material Asset

マテリアルは、先に演出マテリアルを重視する。

扱いたい例:

- 攻撃時だけ光る。
- ダメージ時に白く点滅する。
- 回避時に残像が出る。
- 溜め攻撃中にリムライトやディゾルブ量が変化する。
- ヒット時に特殊なマスクやポストエフェクトと組み合わせる。

`.mat.json` は、シェーダー設定ファイルに近い形にする。

```json
{
  "name": "player_hologram",
  "shader": "HologramUnlit",
  "baseColor": [0.2, 0.8, 1.0, 1.0],
  "emissiveColor": [0.1, 0.7, 1.0],
  "emissiveIntensity": 2.5,
  "customProperties": {
    "rimPower": 3.0,
    "noiseScale": 12.0,
    "dissolveAmount": 0.35
  },
  "textures": {
    "albedo": "Assets/Texture/player_sheet.png",
    "noise": "Assets/Texture/hologram_noise.png"
  }
}
```

内部実装では、最初から複雑なノードエディタを作らず、名前付きプロパティをシェーダー用の定数バッファに詰める方針にする。

確認方法:

- 実ゲーム画面上で確認する。
- 必要ならプレビュー球やプレビュー板を用意する。
- `ModelComponent` のマテリアルスロットに指定して確認できる程度から始める。

## Effect Asset

Effect Asset は、ゲームの手触りに関わる演出をまとめるアセットにする。

例:

```txt
hit_impact.effect.json
just_dodge.effect.json
attack_charge.effect.json
```

中身の候補:

- Hit Stop。
- Camera Shake。
- FOV 変化。
- Camera Distance / Offset 変化。
- Particle 再生。
- Material 変更。
- Post Effect。

既存の `GameEffectController` が持っている TimeScale / FOV / Camera Shake などは、Effect Asset から呼び出せる形にするとよい。

## Component Inspector

Component の Inspector は、v2 で設計を少し整理する。

重要なのは、Inspector で触れる値と JSON に保存される値が一致すること。

最小構成:

```cpp
component->DrawInspector();
component->Serialize(json);
component->Deserialize(json);
```

または、後から外部 Registry に寄せる。

```cpp
InspectorRegistry::Draw(component);
AssetSerializer::Save(component);
AssetSerializer::Load(component);
```

最初は前者で十分。後から共通化する。

## Field

フィールドは MiEngine 内で本格的に作るより、Blender で配置した glTF / FBX を読み込む方向にする。

想定ステージ:

- ある程度の広さの検証用アリーナ。
- 高低差がある。
- 階段や歩道橋のような構造がある。
- 円柱などの障害物がある。
- 背景オブジェクトを差し替えることで見た目を変えられる。

名前ルールの例:

```txt
Visual_*
Collider_*
Spawn_Player
Spawn_Enemy_*
Gimmick_*
```

MiEngine 側は、読み込んだノード名を見て、Visual / Collider / Spawn / Gimmick に変換する。

最初から完全な配置エディタは作らない。

## 実装順の候補

1. v2 の基礎プロジェクトを作り直す。
   - Renderer / Direct3D の Device / Context 周りを整理する。
   - v1 で掴んだレンダリングの流れを、v2 では扱いやすい形に再設計する。

2. Particle Asset。
   - `.particle.json` Save / Load。
   - Particle Editor Window。
   - ゲーム画面上で即確認。

3. Material Asset。
   - `.mat.json` Save / Load。
   - 名前付き custom property。
   - ModelComponent のスロット指定。
   - Reload ボタン。

4. Effect Asset。
   - Hit Stop / Camera Shake / FOV / Particle / Post Effect をまとめる。
   - 攻撃や被弾から呼び出せるようにする。

5. Player / Attack Tuning Asset。
   - プレイヤー移動値。
   - 攻撃値。
   - 当たり判定。
   - 使用する Effect / Particle / Material 参照。

6. Field Import。
   - Blender 配置の glTF / FBX 読み込み。
   - 名前ルールによる Collider / Spawn 変換。

## 最初の到達点

最初の v2 らしい成果物は、次の状態を目指す。

> Particle Editor Window でパーティクルを編集し、`.particle.json` に保存し、ゲーム画面上で再読み込みして確認できる。

ここまでできると、MiEngine v2 の方向性である「C++ 再コンパイルなしの見た目・演出調整」が明確に見える。
