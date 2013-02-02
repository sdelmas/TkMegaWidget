#!wish3.3 -f
# Program: tkencrypt
# Copyright: Sven Delmas
#

# module inclusion
global env
global cbLoadPath
global cbLoadInfo
set cbLoadInfo 0
if {[info exists env(CB_LOAD_PATH)]} {
  if {[string first $env(CB_LOAD_PATH) .:/usr/local/lib/] == -1} {
    set cbLoadPath $env(CB_LOAD_PATH):.:/usr/local/lib/
  } {
    set cbLoadPath .:/usr/local/lib/
  }
} {
  set cbLoadPath .:/usr/local/lib/
}

global argc
global argv
set tmpArgv ""
for {set counter 0} {$counter < $argc} {incr counter 1} {
  case [string tolower [lindex $argv $counter]] in {
    {-cbloadpath} {
      incr counter 1
      set cbLoadPath "[lindex $argv $counter]:$cbLoadPath"
    }
    {-cbstartup} {
      incr counter 1
      source [lindex $argv $counter]
    }
    {-cbbindfile} {
      incr counter 1
      set env(CB_BIND_FILE) "[lindex $argv $counter]"
    }
    {-cbcolorfile} {
      incr counter 1
      set env(CB_COLOR_FILE) "[lindex $argv $counter]"
    }
    {-cbcursorfile} {
      incr counter 1
      set env(CB_CURSOR_FILE) "[lindex $argv $counter]"
    }
    {-cbfontfile} {
      incr counter 1
      set env(CB_FONT_FILE) "[lindex $argv $counter]"
    }
    {-cbmodelmono} {
      tk colormodel . monochrome
    }
    {-cbmodelcolor} {
      tk colormodel . color
    }
    {-cbloading} {
      set cbLoadInfo 1
    }
    {-cbnoloading} {
      set cbLoadInfo 0
    }
    {default} {
      lappend tmpArgv [lindex $argv $counter]
    }
  }
}
set argv $tmpArgv
set argc [llength $tmpArgv]
unset counter
unset tmpArgv


# procedure to show window .
proc ShowWindow. {args} {

  # Window manager configurations
  wm positionfrom . user
  wm sizefrom . ""
  wm maxsize . 1024 1024
  wm title . {cb}


  # build widget .frame1
  frame .frame1 \
    -borderwidth {2} \
    -relief {raised}

  # build widget .frame1.button6
  button .frame1.button6 \
    -command {SaveKey} \
    -text {Save Key} \
    -width {9}

  # build widget .frame1.button4
  button .frame1.button4 \
    -command {Quit} \
    -text {Quit} \
    -width {9}

  # build widget .frame1.button3
  button .frame1.button3 \
    -command {Decrypt} \
    -text {Decrypt} \
    -width {9}

  # build widget .frame1.button2
  button .frame1.button2 \
    -command {Encrypt} \
    -text {Encrypt} \
    -width {9}

  # pack widget .frame1
  pack .frame1.button2 -side left -padx 5 -pady 5 -expand 1
  pack .frame1.button3 -side left -padx 5 -pady 5 -expand 1
  pack .frame1.button4 -side left -padx 5 -pady 5 -expand 1
  pack .frame1.button6 -side left -expand 1

  # build widget .frame0
  frame .frame0 \
    -relief {raised}

  # build widget .frame0.frame5
  frame .frame0.frame5 \
    -borderwidth {2} \
    -relief {raised}

  # build widget .frame0.frame5.label1
  label .frame0.frame5.label1 \
    -borderwidth {0} \
    -text {Encryption Key:}

  # build widget .frame0.frame5.frame
  frame .frame0.frame5.frame

  # build widget .frame0.frame5.frame.scrollbar1
  scrollbar .frame0.frame5.frame.scrollbar1 \
    -command {.frame0.frame5.frame.entry2 xview} \
    -orient {horizontal} \
    -width {11}

  # build widget .frame0.frame5.frame.entry2
  entry .frame0.frame5.frame.entry2 \
    -relief {sunken} \
    -xscrollcommand {.frame0.frame5.frame.scrollbar1 set}

  # pack widget .frame0.frame5.frame
  pack .frame0.frame5.frame.entry2 -side top -expand 1 -fill both
  pack .frame0.frame5.frame.scrollbar1 -side top -fill x

  # pack widget .frame0.frame5
  pack .frame0.frame5.label1 -side left -fill y
  pack .frame0.frame5.frame -side top -expand 1 -fill both

  # build widget .frame0.frame
  frame .frame0.frame \
    -borderwidth {2} \
    -relief {raised}

  # build widget .frame0.frame.label1
  label .frame0.frame.label1 \
    -text {Filename:}

  # build widget .frame0.frame.frame
  frame .frame0.frame.frame

  # build widget .frame0.frame.frame.scrollbar1
  scrollbar .frame0.frame.frame.scrollbar1 \
    -command {.frame0.frame.frame.entry2 xview} \
    -orient {horizontal} \
    -width {11}

  # build widget .frame0.frame.frame.entry2
  entry .frame0.frame.frame.entry2 \
    -relief {sunken} \
    -xscrollcommand {.frame0.frame.frame.scrollbar1 set}
  # bindings
  bind .frame0.frame.frame.entry2 <Shift-Button-1> {[SN Filename] delete 0 end
[SN Filename] insert 0 [FSBox]}

  # pack widget .frame0.frame.frame
  pack .frame0.frame.frame.entry2 -side top -expand 1 -fill both
  pack .frame0.frame.frame.scrollbar1 -side top -fill x

  # pack widget .frame0.frame
  pack .frame0.frame.label1 -side left -fill both
  pack .frame0.frame.frame -side top -expand 1 -fill both

  # pack widget .frame0
  pack .frame0.frame -side top -expand 1 -fill both
  pack .frame0.frame5 -side top -expand 1 -fill both

  # pack widget .
  pack .frame0 -side top -expand 1 -fill both
  pack .frame1 -side top -fill both

  .frame0.frame5.frame.entry2 insert end {}
  .frame0.frame.frame.entry2 insert end {}


  if {"[info commands CBTB_commServerCommand]" != ""} {
    CBTB_commServerCommand "CBTB widgettreebind ."
  }
}


# User defined procedures


# Procedure: Decrypt
proc Decrypt {} {
if {"[[SN Key] get]" != "" && "[[SN Filename] get]" != "" &&
    "[file extension [[SN Filename] get]]" == ".crypt"} {
  exec des -D -k [string trim [[SN Key] get]] [[SN Filename] get] [file rootname [[SN Filename] get]]
}
}


# Procedure: Encrypt
proc Encrypt {} {
if {"[[SN Key] get]" != "" && "[[SN Filename] get]" != ""} {
  exec des -E -k [string trim [[SN Key] get]] [[SN Filename] get] [[SN Filename] get].crypt
}
}


