# kuusen-ai-v4 / 第４回 空戦AIチャレンジ

AI agent developed for the 4th Air Combat AI Challenge (SIGNATE, 2025), hosted by Japan’s ATLA. This project builds a fighter aircraft control system using a 3D simulation environment. The goal is to protect an escort plane while defeating enemy forces. Compatible with Linux (WSL/Docker). Includes rule-based and RL-based agent design.

[コンペページ](https://user.competition.signate.jp/ja/competition/detail/?competition=de1556abda294254b30bdec61520f764)

[説明資料](./simulator_dist/docs/html/core/index.html)


## 環境構築手順

### Dockerによる環境構築

#### 前提条件
- Docker Desktop がインストール済み
- NVIDIA GPU を利用する場合は NVIDIA Container Toolkit の導入が必要

#### 実行手順
```bash
cd simulator_dist
docker compose up -d
docker exec -it atla4 bash
cd /workspace  # バインドマウントされた作業ディレクトリ
```
atla4 は docker-compose.yml の container_name によります。

コンテナに入った後、以下のように GPU が認識されているか確認できます：
```bash
python -c "import torch; print(torch.cuda.is_available())"
```

#### GUI操作やJupyter Labの利用
- `jupyter lab` や `noVNC` を使った GUI 開発環境も含まれています。
- 詳細は `simulator_dist/docs/` にある説明資料をご参照ください。

### ローカル構成（WSL + RTX3060 + uv）での環境構築

#### 前提条件
- Windows 上の WSL2 + Ubuntu 22.04
- CUDA 対応 GPU（例：RTX3060）と NVIDIA ドライバ
- Python 3.10 以上、および uv（仮想環境管理）

#### 手順
```bash
# uv が未インストールなら
pip install uv

# プロジェクトルート
cd simulator_dist

# 仮想環境の作成と有効化
uv venv .venv
source .venv/bin/activate

# 依存パッケージのインストール
cd dist/linux
uv pip install -r requirements.txt

# .whl ファイルがある場合（必要に応じて）
uv pip install *.whl

# GPU確認
python -c "import torch; print(torch.cuda.is_available())"
```

### シミュレータの動作確認
```bash
cd simulator_dist
python validate.py
```

### 補足
- sample_submit/ フォルダには提出用フォーマットのサンプルが含まれています。
- Docker 版・ローカル版いずれも Agents/ 以下に任意のエージェントを追加して開発・評価できます。


## License

This repository includes code under two different licensing conditions:

- **simulator_dist/** and related files are provided by ATLA for the Air Combat AI Challenge and must not be redistributed.  
- All original code (e.g., `Agents/`, `scripts/`) is licensed under the [Apache License 2.0](./LICENSE-Apache-2.0).

Please see [SIGNATE competition terms](https://signate.jp/competitions/1635) for details on simulator usage.
