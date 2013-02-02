wm maxsize . 1000 1000

catch "megawidget .o -crypting 0 -libfile libFileError.tcl" res
catch "megawidget .o -crypting 0 -libfile libFileError.tcl" res
if {"$res" != "loading of lib file aborted"} {
  puts stdout "Error2: $res"
}

megawidget .l

.l minoradd minorcommand1 {puts stdout minorcommand1}
.l minoradd minorcommand2 {puts stdout minorcommand2}
.l minoradd minorcommand3 {dontFind}

.l optionadd -opt1 opt1 Opt1 val1 defVal1 normal {configStandard}
.l optionadd -opt2 opt2 Opt2 val2 defVal2 readonly {configStandard}
.l optionadd -opt3 opt3 Opt3 val3 defVal3 hidden {configStandard}
.l optionadd -opt4 opt4 Opt4 val4 defVal4 normal {dontFind}
.l optionadd -opt5 opt5 Opt5 val5 defVal5 normal {configError}
.l optionadd -background background Background val3 defVal3 normal {configStandard}

button .l.b -text test1 -command "winfo children ."

frame .l.f

button .l.f.b -text test2 -command "puts stdout aaa1; wm maxsize ."

pack .l.f.b -side top
pack .l.b -side top
pack .l.f -side top -fill both -expand 1
pack .l -side top -fill both -expand 1

proc createCommand {args} {
  if {"$args" != ".n -libfile libFile.tcl -userfile userFile.tcl -createcommand createCommand"} {
    puts stdout "Error1: $args"
  }
}

proc configError {args} {
  error "configure aborted"
}

proc configStandard {args} {
  puts stdout "$args"
}

megawidget .m \
  -crypting 1 \
  -interpreter test

megawidget .n \
  -libfile libFile.tcl \
  -userfile userFile.tcl \
  -createcommand "createCommand"

megawidget .q \
  -crypting 1 \
  -interpreter test \
  -libfile libFile.tcl.crypt \
  -userfile userFile.tcl.crypt \
  -createcommand "localCreateCommand"

megawidget .p \
  -crypting 1 \
  -userfile userFile.tcl.crypt \
  -interpreter test \
  -createcommand "localCreateCommand2"

catch "megawidget .o -libfile libFileError.tcl" res
if {"$res" != "loading of lib file aborted"} {
  puts stdout "Error2: $res"
}

catch "megawidget .o -userfile userFileError.tcl" res
if {"$res" != "loading of user file aborted"} {
  puts stdout "Error3: $res"
}

catch "megawidget .o -createcommand dontFind" res
if {"$res" != "invalid command name \"dontFind\""} {
  puts stdout "Error4: $res"
}

catch "megawidget .o -crypting 0 -libfile libFile1.tcl" res
if {"$res" != "couldn't open file \"libFile1.tcl\": No such file or directory"} {
  puts stdout "Error5: $res"
}

catch "megawidget .o1 -crypting 1 -libfile libFile.tcl.crypt" res
if {"$res" != "crypting only in a secure interpreter"} {
  puts stdout "Error6: $res"
}

catch "megawidget .o2 -crypting 0 -libfile libFile.tcl -interpreter test" res
if {"$res" != "previous crypting selection does not match"} {
  puts stdout "Error7: $res"
}

catch "megawidget .o -crypting 1 -libfile libFile1.tcl -interpreter test" res
if {"$res" != "couldn't open file \"libFile1.tcl\": No such file or directory"} {
  puts stdout "Error8: $res"
}

catch ".m minoradd dontFind" res
if {"$res" != "minor command \"minoradd\" only available in secure interpreter"} {
  puts stdout "Error9: $res"
}

catch ".m minorlist" res
if {"$res" != "minor command \"minorlist\" only available in secure interpreter"} {
  puts stdout "Error10: $res"
}

catch ".m minordelete dontFind" res
if {"$res" != "minor command \"minordelete\" only available in secure interpreter"} {
  puts stdout "Error11: $res"
}

catch ".l minorlist" res
if {"$res" != "minorcommand1 minorcommand2 minorcommand3 "} {
  puts stdout "Error12: $res"
}

catch ".l minorlist minorcommand1" res
if {"$res" != "puts stdout minorcommand1 "} {
  puts stdout "Error13: $res"
}

catch ".l minorlist minorcommand2 " res
if {"$res" != "puts stdout minorcommand2 "} {
  puts stdout "Error14: $res"
}

catch ".l minorlist dontFind" res
if {"$res" != "unknown minor command: \"dontFind\""} {
  puts stdout "Error15: $res"
}

catch ".l minordelete minorcommand2" res
if {"$res" != ""} {
  puts stdout "Error16: $res"
}
catch ".l minorlist" res
if {"$res" != "minorcommand1 minorcommand3 "} {
  puts stdout "Error17: $res"
}

catch ".l minorcommand3" res
if {"$res" != "invalid command name \"dontFind\""} {
  puts stdout "Error18: $res"
}

catch ".l minordelete minorcommand1" res
catch ".l minordelete minorcommand3" res
if {"$res" != ""} {
  puts stdout "Error19: $res"
}
catch ".l minorlist" res
if {"$res" != ""} {
  puts stdout "Error20: $res"
}