# Procedure: FSBox
proc FSBox { {fsBoxMessage "Select file:"} {fsBoxFileName ""} {fsBoxActionOk ""} {fsBoxActionCancel ""}} {
##########
# Procedure: FSBox
# Description: show file selector box
# Arguments: fsBoxMessage - the text to display
#            fsBoxFileName - a file name that should be selected
#            fsBoxActionOk - the action that should be performed on ok
#            fsBoxActionCancel - the action that should be performed on cancel
# Returns: the filename that was selected, or nothing
# Sideeffects: none
##########
# 
# global fsBox(activeBackground) - active background color
# global fsBox(activeForeground) - active foreground color
# global fsBox(background) - background color
# global fsBox(font) - text font
# global fsBox(foreground) - foreground color
# global fsBox(extensions) - scan directory for extensions
# global fsBox(scrollActiveForeground) - scrollbar active background color
# global fsBox(scrollBackground) - scrollbar background color
# global fsBox(scrollForeground) - scrollbar foreground color
# global fsBox(scrollSide) - side where scrollbar is located

  global fsBox

  set tmpButtonOpt ""
  set tmpFrameOpt ""
  set tmpMessageOpt ""
  set tmpScaleOpt ""
  set tmpScrollOpt ""
  if {"$fsBox(activeBackground)" != ""} {
    append tmpButtonOpt "-activebackground \"$fsBox(activeBackground)\" "
  }
  if {"$fsBox(activeForeground)" != ""} {
    append tmpButtonOpt "-activeforeground \"$fsBox(activeForeground)\" "
  }
  if {"$fsBox(background)" != ""} {
    append tmpButtonOpt "-background \"$fsBox(background)\" "
    append tmpFrameOpt "-background \"$fsBox(background)\" "
    append tmpMessageOpt "-background \"$fsBox(background)\" "
  }
  if {"$fsBox(font)" != ""} {
    append tmpButtonOpt "-font \"$fsBox(font)\" "
    append tmpMessageOpt "-font \"$fsBox(font)\" "
  }
  if {"$fsBox(foreground)" != ""} {
    append tmpButtonOpt "-foreground \"$fsBox(foreground)\" "
    append tmpMessageOpt "-foreground \"$fsBox(foreground)\" "
  }
  if {"$fsBox(scrollActiveForeground)" != ""} {
    append tmpScrollOpt "-activeforeground \"$fsBox(scrollActiveForeground)\" "
  }
  if {"$fsBox(scrollBackground)" != ""} {
    append tmpScrollOpt "-background \"$fsBox(scrollBackground)\" "
  }
  if {"$fsBox(scrollForeground)" != ""} {
    append tmpScrollOpt "-foreground \"$fsBox(scrollForeground)\" "
  }

  if {[file exists [file tail $fsBoxFileName]] &&
      [IsAFile [file tail $fsBoxFileName]]} {
    set fsBox(name) [file tail $fsBoxFileName]
  } {
    set fsBox(name) ""
  }
  if {[file exists $fsBoxFileName] && [IsADir $fsBoxFileName]} {
    set fsBox(path) $fsBoxFileName
  } {
    if {"[file rootname $fsBoxFileName]" != "."} {
      set fsBox(path) [file rootname $fsBoxFileName]
    }
  }
  if {$fsBox(showPixmap)} {
    set fsBox(path) [string trimleft $fsBox(path) @]
  }
  if {"$fsBox(path)" != "" && [file exists $fsBox(path)] &&
      [IsADir $fsBox(path)]} {
    set fsBox(internalPath) $fsBox(path)
  } {
    if {"$fsBox(internalPath)" == "" ||
        ![file exists $fsBox(internalPath)]} {
      set fsBox(internalPath) [pwd]
    }
  }
  # build widget structure

  # start build of toplevel
  if {"[info commands CBSavedDestroy]" != ""} {
    catch {CBSavedDestroy .fsBox}
  } {
    catch {destroy .fsBox}
  }
  toplevel .fsBox     -borderwidth 0
  catch ".fsBox config $tmpFrameOpt"
  wm geometry .fsBox 350x300 
  wm title .fsBox {File select box}
  wm maxsize .fsBox 1000 1000
  wm minsize .fsBox 100 100
  # end build of toplevel

  label .fsBox.message1     -anchor c     -relief raised     -text "$fsBoxMessage"
  catch ".fsBox.message1 config $tmpMessageOpt"

  frame .fsBox.frame1     -borderwidth 0     -relief raised
  catch ".fsBox.frame1 config $tmpFrameOpt"

  button .fsBox.frame1.ok     -text "OK"     -command "
      global fsBox
      set fsBox(name) \[.fsBox.file.file get\]
      if {$fsBox(showPixmap)} {
        set fsBox(path) @\[.fsBox.path.path get\]
      } {
        set fsBox(path) \[.fsBox.path.path get\]
      }
      set fsBox(internalPath) \[.fsBox.path.path get\]
      $fsBoxActionOk
      if {\"\[info commands CBSavedDestroy\]\" != \"\"} {
        catch {CBSavedDestroy .fsBox}
      } {
        catch {destroy .fsBox}
      }"
  catch ".fsBox.frame1.ok config $tmpButtonOpt"

  button .fsBox.frame1.rescan     -text "Rescan"     -command {
      global fsBox
      FSBoxFSShow [.fsBox.path.path get]         [.fsBox.pattern.pattern get] $fsBox(all)}
  catch ".fsBox.frame1.rescan config $tmpButtonOpt"

  button .fsBox.frame1.cancel     -text "Cancel"     -command "
      global fsBox
      set fsBox(name) {}
      set fsBox(path) {}
      $fsBoxActionCancel
      if {\"\[info commands CBSavedDestroy\]\" != \"\"} {
        catch {CBSavedDestroy .fsBox}
      } {
        catch {destroy .fsBox}
      }"
  catch ".fsBox.frame1.cancel config $tmpButtonOpt"

  if {$fsBox(showPixmap)} {
    frame .fsBox.frame2       -borderwidth 0       -relief raised
    catch ".fsBox.frame2 config $tmpFrameOpt"

    scrollbar .fsBox.frame2.scrollbar3       -command {.fsBox.frame2.canvas2 xview}       -orient {horizontal}       -relief {raised}
    catch ".fsBox.frame2.scrollbar3 config $tmpScrollOpt"

    scrollbar .fsBox.frame2.scrollbar1       -command {.fsBox.frame2.canvas2 yview}       -relief {raised}
    catch ".fsBox.frame2.scrollbar1 config $tmpScrollOpt"

    canvas .fsBox.frame2.canvas2       -confine {true}       -relief {raised}       -scrollregion {0c 0c 20c 20c}       -width {100}       -xscrollcommand {.fsBox.frame2.scrollbar3 set}       -yscrollcommand {.fsBox.frame2.scrollbar1 set}
    catch ".fsBox.frame2.canvas2 config $tmpFrameOpt"

    .fsBox.frame2.canvas2 addtag currentBitmap withtag [.fsBox.frame2.canvas2 create bitmap 5 5 -anchor nw]
  }

  frame .fsBox.path     -borderwidth 0     -relief raised
  catch ".fsBox.path config $tmpFrameOpt"

  frame .fsBox.path.paths     -borderwidth 2     -relief raised
  catch ".fsBox.path.paths config $tmpFrameOpt"

  menubutton .fsBox.path.paths.paths     -borderwidth 0     -menu ".fsBox.path.paths.paths.menu"     -relief flat     -text "Pathname:"
  catch ".fsBox.path.paths.paths config $tmpButtonOpt"

  menu .fsBox.path.paths.paths.menu
  catch ".fsBox.path.paths.paths.menu config $tmpButtonOpt"

  .fsBox.path.paths.paths.menu add command      -label "[string trimright $fsBox(internalPath) {/@}]"      -command "
       global fsBox
       FSBoxFSShow \[.fsBox.path.path get\]          \[.fsBox.pattern.pattern get\] \$fsBox(all)
       .fsBox.path.path delete 0 end
       .fsBox.path.path insert 0 [string trimright $fsBox(internalPath) {/@}]"

  entry .fsBox.path.path     -relief raised
  catch ".fsBox.path.path config $tmpMessageOpt"

  if {![IsADir $fsBox(internalPath)]} {
    set $fsBox(internalPath) [pwd]
  }
  .fsBox.path.path insert 0 $fsBox(internalPath)

  frame .fsBox.pattern     -borderwidth 0     -relief raised
  catch ".fsBox.pattern config $tmpFrameOpt"

  frame .fsBox.pattern.patterns     -borderwidth 2     -relief raised
  catch ".fsBox.pattern.patterns config $tmpFrameOpt"

  menubutton .fsBox.pattern.patterns.patterns     -borderwidth 0     -menu ".fsBox.pattern.patterns.patterns.menu"     -relief flat     -text "Selection pattern:"
  catch ".fsBox.pattern.patterns.patterns config $tmpButtonOpt"

  menu .fsBox.pattern.patterns.patterns.menu
  catch ".fsBox.pattern.patterns.patterns.menu config $tmpButtonOpt"

  .fsBox.pattern.patterns.patterns.menu add checkbutton     -label "Scan extensions"     -variable fsBoxExtensions     -command {
      global fsBox
      FSBoxFSShow [.fsBox.path.path get]         [.fsBox.pattern.pattern get] $fsBox(all)}

  entry .fsBox.pattern.pattern     -relief raised
  catch ".fsBox.pattern.pattern config $tmpMessageOpt"

  .fsBox.pattern.pattern insert 0 $fsBox(pattern)
  
  frame .fsBox.files     -borderwidth 0     -relief raised
  catch ".fsBox.files config $tmpFrameOpt"

  scrollbar .fsBox.files.vscroll     -relief raised     -command ".fsBox.files.files yview"
  catch ".fsBox.files.vscroll config $tmpScrollOpt"

  scrollbar .fsBox.files.hscroll     -orient horiz     -relief raised     -command ".fsBox.files.files xview"
  catch ".fsBox.files.hscroll config $tmpScrollOpt"

  listbox .fsBox.files.files     -exportselection false     -relief raised     -xscrollcommand ".fsBox.files.hscroll set"     -yscrollcommand ".fsBox.files.vscroll set"
  catch ".fsBox.files.files config $tmpMessageOpt"

  frame .fsBox.file     -borderwidth 0     -relief raised
  catch ".fsBox.file config $tmpFrameOpt"

  label .fsBox.file.labelfile     -relief raised     -text "Filename:"
  catch ".fsBox.file.labelfile config $tmpMessageOpt"

  entry .fsBox.file.file     -relief raised
  catch ".fsBox.file.file config $tmpMessageOpt"

  .fsBox.file.file delete 0 end
  .fsBox.file.file insert 0 $fsBox(name)
  
  checkbutton .fsBox.pattern.all     -offvalue 0     -onvalue 1     -text "Show all files"     -variable fsBox(all)     -command {
      global fsBox
      FSBoxFSShow [.fsBox.path.path get]         [.fsBox.pattern.pattern get] $fsBox(all)}
  catch ".fsBox.pattern.all config $tmpButtonOpt"

  FSBoxFSShow $fsBox(internalPath) $fsBox(pattern) $fsBox(all)

  # bindings
  bind .fsBox.files.files <Double-Button-1> "
    FSBoxFSFileSelectDouble %W $fsBox(showPixmap) \{$fsBoxActionOk\} %y"
  bind .fsBox.files.files <ButtonPress-1> "
    FSBoxFSFileSelect %W $fsBox(showPixmap) %y"
  bind .fsBox.files.files <Button1-Motion> "
    FSBoxFSFileSelect %W $fsBox(showPixmap) %y"
  bind .fsBox.files.files <Shift-Button1-Motion> "
    FSBoxFSFileSelect %W $fsBox(showPixmap) %y"
  bind .fsBox.files.files <Shift-ButtonPress-1> "
    FSBoxFSFileSelect %W $fsBox(showPixmap) %y"

  bind .fsBox.path.path <Tab> {
    FSBoxFSNameComplete path}
  bind .fsBox.path.path <Return> {
    global fsBox
    FSBoxFSShow [.fsBox.path.path get]       [.fsBox.pattern.pattern get] $fsBox(all)
    FSBoxFSInsertPath
    .fsBox.file.file icursor end
    focus .fsBox.file.file}
  catch "bind .fsBox.path.path <Up> {}"
  bind .fsBox.path.path <Down> {
    .fsBox.file.file icursor end
    focus .fsBox.file.file}

  bind .fsBox.file.file <Tab> {
    FSBoxFSNameComplete file}
  bind .fsBox.file.file <Return> "
    global fsBox
    set fsBox(name) \[.fsBox.file.file get\]
    if {$fsBox(showPixmap)} {
      set fsBox(path) @\[.fsBox.path.path get\]
    } {
      set fsBox(path) \[.fsBox.path.path get\]
    }
    set fsBox(internalPath) \[.fsBox.path.path get\]
    $fsBoxActionOk
    if {\"\[info commands CBSavedDestroy\]\" != \"\"} {
      catch {CBSavedDestroy .fsBox}
    } {
      catch {destroy .fsBox}
    }"
  bind .fsBox.file.file <Up> {
    .fsBox.path.path icursor end
    focus .fsBox.path.path}
  bind .fsBox.file.file <Down> {
    .fsBox.pattern.pattern icursor end
    focus .fsBox.pattern.pattern}

  bind .fsBox.pattern.pattern <Return> {
    global fsBox
    FSBoxFSShow [.fsBox.path.path get]       [.fsBox.pattern.pattern get] $fsBox(all)}
  bind .fsBox.pattern.pattern <Up> {
    .fsBox.file.file icursor end
    focus .fsBox.file.file}
  catch "bind .fsBox.pattern.pattern <Down> {}"

  # packing
  pack .fsBox.files.vscroll -side $fsBox(scrollSide) -fill y
  pack .fsBox.files.hscroll -side bottom -fill x
  pack .fsBox.files.files -side left -fill both -expand 1
  pack .fsBox.file.labelfile -side left
  pack .fsBox.file.file -side left -fill both -expand 1
  pack .fsBox.frame1.ok -side left -fill both -expand 1
  pack .fsBox.frame1.rescan -side left -fill both -expand 1
  pack .fsBox.frame1.cancel -side left -fill both -expand 1
  pack .fsBox.path.paths.paths -side left
  pack .fsBox.pattern.patterns.patterns -side left
  pack .fsBox.path.paths -side left
  pack .fsBox.path.path -side left -fill both -expand 1
  pack .fsBox.pattern.patterns -side left
  pack .fsBox.pattern.all -side right -fill both
  pack .fsBox.pattern.pattern -side left -fill both -expand 1
  if {$fsBox(showPixmap)} {
    pack .fsBox.frame2.scrollbar1 -side left -fill y
    pack .fsBox.frame2.canvas2 -side top -expand 1 -fill both
    pack .fsBox.frame2.scrollbar3 -side top -fill x 
    pack .fsBox.message1 -side top -fill both
    pack .fsBox.frame1 -side bottom -fill both
    pack .fsBox.pattern -side bottom -fill both
    pack .fsBox.file -side bottom -fill both
    pack .fsBox.path -side bottom -fill both
    pack .fsBox.frame2 -side right -fill both
    pack .fsBox.files -side left -fill both -expand 1
  } {
    pack .fsBox.message1 -side top -fill both
    pack .fsBox.frame1 -side bottom -fill both
    pack .fsBox.pattern -side bottom -fill both
    pack .fsBox.file -side bottom -fill both
    pack .fsBox.path -side bottom -fill both
    pack .fsBox.files -side left -fill both -expand 1
  }

  if {"$fsBoxActionOk" == "" && "$fsBoxActionCancel" == ""} {
    # wait for the box to be destroyed
    update idletask
    grab .fsBox
    tkwait window .fsBox

    if {"[string trim $fsBox(path)]" != "" ||
        "[string trim $fsBox(name)]" != ""} {
      if {"[string trimleft [string trim $fsBox(name)] /]" == ""} {
        return [string trimright [string trim $fsBox(path)] /]
      } {
        return [string trimright [string trim $fsBox(path)] /]/[string trimleft [string trim $fsBox(name)] /]
      }
    }
  }
}


