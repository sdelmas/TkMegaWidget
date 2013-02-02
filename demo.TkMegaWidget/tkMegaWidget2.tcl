wm maxsize . 1000 1000
frame .m
megawidget .m.n -background red -interpreter test -tksupport 1 -borderwidth 2 -relief raised -libfile tkMegaWidget3.tcl -createcommand create
pack .m.n -side bottom -fill both -expand 1
#pack .m.n -side bottom
#place .m.n -x 10 -y 20 -width 90 -height 50
#place .m.n -relx .1 -rely .1 -relwidth .4 -relheight .2
button .b1 -text {Main test} -command {puts stdout "children [winfo children .]"; puts stdout "children [winfo children .m]"}
pack .b1
pack .m -fill both -expand 1