catch ".l minordelete dontFind" res
if {"$res" != "unknown minor command: \"dontFind\""} {
  puts stdout "Error21: $res"
}

.l minoradd minorcommand1 {puts stdout minorcommand1}

catch ".m dontFind" res
if {"$res" != "unknown minor command: \"dontFind\""} {
  puts stdout "Error22: $res"
}

catch ".m optionadd -opt1 opt1 Opt1 val1 defVal1 normal {configStandard}" res
if {"$res" != "minor command \"optionadd\" only available in secure interpreter"} {
  puts stdout "Error23: $res"
}

catch ".m optionlist" res
if {"$res" != "minor command \"optionlist\" only available in secure interpreter"} {
  puts stdout "Error24: $res"
}

catch ".m optiondelete dontFind" res
if {"$res" != "minor command \"optiondelete\" only available in secure interpreter"} {
  puts stdout "Error25: $res"
}

catch ".l optionlist" res
if {"[lsort $res]" != "-background -opt1 -opt2 -opt3 -opt4 -opt5"} {
  puts stdout "Error26: [lsort $res]"
}

catch ".l optionlist -opt1" res
if {"$res" != "-opt1 {opt1} {Opt1} {defVal1} {val1} {configStandard} normal"} {
  puts stdout "Error27: $res"
}

catch ".l optionlist -opt2" res
if {"$res" != "-opt2 {opt2} {Opt2} {defVal2} {val2} {configStandard} readonly"} {
  puts stdout "Error28: $res"
}

catch ".l optionlist dontFind" res
if {"$res" != "unknown option \"dontFind\""} {
  puts stdout "Error29: $res"
}

catch ".l optiondelete dontFind" res
if {"$res" != "unknown option \"dontFind\""} {
  puts stdout "Error30: $res"
}

catch ".l config -opt2" res
if {"$res" != "-opt2 {opt2} {Opt2} {defVal2} {val2}"} {
  puts stdout "Error31: $res"
}

catch ".l conf -opt2" res
if {"$res" != "-opt2 {opt2} {Opt2} {defVal2} {val2}"} {
  puts stdout "Error32: $res"
}

catch ".l confhidden -opt2" res
if {"$res" != "val2"} {
  puts stdout "Error33: $res"
}

catch ".l config -opt2 newValue" res
if {"$res" != "not a valid option \"-opt2\""} {
  puts stdout "Error34: $res"
}

catch ".l confhidden -opt2 newValue" res
if {"$res" != "not a valid option \"-opt2\""} {
  puts stdout "Error35: $res"
}

catch ".l confhidden -opt3 newValue" res
if {"$res" != ""} {
  puts stdout "Error36: $res"
}

catch ".l confhidden" res
if {"$res" != "{-background background Background #d9d9d9 #d9d9d9} {-bd borderWidth} {-bg background} {-borderwidth borderWidth BorderWidth 0 0} {-class class Class {} {}} {-colormap colormap Colormap {} {}} {-createcommand createCommand CreateCommand {} {}} {-crypting crypting Crypting 0 0} {-cursor cursor Cursor {} {}} {-height height Height 0 0} {-highlightbackground highlightBackground HighlightBackground #d9d9d9 #d9d9d9} {-highlightcolor highlightColor HighlightColor Black Black} {-highlightthickness highlightThickness HighlightThickness 0 0} {-interpreter interpreter Interpreter {} {}} {-libfile libFile LibFile {} {}} {-relief relief Relief flat flat} {-takefocus takeFocus TakeFocus 0 0} {-tksupport tkSupport TkSupport 0 0} {-userfile userFile UserFile {} {}} {-visual visual Visual {} {}} {-width width Width 0 0} {-debug debug Debug 0 0} {-opt3 {opt3} {Opt3} {defVal3} {newValue}}" &&
  "$res" != "{-background background Background #d9d9d9 #d9d9d9} {-bd borderWidth} {-bg background} {-borderwidth borderWidth BorderWidth 0 0} {-class class Class {} {}} {-colormap colormap Colormap {} {}} {-createcommand createCommand CreateCommand {} {}} {-crypting crypting Crypting 0 0} {-cursor cursor Cursor {} {}} {-height height Height 0 0} {-highlightbackground highlightBackground HighlightBackground #d9d9d9 #d9d9d9} {-highlightcolor highlightColor HighlightColor Black Black} {-highlightthickness highlightThickness HighlightThickness 0 0} {-interpreter interpreter Interpreter {} {}} {-libfile libFile LibFile {} {}} {-relief relief Relief flat flat} {-takefocus takeFocus TakeFocus 0 0} {-tksupport tkSupport TkSupport 0 0} {-userfile userFile UserFile {} {}} {-visual visual Visual {} {}} {-width width Width 0 0} {-debug debug Debug 0 0} {-opt3 {opt3} {Opt3} {defVal3} {newValue}}"} {
  puts stdout "Error37: $res"
}

catch ".l conf -opt4" res
if {"$res" != "-opt4 {opt4} {Opt4} {defVal4} {val4}"} {
  puts stdout "Error38: $res"
}

