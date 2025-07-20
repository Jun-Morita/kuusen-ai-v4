# kuusen-ai-v4 / 第4回 空戦AIチャレンジ

AI agent developed for the 4th Air Combat AI Challenge (SIGNATE, 2025), hosted by Japan’s ATLA. This project builds a fighter aircraft control system using a 3D simulation environment. The goal is to protect an escort plane while defeating enemy forces.

[コンペページ](https://user.competition.signate.jp/ja/competition/detail/?competition=de1556abda294254b30bdec61520f764)

[シミュレータ説明資料 (HTML)](./simulator_dist/docs/html/core/index.html)

---

## 1. 推奨動作環境

以下の項目が主要な依存です。詳細は各セクションを参照してください。

- **OS**: Ubuntu 22.04 LTS
- **C++ コンパイラ**: gcc 11.4.0 (C++20)
- **Boost**: 1.88.0
- **Eigen**: 3.4.0
- **CMake**: 3.22.1
- **pybind11**: 2.13.6
- **Python**: 3.10
- **PyTorch**: ≥2.1.0
- **ray[rllib,tune]**: 2.7.0〜2.46.0
- **gymnasium**: 0.28.1〜1.0.0
- 他 C++ ライブラリ: cereal, NLopt, nlohmann’s json, thread-pool など（詳細は C++依存ライブラリ一覧へ）

---

## 2. リポジトリ構成

```
├── simulator_dist/          # ATLA 提供のシミュレータ本体
│   ├── docs/               # Doxygen 生成ドキュメント
│   ├── install_all.py      # ビルド・インストール用スクリプト
│   ├── core_plugins/       # コアプラグイン
│   └── user_plugins/       # ユーザープラグイン
├── Agents/                  # 独自エージェントコード
├── sample_submit/           # 提出用フォーマット例
├── scripts/                 # 補助スクリプト (evaluator, training など)
└── LICENSE-Apache-2.0       # オリジナルコードのライセンス
```

---

## 3. インストール方法

### 3.1 Python 仮想環境の準備

```bash
# 任意の仮想環境管理 (venv / uv / conda) を使用してください
python3 -m venv .venv
source .venv/bin/activate
pip install "setuptools>=62.4"
```

### 3.2 シミュレータ本体とプラグインのビルド・インストール

リポジトリ直下で以下を順に実行します。

```bash
# 本体部分のみ
python simulator_dist/install_all.py -i -c
# core_plugins 全て
python simulator_dist/install_all.py -i -p
# user_plugins 全て
python simulator_dist/install_all.py -i -u
# 第4回チャレンジ用サンプル
python simulator_dist/install_all.py -i -m "./simulator_dist/sample/modules/R7ContestSample"
```

### 3.3 深層学習フレームワークのインストール

以下は要件に合わせて任意のバージョンをインストールしてください。

```bash
pip install torch torchvision  # PyTorch
pip install tensorflow tensorboard  # TensorFlow (必要に応じて)
```

---

## 4. Docker コンテナの使用

簡易に環境を再現する場合は付属スクリプトでイメージを構築・起動可能です。

### 4.1 イメージのビルド

```bash
bash docker/build_docker_image.sh [cli|vnc|xrdp]
```

### 4.2 各モードの説明

- **cli** : GUI 非対応のシンプルな CLI モード。WSL やヘッドレス環境で推奨。
- **vnc** : コンテナ内で VNC サーバーを起動し、VNC クライアント (例: TigerVNC, RealVNC) で GUI を利用。
- **xrdp** : コンテナ内で xrdp サーバーを立て、Windows 標準のリモートデスクトップ (mstsc) で接続。

### 4.3 コンテナの起動

```bash
# 一般的な起動例
bash docker/run_container.sh [cli|vnc|xrdp]
```

#### WSL 環境の場合

- GUI を利用しない CLI モード (cli) を推奨します。

```bash
bash docker/run_container.sh cli
```

# コンテナ内仮想環境の有効化

source /opt/data/venv/acac\_4th/bin/activate

---

## 5. 動作確認 動作確認

### 5.1 シミュレータ本体の検証

```bash
# 仮想環境 or コンテナ内で実行
cd simulator_dist
python validate.py
```

### 5.2 ルールベースモデル同士のバトル

```bash
cd simulator_dist/sample/scripts/R7Contest/MinimumEvaluation
python evaluator.py "Rule-Fixed" "Rule-Fixed" -n 1 -v -l "../result"
```

GUI 無効化:

```bash
python evaluator.py "Rule-Fixed" "Rule-Fixed" -n 1 -l "../result"
```

---

## 6. エージェント開発

- `Agents/` 以下に独自エージェントのフォルダを作成し、`python evaluator.py` 等で評価可能。
- 学習用サンプル: `sample/scripts/R7Contest/HandyRLSample` を参照。

---

## 7. アンインストール

```bash
# 本体
python simulator_dist/uninstall_all.py -c
# core_plugins
python simulator_dist/uninstall_all.py -p
# user_plugins
python simulator_dist/uninstall_all.py -u
```

---

## 8. ライセンス

- **simulator\_dist/** など ATLA 提供部分: 再配布禁止
- オリジナルコード (Agents/, scripts/): Apache License 2.0
- 詳細は [SIGNATE competition terms](https://signate.jp/competitions/1635) を参照

