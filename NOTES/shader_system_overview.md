# シェーダー周りの構成メモ

このメモは、現行のシェーダー関連ファイルがどこにあり、それぞれ何を担当しているかを整理するためのものです。

## 全体像

シェーダー周りは、大きく次の役割に分かれています。

```txt
Shader/
  HLSL ソース置き場

*.cso
  コンパイル済みシェーダーバイナリ

Source/Engine/Graphics/shader_definitions.h
  頂点型、ベースシェーダー種別、シェーダー名定義

Source/Engine/Graphics/shader_resource.h
  GPU リソースを持つ構造体群

Source/Engine/Graphics/shader_repository.*
  シェーダーリソースの生成、取得、キャッシュ管理

Source/Engine/Manager/shader_manager.*
  シェーダーのバインド、共通定数バッファの更新
```

`ShaderRepository` が「リソースを持つ場所」、`ShaderManager` が「描画時に GPU パイプラインへセットする場所」です。

## `Shader/`

HLSL のソースファイルを置くフォルダです。

例:

- `Shader/BaseShader/`
- `Shader/CustomUiShader/`

実行時に直接読む対象ではなく、ビルドまたは手動コンパイルによって `.cso` を生成する元データです。

## ルート直下の `.cso`

`lit_vs.cso`、`lit_ps.cso`、`ui_vs.cso` などのコンパイル済みシェーダーバイナリです。

`ShaderRepository` は現在、ファイルパスとして渡された `.cso` を `std::ifstream` で読み込み、Direct3D の `CreateVertexShader` / `CreatePixelShader` に渡して GPU リソースを作成します。

代表例:

- `lit_vs.cso`
- `lit_ps.cso`
- `skinned_lit_vs.cso`
- `unlit_vs.cso`
- `unlit_ps.cso`
- `ui_vs.cso`
- `ui_ps.cso`
- `decal_lit_ps.cso`
- `ttf_ui_ps.cso`

## `shader_definitions.h`

シェーダー関連の共通定義を持つファイルです。

主な内容:

- `VertexType`
- `LitVertex`
- `SkinnedLitVertex`
- `UnlitVertex`
- `SpriteVertex`
- `ShaderBase`
- `SHADER_BASE_NAMES`

`VertexType` は、頂点シェーダー生成時の input layout 作成に使われます。

`ShaderBase` は、エンジンが標準で持つベースシェーダーの種類です。

```txt
None
Lit
SkinnedLit
Unlit
Ui
```

## `shader_resource.h`

シェーダー関連リソースの入れ物を定義します。

### `VertexShaderResource`

頂点シェーダーのリソースです。

保持するもの:

- `.cso` の `filePath`
- `ID3D11VertexShader`
- `ID3D11InputLayout`
- `VertexType`

頂点シェーダーは input layout とセットで扱う必要があるため、生成には `filePath` だけでなく `VertexType` が必要です。

### `PixelShaderResource`

ピクセルシェーダーのリソースです。

保持するもの:

- `.cso` の `filePath`
- `ID3D11PixelShader`

ピクセルシェーダーは `filePath` だけで生成できるため、現行では `GetPixelShaderResource()` がキャッシュ未登録時に生成まで行います。

### `ConstantBufferResource`

定数バッファのリソースです。

保持するもの:

- 名前
- バインドスロット
- バッファサイズ
- VS / PS へのバインド有無
- `ID3D11Buffer`
- 使用方法

`ShaderManager`、`MaterialRepository`、`ModelRepository`、各 RenderPass などから追加され、`ShaderProgramResource` に紐づけられます。

### `ShaderProgramResource`

実際に描画で使うシェーダー構成です。

保持するもの:

- 名前
- ベースシェーダー
- 上書き用頂点シェーダー
- 上書き用ピクセルシェーダー
- 追加定数バッファ
- 最終的に使う頂点シェーダー
- 最終的に使うピクセルシェーダー
- 最終的に使う定数バッファ一覧

`baseShader` を指定すると、ベースの構成を引き継ぎつつ、必要なシェーダーや定数バッファだけを差し替えられます。

## `shader_repository.h / .cpp`

シェーダー関連リソースの生成、取得、キャッシュ管理を担当します。

持っているキャッシュ:

- `m_shaderCache`
- `m_vertexShaderCache`
- `m_pixelShaderCache`
- `m_constantBufferCache`

### `Initialize()`

Direct3D の device / context を取得し、標準のベースシェーダーを生成します。

生成されるベースシェーダー:

- `Lit`
- `SkinnedLit`
- `Unlit`
- `Ui`

`SkinnedLit` は、頂点シェーダーだけ `skinned_lit_vs.cso` を使い、ピクセルシェーダーは `Lit` と同じものを共有します。

### `GenerateShaderProgramResource()`

`ShaderProgramResource` を生成、または同名キャッシュを上書きします。

`baseShader` が設定されている場合は、ベースシェーダーから次を引き継ぎます。

- 頂点シェーダー
- ピクセルシェーダー
- 定数バッファ一覧

その上で、`overrideVertexShader`、`overridePixelShader`、`additionalConstantBuffers` があれば反映します。

### `GenerateVertexShaderResource()`

頂点シェーダーを生成、または同じ `filePath` のキャッシュを上書きします。

処理内容:

1. `.cso` を読み込む
2. `ID3D11VertexShader` を作成する
3. `VertexType` に応じた input layout を作成する
4. `VertexShaderResource` としてキャッシュする