# Procedure: FSBoxBindSelectOne
proc FSBoxBindSelectOne { fsBoxW fsBoxY} {

  set fsBoxNearest [$fsBoxW nearest $fsBoxY]
  if {$fsBoxNearest >= 0} {
    $fsBoxW select clear 0 end
    $fsBoxW select set $fsBoxNearest
  }
}


# Procedure: FSBoxFSFileSelect
proc FSBoxFSFileSelect { fsBoxW fsBoxShowPixmap fsBoxY} {
  global fsBox

  FSBoxBindSelectOne $fsBoxW $fsBoxY
  set fsBoxNearest [$fsBoxW nearest $fsBoxY]
  if {$fsBoxNearest >= 0} {
    set fsBoxTmpEntry [$fsBoxW get $fsBoxNearest]
    if {"[string index $fsBoxTmpEntry           [expr [string length $fsBoxTmpEntry]-1]]" == "/" ||
        "[string index $fsBoxTmpEntry           [expr [string length $fsBoxTmpEntry]-1]]" == "@"} {
      set fsBoxFileName [string range $fsBoxTmpEntry 0             [expr [string length $fsBoxTmpEntry]-2]]
      if {![IsADir [string trimright $fsBox(internalPath)/$fsBoxFileName @]] &&
          ![IsASymlink [string trimright $fsBox(internalPath)/$fsBoxFileName @]]} {
        set fsBoxFileName $fsBoxTmpEntry
      }
    } {
      if {"[string index $fsBoxTmpEntry             [expr [string length $fsBoxTmpEntry]-1]]" == "*"} {
        set fsBoxFileName [string range $fsBoxTmpEntry 0           [expr [string length $fsBoxTmpEntry]-2]]
        if {![file executable $fsBox(internalPath)/$fsBoxFileName]} {
          set fsBoxFileName $fsBoxTmpEntry
        }
      } {
        set fsBoxFileName $fsBoxTmpEntry
      }
    }
    if {![IsADir [string trimright $fsBox(internalPath)/$fsBoxFileName @]]} {
      set fsBox(name) $fsBoxFileName
      .fsBox.file.file delete 0 end
      .fsBox.file.file insert 0 $fsBox(name)
      if {$fsBoxShowPixmap} {
        catch ".fsBox.frame2.canvas2 itemconfigure currentBitmap -bitmap \"@$fsBox(internalPath)/$fsBox(name)\""
      }
    }
  }
}


