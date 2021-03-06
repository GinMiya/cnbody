reset

# ========================================
# 軸設定
set xr [-1:1]
set yr [-1:1]
set zr [-1:1]
set ticslevel 0
set view equal xyz
unset tics

# ========================================
# グラフ設定
unset grid # グラフに補助線を引く
unset title # 凡例を消す
set noborder

# ========================================
# 読み込みファイル設定
# set datafile separator "," # CSVの場合は有効にする

# ========================================
# 変数設定
if ( ARGC > 0 ) { # シェルから渡された引数の有無を判定
  NSTART  = ARG1 + 0
  NFILE   = ARG2 - 1
  DERAY   = ARG3
} else {
  NFILE   = 100 # 読み込みファイル数
  NSTART  = 0 # 読み込み開始ID
  PICDIR  = "pic/" # 出力先ディレクトリ
  DATADIR = "result/" # 読み込み元ディレクトリ
  DERAY   = 2 # コマ送りスピード
}
PICNAME = PICDIR."position.gif" # 画像ファイル名

# ========================================
# terminal 設定
set term gif animate delay DERAY size 600,400 background rgb "black"  # git出力のterminal
set output PICNAME

# ========================================
# グラフ描画
do for [i=NSTART: NFILE] {
  set view 68,(i*360/NFILE),1,1
  loadnamen = DATADIR.sprintf("%d.dat",i)
  spl loadnamen u 1:2:3 w p pt 7 ps 0.2 lc rgb "yellow" notitle
}
