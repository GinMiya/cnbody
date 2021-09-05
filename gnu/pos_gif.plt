if(exist ("n") ==0 || n < 0 ) n = n0

filename = sprintf("result/%d.dat",n)
splot filename u 1:2:3 w dot notitle

n = n + dn
if ( n < nm ) reread
undefine n
