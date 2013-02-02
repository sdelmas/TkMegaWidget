puts stdout "Loading user file..."

if {"[info commands mega.*]" != ""} {
  puts stdout "Interpset and Interpeval"
  [lindex [info commands mega.*] 0] interpset testVar1 aaa
  [lindex [info commands mega.*] 0] interpeval set testVar2 bbb
  puts stdout "Interpset and Interpeval...done"

  proc configError2 {args} {
    error "configure aborted"
  }

  proc configStandard2 {args} {
    puts stdout "$args"
  }

  proc testConfig {args} {
    catch "mega.p config -opt2" res
    if {"$res" != "-opt2 {opt2} {Opt2} {defVal2} {val2}"} {
      puts stdout "User File Error1: $res"
    }

    catch "mega.p conf -opt2" res
    if {"$res" != "-opt2 {opt2} {Opt2} {defVal2} {val2}"} {
      puts stdout "User File Error2: $res"
    }

    catch "mega.p confhidden -opt2" res
    if {"$res" != "val2"} {
      puts stdout "User File Error3: $res"
    }

    catch "mega.p config -opt2 newValue" res
    if {"$res" != "not a valid option \"-opt2\""} {
      puts stdout "User File Error4: $res"
    }   

    catch "mega.p confhidden -opt2 newValue" res
    if {"$res" != "not a valid option \"-opt2\""} {
      puts stdout "User File Error5: $res"
    }

    catch "mega.p confhidden -opt3 newValue" res
    if {"$res" != ""} {
       puts stdout "User File Error6: $res"
    }

    catch "mega.p confhidden" res
    if {"$res" != "{-background background Background #d9d9d9 #d9d9d9} {-bd borderWidth} {-bg background} {-borderwidth borderWidth BorderWidth 0 0} {-class class Class {} {}} {-colormap colormap Colormap {} {}} {-createcommand createCommand CreateCommand {} localCreateCommand2} {-crypting crypting Crypting 0 0} {-cursor cursor Cursor {} {}} {-height height Height 0 0} {-highlightbackground highlightBackground HighlightBackground #d9d9d9 #d9d9d9} {-highlightcolor highlightColor HighlightColor Black Black} {-highlightthickness highlightThickness HighlightThickness 0 0} {-interpreter interpreter Interpreter {} test} {-libfile libFile LibFile {} {}} {-relief relief Relief flat flat} {-takefocus takeFocus TakeFocus 0 0} {-tksupport tkSupport TkSupport 0 0} {-userfile userFile UserFile {} userFile.tcl.crypt} {-visual visual Visual {} {}} {-width width Width 0 0} {-opt3 {opt3} {Opt3} {defVal3} {newValue}}" &&
  "$res" != "{-background background Background #d9d9d9 #d9d9d9} {-bd borderWidth} {-bg background} {-borderwidth borderWidth BorderWidth 0 0} {-class class Class {} {}} {-colormap colormap Colormap {} {}} {-createcommand createCommand CreateCommand {} localCreateCommand2} {-crypting crypting Crypting 0 0} {-cursor cursor Cursor {} {}} {-height height Height 0 0} {-highlightbackground highlightBackground HighlightBackground #d9d9d9 #d9d9d9} {-highlightcolor highlightColor HighlightColor Black Black} {-highlightthickness highlightThickness HighlightThickness 0 0} {-interpreter interpreter Interpreter {} test} {-libfile libFile LibFile {} {}} {-relief relief Relief flat flat} {-takefocus takeFocus TakeFocus 0 0} {-tksupport tkSupport TkSupport 0 0} {-userfile userFile UserFile {} userFile.tcl.crypt} {-visual visual Visual {} {}} {-width width Width 0 0} {-debug debug Debug 0 0} {-opt3 {opt3} {Opt3} {defVal3} {newValue}}" &&
  "$res" != "{-background background Background #d9d9d9 #d9d9d9} {-bd borderWidth} {-bg background} {-borderwidth borderWidth BorderWidth 0 0} {-class class Class {} {}} {-colormap colormap Colormap {} {}} {-createcommand createCommand CreateCommand {} localCreateCommand2} {-crypting crypting Crypting 0 1} {-cursor cursor Cursor {} {}} {-height height Height 0 0} {-highlightbackground highlightBackground HighlightBackground #d9d9d9 #d9d9d9} {-highlightcolor highlightColor HighlightColor Black Black} {-highlightthickness highlightThickness HighlightThickness 0 0} {-interpreter interpreter Interpreter {} test} {-libfile libFile LibFile {} {}} {-relief relief Relief flat flat} {-takefocus takeFocus TakeFocus 0 0} {-tksupport tkSupport TkSupport 0 0} {-userfile userFile UserFile {} userFile.tcl.crypt} {-visual visual Visual {} {}} {-width width Width 0 0} {-opt3 {opt3} {Opt3} {defVal3} {newValue}}" &&
  "$res" != "{-background background Background #d9d9d9 #d9d9d9} {-bd borderWidth} {-bg background} {-borderwidth borderWidth BorderWidth 0 0} {-class class Class {} {}} {-colormap colormap Colormap {} {}} {-createcommand createCommand CreateCommand {} localCreateCommand2} {-crypting crypting Crypting 0 1} {-cursor cursor Cursor {} {}} {-height height Height 0 0} {-highlightbackground highlightBackground HighlightBackground #d9d9d9 #d9d9d9} {-highlightcolor highlightColor HighlightColor Black Black} {-highlightthickness highlightThickness HighlightThickness 0 0} {-interpreter interpreter Interpreter {} test} {-libfile libFile LibFile {} {}} {-relief relief Relief flat flat} {-takefocus takeFocus TakeFocus 0 0} {-tksupport tkSupport TkSupport 0 0} {-userfile userFile UserFile {} userFile.tcl.crypt} {-visual visual Visual {} {}} {-width width Width 0 0} {-debug debug Debug 0 0} {-opt3 {opt3} {Opt3} {defVal3} {newValue}}"} {
      puts stdout "User File Error7: $res"
    }

    catch "mega.p conf -opt4" res
    if {"$res" != "-opt4 {opt4} {Opt4} {defVal4} {val4}"} {
      puts stdout "User File Error8: $res"
    }

    catch "mega.p confhidden -opt4" res
    if {"$res" != "val4"} {
      puts stdout "User File Error9: $res"
    }

    catch "mega.p confhidden -opt4 newValue" res
    if {"$res" != "not a valid option \"-opt4\""} {
      puts stdout "User File Error10: $res"
    }

    catch "mega.p conf -opt4 newValue" res
    if {"$res" != "invalid command name \"dontFind\""} {
      puts stdout "User File Error11: $res"
    }

    catch "mega.p conf -opt5" res
    if {"$res" != "-opt5 {opt5} {Opt5} {defVal5} {val5}"} {
      puts stdout "User File Error12: $res"
    }

    catch "mega.p confhidden -opt5" res
    if {"$res" != "val5"} {
      puts stdout "User File Error13: $res"
    }

    catch "mega.p conf -opt5 newValue" res
    if {"$res" != "configure aborted"} {
      puts stdout "User File Error14: $res"
    }

    catch "mega.p optiondelete -background" res

    catch "mega.p config -dontFind" res
    if {"$res" != "unknown option \"-dontFind\""} {
      puts stdout "User File Error15: $res"
    }

    catch "mega.p config -dontFind aaa" res
    if {"$res" != "unknown option \"-dontFind\""} {
      puts stdout "User File Error16: $res"
    }
  }
}

