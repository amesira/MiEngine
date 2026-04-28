# PlayerBehavior フォルダ要約

## 概要

- このフォルダは、プレイヤーの入力・状態管理・各処理 Behavior をまとめた領域です。
- 現在の構成は大きく次のように分かれています。
  - `PlayerBehavior`
    - プレイヤー全体の司令塔
    - 入力収集と各 machine の更新を担当
  - `PlayerStateMachineBehavior`
    - 移動系状態の管理を担当
  - `PlayerCombatMachineBehavior`
    - 戦闘系状態の管理を担当
  - `PlayerVisualMachineBehavior`
    - 見た目系状態の管理を担当
  - `PlayerState/`
    - 実際の移動処理、攻撃処理などの具体処理 Behavior
  - `player_context.h`
    - 各 machine と各 Behavior の間で共有するコンテキスト

## 各ファイルの役割

### `player_behavior.h / player_behavior.cpp`

- プレイヤー全体の中心となる Behavior です。
- `PlayerContext` を内部に持ちます。
- `UpdateInput()` で入力を収集し、毎フレーム `context.input` を更新します。
- その後、以下を順番に更新します。
  - `PlayerStateMachineBehavior`
  - `PlayerCombatMachineBehavior`
  - `PlayerVisualMachineBehavior`
- `Start()` では以下の参照を取得しています。
  - `PlayerMoveBehavior`
  - `PlayerAttackBehavior`
  - `PlayerStateMachineBehavior`
  - `PlayerCombatMachineBehavior`
  - `PlayerVisualMachineBehavior`
  - `MainCamera` の `Transform` / `CameraComponent`

### `player_context.h`

- `PlayerInput` と `PlayerContext` を定義しています。
- `PlayerInput` には現在、以下の情報があります。
  - `horizontal / vertical`
  - `moveInput`
  - ジャンプ入力
  - 攻撃の押下、押しっぱなし、離した瞬間
- `PlayerContext` には現在、以下を持ちます。
  - 入力
  - `PlayerState`
  - `PlayerCombatState`
  - `PlayerVisualState`
  - `moveBehavior`
  - `attackBehavior`

### `player_state_machine_behavior.h / player_state_machine_behavior.cpp`

- 移動系状態の管理を行う Behavior です。
- 状態そのものは内部に閉じず、`PlayerContext` 側の `PlayerState` を共有して使います。
- 現在は以下の状態を使う前提です。
  - `Idle`
  - `Move`
  - `Dodge`
  - `Stunned`
- 実際に処理されているのは `Idle` と `Move` です。
- 移動入力の有無で `Idle / Move` を切り替えつつ、以下を呼び出しています。
  - `moveBehavior->UpdateMove()`
  - `moveBehavior->UpdateRotation()`

### `player_combat_machine_behavior.h / player_combat_machine_behavior.cpp`

- 戦闘系状態の管理を行う Behavior です。
- 状態そのものは `PlayerContext` 側の `PlayerCombatState` を共有して使います。
- 現在は最小実装として、
  - 攻撃ボタンを押している間は `Attack`
  - それ以外は `None`
  にしています。

### `player_visual_machine_behavior.h / player_visual_machine_behavior.cpp`

- 見た目系状態の管理を行う Behavior です。
- 状態そのものは `PlayerContext` 側の `PlayerVisualState` を共有して使います。
- 現在は最小実装として、
  - `Combat` が `Attack` なら visual も `Attack`
  - それ以外で移動中なら `Move`
  - 何もなければ `Normal`
  にしています。
- まだ実際の傾きや姿勢補正は入っておらず、見た目状態の決定のみです。

### `PlayerState/player_move_behavior.h / .cpp`

- 移動の実処理を担当する Behavior です。
- `Start()` で以下を取得します。
  - `TransformComponent`
  - `RigidbodyComponent`
- `UpdateMove()`
  - `context.input.moveInput` を使って移動方向を作る
  - `Rigidbody` の `XZ` 速度へ加算する
  - 水平方向の速度を `clamp` する
  - 接地中ならジャンプ入力も処理する
- `UpdateRotation()`
  - 現在の速度方向を向くように `Y` 回転を更新する
  - `lerp` で滑らかに回転させている

### `PlayerState/player_attack_behavior.h / .cpp`

- 攻撃処理用の Behavior です。
- 現在は未実装です。

## 現在の設計意図

- このフォルダは、次のような層構造を目指しています。
  - `PlayerBehavior`
    - 入力収集と全体進行
  - `StateMachine / CombatMachine / VisualMachine`
    - 状態遷移と判断
  - `Move / Attack Behavior`
    - 実際の処理
- 状態の所有は `PlayerContext` 側に寄せ、各 machine はその共有状態を書き換える役割です。
- この分け方は今後、
  - 銃の攻撃
  - 回避
  - 被弾
  - リロード
  - 見た目の傾き
  などを増やしていくときに相性が良いです。

## 現状の実装状況

- 移動系の骨組みはできています。
- 戦闘系と見た目系は、共有状態を更新する最小構成まで入っています。
- 移動処理の実体は `PlayerMoveBehavior` にあります。
- `AttackBehavior` 自体はこれから実装する前提の状態です。

## 注意点

- 状態は以下の 3 軸に分かれています。
  - `PlayerState`
  - `PlayerCombatState`
  - `PlayerVisualState`
- 今後はこれらを混ぜて使いすぎず、責務を分けたまま育てると保守しやすいです。
- 特に `Visual` は、状態だけでなく将来的に補間値や傾き量などの連続値を持つ可能性があります。

## 次にやると良さそうなこと

- 最低限の攻撃処理を `AttackBehavior` に実装する
- `CombatState` を `None / Attack` 以外にも広げる
- `VisualMachine` に体の傾きや姿勢補正を実装する
- 必要なら locomotion / combat / visual の各状態に `Enter / Exit` 時の処理を足す