# Procedure: FSBoxFSFileSelectDouble
proc FSBoxFSFileSelectDouble { fsBoxW fsBoxShowPixmap fsBoxAction fsBoxY} {
  global fsBox

  FSBoxBindSelectOne $fsBoxW $fsBoxY
  set fsBoxNearest [$fsBoxW nearest $fsBoxY]
  if {$fsBoxNearest >= 0} {
    set fsBoxTmpEntry [$fsBoxW get $fsBoxNearest]
    if {"$fsBoxTmpEntry" == "../"} {
      set fsBoxTmpEntry [string trimright [string trim $fsBox(internalPath)] "@/"]
      if {"$fsBoxTmpEntry" == ""} {
        return
      }
      FSBoxFSShow [file dirname $fsBoxTmpEntry]         [.fsBox.pattern.pattern get] $fsBox(all)
      .fsBox.path.path delete 0 end
      .fsBox.path.path insert 0 $fsBox(internalPath)
    } {
      if {"[string index $fsBoxTmpEntry             [expr [string length $fsBoxTmpEntry]-1]]" == "/" ||
          "[string index $fsBoxTmpEntry             [expr [string length $fsBoxTmpEntry]-1]]" == "@"} {
        set fsBoxFileName [string range $fsBoxTmpEntry 0               [expr [string length $fsBoxTmpEntry]-2]]
        if {![IsADir [string trimright $fsBox(internalPath)/$fsBoxFileName @]] &&
            ![IsASymlink [string trimright $fsBox(internalPath)/$fsBoxFileName @]]} {
          set fsBoxFileName $fsBoxTmpEntry
        }
      } {
        if {"[string index $fsBoxTmpEntry               [expr [string length $fsBoxTmpEntry]-1]]" == "*"} {
          set fsBoxFileName [string range $fsBoxTmpEntry 0                 [expr [string length $fsBoxTmpEntry]-2]]
          if {![file executable $fsBox(internalPath)/$fsBoxFileName]} {
            set fsBoxFileName $fsBoxTmpEntry
          }
        } {
          set fsBoxFileName $fsBoxTmpEntry
        }
      }
      if {[IsADir [string trimright $fsBox(internalPath)/$fsBoxFileName @]]} {
        set fsBox(internalPath) "[string trimright $fsBox(internalPath) {/@}]/$fsBoxFileName"
        FSBoxFSShow $fsBox(internalPath)           [.fsBox.pattern.pattern get] $fsBox(all)
        .fsBox.path.path delete 0 end
        .fsBox.path.path insert 0 $fsBox(internalPath)
      } {
        set fsBox(name) $fsBoxFileName
        if {$fsBoxShowPixmap} {
          set fsBox(path) @$fsBox(internalPath)
        } {
          set fsBox(path) $fsBox(internalPath)
        }
        if {"$fsBoxAction" != ""} {
          eval "global fsBox; $fsBoxAction"
        }
        if {"[info commands CBSavedDestroy]" != ""} {
          catch {CBSavedDestroy .fsBox}
        } {
          catch {destroy .fsBox}
        }
      }
    }
  }
}


# Procedure: FSBoxFSInsertPath
proc FSBoxFSInsertPath {} {
  global fsBox

  set fsBoxLast [.fsBox.path.paths.paths.menu index last]
  set fsBoxNewEntry [string trimright [.fsBox.path.path get] "/@"]
  for {set fsBoxCounter 0} {$fsBoxCounter <= $fsBoxLast} {incr fsBoxCounter 1} {
    if {"$fsBoxNewEntry" ==           "[lindex [.fsBox.path.paths.paths.menu entryconfigure                     $fsBoxCounter -label] 4]"} {
      return
    }
  }
  if {$fsBoxLast < 9} {
    .fsBox.path.paths.paths.menu add command       -label "$fsBoxNewEntry"       -command "
        global fsBox
        FSBoxFSShow $fsBoxNewEntry           \[.fsBox.pattern.pattern get\] \$fsBox(all)
        .fsBox.path.path delete 0 end
        .fsBox.path.path insert 0 $fsBoxNewEntry"
  } {
    for {set fsBoxCounter 0} {$fsBoxCounter < $fsBoxLast} {incr fsBoxCounter 1} {
      .fsBox.path.paths.paths.menu entryconfigure         $fsBoxCounter -label           [lindex [.fsBox.path.paths.paths.menu entryconfigure             [expr $fsBoxCounter+1] -label] 4]
      .fsBox.path.paths.paths.menu entryconfigure $fsBoxCounter         -command "
          global fsBox
          FSBoxFSShow [lindex [.fsBox.path.paths.paths.menu entryconfigure             [expr $fsBoxCounter+1] -label] 4]             \[.fsBox.pattern.pattern get\] \$fsBox(all)
          .fsBox.path.path delete 0 end
          .fsBox.path.path insert 0 [lindex             [.fsBox.path.paths.paths.menu entryconfigure               [expr $fsBoxCounter+1] -label] 4]"
    }
    .fsBox.path.paths.paths.menu entryconfigure $fsBoxLast       -label "$fsBoxNewEntry"
    .fsBox.path.paths.paths.menu entryconfigure $fsBoxCounter       -command "
        global fsBox
        FSBoxFSShow \[.fsBox.path.path get\]           \[.fsBox.pattern.pattern get\] \$fsBox(all)
        .fsBox.path.path delete 0 end
        .fsBox.path.path insert 0 $fsBoxNewEntry"
  }
}