proc localCreateCommand {args} {
  if {"$args" != ".q -crypting 1 -interpreter test -libfile libFile.tcl.crypt -userfile userFile.tcl.crypt -createcommand localCreateCommand"} {
    puts stdout "User File Error17: $args"
  }
}

proc localCreateCommand2 {args} {
  mega.p minoradd testconfig testConfig
  mega.p optionadd -opt1 opt1 Opt1 val1 defVal1 normal {configStandard2}
  mega.p optionadd -opt2 opt2 Opt2 val2 defVal2 readonly {configStandard2}
  mega.p optionadd -opt3 opt3 Opt3 val3 defVal3 hidden {configStandard2}
  mega.p optionadd -opt4 opt4 Opt4 val4 defVal4 normal {dontFind}
  mega.p optionadd -opt5 opt5 Opt5 val5 defVal5 normal {configError2}
}

if {"[lsort [info procs]]" != "configError2 configStandard2 localCreateCommand localCreateCommand2 testConfig" &&
    "[lsort [info procs]]" != "auto_execok auto_load auto_mkindex auto_reset configError configStandard createCommand localCreateCommand localCreateCommand2 tkButtonDown tkButtonEnter tkButtonInvoke tkButtonLeave tkButtonUp tkCancelRepeat tkCheckRadioInvoke tkEntryAutoScan tkEntryBackspace tkEntryButton1 tkEntryClipboardKeysyms tkEntryInsert tkEntryKeySelect tkEntryMouseSelect tkEntrySeeInsert tkEntrySetCursor tkEntryTranspose tkFirstMenu tkListboxAutoScan tkListboxBeginExtend tkListboxBeginSelect tkListboxBeginToggle tkListboxCancel tkListboxDataExtend tkListboxExtendUpDown tkListboxMotion tkListboxSelectAll tkListboxUpDown tkMbButtonUp tkMbEnter tkMbLeave tkMbMotion tkMbPost tkMenuButtonDown tkMenuEscape tkMenuFind tkMenuFindName tkMenuFirstEntry tkMenuInvoke tkMenuLeave tkMenuLeftRight tkMenuMotion tkMenuNextEntry tkMenuUnpost tkPostOverPoint tkScaleActivate tkScaleButtonDown tkScaleControlPress tkScaleDrag tkScaleEndDrag tkScaleIncrement tkScreenChanged tkScrollButtonDown tkScrollButtonUp tkScrollByPages tkScrollByUnits tkScrollDrag tkScrollEndDrag tkScrollSelect tkScrollStartDrag tkScrollToPos tkScrollTopBottom tkTextAutoScan tkTextButton1 tkTextClipboardKeysyms tkTextInsert tkTextKeyExtend tkTextKeySelect tkTextNextPara tkTextPrevPara tkTextResetAnchor tkTextScrollPages tkTextSelectTo tkTextSetCursor tkTextTranspose tkTextUpDownLine tkTraverseToMenu tkTraverseWithinMenu tk_popup unknown"} {
  puts stdout "User File Error18: [lsort [info procs]]"
}

puts stdout "Loading user file...done"

