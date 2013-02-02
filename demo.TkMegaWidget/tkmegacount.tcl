# The create procedure
proc tkmegacount {pathName args} {
  # Create the mega widget with the specified class
  megawidget $pathName -class TkMegaCount

  # Add the widget options
  $pathName optionadd -value value Value 0 0 normal {tkmegacount_config}
  $pathName optionadd -interval interval Interval 1 1 normal {tkmegacount_config}
  
  # Create the widgets
  button $pathName.decr -text "<" -command "$pathName decrement"
  label $pathName.value -text "0" -width 10 -relief raised
  button $pathName.incr -text ">" -command "$pathName increment"

  # Display the widgets
  pack $pathName.decr -side left
  pack $pathName.value -side left
  pack $pathName.incr -side left

  # Add the minor commands
  $pathName minoradd decrement {tkmegacount_decrement}
  $pathName minoradd increment {tkmegacount_increment}

  # Return the widget path name
  return $pathName
}

# The configure procedure
proc tkmegacount_config {pathName resource oldValue newValue} {

  # choose the correct resource
  case $resource {
    {-background -foreground -font -cursor} {
      $pathName recursiveconf $resource $newValue
    }
    {-value} {
      $pathName.value config -text $newValue
    }
  }
}

# The minor command to decrement
proc tkmegacount_decrement {pathName args} {
  # Get the option values
  set value [$pathName cget -value]
  set interval [$pathName cget -interval]

  # Set the new value
  $pathName config -value [expr $value-$interval]
}

# The minor command to increment
proc tkmegacount_increment {pathName args} {
  # Get the option values
  set value [$pathName cget -value]
  set interval [$pathName cget -interval]

  # Set the new value
  $pathName config -value [expr $value+$interval]
}

pack [tkmegacount .m -toplevel 0 -startvalue 10]
