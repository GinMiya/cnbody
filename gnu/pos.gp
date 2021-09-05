reset
set xr [-3:3]
set yr [-3:3]
set zr [-3:3]
set ticslevel 0
set view equal xyz

set grid
set term gif animate delay 2
set output "position.gif"
unset title

set datafile separator ","

n0 = 0
nm = 380
dn = 1

load 'gnu/pos_gif.plt'