# Procedure: FSBoxFSNameComplete
proc FSBoxFSNameComplete { fsBoxType} {
  global fsBox

  set fsBoxNewFile ""
  if {"$fsBoxType" == "path"} {
    set fsBoxDirName [file dirname [.fsBox.path.path get]]
    set fsBoxFileName [file tail [.fsBox.path.path get]]
  } {
    set fsBoxDirName [file dirname [.fsBox.path.path get]/]
    set fsBoxFileName [file tail [.fsBox.file.file get]]
  }

  set fsBoxNewFile ""
  if {[IsADir [string trimright $fsBoxDirName @]]} {
    catch "glob -nocomplain $fsBoxDirName/${fsBoxFileName}*" fsBoxResult
    foreach fsBoxCounter $fsBoxResult {
      if {"$fsBoxNewFile" == ""} {
        set fsBoxNewFile [file tail $fsBoxCounter]
      } {
        if {"[string index [file tail $fsBoxCounter] 0]" !=
            "[string index $fsBoxNewFile 0]"} {
          set fsBoxNewFile ""
          break
        }
        set fsBoxCounter1 0
        set fsBoxTmpFile1 $fsBoxNewFile
        set fsBoxTmpFile2 [file tail $fsBoxCounter]
        set fsBoxLength1 [string length $fsBoxTmpFile1]
        set fsBoxLength2 [string length $fsBoxTmpFile2]
        set fsBoxNewFile ""
        if {$fsBoxLength1 > $fsBoxLength2} {
          set fsBoxLength1 $fsBoxLength2
        }
        while {$fsBoxCounter1 < $fsBoxLength1} {
          if {"[string index $fsBoxTmpFile1 $fsBoxCounter1]" ==                 "[string index $fsBoxTmpFile2 $fsBoxCounter1]"} {
            append fsBoxNewFile [string index $fsBoxTmpFile1 $fsBoxCounter1]
          } {
            break
          }
          incr fsBoxCounter1 1
        }
      }
    }
  }
  if {"$fsBoxNewFile" != ""} {
    if {[IsADir [string trimright $fsBoxDirName/$fsBoxNewFile @]] ||
        ![IsAFile [string trimright $fsBoxDirName/$fsBoxNewFile @]]} {
      if {[IsADir [string trimright $fsBoxDirName/$fsBoxNewFile @]]} {
        if {"$fsBoxDirName" == "/"} {
          .fsBox.path.path delete 0 end
          .fsBox.path.path insert 0 "/[string trimright [string trim $fsBoxNewFile /] @]/"
        } {
          .fsBox.path.path delete 0 end
          .fsBox.path.path insert 0 "[string trimright $fsBoxDirName /]/[string trimright [string trim $fsBoxNewFile /] @]/"
        }
        FSBoxFSShow [.fsBox.path.path get]           [.fsBox.pattern.pattern get] $fsBox(all)
        FSBoxFSInsertPath
      } {
        .fsBox.path.path delete 0 end
        .fsBox.path.path insert 0 "[string trimright $fsBoxDirName /]/[string trimright [string trim $fsBoxNewFile /] @]"
      }
    } {
      .fsBox.path.path delete 0 end
      .fsBox.path.path insert 0 "[string trimright $fsBoxDirName {@/}]/"
      .fsBox.file.file delete 0 end
      .fsBox.file.file insert 0 $fsBoxNewFile
      .fsBox.file.file icursor end
      focus .fsBox.file.file
    }
  }
}


# Procedure: FSBoxFSShow
proc FSBoxFSShow { fsBoxPath fsBoxPattern fsBoxAll} {
  global fsBox

  set tmpButtonOpt ""
  if {"$fsBox(activeBackground)" != ""} {
    append tmpButtonOpt "-activebackground \"$fsBox(activeBackground)\" "
  }
  if {"$fsBox(activeForeground)" != ""} {
    append tmpButtonOpt "-activeforeground \"$fsBox(activeForeground)\" "
  }
  if {"$fsBox(background)" != ""} {
    append tmpButtonOpt "-background \"$fsBox(background)\" "
  }
  if {"$fsBox(font)" != ""} {
    append tmpButtonOpt "-font \"$fsBox(font)\" "
  }
  if {"$fsBox(foreground)" != ""} {
    append tmpButtonOpt "-foreground \"$fsBox(foreground)\" "
  }

  set fsBox(pattern) $fsBoxPattern
  if {[file exists $fsBoxPath] && [file readable $fsBoxPath] &&
      [IsADir $fsBoxPath]} {
    set fsBox(internalPath) $fsBoxPath
  } {
    if {[file exists $fsBoxPath] && [file readable $fsBoxPath] &&
        [IsAFile $fsBoxPath]} {
      set fsBox(internalPath) [file dirname $fsBoxPath]
      .fsBox.file.file delete 0 end
      .fsBox.file.file insert 0 [file tail $fsBoxPath]
      set fsBoxPath $fsBox(internalPath)
    } {
      while {"$fsBoxPath" != "" && "$fsBoxPath" != "/" &&
             ![file isdirectory $fsBoxPath]} {
        set fsBox(internalPath) [file dirname $fsBoxPath]
         set fsBoxPath $fsBox(internalPath)
      }
    }
  }
  if {"$fsBoxPath" == ""} {
    set fsBoxPath "/"
    set fsBox(internalPath) "/"
  }
  .fsBox.path.path delete 0 end
  .fsBox.path.path insert 0 $fsBox(internalPath)

  if {[.fsBox.files.files size] > 0} {
    .fsBox.files.files delete 0 end
  }
  if {$fsBoxAll} {
    if {[catch "exec ls -F -a $fsBoxPath" fsBoxResult]} {
      puts stderr "$fsBoxResult"
    }
  } {
    if {[catch "exec ls -F $fsBoxPath" fsBoxResult]} {
      puts stderr "$fsBoxResult"
    }
  }
  set fsBoxElementList [lsort $fsBoxResult]

  foreach fsBoxCounter [winfo children .fsBox.pattern.patterns.patterns] {
    if {[string length [info commands CBSavedDestroy]] > 0} {
      catch {CBSavedDestroy $fsBoxCounter}
    } {
      catch {destroy $fsBoxCounter}
    }
  }
  menu .fsBox.pattern.patterns.patterns.menu
  catch ".fsBox.pattern.patterns.patterns.menu config $tmpButtonOpt"

  if {$fsBox(extensions)} {
    .fsBox.pattern.patterns.patterns.menu add command       -label "*"       -command {
        global fsBox
        set fsBox(pattern) "*"
        .fsBox.pattern.pattern delete 0 end
        .fsBox.pattern.pattern insert 0 $fsBox(pattern)
        FSBoxFSShow [.fsBox.path.path get] $fsBox(pattern)           $fsBox(all)}
  }

  if {"$fsBoxPath" != "/"} {
    .fsBox.files.files insert end "../"
  }
  foreach fsBoxCounter $fsBoxElementList {
    if {[string match $fsBoxPattern $fsBoxCounter] ||
        [IsADir [string trimright $fsBoxPath/$fsBoxCounter "/@"]]} {
      if {"$fsBoxCounter" != "../" &&
          "$fsBoxCounter" != "./"} {
        .fsBox.files.files insert end $fsBoxCounter
      }
    }

    if {$fsBox(extensions)} {
      catch "file rootname $fsBoxCounter" fsBoxRootName
      catch "file extension $fsBoxCounter" fsBoxExtension
      set fsBoxExtension [string trimright $fsBoxExtension "/*@"]
      if {"$fsBoxExtension" != "" && "$fsBoxRootName" != ""} {
        set fsBoxInsert 1
        set fsBoxLast [.fsBox.pattern.patterns.patterns.menu index last]
        for {set fsBoxCounter1 0} {$fsBoxCounter1 <= $fsBoxLast} {incr fsBoxCounter1 1} {
          if {"*$fsBoxExtension" ==                 "[lindex [.fsBox.pattern.patterns.patterns.menu entryconfigure                         $fsBoxCounter1 -label] 4]"} {
            set fsBoxInsert 0
          }
        }
	if {$fsBoxInsert} {
          .fsBox.pattern.patterns.patterns.menu add command             -label "*$fsBoxExtension"             -command "
              global fsBox
              set fsBox(pattern) \"*$fsBoxExtension\"
              .fsBox.pattern.pattern delete 0 end
              .fsBox.pattern.pattern insert 0 \$fsBox(pattern)
              FSBoxFSShow \[.fsBox.path.path get\] \$fsBox(pattern)                 \$fsBox(all)"
        }
      }
    }
  }
  if {$fsBox(extensions)} {
    .fsBox.pattern.patterns.patterns.menu add separator
  }
  if {$fsBox(extensions) || 
      "[.fsBox.pattern.patterns.patterns.menu index last]" == "none"} {
    .fsBox.pattern.patterns.patterns.menu add checkbutton       -label "Scan extensions"       -variable "fsBox(extensions)"       -command {
        global fsBox
        FSBoxFSShow [.fsBox.path.path get]           [.fsBox.pattern.pattern get] $fsBox(all)}
  }
}


