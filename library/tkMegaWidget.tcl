# tkMegaWidget.tcl --
#
# Copyright (c) 1993 by Sven Delmas
# All rights reserved.
# See the file COPYRIGHT for the copyright notes.
#
# Default tkMegaWidget startup file for Tcl-based applications.
# Defines "unknown" procedure and auto-load facilities. This is a
# modified version of the original init.tcl
#
# Default system startup file for TkMegaWidget interpreters. Mainly
# defines the "unknown" procedure.
#

# unknown:
# Invoked when a Tcl command in a TkMegaWidget is invoked that
# doesn't exist in the interpreter. This command is forwarded to
# the global interpreter.

if {[string compare "" [lindex [info commands mega.*] 0]]} {

proc unknown args {
    if {![string compare "" [lindex [info commands mega.*] 0]]} {
      return -code 1 "No widget attached to this interpreter!"
    }
    return [uplevel [lindex [info commands mega.*] 0] interpeval $args]
}

}

# eof
