proc createCommand {pathName args} {
  checkbutton $pathName.debug -text "debug" -variable debug -command "setDebug $pathName"
  pack $pathName.debug
  button $pathName.local -text "local" -command "puts b\[mega$pathName interpeval b\]"
  pack $pathName.local
  button $pathName.stack -text "stack" -command "mega$pathName interpeval w"
  pack $pathName.stack
  button $pathName.step -text "step" -command "mega$pathName interpeval s"
  pack $pathName.step
  button $pathName.continue -text "continue" -command "mega$pathName interpeval c"
  pack $pathName.continue
  button $pathName.break -text "break" -command "mega$pathName interpeval b -glob \"sabber*\""
  pack $pathName.break
}

proc setDebug {pathName} {
  global debug

  mega$pathName config -debug $debug
}

# eof