頂点シェーダーは input layout が必要なため、生成には `VertexType` が必要です。

### `GetVertexShaderResource()`

頂点シェーダーキャッシュを取得します。

未登録の場合は `nullptr` を返します。  
ピクセルシェーダーと違い、生成に `VertexType` が必要なので、現行では `Get` 時の自動生成は行いません。

### `GeneratePixelShaderResource()`

ピクセルシェーダーを生成、または同じ `filePath` のキャッシュを上書きします。

処理内容:

1. `.cso` を読み込む
2. `ID3D11PixelShader` を作成する
3. `PixelShaderResource` としてキャッシュする

### `GetPixelShaderResource()`

ピクセルシェーダーキャッシュを取得します。

現行では、キャッシュに存在しない場合は `GeneratePixelShaderResource(filePath)` を呼び、生成したものを返します。

このため、呼び出し側は「すでにロード済みかどうか」を意識せず、必要なピクセルシェーダーを取得できます。

### `GenerateConstantBufferResource()`

定数バッファを生成、または同名キャッシュを上書きします。

作成後は `AddConstantBufferToShaderProgram()` によって、各 `ShaderProgramResource` に追加されます。

### `AddConstantBufferToShaderProgram()`

指定したシェーダープログラムに定数バッファを追加します。

共通定数バッファや、描画パスごとの定数バッファをベースシェーダーへ後から足す用途で使われています。

## `shader_manager.h / .cpp`

描画時にシェーダーと定数バッファを GPU パイプラインへバインドする担当です。

### `Initialize()`

共通定数バッファを生成します。

- `TransformBuffer`
- `CameraBuffer`

生成後、すべてのベースシェーダーに追加します。

### `BindShader()`

`ShaderProgramResource` を元に、次のものを Direct3D の pipeline に設定します。

- vertex shader
- pixel shader
- input layout
- constant buffers

呼び出し口は 3 種類あります。

- `BindShader(ShaderBase shaderBase)`
- `BindShader(const std::string& shaderName)`
- `BindShader(const ShaderProgramResource* shaderProgram)`

### `BindTransformCB()`

`TransformBuffer` を更新します。

主にワールド行列、法線用行列、逆ワールド行列を GPU に送ります。

### `BindCameraCB()`

`CameraBuffer` を更新します。

主に view / projection、カメラ位置、逆 view / 逆 projection を GPU に送ります。

## カスタムシェーダーの作り方

ベースシェーダーを元に、一部だけ差し替える形で作れます。

例: デカール用シェーダー

```cpp
ShaderProgramResource decalShader;
decalShader.name = "DecalLit";
decalShader.baseShader = SHADER_REPOSITORY->GetShaderProgramResource(ShaderBase::Lit);
decalShader.overridePixelShader = SHADER_REPOSITORY->GetPixelShaderResource("decal_lit_ps.cso");
m_decalShader = SHADER_REPOSITORY->GenerateShaderProgramResource(decalShader);
```

この例では、`Lit` の頂点シェーダーや定数バッファはそのまま使い、ピクセルシェーダーだけ `decal_lit_ps.cso` に差し替えています。

例: TTF スプライト用シェーダー

```cpp
ShaderProgramResource ttfShader;
ttfShader.name = "TTFUi";
ttfShader.baseShader = SHADER_REPOSITORY->GetShaderProgramResource(ShaderBase::Ui);
ttfShader.overridePixelShader = SHADER_REPOSITORY->GeneratePixelShaderResource("ttf_ui_ps.cso");
m_pFontShader = SHADER_REPOSITORY->GenerateShaderProgramResource(ttfShader);
```

## 描画までの流れ

```txt
1. ShaderRepository::Initialize()
   ベースシェーダーを生成する

2. ShaderManager::Initialize()
   Transform / Camera の共通定数バッファを生成し、ベースシェーダーへ登録する

3. MaterialRepository / ModelRepository / RenderPass
   必要な定数バッファやカスタムシェーダープログラムを追加する

4. RenderPass
   EngineServiceLocator::BindShader() を呼ぶ

5. ShaderManager::BindShader()
   VS / PS / InputLayout / ConstantBuffer を GPU に設定する
```

## 現行の責務境界

`ShaderRepository`

- `.cso` から GPU リソースを作る
- 作ったリソースをキャッシュする
- シェーダープログラム構成を登録する
- 定数バッファリソースを作る

`ShaderManager`

- 既に作られたシェーダープログラムを描画時にバインドする
- 共通定数バッファの中身を更新する

`RenderPass`

- どのシェーダーを使うかを決める
- 必要ならベースシェーダーからカスタムシェーダープログラムを作る
- 描画直前に `BindShader()` を呼ぶ

## 注意点

- 頂点シェーダーは `VertexType` と input layout が絡むため、`GetVertexShaderResource()` は未登録時に自動生成しません。
- ピクセルシェーダーは `filePath` だけで生成できるため、`GetPixelShaderResource()` が未登録時の生成も担当します。
- `Generate*Resource()` 系は、同名または同パスのキャッシュが存在する場合でも作り直します。
- `ShaderProgramResource` は生ポインタで各リソースを参照します。実体の所有権は `ShaderRepository` 側のキャッシュにあります。
- `ShaderRepository::Finalize()` は現在 `m_shaderCache` のみ clear しています。頂点、ピクセル、定数バッファのキャッシュ寿命を明確にしたい場合は、今後整理対象になります。
