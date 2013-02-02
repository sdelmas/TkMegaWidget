proc create {pathName args} {
  button .n.b1 -text "testing testing 1" -command {puts stdout $mega_interpreter$mega_crypting; puts stdout ".n size [winfo width .n]x[winfo height .n]"; puts stdout "children [winfo children .]"; puts stdout "children [winfo children .n]" }
  button .n.b2 -text "testing testing 2" -command {puts stdout $mega_interpreter$mega_crypting; puts stdout ".n size [winfo width .n]x[winfo height .n]"; puts stdout "children [winfo children .]"; puts stdout "children [winfo children .n]" }
  button .n.b3 -text "testing testing 3" -command {puts stdout $mega_interpreter$mega_crypting; puts stdout ".n size [winfo width .n]x[winfo height .n]"; puts stdout "children [winfo children .]"; puts stdout "children [winfo children .n]" }
  button .n.b4 -text "testing testing 4" -command {puts stdout $mega_interpreter$mega_crypting; puts stdout ".n size [winfo width .n]x[winfo height .n]"; puts stdout "children [winfo children .]"; puts stdout "children [winfo children .n]" }
  pack .n.b1 -fill both -expand 1
  pack .n.b2 -side right
  place .n.b3 -x 50 -y 30 -width 80 -height 30
  place .n.b4 -relx .1 -rely .1 -relwidth .4 -relheight .3
}

