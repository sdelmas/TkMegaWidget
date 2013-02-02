/*
 * Copyright (c) 1995 by Sven Delmas
 * All rights reserved.
 * See the file COPYRIGHT for the copyright notes.
 *
 */

#if !defined(TK_MEGAWIDGET_H)
#define TK_MEGAWIDGET_H 1

EXTERN int TkMegaWidget_Init _ANSI_ARGS_((Tcl_Interp *interp));
EXTERN char *TkInitMegaWidget _ANSI_ARGS_((Tcl_Interp *interp,
					   Tk_Window tkwin, int toplevel,
					   int argc, char *argv[]));

#endif

/* eof */