# Procedure: IsADir
proc IsADir { pathName} {
##########
# Procedure: IsADir
# Description: check if name is a directory (including symbolic links)
# Arguments: pathName - the path to check
# Returns: 1 if its a directory, otherwise 0
# Sideeffects: none
##########

  if {[file isdirectory $pathName]} {
    return 1
  } {
    catch "file type $pathName" fileType
    if {"$fileType" == "link"} {
      if {[catch "file readlink $pathName" linkName]} {
        return 0
      }
      catch "file type $linkName" fileType
      while {"$fileType" == "link"} {
        if {[catch "file readlink $linkName" linkName]} {
          return 0
        }
        catch "file type $linkName" fileType
      }
      return [file isdirectory $linkName]
    }
  }
  return 0
}


# Procedure: IsAFile
proc IsAFile { fileName} {
##########
# Procedure: IsAFile
# Description: check if filename is a file (including symbolic links)
# Arguments: fileName - the filename to check
# Returns: 1 if its a file, otherwise 0
# Sideeffects: none
##########

  if {[file isfile $fileName]} {
    return 1
  } {
    catch "file type $fileName" fileType
    if {"$fileType" == "link"} {
      if {[catch "file readlink $fileName" linkName]} {
        return 0
      }
      catch "file type $linkName" fileType
      while {"$fileType" == "link"} {
        if {[catch "file readlink $linkName" linkName]} {
          return 0
        }
        catch "file type $linkName" fileType
      }
      return [file isfile $linkName]
    }
  }
  return 0
}


# Procedure: IsASymlink
proc IsASymlink { fileName} {
##########
# Procedure: IsASymlink
# Description: check if filename is a symbolic link
# Arguments: fileName - the path/filename to check
# Returns: none
# Sideeffects: none
##########

  catch "file type $fileName" fileType
  if {"$fileType" == "link"} {
    return 1
  }
  return 0
}


# Procedure: Quit
proc Quit {} {
exit 0
}


# Procedure: SaveKey
proc SaveKey {} {
set fd [open Keyfile w]
puts $fd "[[SN Key] get]\n"
close $fd
exec chmod 600 Keyfile
}


# Internal procedures


