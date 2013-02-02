topmegawidget .debug \
  -debug 1 \
  -libfile debugInt.tcl \
  -interpreter debug \
  -tksupport 1 \
  -createcommand createCommand

button .l -text test -command sabber
pack .l

proc sabber {} {
  puts cmd1
  puts cmd2
  puts cmd3
}

b -glob "sabber*"

# eof
