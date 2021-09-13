#!/bin/bash


###############################################################
# ** 出力先ディレクトリ
#    >> OUTPUT_DIR: データ出力のためのディレクトリ
#    >> BINARY_FLG: バイナリデータ出力のフラグ
#    >> MAKE_LOGFILE: logファイルを残す場合は有効にする
###############################################################
OUTPUT_DIR="result/"
BINARY_FLG=0
MAKE_LOGFILE=0

###############################################################
# ** 時間に関するパラメータの指定
#    >> TIME_END: シミュレーションの終了時間
#    >> DT: シミュレーションのタイムステップ
#    >> DT_SNA: 粒子データ出力の時間間隔
#    >> DT_DIAGRAM: 画面出力の時間間隔
###############################################################
TIME_END=3.0
DT=0.0078125
DT_SNAP=0.0078125
DT_DIAGRAM=0.015625

###############################################################
# ** モデルに関するパラメータの指定
#    >> NPTCL: 粒子数
#    >> SOFTLEN: Softening length
#    >> RVIR: ビリアル比
###############################################################
NPTCL=2048
SOFTLEN=0.03125
RVIR=0.1

###############################################################
# ** 使える物理コア・理論コア数の取得
###############################################################
PHYSICAL_CORES=`grep cpu.cores /proc/cpuinfo | sort -u | awk '{print substr($0,13)}'`
THEORETICAL_CORES=`grep processor /proc/cpuinfo | wc -l`
LIMIT_NUM=$((PHYSICAL_CORES*THEORETICAL_CORES))
###############################################################
# ** プロセス,スレッド並列数の指定
#    >> OMP_NUM_OF_THREADS: OpenMP並列数 (THEORETICAL_CORESの場合はMAX並列)
###############################################################
OMP_NUM_OF_THREADS=$THEORETICAL_CORES
###############################################################
# ** 総並列数の計算と計算機環境との整合性をとる
###############################################################
if [ $OMP_NUM_OF_THREADS -gt $THEORETICAL_CORES ]; then
  echo "ERROR!! OpenMP並列数が多すぎんよ〜"
  echo "スレッド並列数は $THEORETICAL_CORES 以下にしてくれ〜"
  exit 1
else
  export OMP_NUM_THREADS=$OMP_NUM_OF_THREADS
fi

###############################################################
# ** 実行オプションの作成　必要内実行オプションはコメントアウトするべし
###############################################################
# OPTION+=" -o $OUTPUT_DIR"
OPTION+=" -t $DT"
OPTION+=" -T $TIME_END"
OPTION+=" -n $NPTCL"
OPTION+=" -s $SOFTLEN"
OPTION+=" -v $RVIR"
OPTION+=" -d $DT_SNAP"
OPTION+=" -D $DT_DIAGRAM"
if [ $BINARY_FLG -eq 1 ]; then
  OPTION+=" -b"
fi

###############################################################
# ** プログラム実行
###############################################################
if [ $MAKE_LOGFILE -eq 1 ]; then
  ./nbody.out $OPTION 1> stdout 2> stderr
else
  ./nbody.out $OPTION
fi
