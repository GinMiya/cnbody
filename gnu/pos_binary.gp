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

do for [i=0: 127] {
  loadnamen = sprintf("result/%d.dat",i)
  spl loadnamen binary format="%f%f%f" u ($1):($2):($3) w dot notitle
}