catch ".l conf -opt4 newValue" res
if {"$res" != "invalid command name \"dontFind\""} {
  puts stdout "Error39: $res"
}

catch ".l conf -opt5" res
if {"$res" != "-opt5 {opt5} {Opt5} {defVal5} {val5}"} {
  puts stdout "Error40: $res"
}

catch ".l confhidden -opt5" res
if {"$res" != "val5"} {
  puts stdout "Error41: $res"
}

catch ".l conf -opt5 newValue" res
if {"$res" != "configure aborted"} {
  puts stdout "Error42: $res"
}

catch ".l optiondelete -opt2" res
if {"$res" != ""} {
  puts stdout "Error43: $res"
}
catch ".l optionlist" res
if {"[lsort $res]" != "-background -opt1 -opt3 -opt4 -opt5"} {
  puts stdout "Error44: $res"
}

catch ".l optiondelete -background" res
catch ".l optiondelete -opt1" res
catch ".l optiondelete -opt3" res
catch ".l optiondelete -opt4" res
catch ".l optiondelete -opt5" res
if {"$res" != ""} {
  puts stdout "Error45: $res"
}
catch ".l optionlist" res
if {"$res" != ""} {
  puts stdout "Error46: $res"
}

.l optionadd -opt1 opt1 Opt1 val1 defVal1 normal {configStandard}

catch ".l config -dontFind" res
if {"$res" != "unknown option \"-dontFind\""} {
  puts stdout "Error47: $res"
}

catch ".l config -dontFind aaa" res
if {"$res" != "unknown option \"-dontFind\""} {
  puts stdout "Error48: $res"
}

catch ".p testconfig" res
if {"$res" != ""} {
  puts stdout "Error49: $res"
}

catch ".l cget -dontFind" res
if {"$res" != "unknown option \"-dontFind\""} {
  puts stdout "Error50: $res"
}

catch ".l cget -background" res
if {"$res" != "#d9d9d9"} {
  puts stdout "Error51: $res"
}

catch ".l cget -opt1" res
if {"$res" != "val1"} {
  puts stdout "Error52: $res"
}

catch ".l cget" res
if {"$res" != "wrong # args: should be \".l cget option\""} {
  puts stdout "Error53: $res"
}

catch ".l cget -opt1 -opt2" res
if {"$res" != "wrong # args: should be \".l cget option\""} {
  puts stdout "Error54: $res"
}

catch "megawidget .r -dontFind" res
if {"$res" != "value for \"-dontFind\" missing"} {
  puts stdout "Error55: $res"
}


catch ".m interpeval puts stdout test" res
if {"$res" != "minor command \"interpeval\" only available in secure interpreter"} {
  puts stdout "Error56: $res"
}

catch ".m interpset puts stdout test" res
if {"$res" != "minor command \"interpset\" only available in secure interpreter"} {
  puts stdout "Error57: $res"
}

catch "set testVar1" res
if {"$res" != "aaa"} {
  puts stdout "Error58: $res"
}

catch "set testVar2" res
if {"$res" != "bbb"} {
  puts stdout "Error59: $res"
}

.l recursiveconf -background red

catch ".l cget -background" res
if {"$res" != "red"} {
  puts stdout "Error60: $res"
}

catch ".l conf -opt1 veryNewValue" res
if {"$res" != ""} {
  puts stdout "Error61: $res"
}

set iconPaneDir ""
set megaWidgetDir ""
set dirNames $tk_library
lappend dirNames ../library
foreach dirname $dirNames {
  if {[file exists $dirname/tkIconPaneInt.tcl]} {
    set iconPaneDir $dirname
  }
  if {[file exists $dirname/tkMegaWidget.tcl]} {
    set megaWidgetDir $dirname
  }
}

if {"$iconPaneDir" != "" && "$megaWidgetDir" != ""} {
  button .l.c1 -text "Delete iconpane0" -command "destroy .multi0"
  pack .l.c1 
  megawidget .multi0 -interpreter .multi \
    -crypting 0 \
    -createcommand tkiconpane_create \
    -userfile $iconPaneDir/tkIconPaneInt.tcl \
    -libfile $megaWidgetDir/tkMegaWidget.tcl
  pack .multi0 -fill both -expand 1

  button .l.c2 -text "Delete iconpane1" -command "destroy .multi1"
  pack .l.c2 
  megawidget .multi1 -interpreter .multi \
    -crypting 0 \
    -createcommand tkiconpane_create \
    -userfile $iconPaneDir/tkIconPaneInt.tcl \
    -libfile $megaWidgetDir/tkMegaWidget.tcl
  pack .multi1 -fill both -expand 1

  button .l.c3 -text "Delete iconpane2" -command "destroy .multi2"
  pack .l.c3
  megawidget .multi2 -interpreter .multi \
    -crypting 0 \
    -createcommand tkiconpane_create \
    -userfile $iconPaneDir/tkIconPaneInt.tcl \
    -libfile $megaWidgetDir/tkMegaWidget.tcl
  pack .multi2 -fill both -expand 1
}

