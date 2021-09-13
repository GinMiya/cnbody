reset

# ========================================
# 軸設定
set xtics font "Times,10"
set ytics font "Times,10"
# set xr [0:3] # x軸の描画範囲
# set yr [0:1] # 左y軸の描画範囲
set logscale y
set format y "10^{%L}"
set xl 'Time' font 'Times,20'
set yl 'Energy error' font 'Times,20'

# ========================================
# グラフ設定
set grid # グラフに補助線を引く
set size square # グラフを正方形にする
# set view equal xy # x軸とy軸の単位の長さが強制的に等しくなるように縮尺を合わせる
# set key outside right # 凡例をグラフの外（右）に出す

# ========================================
# 読み込みファイル設定
if ( !defined(DATADIR) ) {
  DATADIR = "result/" # 読み込み元ディレクトリ
}
if ( !defined(PICDIR) ) {
  PICDIR = "pic/" # 画像保存先ディレクトリ
}
FILENAME = DATADIR."energy.dat"
PICNAME = PICDIR."energy_err.png" # 画像ファイル名
# set datafile separator "," # CSVの場合は有効にする

# ========================================
# terminal 設定
set term png enhanced
set output PICNAME

# ========================================
# グラフ描画
pl FILENAME u ($1):(abs($4)) w lp pt 7 ps 0.3 notitle