# Procedure: Alias
if {"[info procs Alias]" == ""} {
proc Alias { args} {
##########
# Procedure: Alias
# Description: establish an alias for a procedure
# Arguments: args - no argument means that a list of all aliases
#                   is returned. Otherwise the first parameter is
#                   the alias name, and the second parameter is
#                   the procedure that is aliased.
# Returns: nothing, the command that is bound to the alias or a
#          list of all aliases - command pairs. 
# Sideeffects: internalAliasList is updated, and the alias
#              proc is inserted
##########
  global internalAliasList

  if {[llength $args] == 0} {
    return $internalAliasList
  } {
    if {[llength $args] == 1} {
      set cbTmpIndex [lsearch $internalAliasList "[lindex $args 0] *"]
      if {$cbTmpIndex != -1} {
        return [lindex [lindex $internalAliasList $cbTmpIndex] 1]
      }
    } {
      if {[llength $args] == 2} {
        eval "proc [lindex $args 0] {args} {
return \[eval \"[lindex $args 1] \$args\"\]}"
        set cbTmpIndex [lsearch $internalAliasList "[lindex $args 0] *"]
        if {$cbTmpIndex != -1} {
          set internalAliasList [lreplace $internalAliasList $cbTmpIndex $cbTmpIndex "[lindex $args 0] [lindex $args 1]"]
        } {
          lappend internalAliasList "[lindex $args 0] [lindex $args 1]"
        }
      } {
        error "Alias: wrong number or args: $args"
      }
    }
  }
}
}


# Procedure: GetSelection
if {"[info procs GetSelection]" == ""} {
proc GetSelection {} {
##########
# Procedure: GetSelection
# Description: get current selection
# Arguments: none
# Returns: none
# Sideeffects: none
##########

  # the save way
  set cbSelection ""
  catch "selection get" cbSelection
  if {"$cbSelection" == "selection doesn't exist or form \"STRING\" not defined"} {
    return ""
  } {
    return $cbSelection
  }
}
}


# Procedure: MenuPopupAdd
if {"[info procs MenuPopupAdd]" == ""} {
proc MenuPopupAdd { cbW cbButton cbMenu {cbModifier ""} {cbCanvasTag ""}} {
# the popup menu handling is from (I already gave up with popup handling :-):
#
# Copyright 1991,1992 by James Noble.
# Everyone is granted permission to copy, modify and redistribute.
# This notice must be preserved on all copies or derivates.
#
##########
# Procedure: MenuPopupAdd
# Description: attach a popup menu to widget
# Arguments: cbW - the widget
#            cbButton - the button we use
#            cbMenu - the menu to attach
#            {cbModifier} - a optional modifier
#            {cbCanvasTag} - a canvas tagOrId
# Returns: none
# Sideeffects: none
##########
  global tk_popupPriv

  set tk_popupPriv($cbMenu,focus) ""
  set tk_popupPriv($cbMenu,grab) ""
  if {"$cbModifier" != ""} {
    set press "$cbModifier-"
    set motion "$cbModifier-"
    set release "Any-"
  } {
    set press ""
    set motion ""
    set release ""
  }

  bind $cbMenu "<${motion}B${cbButton}-Motion>"     "MenuPopupMotion $cbMenu %W %X %Y"
  bind $cbMenu "<${release}ButtonRelease-${cbButton}>"     "MenuPopupRelease $cbMenu %W"
  if {"$cbCanvasTag" == ""} {
    bind $cbW "<${press}ButtonPress-${cbButton}>"       "MenuPopupPost $cbMenu %X %Y"
    bind $cbW "<${release}ButtonRelease-${cbButton}>"       "MenuPopupRelease $cbMenu %W"
  } {
    $cbW bind $cbCanvasTag "<${press}ButtonPress-${cbButton}>"       "MenuPopupPost $cbMenu %X %Y"
    $cbW bind $cbCanvasTag "<${release}ButtonRelease-${cbButton}>"       "MenuPopupRelease $cbMenu %W"
  }
}
}


# Procedure: MenuPopupMotion
if {"[info procs MenuPopupMotion]" == ""} {
proc MenuPopupMotion { cbMenu cbW cbX cbY} {
##########
# Procedure: MenuPopupMotion
# Description: handle the popup menu motion
# Arguments: cbMenu - the topmost menu
#            cbW - the menu
#            cbX - the root x coordinate
#            cbY - the root x coordinate
# Returns: none
# Sideeffects: none
##########
  global tk_popupPriv

  if {"[info commands $cbW]" != "" && [winfo ismapped $cbW] &&
      "[winfo class $cbW]" == "Menu" &&
      [info exists tk_popupPriv($cbMenu,focus)] &&
      "$tk_popupPriv($cbMenu,focus)" != "" &&
      [info exists tk_popupPriv($cbMenu,grab)] &&
      "$tk_popupPriv($cbMenu,grab)" != ""} {
    set cbPopMinX [winfo rootx $cbW]
    set cbPopMaxX [expr $cbPopMinX+[winfo width $cbW]]
    if {$cbX >= $cbPopMinX && $cbX <= $cbPopMaxX} {
      $cbW activate @[expr $cbY-[winfo rooty $cbW]]
      if {![catch "$cbW entryconfig @[expr $cbY-[winfo rooty $cbW]] -menu" result]} {
        if {"[lindex $result 4]" != ""} {
          foreach binding [bind $cbMenu] {
            bind [lindex $result 4] $binding [bind $cbMenu $binding]
          }
        }
      }
    } {
      $cbW activate none
    }
  }
}
}


# Procedure: MenuPopupPost
if {"[info procs MenuPopupPost]" == ""} {
proc MenuPopupPost { cbMenu cbX cbY} {
##########
# Procedure: MenuPopupPost
# Description: post the popup menu
# Arguments: cbMenu - the menu
#            cbX - the root x coordinate
#            cbY - the root x coordinate
# Returns: none
# Sideeffects: none
##########
  global tk_popupPriv

  if {"[info commands $cbMenu]" != ""} {
    if {![info exists tk_popupPriv($cbMenu,focus)]} {
      set tk_popupPriv($cbMenu,focus) [focus]
    } {
      if {"$tk_popupPriv($cbMenu,focus)" == ""} {
        set tk_popupPriv($cbMenu,focus) [focus]
      }
    }
    set tk_popupPriv($cbMenu,grab) $cbMenu

    catch "$cbMenu activate none"
    catch "$cbMenu post $cbX $cbY"
    catch "focus $cbMenu"
    catch "grab -global $cbMenu"
  }
}
}


# Procedure: MenuPopupRelease
if {"[info procs MenuPopupRelease]" == ""} {
proc MenuPopupRelease { cbMenu cbW} {
##########
# Procedure: MenuPopupRelease
# Description: remove the popup menu
# Arguments: cbMenu - the topmost menu widget
#            cbW - the menu widget
# Returns: none
# Sideeffects: none
##########
  global tk_popupPriv

  if {"[info commands $cbW]" != "" && [winfo ismapped $cbW] &&
      "[winfo class $cbW]" == "Menu" &&
      [info exists tk_popupPriv($cbMenu,focus)] &&
      "$tk_popupPriv($cbMenu,focus)" != "" &&
      [info exists tk_popupPriv($cbMenu,grab)] &&
      "$tk_popupPriv($cbMenu,grab)" != ""} {
    catch "grab release $tk_popupPriv($cbMenu,grab)"
    catch "focus $tk_popupPriv($cbMenu,focus)"
    set tk_popupPriv($cbMenu,focus) ""
    set tk_popupPriv($cbMenu,grab) ""
    if {"[$cbW index active]" != "none"} {
      $cbW invoke active; catch "$cbMenu unpost"
    }
  }
  catch "$cbMenu unpost"
}
}


# Procedure: NoFunction
if {"[info procs NoFunction]" == ""} {
proc NoFunction { args} {
##########
# Procedure: NoFunction
# Description: do nothing (especially with scales and scrollbars)
# Arguments: args - a number of ignored parameters
# Returns: none
# Sideeffects: none
##########
}
}


# Procedure: SN
if {"[info procs SN]" == ""} {
proc SN { {cbName ""}} {
##########
# Procedure: SN
# Description: map a symbolic name to the widget path
# Arguments: cbName
# Returns: the symbolic name
# Sideeffects: none
##########

  SymbolicName $cbName
}
}


# Procedure: SymbolicName
if {"[info procs SymbolicName]" == ""} {
proc SymbolicName { {cbName ""}} {
##########
# Procedure: SymbolicName
# Description: map a symbolic name to the widget path
# Arguments: cbName
# Returns: the symbolic name
# Sideeffects: none
##########

  global symbolicName

  if {"$cbName" != ""} {
    set cbArrayName ""
    append cbArrayName symbolicName ( $cbName )
    if {![catch "set \"$cbArrayName\"" cbValue]} {
      return $cbValue
    } {
      if {"[info commands CBTB_error]" != ""} {
        CBTB_error "Unknown symbolic name:\n$cbName"
      } {
        puts stderr "CB error: unknown symbolic name:\n$cbName"
      }
    }
  }
  return ""
}
}


# Procedure: Unalias
if {"[info procs Unalias]" == ""} {
proc Unalias { aliasName} {
##########
# Procedure: Unalias
# Description: remove an alias for a procedure
# Arguments: aliasName - the alias name to remove
# Returns: none
# Sideeffects: internalAliasList is updated, and the alias
#              proc is removed
##########
  global internalAliasList

  set cbIndex [lsearch $internalAliasList "$aliasName *"]
  if {$cbIndex != -1} {
    rename $aliasName ""
    set internalAliasList [lreplace $internalAliasList $cbIndex $cbIndex]
  }
}
}



# application parsing procedure
proc CBLocalParseAppDefs {cbAppDefFile} {
  global cbAppDefaults

  # basically from: Michael Moore
  if {[file exists $cbAppDefFile] &&
      [file readable $cbAppDefFile] &&
      "[file type $cbAppDefFile]" == "link"} {
    catch "file type $cbAppDefFile" cbType
    while {"$cbType" == "link"} {
      if {[catch "file readlink $cbAppDefFile" cbAppDefFile]} {
        return
      }
      catch "file type $cbAppDefFile" cbType
    }
  }
  if {!("$cbAppDefFile" != "" &&
        [file exists $cbAppDefFile] &&
        [file readable $cbAppDefFile] &&
        "[file type $cbAppDefFile]" == "file")} {
    return
  }
  if {![catch "open $cbAppDefFile r" cbResult]} {
    set cbAppFileContents [read $cbResult]
    close $cbResult
    foreach line [split $cbAppFileContents "\n"] {
      # backup indicates how far to backup.  It applies to the
      # situation where a resource name ends in . and when it
      # ends in *.  In the second case you want to keep the *
      # in the widget name for pattern matching, but you want
      # to get rid of the . if it is the end of the name. 
      set backup -2  
      set line [string trim $line]
      if {[string index $line 0] == "#" || "$line" == ""} {
        # skip comments and empty lines
        continue
      }
      set list [split $line ":"]
      set resource [string trim [lindex $list 0]]
      set i [string last "." $resource]
      set j [string last "*" $resource]
      if {$j > $i} { 
        set i $j
        set backup -1
      }
      incr i
      set name [string range $resource $i end]
      incr i $backup
      set widname [string range $resource 0 $i]
      set value [string trim [lindex $list 1]]
      if {"$widname" != "" && "$widname" != "*"} {
        # insert the widget and resourcename to the application
        # defaults list.
        if {![info exists cbAppDefaults]} {
          set cbAppDefaults ""
        }
        lappend cbAppDefaults [list $widname [string tolower $name] $value]
      }
    }
  }
}

# application loading procedure
proc CBLocalLoadAppDefs {{cbClasses ""} {cbPriority "startupFile"} {cbAppDefFile ""}} {
  global env

  if {"$cbAppDefFile" == ""} {
    set cbFileList ""
    if {[info exists env(XUSERFILESEARCHPATH)]} {
      append cbFileList [split $env(XUSERFILESEARCHPATH) :]
    }
    if {[info exists env(XAPPLRESDIR)]} {
      append cbFileList [split $env(XAPPLRESDIR) :]
    }
    if {[info exists env(XFILESEARCHPATH)]} {
      append cbFileList [split $env(XFILESEARCHPATH) :]
    }
    append cbFileList " /usr/lib/X11/app-defaults"
    append cbFileList " /usr/X11/lib/X11/app-defaults"

    foreach cbCounter1 $cbClasses {
      foreach cbCounter2 $cbFileList {
        set cbPathName $cbCounter2
        if {[regsub -all "%N" "$cbPathName" "$cbCounter1" cbResult]} {
          set cbPathName $cbResult
        }
        if {[regsub -all "%T" "$cbPathName" "app-defaults" cbResult]} {
          set cbPathName $cbResult
        }
        if {[regsub -all "%S" "$cbPathName" "" cbResult]} {
          set cbPathName $cbResult
        }
        if {[regsub -all "%C" "$cbPathName" "" cbResult]} {
          set cbPathName $cbResult
        }
        if {[file exists $cbPathName] &&
            [file readable $cbPathName] &&
            ("[file type $cbPathName]" == "file" ||
             "[file type $cbPathName]" == "link")} {
          catch "option readfile $cbPathName $cbPriority"
          if {"[info commands CBParseAppDefs]" != ""} {
            CBParseAppDefs $cbPathName
          } {
            if {"[info commands CBLocalParseAppDefs]" != ""} {
              CBLocalParseAppDefs $cbPathName
            }
          }
        } {
          if {[file exists $cbCounter2/$cbCounter1] &&
              [file readable $cbCounter2/$cbCounter1] &&
              ("[file type $cbCounter2/$cbCounter1]" == "file" ||
               "[file type $cbCounter2/$cbCounter1]" == "link")} {
            catch "option readfile $cbCounter2/$cbCounter1 $cbPriority"
            if {"[info commands CBParseAppDefs]" != ""} {
              CBParseAppDefs $cbCounter2/$cbCounter1
            } {
              if {"[info commands CBLocalParseAppDefs]" != ""} {
                CBLocalParseAppDefs $cbCounter2/$cbCounter1
              }
            }
          }
        }
      }
    }
  } {
    # load a specific application defaults file
    if {[file exists $cbAppDefFile] &&
        [file readable $cbAppDefFile] &&
        ("[file type $cbAppDefFile]" == "file" ||
         "[file type $cbAppDefFile]" == "link")} {
      catch "option readfile $cbAppDefFile $cbPriority"
      if {"[info commands CBParseAppDefs]" != ""} {
        CBParseAppDefs $cbAppDefFile
      } {
        if {"[info commands CBLocalParseAppDefs]" != ""} {
          CBLocalParseAppDefs $cbAppDefFile
        }
      }
    }
  }
}

# application setting procedure
proc CBLocalSetAppDefs {{cbWidgetPath "."}} {
  global cbAppDefaults

  if {![info exists cbAppDefaults]} {
    return
  }
  foreach cbCounter $cbAppDefaults {
    if {"$cbCounter" == ""} {
      break
    }
    set widname [lindex $cbCounter 0]
    if {[string match $widname ${cbWidgetPath}] ||
        [string match "${cbWidgetPath}*" $widname]} {
      set name [string tolower [lindex $cbCounter 1]]
      set value [lindex $cbCounter 2]
      # Now lets see how many tcl commands match the name
      # pattern specified.
      set widlist [info command $widname]
      if {"$widlist" != ""} {
        foreach widget $widlist {
          # make sure this command is a widget.
          if {![catch "winfo id $widget"] &&
              [string match "${cbWidgetPath}*" $widget]} {
            catch "$widget configure -$name $value" 
          }
        }
      }
    }
  }
}

# end source
proc EndSrc {} {
if {[file exists Keyfile]} {
  set fd [open Keyfile r]
  [SN Filename] delete 0 end
  [SN Key] delete 0 end
  [SN Key] insert 0 [string trim [gets $fd]]
  close $fd
}
}

# prepare auto loading
global auto_path
global tk_library
global cbLoadPath
foreach cbElement [eval list [split $cbLoadPath :] $auto_path] {
  if {[file exists $cbElement/tclIndex]} {
    lappend auto_path $cbElement
  }
}
catch "unset auto_index"

catch "unset auto_oldpath"

catch "unset auto_execs"


# initialize global variables
proc InitGlobals {} {
  global {fsBox}
  set {fsBox(activeBackground)} {}
  set {fsBox(activeForeground)} {}
  set {fsBox(all)} {0}
  set {fsBox(background)} {}
  set {fsBox(button)} {0}
  set {fsBox(extensions)} {0}
  set {fsBox(font)} {}
  set {fsBox(foreground)} {}
  set {fsBox(internalPath)} {/home/garfield/development/TkMegaWidget}
  set {fsBox(name)} {}
  set {fsBox(path)} {/home/garfield/development/TkMegaWidget}
  set {fsBox(pattern)} {*}
  set {fsBox(scrollActiveForeground)} {}
  set {fsBox(scrollBackground)} {}
  set {fsBox(scrollForeground)} {}
  set {fsBox(scrollSide)} {left}
  set {fsBox(showPixmap)} {0}

  # please don't modify the following
  # variables. They are needed by cb.
  global {autoLoadList}
  set {autoLoadList(tkencrypt.tcl)} {0}
  global {internalAliasList}
  set {internalAliasList} {}
  global {moduleList}
  set {moduleList(tkencrypt.tcl)} {}
  global {preloadList}
  set {preloadList(cbInternal)} {}
  global {symbolicName}
  set {symbolicName(Filename)} {.frame0.frame.frame.entry2}
  set {symbolicName(Key)} {.frame0.frame5.frame.entry2}
  set {symbolicName(root)} {.}
  global {cbWmSetPosition}
  set {cbWmSetPosition} {}
  global {cbWmSetSize}
  set {cbWmSetSize} {}
  global {cbAppDefToplevels}
  set {cbAppDefToplevels} {}
}

# initialize global variables
InitGlobals

# display/remove toplevel windows.
ShowWindow.

# parse and apply application defaults.
CBLocalLoadAppDefs TkEncrypt
CBLocalSetAppDefs

# end source
EndSrc

# eof
#

