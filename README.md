# cnbody
C/C++で記述された最も簡単なN体シミュレーションコードです．
gnuplotによる可視化処理を含んでいます．
---
## Overview
このプログラムはN体シミュレーションに必要な処理をのうち，エッセンシャルな部分のみで構成されたプログラミングコードです．
プログラミング言語には，C/C++を利用しており，ローカル環境でも十分動作できるものを目指します．
※非常にシンプルに作成する想定のため，makeでのコンパイルは想定しません．

### Web page link
このプログラムを作成するための手順を記載したWebページを公開予定です．

### Feature
このプログラムは下記のシミュレーション想定で実装されています．
- 重力計算：直接計算法
- 時間積分法：Leap-Frog法
- テーマ：Cold Collapse
 - 初期密度：一様球
 - 初期速度：マクスウェル分布
- 解析ツール：gnuplot
 - 基礎）位置の時間変化をgif化
 - 基礎）全エネルギーの時間変化
 - 発展）タイムステップによるエネルギー誤差
 - 発展）バイナリ形式のファイルを読み込む
- 実行オプション
 - 出力先ディレクトリを変更できる
 - 終了時間を変更できる
 - 粒子数を変更できる
 - Softening lengthが変更できる
 - ファイル出力の周期を変更できる
 - ファイル出力形式をバイナリに変更できる

## Execution options
- -o：出力先ディレクトリ名の指定
- -t：終了時間の指定
- -n:粒子数の指定
- -s:Softening lengthの指定
- -d:ファイル出力の周期をタイムステップ数で指定
- -b：ファイルをバイナリで出力する

## Coding rule
- マクロ：全大文字＋アンダースコア
- グローバル変数：頭文字大文字
- ローカル変数：頭文字小文字
