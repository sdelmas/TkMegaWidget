/* 
 * tkMegaWidget.c --
 *
 *	This module implements "frame" widgets for the Tk
 *	toolkit.  MegaWidgets are windows with a background color
 *	and possibly a 3-D effect, and the ability to handle
 *      major/minor commands and user definable options.
 *
 * Copyright (c) 1995 by Sven Delmas
 * All rights reserved.
 * See the file COPYRIGHT for the copyright notes. The file
 * "license.terms" is part of the Tk distribution and is
 * included in the file COPYRIGHT.
 *
 *
 * This source is based upon the file tkFrame.c from:
 *
 * John Ousterhout
 *
 * Copyright (c) 1990-1994 The Regents of the University of California.
 * Copyright (c) 1994-1995 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#if defined(USE_MEGAWIDGET)

#ifndef lint
static char *AtFSid = "$Header: /usr1/master/RCS/CIMBuilder/ccode/TkMegaWidget/tkMegaWidget.c,v 2.0.0.6 1995/08/07 17:36:15 sven Exp $";
#endif /* not lint */

#include "default.h"
#include "tkPort.h"
#include "tkInt.h"
#include "tkMegaWidget.h"
#if defined(USE_DBG)
#include "Dbg.h"
#endif
#if defined(USE_DES)
#include "des.h"
#endif

/* Sven Delmas */
/*
 * Map the original Frame widget names to the new
 * TkMegaWidget names. This code was derived from 
 * the Frame widget, so to make adaption to new
 * Tk Frame widgets easier the basic code is
 * exactly the same.
 */
#define frame megaWidget
#define Frame MegaWidget
#define frameMinor megaWidgetMinor
#define FrameMinor MegaWidgetMinor
#define frameOption megaWidgetOption
#define FrameOption MegaWidgetOption
#define frameInterps megaWidgetInterps
#define FrameInterps MegaWidgetInterps
#define frameInterpsTable megaWidgetInterpsTable
#define FrameInterpsTable MegaWidgetInterpsTable
#define frameInterpsHashTable megaWidgetInterpsHashTable
#define FrameInterpsHashTable MegaWidgetInterpsHashTable
#define framePtr megaWidgetPtr
#define frameInterpsHashPtr megaWidgetInterpsHashPtr
#define Tk_FrameCmd Tk_MegaWidgetCmd
#define TkInitFrame TkInitMegaWidget
#define ConfigureFrame ConfigureMegaWidget
#define DestroyFrame DestroyMegaWidget
#define DisplayFrame DisplayMegaWidget
#define FrameEventProc MegaWidgetEventProc
#define FrameWidgetCmd MegaWidgetWidgetCmd
#define FrameCmdDeletedProc MegaWidgetCmdDeletedProc
#define MapFrame MapMegaWidget
#define FramePrivateEventProc MegaWidgetPrivateEventProc 
#define RecursiveConfigFrame RecursiveConfigMegaWidget
#define TkFrame_Init TkMegaWidget_Init

/*
 * Support for a separate widget tree for each private
 * interpreter. This code is currently not working!!!
 */
#define TK_SUPPORT 1

/*
 * Add some debugging code. Only for internal use.
 */
/*#define DEBUGGING 1*/

/*
 * I do not know why, but several window managers require multiple
 * reparent events. Right now a value of 25 should be enough.
 */
#define REPARENT_LOOPS 25

/*
 * Additional Flag bits for frames:
 *
 *   CALLED_CONFIG:  This tells the private widget event
 *      processing function, that there was already a 
 *      queued call, so it can return immediately.
 *   TK_MEGA_CONFIG_HIDDEN: This tells the configure command
 *      that only hidden options should be allowd to be
 *      configured.
 *   TK_MEGA_CONFIG_INTERNAL: This tells the configure
 *      command that only internal options should be allowed
 *      to be configured.
 *   TK_MEGA_CONFIG_INITIAL: This tells the configure
 *      command that this is the first time it is called,
 *      and so "set once" should be allowed to be configured.
 */
#define CALLED_CONFIG		16
#define TK_MEGA_CONFIG_HIDDEN   32
#define TK_MEGA_CONFIG_INTERNAL 64
#define TK_MEGA_CONFIG_INITIAL  128

/*
 * Remember the fact that the global hashtables
 * were initialized.
 */
static int initialized = 0;

/*
 * Some debugging variables. This stuff is currently
 * not working.
 */
#if defined(USE_DBG)
static int debug_wait = 0;
static char *debug_ccmd = NULL;
#endif

/*
 * The hash table with the globally known interpreters.
 */
static Tcl_HashTable frameInterpsTable;

/*
 * The known global interpreter instances.
 */
typedef struct {
    int refCount;           /* How often is is interpreter used. */
    Tcl_Interp *interp;     /* The interpreter structure. */
    int crypting;           /* This flag is set to true, when the */
			    /* userFile and libFile files are */
			    /* encrypted. */
#if defined(USE_DBG)
    int debug;              /* Turn on/off debugging. */
#endif
#if defined(TK_SUPPORT)
    Tk_Window mainWin;      /* The new main window in the newly */
                            /* created interpreter. */
    Tk_Window embedWin;     /* The newly created toplevel window */
                            /* that is embedded into the original */
                            /* MegaWidget widget by reparenting it. */
#endif
} FrameInterps;

/*
 * The additional minor commands attached this megawidget.
 */
typedef struct {
    char *minorCmd;         /* The command name. */
} FrameMinor;

/*
 * The additional options attached this megawidget.
 */
typedef struct {
    char *name;             /* The option name. */
    char *class;            /* The option class. */
    char *defaultValue;     /* The options default value. */
    char *value;            /* The options current value. */
    char *optionCmd;        /* The command to call when option is */
                            /* set. This command is called as */
                            /* follows :*/
                            /*   cmd widgetPath optionName oldValue newValue */
    int type;               /* The type of the option. Possible */
			    /* values are: */
                            /*   0 -> normal */
                            /*   1 -> readonly */
                            /*   2 -> hidden */
                            /*   3 -> once */
} FrameOption;
/* End Sven Delmas */

/*
 * A data structure of the following type is kept for each
 * frame that currently exists for this process:
 */

typedef struct {
    Tk_Window tkwin;		/* Window that embodies the frame.  NULL
				 * means that the window has been destroyed
				 * but the data structures haven't yet been
				 * cleaned up. */
    Display *display;		/* Display containing widget.  Used, among
				 * other things, so that resources can be
				 * freed even after tkwin has gone away. */
    Tcl_Interp *interp;		/* Interpreter associated with widget.  Used
				 * to delete widget command. */
    Tcl_Command widgetCmd;	/* Token for frame's widget command. */
    char *className;		/* Class name for widget (from configuration
				 * option).  Malloc-ed. */
    int mask;			/* Either FRAME or TOPLEVEL;  used to select
				 * which configuration options are valid for
				 * widget. */
    char *screenName;		/* Screen on which widget is created.  Non-null
				 * only for top-levels.  Malloc-ed, may be
				 * NULL. */
    char *visualName;		/* Textual description of visual for window,
				 * from -visual option.  Malloc-ed, may be
				 * NULL. */
    char *colormapName;		/* Textual description of colormap for window,
				 * from -colormap option.  Malloc-ed, may be
				 * NULL. */
    Colormap colormap;		/* If not None, identifies a colormap
				 * allocated for this window, which must be
				 * freed when the window is deleted. */
    Tk_3DBorder border;		/* Structure used to draw 3-D border and
				 * background.  NULL means no background
				 * or border. */
    int borderWidth;		/* Width of 3-D border (if any). */
    int relief;			/* 3-d effect: TK_RELIEF_RAISED etc. */
    int highlightWidth;		/* Width in pixels of highlight to draw
				 * around widget when it has the focus.
				 * 0 means don't draw a highlight. */
    XColor *highlightBgColorPtr;
				/* Color for drawing traversal highlight
				 * area when highlight is off. */
    XColor *highlightColorPtr;	/* Color for drawing traversal highlight. */
    int width;			/* Width to request for window.  <= 0 means
				 * don't request any size. */
    int height;			/* Height to request for window.  <= 0 means
				 * don't request any size. */
    Cursor cursor;		/* Current cursor for window, or None. */
    char *takeFocus;		/* Value of -takefocus option;  not used in
				 * the C code, but used by keyboard traversal
				 * scripts.  Malloc'ed, but may be NULL. */
    int flags;			/* Various flags;  see below for
				 * definitions. */

/* Sven Delmas */
    char *createCommand;	/* This command is evaluated at startup. */
    char *userFile;		/* This is an user file that is */
				/* loaded at startup. */
    char *libFile;		/* This is the system library. */
    int crypting;               /* This flag is set to true, when the */
				/* userFile and libFile files are */
				/* encrypted. */
    int tkSupport;              /* This flag is set to true, when the */
				/* local interpreter should use a */
				/* local widget tree too. */
				/* encrypted. */
    char *interpreter;		/* The interpreter to use. */
    Tcl_Interp *internalInterp; /* The interpreter attatched to the
			         * widget. */
    char *pathName;		/* The pathName of this widget. Used
				 * when the widget is destroyed. */
#if defined(USE_DBG)
    int debug;                  /* Turn on/off debugging. */
#endif
#if defined(TK_SUPPORT)
    Tk_Window mainWin;          /* The new main window in the newly */
                                /* created interpreter. */
    Tk_Window embedWin;         /* The newly created toplevel window */
                                /* that is embedded into the original */
                                /* MegaWidget widget by reparenting it. */
#endif
    Tcl_HashTable minors;       /* All minor commands attatched to the
				 * widget. */
    Tcl_HashTable options;      /* All options attatched to the 
				 * widget. */
/* End Sven Delmas */
} Frame;

/*
 * Flag bits for frames:
 *
 * REDRAW_PENDING:		Non-zero means a DoWhenIdle handler
 *				has already been queued to redraw
 *				this window.
 * GOT_FOCUS:			Non-zero means this widget currently
 *				has the input focus.
 */

#define REDRAW_PENDING		1
#define GOT_FOCUS		4

/*
 * The following flag bits are used so that there can be separate
 * defaults for some configuration options for frames and toplevels.
 */

#define FRAME		TK_CONFIG_USER_BIT
#define TOPLEVEL	(TK_CONFIG_USER_BIT << 1)
#define BOTH		(FRAME | TOPLEVEL)

static Tk_ConfigSpec configSpecs[] = {
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_FRAME_BG_COLOR, Tk_Offset(Frame, border),
	BOTH|TK_CONFIG_COLOR_ONLY|TK_CONFIG_NULL_OK},
    {TK_CONFIG_BORDER, "-background", "background", "Background",
	DEF_FRAME_BG_MONO, Tk_Offset(Frame, border),
	BOTH|TK_CONFIG_MONO_ONLY|TK_CONFIG_NULL_OK},
    {TK_CONFIG_SYNONYM, "-bd", "borderWidth", (char *) NULL,
	(char *) NULL, 0, BOTH},
    {TK_CONFIG_SYNONYM, "-bg", "background", (char *) NULL,
	(char *) NULL, 0, BOTH},
    {TK_CONFIG_PIXELS, "-borderwidth", "borderWidth", "BorderWidth",
	DEF_FRAME_BORDER_WIDTH, Tk_Offset(Frame, borderWidth), BOTH},
    {TK_CONFIG_STRING, "-class", "class", "Class",
	"", Tk_Offset(Frame, className), BOTH},
    {TK_CONFIG_STRING, "-colormap", "colormap", "Colormap",
	DEF_FRAME_COLORMAP, Tk_Offset(Frame, colormapName),
	BOTH|TK_CONFIG_NULL_OK},
    {TK_CONFIG_STRING, "-createcommand", "createCommand", "CreateCommand",
	"", Tk_Offset(Frame, createCommand), BOTH},
    {TK_CONFIG_INT, "-crypting", "crypting", "Crypting",
	"0", Tk_Offset(Frame, crypting), BOTH},
    {TK_CONFIG_ACTIVE_CURSOR, "-cursor", "cursor", "Cursor",
	DEF_FRAME_CURSOR, Tk_Offset(Frame, cursor), BOTH|TK_CONFIG_NULL_OK},
    {TK_CONFIG_PIXELS, "-height", "height", "Height",
	DEF_FRAME_HEIGHT, Tk_Offset(Frame, height), BOTH},
    {TK_CONFIG_COLOR, "-highlightbackground", "highlightBackground",
	"HighlightBackground", DEF_FRAME_HIGHLIGHT_BG,
	Tk_Offset(Frame, highlightBgColorPtr), BOTH},
    {TK_CONFIG_COLOR, "-highlightcolor", "highlightColor", "HighlightColor",
	DEF_FRAME_HIGHLIGHT, Tk_Offset(Frame, highlightColorPtr), BOTH},
    {TK_CONFIG_PIXELS, "-highlightthickness", "highlightThickness",
	"HighlightThickness",
	DEF_FRAME_HIGHLIGHT_WIDTH, Tk_Offset(Frame, highlightWidth), BOTH},
    {TK_CONFIG_STRING, "-interpreter", "interpreter", "Interpreter",
	"", Tk_Offset(Frame, interpreter), BOTH},
    {TK_CONFIG_STRING, "-libfile", "libFile", "LibFile",
	"", Tk_Offset(Frame, libFile), BOTH},
    {TK_CONFIG_RELIEF, "-relief", "relief", "Relief",
	DEF_FRAME_RELIEF, Tk_Offset(Frame, relief), BOTH},
    {TK_CONFIG_STRING, "-screen", "screen", "Screen",
	DEF_TOPLEVEL_SCREEN, Tk_Offset(Frame, screenName),
	TOPLEVEL|TK_CONFIG_NULL_OK},
    {TK_CONFIG_STRING, "-takefocus", "takeFocus", "TakeFocus",
	DEF_FRAME_TAKE_FOCUS, Tk_Offset(Frame, takeFocus),
	BOTH|TK_CONFIG_NULL_OK},
    {TK_CONFIG_INT, "-tksupport", "tkSupport", "TkSupport",
	"0", Tk_Offset(Frame, tkSupport), BOTH},
    {TK_CONFIG_STRING, "-userfile", "userFile", "UserFile",
	"", Tk_Offset(Frame, userFile), BOTH},
    {TK_CONFIG_STRING, "-visual", "visual", "Visual",
	DEF_FRAME_VISUAL, Tk_Offset(Frame, visualName),
	BOTH|TK_CONFIG_NULL_OK},
    {TK_CONFIG_PIXELS, "-width", "width", "Width",
	DEF_FRAME_WIDTH, Tk_Offset(Frame, width), BOTH},
#if defined(USE_DBG)
    {TK_CONFIG_BOOLEAN, "-debug", "debug",
	"Debug", "0", Tk_Offset(Frame, debug), BOTH},
#endif
    {TK_CONFIG_END, (char *) NULL, (char *) NULL, (char *) NULL,
	(char *) NULL, 0, 0}
};

/*
 * Forward declarations for procedures defined later in this file:
 */

static int		ConfigureFrame _ANSI_ARGS_((Tcl_Interp *interp,
			    Frame *framePtr, int argc, char **argv,
			    int flags));
static void		DestroyFrame _ANSI_ARGS_((char *memPtr));
static void		DisplayFrame _ANSI_ARGS_((ClientData clientData));
static void		FrameCmdDeletedProc _ANSI_ARGS_((
			    ClientData clientData));
static void		FrameEventProc _ANSI_ARGS_((ClientData clientData,
			    XEvent *eventPtr));
static int		FrameWidgetCmd _ANSI_ARGS_((ClientData clientData,
			    Tcl_Interp *interp, int argc, char **argv));
static void		MapFrame _ANSI_ARGS_((ClientData clientData));
static void		FramePrivateEventProc _ANSI_ARGS_(
			    (ClientData clientData, XEvent *eventPtr));
static void		RecursiveConfigFrame _ANSI_ARGS_((Tcl_Interp *interp,
			    TkWindow *rootWinPtr, int argc, char **argv));

#if defined(USE_DBG)
/* Sven Delmas */
/* This code is experimental... */
/* This is the original Don Libes function... only under
 * a new name to prevent name collisions.
 */
static int
debug_interactor(interp)
    Tcl_Interp *interp;
{
    int rc;

    debug_wait = 1;
fprintf(stderr, "interact\n");
    while(debug_wait) {
	Tk_DoOneEvent(0);
    }
fprintf(stderr, "eval %s<\n", debug_ccmd);
    rc = Tcl_Eval(interp, debug_ccmd);
    
return TCL_OK;
#if 0
    int rc;
    char *ccmd;		/* pointer to complete command */
    char line[BUFSIZ+1];	/* space for partial command */
    int newcmd = TRUE;
    Interp *iPtr = (Interp *)interp;

    Tcl_DString dstring;

    Tcl_DStringInit(&dstring);

    while (TRUE) {
	struct cmd_list *c;

	if (newcmd) {
	    print(interp,"dbg%d.%d> ",iPtr->numLevels,iPtr->curEventNum+1);
	} else {
	    print(interp,"dbg+> ");
	}
	fflush(stdout);

	if (0 >= (rc = read(0,line,BUFSIZ))) {
	    if (!newcmd) line[0] = 0;
	    else exit(0);
	} else line[rc] = '\0';
	
	ccmd = Tcl_DStringAppend(&dstring,line,rc);
	if (!Tcl_CommandComplete(ccmd)) {
	    newcmd = FALSE;
	    continue;	/* continue collecting command */
	}
	newcmd = TRUE;
	
	/* if user pressed return with no cmd, use previous one */
	if ((ccmd[0] == '\n' || ccmd[0] == '\r') && ccmd[1] == '\0') {
	    
	    /* this loop is guaranteed to exit through break */
	    for (c = cmd_list;c->cmdname;c++) {
		if (c->cmdtype == last_action_cmd) break;
	    }
	    
	    /* recreate textual version of command */
	    Tcl_DStringAppend(&dstring,c->cmdname,-1);
	    
	    if (c->cmdtype == step ||
		c->cmdtype == next ||
		c->cmdtype == Next) {
		char num[10];
		
		sprintf(num," %d",last_step_count);
		Tcl_DStringAppend(&dstring,num,-1);
	    }
	}
	
	rc = Tcl_RecordAndEval(interp,ccmd,0);
	Tcl_DStringFree(&dstring);
	switch (rc) {
	case TCL_OK:
	    if (*interp->result != 0)
		print(interp,"%s\n",interp->result);
	    continue;
	case TCL_ERROR:
	    print(interp,"%s\n",Tcl_GetVar(interp,"errorInfo",TCL_GLOBAL_ONLY));
	    /* since user is typing by hand, we expect lots
	       of errors, and want to give another chance */
	    continue;
	case TCL_BREAK:
	case TCL_CONTINUE:
#define finish(x)	{rc = x; goto done;}
	    finish(rc);
	case TCL_RETURN:
	    finish(TCL_OK);
	default:
	    /* note that ccmd has trailing newline */
	    print(interp,"error %d: %s\n",rc,ccmd);
	    continue;
	}
    }

    /* cannot fall thru here, must jump to label */
  done:
    Tcl_DStringFree(&dstring);

    return(rc);
#endif
}

/*
static int
TkMega_DebugCmd(clientData, interp, argc, argv)
ClientData clientData;
Tcl_Interp *interp;
int argc;
char **argv;
{
  int now = 0;
  int mega_tcl_debugger_was_available = mega_tcl_debugger_available;

  if (argc > 3) goto usage;
  if (argc == 1) {
    sprintf(interp->result,"%d",mega_tcl_debugger_available);
    return TCL_OK;
  }
  argv++;

  while (*argv) {
    if (!strcmp(*argv,"-now")) {
      now = 1;
      argv++;
    }
    else break;
  }
  
  if (!*argv) {
    if (now) {
      Dbg_On(interp,1);
      mega_tcl_debugger_available = 1;
    } else {
      goto usage;
    }
  } else if (!strcmp(*argv,"0")) {
    Dbg_Off(interp);
    mega_tcl_debugger_available = 0;
  } else {
    Dbg_On(interp,now);
    mega_tcl_debugger_available = 1;
  }
  sprintf(interp->result,"%d",mega_tcl_debugger_was_available);
  return(TCL_OK);

 usage:
  sprintf(interp->result,"usage: [[-now] 1|0]");
  return TCL_ERROR;
}
  Tcl_CreateCommand(interp, "debug", TkMega_DebugCmd,
                    (ClientData) main, (void (*)()) NULL);
*/
/* End Sven Delmas */
#endif

/*
 *--------------------------------------------------------------
 *
 * Tk_FrameCmd --
 *
 *	This procedure is invoked to process the "frame" and
 *	"toplevel" Tcl commands.  See the user documentation for
 *	details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *--------------------------------------------------------------
 */

int
Tk_FrameCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Main window associated with
				 * interpreter. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    Tk_Window tkwin = (Tk_Window) clientData;
    Frame *framePtr;
    Tk_Window new = NULL;
    char *className, *screenName, *visualName, *colormapName, *arg;
    int i, c, toplevel, depth;
    size_t length;
    Colormap colormap;
    Visual *visual;
    Display *display;

    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " pathName ?options?\"", (char *) NULL);
	return TCL_ERROR;
    }

    /*
     * Pre-process the argument list.  Scan through it to find any
     * "-class", "-screen", "-visual", and "-newcmap" options.  These
     * arguments need to be processed specially, before the window
     * is configured using the usual Tk mechanisms.
     */

    toplevel = (argv[0][0] == 't');
    className = colormapName = screenName = visualName = NULL;
    for (i = 2; i < argc; i += 2) {
	arg = argv[i];
	length = strlen(arg);
	if (length < 2) {
	    continue;
	}
	c = arg[1];
	if ((c == 'c') && (strncmp(arg, "-class", length) == 0)
		&& (length >= 3)) {
	    className = argv[i+1];
	} else if ((c == 'c')
		&& (strncmp(arg, "-colormap", length) == 0)) {
	    colormapName = argv[i+1];
	} else if ((c == 's') && toplevel
		&& (strncmp(arg, "-screen", length) == 0)) {
	    screenName = argv[i+1];
	} else if ((c == 'v')
		&& (strncmp(arg, "-visual", length) == 0)) {
	    visualName = argv[i+1];
	}
    }

    /*
     * Create the window, and deal with the special options -classname,
     * -colormap, -screenname, and -visual.  The order here is tricky,
     * because we want to allow values for these options to come from
     * the database, yet we can't do that until the window is created.
     */

    if (screenName == NULL) {
	screenName = (toplevel) ? "" : NULL;
    }
    new = Tk_CreateWindowFromPath(interp, tkwin, argv[1], screenName);
    if (new == NULL) {
	goto error;
    }
    if (className == NULL) {
	className = Tk_GetOption(new, "class", "Class");
	if (className == NULL) {
	    className = (toplevel) ? "Toplevel" : "MegaWidget";
	}
    }
    Tk_SetClass(new, className);
    if (visualName == NULL) {
	visualName = Tk_GetOption(new, "visual", "Visual");
    }
    if (colormapName == NULL) {
	colormapName = Tk_GetOption(new, "colormap", "Colormap");
    }
    colormap = None;
    if (visualName != NULL) {
	visual = Tk_GetVisual(interp, new, visualName, &depth,
		(colormapName == NULL) ? &colormap : (Colormap *) NULL);
	if (visual == NULL) {
	    goto error;
	}
	Tk_SetWindowVisual(new, visual, depth, colormap);
    }
    if (colormapName != NULL) {
	colormap = Tk_GetColormap(interp, new, colormapName);
	if (colormap == None) {
	    goto error;
	}
	Tk_SetWindowColormap(new, colormap);
    }

    /*
     * Create the widget record, process configuration options, and
     * create event handlers.  Then fill in a few additional fields
     * in the widget record from the special options.
     */

    display = Tk_Display(new);
    framePtr = (Frame *) TkInitFrame(interp, new, toplevel, argc-2, argv+2);
    if (framePtr == NULL) {
	if (colormap != None) {
	    Tk_FreeColormap(display, colormap);
	}
	return TCL_ERROR;
    }
    framePtr->colormap = colormap;
    return TCL_OK;

    error:
    if (new != NULL) {
	Tk_DestroyWindow(new);
    }
    return TCL_ERROR;
}

/*
 *----------------------------------------------------------------------
 *
 * TkInitFrame --
 *
 *	This procedure initializes a frame or toplevel widget.  It's
 *	separate from Tk_FrameCmd so that it can be used for the
 *	main window, which has already been created elsewhere.
 *
 * Results:
 *	Returns NULL if an error occurred while initializing the
 *	frame.  Otherwise returns a pointer to the frame's widget
 *	record (for use by Tk_FrameCmd, if it was the caller).
 *
 * Side effects:
 *	A widget record gets allocated, handlers get set up, etc..
 *
 *----------------------------------------------------------------------
 */

char *
TkInitFrame(interp, tkwin, toplevel, argc, argv)
    Tcl_Interp *interp;			/* Interpreter associated with the
					 * application. */
    Tk_Window tkwin;			/* Window to use for frame or
					 * top-level.   Caller must already
					 * have set window's class. */
    int toplevel;			/* Non-zero means that this is a
					 * top-level window, 0 means it's a
					 * frame. */
    int argc;				/* Number of configuration arguments
					 * (not including class command and
					 * window name). */
    char *argv[];			/* Configuration arguments. */
{
    register Frame *framePtr;
    int fileId = -1, new, readLen;
    unsigned int counter;
    char *cmdBuffer = NULL, *readBuffer = NULL, *value, msg[200], iv[8];
    struct stat statBuf;
    FrameInterps *frameInterps;
    Tcl_HashEntry *frameInterpsHashPtr;
    Tcl_DString command;
#if defined(USE_DES)
    des_cblock kk;
    des_key_schedule ks;
#endif

    if (!initialized) {
        /* initialize the hash table with the interpreters */
        /* if this is was not already done. */
	initialized = 1;
	Tcl_InitHashTable(&frameInterpsTable, TCL_STRING_KEYS);
    }

    framePtr = (Frame *) ckalloc(sizeof(Frame));
    framePtr->tkwin = tkwin;
    framePtr->display = Tk_Display(tkwin);
    framePtr->interp = interp;
    framePtr->widgetCmd = Tcl_CreateCommand(interp,
	    Tk_PathName(framePtr->tkwin), FrameWidgetCmd,
	    (ClientData) framePtr, FrameCmdDeletedProc);
    framePtr->className = NULL;
    framePtr->mask = (toplevel) ? TOPLEVEL : FRAME;
    framePtr->screenName = NULL;
    framePtr->visualName = NULL;
    framePtr->colormapName = NULL;
    framePtr->colormap = None;
    framePtr->border = NULL;
    framePtr->borderWidth = 0;
    framePtr->relief = TK_RELIEF_FLAT;
    framePtr->highlightWidth = 0;
    framePtr->highlightBgColorPtr = NULL;
    framePtr->highlightColorPtr = NULL;
    framePtr->width = 0;
    framePtr->height = 0;
    framePtr->cursor = None;
    framePtr->takeFocus = NULL;
    framePtr->flags = 0;

/* Sven Delmas */
    framePtr->createCommand = NULL;
    framePtr->crypting = 0;
    framePtr->libFile = NULL;
    framePtr->userFile = NULL;
    framePtr->interpreter = NULL;
    framePtr->internalInterp = interp;
    framePtr->tkSupport = 0;
    framePtr->pathName =
	ckalloc(strlen(Tk_PathName(framePtr->tkwin)) + 2);
    strcpy(framePtr->pathName, Tk_PathName(framePtr->tkwin));
#if defined(USE_DBG)
    framePtr->debug = 0;
#endif
#if defined(TK_SUPPORT)
    framePtr->mainWin = NULL;
    framePtr->embedWin = NULL;
#endif
    Tcl_InitHashTable(&framePtr->minors, TCL_STRING_KEYS);
    Tcl_InitHashTable(&framePtr->options, TCL_STRING_KEYS);

    Tk_CreateEventHandler(framePtr->tkwin,
	    ExposureMask|StructureNotifyMask|FocusChangeMask,
	    FrameEventProc, (ClientData) framePtr);
    if (ConfigureFrame(interp, framePtr, argc, argv,
	    TK_MEGA_CONFIG_INITIAL) != TCL_OK) {
	goto error;
    }

    if (framePtr->interpreter != NULL &&
	strlen(framePtr->interpreter) > (size_t) 0) {
        /* Create new interpreter. */
	frameInterpsHashPtr =
	    Tcl_CreateHashEntry(&frameInterpsTable,
				framePtr->interpreter, &new);
	if (new) {
            /* Really create it... there was none. */
	    frameInterps = (FrameInterps *)
		ckalloc(sizeof(FrameInterps));
	    frameInterps->refCount = 1;
	    frameInterps->crypting = framePtr->crypting;
#if defined(USE_DBG)
	    frameInterps->debug = framePtr->debug;
#endif
#if defined(TK_SUPPORT)
	    frameInterps->mainWin = NULL;
	    frameInterps->embedWin = NULL;
#endif
	    framePtr->internalInterp = interp;
	    Tcl_SetHashValue(frameInterpsHashPtr, frameInterps);
	} else {
            /* There was already an interpreter with that */
            /* name... use this one. */
	    frameInterps = (FrameInterps *)
		Tcl_GetHashValue(frameInterpsHashPtr);
	    frameInterps->refCount++;
	    framePtr->internalInterp = frameInterps->interp;
#if defined(TK_SUPPORT)
	    framePtr->mainWin = frameInterps->mainWin;
#endif
	}

	/* A new interpreter has to be created... */
	if (framePtr->internalInterp == interp) {
	    frameInterps->interp = Tcl_CreateInterp();
#if defined(TK_SUPPORT)
	    if (frameInterps->mainWin == NULL &&
		framePtr->tkSupport) {
                /* Create a new main window in the new interpreter. */
		frameInterps->mainWin = 
		    Tk_CreateMainWindow(frameInterps->interp, "", 
					framePtr->interpreter, "Tk");
		if (frameInterps->mainWin == NULL) {
		    fprintf(stderr, "%s\n", frameInterps->interp->result);
		    exit(1);
		}
		framePtr->mainWin = frameInterps->mainWin;
		/* Initialize the new interpreter. */
		if (Tcl_AppInit(frameInterps->interp) != TCL_OK) {
		    fprintf(stderr, "Tcl_AppInit failed: %s\n", 
			    frameInterps->interp->result);
		}
	    }
#endif
	    framePtr->internalInterp = frameInterps->interp;
            /* Set up the global variables in the new interpreter. */
	    if ((value = Tcl_GetVar(interp, "argc",
				    TCL_GLOBAL_ONLY)) != NULL) {
		Tcl_SetVar(framePtr->internalInterp, "argc",
			   value, TCL_GLOBAL_ONLY);
	    }
	    if ((value = Tcl_GetVar(interp, "argv",
				    TCL_GLOBAL_ONLY)) != NULL) {
		Tcl_SetVar(framePtr->internalInterp, "argv",
			   value, TCL_GLOBAL_ONLY);
	    }
	    if ((value = Tcl_GetVar(interp, "argv0",
				    TCL_GLOBAL_ONLY)) != NULL) {
		Tcl_SetVar(framePtr->internalInterp, "argv0",
			   value, TCL_GLOBAL_ONLY);
	    }
	    Tcl_SetVar(framePtr->internalInterp, "tcl_interactive",
		       "0", TCL_GLOBAL_ONLY);
	    Tcl_SetVar(framePtr->internalInterp, "mega_interpreter",
		       framePtr->interpreter, TCL_GLOBAL_ONLY);
	    sprintf(msg, "%d", framePtr->crypting);
	    Tcl_SetVar(framePtr->internalInterp, "mega_crypting",
		       msg, TCL_GLOBAL_ONLY);
	}

#if defined(TK_SUPPORT)
	if (framePtr->mainWin != NULL) {
            /* Create a toplevel that will be embedded into */
            /* the MegaWidget widget. This makes it independant */
            /* from the general widget structure. */
	    Tcl_DStringInit(&command);
	    Tcl_DStringAppend(&command, "toplevel .", -1);
	    Tcl_DStringAppend(&command, Tk_Name(framePtr->tkwin), -1);
	    Tcl_DStringAppend(&command, " ; wm minsize .", -1);
	    Tcl_DStringAppend(&command, Tk_Name(framePtr->tkwin), -1);
	    Tcl_DStringAppend(&command, " 0 0 ; wm maxsize . ", -1);
	    Tcl_DStringAppend(&command, Tk_Name(framePtr->tkwin), -1);
	    Tcl_DStringAppend(&command, " 10000 10000", -1);
	    Tcl_Eval(framePtr->internalInterp, command.string);
	    Tcl_DStringFree(&command);
	    Tcl_DStringInit(&command);
	    Tcl_DStringAppend(&command, ".", -1);
	    Tcl_DStringAppend(&command, Tk_Name(framePtr->tkwin), -1);
	    frameInterps->embedWin =
		Tk_NameToWindow(framePtr->internalInterp, 
				command.string, framePtr->mainWin);
	    Tcl_DStringFree(&command);
	    if (frameInterps->embedWin == NULL) {
		goto error;
	    }
	    Tk_CreateEventHandler(frameInterps->embedWin,
				  ExposureMask|StructureNotifyMask,
				  FramePrivateEventProc,
				  (ClientData) framePtr);
	    framePtr->embedWin = frameInterps->embedWin;

	    Tk_MakeWindowExist(tkwin);
	    Tk_MakeWindowExist(framePtr->mainWin);
	    Tk_MakeWindowExist(framePtr->embedWin);
	    if (framePtr->mainWin != NULL) {
		Tcl_DStringInit(&command);
		Tcl_DStringAppend(&command, "wm withdraw .", -1);
		Tcl_Eval(framePtr->internalInterp, command.string);
	    }
	    if (framePtr->embedWin != NULL) {
		for (counter = 0; counter < REPARENT_LOOPS; counter++) {
		    XReparentWindow(Tk_Display(framePtr->embedWin), 
				    Tk_WindowId(framePtr->embedWin),
				    Tk_WindowId(tkwin), 0, 0);
		    XSync(Tk_Display(framePtr->embedWin), False);
		}
	    }
	}
#endif

	/* Create the new widget command in the additional interpreter. */
	/* The command gets the prefix "mega" to allow the access to */
	/* the original widget command. */
	value = ckalloc(strlen(Tk_PathName(framePtr->tkwin)) + 20);
	strcpy(value, "mega");
	strcat(value, Tk_PathName(framePtr->tkwin));
	Tcl_CreateCommand(framePtr->internalInterp,
			  value, FrameWidgetCmd,
			  (ClientData) framePtr,
			  (void (*)()) NULL);
	ckfree((char *) value);
    } else {
        /* Use the global interpreter. */
	frameInterpsHashPtr =
	    Tcl_CreateHashEntry(&frameInterpsTable,
				"TKFRAME-MAIN", &new);
	if (new) {
	    frameInterps = (FrameInterps *)
		ckalloc(sizeof(FrameInterps));
	    frameInterps->refCount = 1;
	    frameInterps->crypting = 0;
	    Tcl_SetHashValue(frameInterpsHashPtr, frameInterps);
	} else {
	    frameInterps = (FrameInterps *)
		Tcl_GetHashValue(frameInterpsHashPtr);
	    frameInterps->refCount++;
	}
	frameInterps->interp = interp;
    }

    /* Once we decided whether to use crypting ot not we have to */
    /* contionue with that. We also want to guarantee the crypting */
    /* is only done in a secure interpreter. */
    if (framePtr->crypting &&
	interp == framePtr->internalInterp) {
	Tcl_AppendResult(interp, "crypting only in a secure interpreter", 
			 (char *) NULL);
	goto error;
    }
    if (framePtr->crypting != frameInterps->crypting) {
	Tcl_AppendResult(interp, "previous crypting selection does not match",
			 (char *) NULL);
	goto error;
    }

    /* Initialize decryption. */
#if defined(USE_DES)
    if (framePtr->crypting &&
	(framePtr->libFile != NULL || framePtr->userFile != NULL)) {
	des_string_to_key("abcdabcd", (des_cblock *) kk);
	des_set_key((des_cblock *) kk, ks);
    }
#endif

    /* Initialize the interpreter from Tcl libs. */
    if (framePtr->libFile != NULL &&
	strlen(framePtr->libFile) > (size_t) 0) {
	fileId = open(framePtr->libFile, O_RDONLY, 0);
	if (fileId < 0) {
	    Tcl_AppendResult(interp, "couldn't open file \"",
			     framePtr->libFile, "\": ",
			     Tcl_PosixError(framePtr->internalInterp),
			     (char *) NULL);
	    goto error;
	}
	if (fstat(fileId, &statBuf) == -1) {
	    Tcl_AppendResult(interp, "couldn't stat file \"",
			     framePtr->libFile, "\": ",
			     Tcl_PosixError(framePtr->internalInterp),
			     (char *) NULL);
	    goto error;
	}
	readLen = statBuf.st_size;
	readBuffer = (char *) ckalloc((unsigned) readLen + 9);
	cmdBuffer = (char *) ckalloc((unsigned) readLen + 9);
	if (read(fileId, readBuffer, (size_t) readLen) != readLen) {
	    Tcl_AppendResult(interp, "couldn't read file \"",
			     framePtr->libFile, "\": ",
			     Tcl_PosixError(framePtr->internalInterp),
			     (char *) NULL);
	    goto error;
	}
	if (close(fileId) != 0) {
	    Tcl_AppendResult(interp, "couldn't close file \"",
			     framePtr->libFile, "\": ",
			     Tcl_PosixError(framePtr->internalInterp),
			     (char *) NULL);
	    fileId = -1;
	    goto error;
	}
	fileId = -1;
	readBuffer[readLen] = '\0';
	memset(iv, 0, sizeof(iv));
	if (framePtr->crypting) {
#if defined(USE_DES)
	    des_cbc_encrypt((des_cblock *) readBuffer,
			    (des_cblock *) cmdBuffer,
			    (long) readLen, ks,
			    (des_cblock *) iv, DES_DECRYPT);
	    cmdBuffer[readLen - 8 + cmdBuffer[readLen-1]] = '\0';
#else
	    Tcl_AppendResult(interp, "no encryption available \"",
			     framePtr->libFile, (char *) NULL);
	    goto error;
#endif
	} else {
            strcpy(cmdBuffer, readBuffer);
	}
	if (Tcl_Eval(framePtr->internalInterp, cmdBuffer) == TCL_ERROR) {
	    if (framePtr->internalInterp != interp) {
		Tcl_AppendResult(interp,
				 framePtr->internalInterp->result,
				 (char *) NULL);
	    }
	    sprintf(msg, "\n    (file \"%.150s\" line %d)",
		    framePtr->libFile,
		    framePtr->internalInterp->errorLine);
	    Tcl_AddErrorInfo(interp, msg);
	    goto error;
	}
	ckfree((char *) cmdBuffer);
	cmdBuffer = NULL;
	ckfree((char *) readBuffer);
	readBuffer = NULL;
    }

    /* Load the user library file. */
    if (framePtr->userFile != NULL &&
	strlen(framePtr->userFile) > (size_t) 0) {
	fileId = open(framePtr->userFile, O_RDONLY, 0);
	if (fileId < 0) {
	    Tcl_AppendResult(interp, "couldn't open file \"",
			     framePtr->userFile, "\": ",
			     Tcl_PosixError(framePtr->internalInterp),
			     (char *) NULL);
	    goto error;
	}
	if (fstat(fileId, &statBuf) == -1) {
	    Tcl_AppendResult(interp, "couldn't stat file \"",
			     framePtr->userFile, "\": ",
			     Tcl_PosixError(framePtr->internalInterp),
			     (char *) NULL);
	    goto error;
	}
	readLen = statBuf.st_size;
	readBuffer = (char *) ckalloc((unsigned) readLen + 9);
	cmdBuffer = (char *) ckalloc((unsigned) readLen + 9);
	if (read(fileId, readBuffer, (size_t) readLen) != readLen) {
	    Tcl_AppendResult(interp, "couldn't read file \"",
			     framePtr->userFile, "\": ",
			     Tcl_PosixError(framePtr->internalInterp),
			     (char *) NULL);
	    goto error;
	}
	if (close(fileId) != 0) {
	    Tcl_AppendResult(interp, "couldn't close file \"",
			     framePtr->userFile, "\": ",
			     Tcl_PosixError(framePtr->internalInterp),
			     (char *) NULL);
	    fileId = -1;
	    goto error;
	}
	fileId = -1;
	readBuffer[readLen] = '\0';
	memset(iv, 0, sizeof(iv));
	if (framePtr->crypting) {
#if defined(USE_DES)
	    des_cbc_encrypt((des_cblock *) readBuffer,
			    (des_cblock *) cmdBuffer,
			    (long) readLen, ks,
			    (des_cblock *) iv, DES_DECRYPT);
	    cmdBuffer[readLen - 8 + cmdBuffer[readLen-1]] = '\0';
#else
	    Tcl_AppendResult(interp, "no encryption available \"",
			     framePtr->libFile, (char *) NULL);
	    goto error;
#endif
	} else {
	    strcpy(cmdBuffer, readBuffer);
	}
	if (Tcl_Eval(framePtr->internalInterp, cmdBuffer) == TCL_ERROR) {
	    if (framePtr->internalInterp != interp) {
		Tcl_AppendResult(interp,
				 framePtr->internalInterp->result,
				 (char *) NULL);
	    }
	    sprintf(msg, "\n    (file \"%.150s\" line %d)",
		    framePtr->userFile,
		    framePtr->internalInterp->errorLine);
	    Tcl_AddErrorInfo(interp, msg);
	    goto error;
	}
	ckfree((char *) cmdBuffer);
	cmdBuffer = NULL;
	ckfree((char *) readBuffer);
	readBuffer = NULL;
    }

    /* Run widget specific initialization command. */
    if (framePtr->createCommand != NULL &&
	strlen(framePtr->createCommand) > (size_t) 0) {
	Tcl_DStringInit(&command);
	Tcl_DStringAppend(&command, framePtr->createCommand, -1);
	Tcl_DStringAppend(&command, " ", -1);
	Tcl_DStringAppend(&command, Tk_PathName(framePtr->tkwin), -1);
	for (counter = 0; counter < argc; counter++) {
	    Tcl_DStringAppend(&command, " ", -1);
	    Tcl_DStringAppend(&command, argv[counter], -1);
	}
	if (Tcl_Eval(framePtr->internalInterp,
		     command.string) != TCL_OK) {
	    if (framePtr->internalInterp != interp) {
		Tcl_AppendResult(interp,
				 framePtr->internalInterp->result,
				 (char *) NULL);
	    }
	    Tcl_DStringFree(&command);
	    goto error;
	}
	Tcl_DStringFree(&command);
    }
/* End Sven Delmas */

    if (toplevel) {
	Tk_DoWhenIdle(MapFrame, (ClientData) framePtr);
    }
    interp->result = Tk_PathName(framePtr->tkwin);
    return (char *) framePtr;

    error:
/* Sven Delmas */
    if (fileId >= 0) {
	close(fileId);
    }
    if (cmdBuffer != NULL) {
	ckfree((char *) cmdBuffer);
    }
    if (readBuffer != NULL) {
	ckfree((char *) readBuffer);
    }
/* End Sven Delmas */
    Tk_DestroyWindow(framePtr->tkwin);
    return NULL;
}

/*
 *--------------------------------------------------------------
 *
 * FrameWidgetCmd --
 *
 *	This procedure is invoked to process the Tcl command
 *	that corresponds to a frame widget.  See the user
 *	documentation for details on what it does.
 *
 * Results:
 *	A standard Tcl result.
 *
 * Side effects:
 *	See the user documentation.
 *
 *--------------------------------------------------------------
 */

static int
FrameWidgetCmd(clientData, interp, argc, argv)
    ClientData clientData;	/* Information about frame widget. */
    Tcl_Interp *interp;		/* Current interpreter. */
    int argc;			/* Number of arguments. */
    char **argv;		/* Argument strings. */
{
    register Frame *framePtr = (Frame *) clientData;
    int result = TCL_OK;
    char c, i;
    int new;
    int counter;
    char *value;
    size_t length;
    Tcl_HashEntry *hashEntry;
    Tcl_HashSearch search;
    FrameMinor *minorPtr;
    FrameOption *optionPtr;
    Tcl_DString command;
  
    if (argc < 2) {
	Tcl_AppendResult(interp, "wrong # args: should be \"",
		argv[0], " option ?arg arg ...?\"", (char *) NULL);
	return TCL_ERROR;
    }
    Tk_Preserve((ClientData) framePtr);
    c = argv[1][0];
    length = strlen(argv[1]);
/* Sven Delmas */
    if ((c == 'c') && (strncmp(argv[1], "cget", length) == 0)
	    && (length >= 2)) {
	if (argc != 3) {
	    Tcl_AppendResult(interp, "wrong # args: should be \"",
		    argv[0], " cget option\"",
		    (char *) NULL);
	    result = TCL_ERROR;
	    goto done;
	}
	result = Tk_ConfigureValue(interp, framePtr->tkwin, configSpecs,
		(char *) framePtr, argv[2], framePtr->mask);
	if (result == TCL_ERROR) {
            /* Standard processing of option failed. Now try */
            /* to use the user defined options. */
	    result = TCL_OK;
	    Tcl_ResetResult(interp);
	    hashEntry = Tcl_FindHashEntry(&framePtr->options, argv[2]);
	    if (hashEntry != NULL) {
		optionPtr = (FrameOption *) Tcl_GetHashValue(hashEntry);
		if (optionPtr->type == 0 || optionPtr->type == 1) {
		    Tcl_AppendResult(interp, optionPtr->value,
				     (char *) NULL);
		} else {
		    Tcl_AppendResult(interp, "no access to private option \"",
				     argv[2], "\"", (char *) NULL);
		    result = TCL_ERROR;
		}
	    } else {
		Tcl_AppendResult(interp, "unknown option \"", argv[2],
				 "\"", (char *) NULL);
		result = TCL_ERROR;
	    }
	}
    } else if ((c == 'c') && (strncmp(argv[1], "configure", length) == 0)
	    && (length >= 2)) {
	if (argc == 2) {
            /* Retrieve the standard options. */
	    result = Tk_ConfigureInfo(interp, framePtr->tkwin, configSpecs,
		    (char *) framePtr, (char *) NULL, framePtr->mask);
            /* Now add the user defined options, except for those */
            /* options that are standard options. */
	    for (hashEntry = Tcl_FirstHashEntry(&framePtr->options, &search);
		 hashEntry != NULL; hashEntry = Tcl_NextHashEntry(&search)) {
		optionPtr = (FrameOption *) Tcl_GetHashValue(hashEntry);
		if ((optionPtr->type == 0 || optionPtr->type == 1) &&
		    strcmp(Tcl_GetHashKey(&framePtr->options,
					  hashEntry), "-background") != 0 &&
		    strcmp(Tcl_GetHashKey(&framePtr->options,
					  hashEntry), "-bd") != 0 &&
		    strcmp(Tcl_GetHashKey(&framePtr->options,
					  hashEntry), "-bg") != 0 &&
		    strcmp(Tcl_GetHashKey(&framePtr->options,
					  hashEntry), "-borderwidth") != 0 &&
		    strcmp(Tcl_GetHashKey(&framePtr->options,
					  hashEntry), "-cursor") != 0 &&
		    strcmp(Tcl_GetHashKey(&framePtr->options,
					  hashEntry), "-height") != 0 &&
		    strcmp(Tcl_GetHashKey(&framePtr->options,
					  hashEntry), "-relief") != 0 &&
		    strcmp(Tcl_GetHashKey(&framePtr->options,
					  hashEntry), "-width") != 0 &&
		    strcmp(Tcl_GetHashKey(&framePtr->options,
					  hashEntry), "-debug") != 0) {
		    Tcl_AppendResult(interp, " {",
				     Tcl_GetHashKey(&framePtr->options,
						    hashEntry), " ",
				     (char *) NULL);
		    Tcl_AppendResult(interp, "{", optionPtr->name, "} {",
				     optionPtr->class, "} {",
				     optionPtr->defaultValue, "} {",
				     optionPtr->value, "}", "}",
				     (char *) NULL);
		}
	    }
	} else if (argc == 3) {
	    result = Tk_ConfigureInfo(interp, framePtr->tkwin, configSpecs,
		    (char *) framePtr, argv[2], framePtr->mask);
	    if (result == TCL_ERROR) {
                /* Standard processing of option failed. Now try */
                /* to use the user defined options. */
		result = TCL_OK;
		Tcl_ResetResult(interp);
		hashEntry = Tcl_FindHashEntry(&framePtr->options, argv[2]);
		if (hashEntry != NULL) {
		    optionPtr = (FrameOption *) Tcl_GetHashValue(hashEntry);
		    if (optionPtr->type == 0 || optionPtr->type == 1) {
			Tcl_AppendResult(interp,
					 Tcl_GetHashKey(&framePtr->options,
							hashEntry), " ",
					 (char *) NULL);
			Tcl_AppendResult(interp, "{", optionPtr->name, "} {",
					 optionPtr->class, "} {",
					 optionPtr->defaultValue, "} {",
					 optionPtr->value, "}",
					 (char *) NULL);
		    } else {
			Tcl_AppendResult(interp, "no access to private option \"",
					 argv[2], "\"", (char *) NULL);
			result = TCL_ERROR;
		    }
		} else {
		    Tcl_AppendResult(interp, "unknown option \"", argv[2],
				     "\"", (char *) NULL);
		    result = TCL_ERROR;
		}
	    }
	} else {
	    /*
	     * Don't allow the options -class, -newcmap, -screen,
	     * or -visual to be changed.
	     */

	    for (i = 2; i < argc; i++) {
		length = strlen(argv[i]);
		if (length < 2) {
		    continue;
		}
		c = argv[i][1];
		if (((c == 'c') && (strncmp(argv[i], "-class", length) == 0)
			&& (length >= 2))
			|| ((c == 'c') && (framePtr->mask == TOPLEVEL)
			&& (strncmp(argv[i], "-colormap", length) == 0))
			|| ((c == 's') && (framePtr->mask == TOPLEVEL)
			&& (strncmp(argv[i], "-screen", length) == 0))
			|| ((c == 'v') && (framePtr->mask == TOPLEVEL)
			&& (strncmp(argv[i], "-visual", length) == 0))) {
		    Tcl_AppendResult(interp, "can't modify ", argv[i],
			    " option after widget is created", (char *) NULL);
		    result = TCL_ERROR;
		    goto done;
		}
	    }
	    result = ConfigureFrame(interp, framePtr, argc-2, argv+2,
		    TK_CONFIG_ARGV_ONLY);
	}
    } else if ((c == 'c') && (strncmp(argv[1], "confhidden", length) == 0)) {
        /* Do the same as for the standard configure command, */
        /* except that only hidden options can be manipulated. */
	if (interp != framePtr->internalInterp) {
	    Tcl_AppendResult(interp, "minor command \"", argv[1],
			     "\" only available in secure interpreter",
			     (char *) NULL);
	    result = TCL_ERROR;
	    goto done;
	}
	if (argc == 2) {
	    result = Tk_ConfigureInfo(interp, framePtr->tkwin,
				      configSpecs, (char *) framePtr,
				      (char *) NULL, framePtr->mask);
	    for (hashEntry = Tcl_FirstHashEntry(&framePtr->options, &search);
		 hashEntry != NULL; hashEntry = Tcl_NextHashEntry(&search)) {
		optionPtr = (FrameOption *) Tcl_GetHashValue(hashEntry);
		if (optionPtr->type == 2) {
		    Tcl_AppendResult(interp, " {",
				     Tcl_GetHashKey(&framePtr->options,
						    hashEntry), " ",
				     (char *) NULL);
		    Tcl_AppendResult(interp, "{", optionPtr->name, "} {",
				     optionPtr->class, "} {",
				     optionPtr->defaultValue, "} {",
				     optionPtr->value, "}", "}",
				     (char *) NULL);
		}
	    }
	} else if (argc == 3) {
	    result = Tk_ConfigureInfo(interp, framePtr->tkwin,
				      configSpecs, (char *) framePtr,
				      argv[2], framePtr->mask);
	    if (result == TCL_ERROR) {
		result = TCL_OK;
		Tcl_ResetResult(interp);
		hashEntry = Tcl_FindHashEntry(&framePtr->options, argv[2]);
		if (hashEntry != NULL) {
		    optionPtr = (FrameOption *) Tcl_GetHashValue(hashEntry);
		    if (optionPtr->type == 2 ||
			framePtr->internalInterp == interp) {
			Tcl_AppendResult(interp, optionPtr->value, (char *) NULL);
		    } else {
			Tcl_AppendResult(interp, "no access to hidden option \"",
					 argv[2], "\"", (char *) NULL);
			result = TCL_ERROR;
		    }
		} else {
		    Tcl_AppendResult(interp, "unknown hidden option \"", argv[2],
				     "\"", (char *) NULL);
		    result = TCL_ERROR;
		}
	    }
	} else {
	    result = ConfigureFrame(interp, framePtr, argc-2,
					 argv+2,
					 TK_CONFIG_ARGV_ONLY |
					 TK_MEGA_CONFIG_HIDDEN);
	}
    } else if ((c == 'c') && (strncmp(argv[1], "confinternal", length) == 0)) {
	if (interp != framePtr->internalInterp) {
	    Tcl_AppendResult(interp, "minor command \"", argv[1],
			     "\" only available in secure interpreter",
			     (char *) NULL);
	    result = TCL_ERROR;
	    goto done;
	}
	if (argc > 3) {
	    result = ConfigureFrame(interp, framePtr, argc-2,
					 argv+2, TK_CONFIG_ARGV_ONLY |
					 TK_MEGA_CONFIG_INTERNAL);
	}
    } else if ((c == 'i') && (strncmp(argv[1], "interpeval", length) == 0)
	    && (length >= 6)) {
	if (interp != framePtr->internalInterp) {
	    Tcl_AppendResult(interp, "minor command \"", argv[1],
			     "\" only available in secure interpreter",
			     (char *) NULL);
	    result = TCL_ERROR;
	    goto done;
	}
	if (argc >= 3) {
	    Tcl_DStringInit(&command);
	    for (counter = 2; counter < argc; counter++) {
		Tcl_DStringAppend(&command, "{", -1);
		Tcl_DStringAppend(&command, argv[counter], -1);
		Tcl_DStringAppend(&command, "} ", -1);
	    }
#if defined(USE_DBG)
	    if (debug_wait) {
		if (debug_ccmd != NULL) {
		    ckfree((char *) debug_ccmd);
		}
		debug_ccmd = ckalloc((size_t) Tcl_DStringLength(&command) + 1);
		strcpy(debug_ccmd, command.string);
		debug_wait = 0;
	    } else {
#endif
		result = Tcl_GlobalEval(framePtr->interp, command.string);
		if (result == TCL_ERROR) {
		    Tcl_AddErrorInfo(interp, framePtr->interp->result);
		}
		if (framePtr->internalInterp != framePtr->interp) {
		    Tcl_ResetResult(interp);
		    Tcl_AppendResult(interp, framePtr->interp->result,
				     (char *) NULL);
		}
#if defined(USE_DBG)
	    }
#endif
	    Tcl_DStringFree(&command);
	} else {
	    Tcl_AppendResult(interp, "wrong # args",
			     ": should be \"", argv[0], " ", argv[1],
			     " arg ?arg ...?\"",
			     (char *) NULL);
	    result = TCL_ERROR;
	}
    } else if ((c == 'i') && (strncmp(argv[1], "interpset", length) == 0)
	    && (length >= 6)) {
	if (interp != framePtr->internalInterp) {
	    Tcl_AppendResult(interp, "minor command \"", argv[1],
			     "\" only available in secure interpreter",
			     (char *) NULL);
	    result = TCL_ERROR;
	    goto done;
	}
	if (argc == 3) {
	    if ((value = Tcl_GetVar(framePtr->interp, argv[2],
				    TCL_GLOBAL_ONLY)) == NULL) {
		Tcl_AddErrorInfo(interp, framePtr->interp->result);
		result = TCL_ERROR;
	    }
	    Tcl_ResetResult(interp);
	    Tcl_AppendResult(interp, value, (char *) NULL);
	} else {
	    if (argc == 4) {
		if ((value = Tcl_SetVar(framePtr->interp,
					argv[2], argv[3],
					TCL_GLOBAL_ONLY)) == NULL) {
		    Tcl_AddErrorInfo(interp, framePtr->interp->result);
		    result = TCL_ERROR;
		}
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp, value, (char *) NULL);
	    } else {
		Tcl_AppendResult(interp, "wrong # args",
				 ": should be \"", argv[0], " ", argv[1],
				 " varName ?newValue?\"",
				 (char *) NULL);
		result = TCL_ERROR;
	    }
	}
    } else if ((c == 'm') && (strncmp(argv[1], "minoradd", length) == 0)
	    && (length >= 6)) {
	if (interp != framePtr->internalInterp) {
	    Tcl_AppendResult(interp, "minor command \"", argv[1],
			     "\" only available in secure interpreter",
			     (char *) NULL);
	    result = TCL_ERROR;
	    goto done;
	}
	if (argc == 4) {
	    hashEntry = Tcl_CreateHashEntry(&framePtr->minors,
					    argv[2], &new);
	    if (new) {
		minorPtr = (FrameMinor *) ckalloc(sizeof(FrameMinor));
		Tcl_SetHashValue(hashEntry, minorPtr);
	    } else {
		minorPtr = (FrameMinor *) Tcl_GetHashValue(hashEntry);
		ckfree((char *) minorPtr->minorCmd);
	    }
	    minorPtr->minorCmd = (char *) ckalloc(strlen(argv[3])+1);
	    strcpy(minorPtr->minorCmd, argv[3]);
	} else {
	    Tcl_AppendResult(interp, "wrong number of arguments",
			     ": must be \"", argv[1],
			     " minorName minorProc\"",
			     (char *) NULL);
	    result = TCL_ERROR;
	}
    } else if ((c == 'm') && (strncmp(argv[1], "minordelete", length) == 0)
	    && (length >= 6)) {
	if (interp != framePtr->internalInterp) {
	    Tcl_AppendResult(interp, "minor command \"", argv[1],
			     "\" only available in secure interpreter",
			     (char *) NULL);
	    result = TCL_ERROR;
	    goto done;
	}
	if (argc == 2) {
	    Tcl_ResetResult(interp);
	    for (hashEntry = Tcl_FirstHashEntry(&framePtr->minors, &search);
		 hashEntry != NULL; hashEntry = Tcl_NextHashEntry(&search)) {
		minorPtr = (FrameMinor *) Tcl_GetHashValue(hashEntry);
		Tcl_DeleteHashEntry(hashEntry);
		ckfree((char *) minorPtr->minorCmd);
		ckfree((char *) minorPtr);
	    }
	} else {
	    if (argc == 3) {
		Tcl_ResetResult(interp);
		hashEntry = Tcl_FindHashEntry(&framePtr->minors, argv[2]);
		if (hashEntry != NULL) {
		    minorPtr = (FrameMinor *) Tcl_GetHashValue(hashEntry);
		    Tcl_DeleteHashEntry(hashEntry);
		    ckfree((char *) minorPtr->minorCmd);
		    ckfree((char *) minorPtr);
		} else {
		    Tcl_AppendResult(interp, "unknown minor command: \"",
				     argv[2], "\"", (char *) NULL);
		    result = TCL_ERROR;
		}
	    } else {
		Tcl_AppendResult(interp, "wrong number of arguments",
				 ": must be \"", argv[1],
				 " ?minorName?\"", (char *) NULL);
		result = TCL_ERROR;
	    }
	}
    } else if ((c == 'm') && (strncmp(argv[1], "minorlist", length) == 0)
	    && (length >= 6)) {
	if (interp != framePtr->internalInterp) {
	    Tcl_AppendResult(interp, "minor command \"", argv[1],
			     "\" only available in secure interpreter",
			     (char *) NULL);
	    result = TCL_ERROR;
	    goto done;
	}
	if (argc == 2) {
	    Tcl_ResetResult(interp);
	    for (hashEntry = Tcl_FirstHashEntry(&framePtr->minors, &search);
		 hashEntry != NULL; hashEntry = Tcl_NextHashEntry(&search)) {
		Tcl_AppendResult(interp, Tcl_GetHashKey(&framePtr->minors,
							hashEntry),
				 " ", (char *) NULL);
	    }
	} else {
	    if (argc == 3) {
		Tcl_ResetResult(interp);
		hashEntry = Tcl_FindHashEntry(&framePtr->minors,
					      argv[2]);
		if (hashEntry != NULL) {
		    minorPtr = (FrameMinor *) Tcl_GetHashValue(hashEntry);
		    Tcl_AppendResult(interp, minorPtr->minorCmd,
				     " ", (char *) NULL);
		} else {
		    Tcl_AppendResult(interp, "unknown minor command: \"",
				     argv[2], "\"", (char *) NULL);
		    result = TCL_ERROR;
		}
	    } else {
		Tcl_AppendResult(interp, "wrong number of arguments",
				 ": must be \"", argv[1],
				 " ?minorName?\"", (char *) NULL);
		result = TCL_ERROR;
	    }
	}
    } else if ((c == 'o') && (strncmp(argv[1], "optionadd", length) == 0)
	    && (length >= 7)) {
	if (interp != framePtr->internalInterp) {
	    Tcl_AppendResult(interp, "minor command \"", argv[1],
			     "\" only available in secure interpreter",
			     (char *) NULL);
	    result = TCL_ERROR;
	    goto done;
	}
	if (argc == 9) {
	    hashEntry = Tcl_CreateHashEntry(&framePtr->options,
					    argv[2], &new);
	    if (new) {
		optionPtr = (FrameOption *) ckalloc(sizeof(FrameOption));
		Tcl_SetHashValue(hashEntry, optionPtr);
	    } else {
		optionPtr = (FrameOption *) Tcl_GetHashValue(hashEntry);
		ckfree((char *) optionPtr->name);
		ckfree((char *) optionPtr->class);
		ckfree((char *) optionPtr->defaultValue);
		ckfree((char *) optionPtr->value);
		ckfree((char *) optionPtr->optionCmd);
	    }
	    optionPtr->name = (char *) ckalloc(strlen(argv[3])+1);
	    strcpy(optionPtr->name, argv[3]);
	    optionPtr->class = (char *) ckalloc(strlen(argv[4])+1);
	    strcpy(optionPtr->class, argv[4]);
	    optionPtr->defaultValue = (char *) ckalloc(strlen(argv[6])+1);
	    strcpy(optionPtr->defaultValue, argv[6]);
	    optionPtr->value = (char *) ckalloc(strlen(argv[5])+1);
	    strcpy(optionPtr->value, argv[5]);
	    if (strcmp(argv[7], "readonly") == 0) {
		optionPtr->type = 1;
	    } else {
		if (strcmp(argv[7], "hidden") == 0) {
		    optionPtr->type = 2;
		} else {
		    if (strcmp(argv[7], "once") == 0) {
			optionPtr->type = 3;
		    } else {
			optionPtr->type = 0;
		    }
		}
	    }
	    optionPtr->optionCmd = (char *) ckalloc(strlen(argv[8])+1);
	    strcpy(optionPtr->optionCmd, argv[8]);
	} else {
	    Tcl_AppendResult(interp, "wrong number of arguments",
			     ": must be \"", argv[1],
			     " commandlineSwitch optionName ",
			     "className defaultValue value ",
			     "type optionProc\"", (char *) NULL);
	    result = TCL_ERROR;
	}
    } else if ((c == 'o') && (strncmp(argv[1], "optiondelete", length) == 0)
	    && (length >= 7)) {
	if (interp != framePtr->internalInterp) {
	    Tcl_AppendResult(interp, "minor command \"", argv[1],
			     "\" only available in secure interpreter",
			     (char *) NULL);
	    result = TCL_ERROR;
	    goto done;
	}
	if (argc == 2) {
	    Tcl_ResetResult(interp);
	    for (hashEntry = Tcl_FirstHashEntry(&framePtr->options, &search);
		 hashEntry != NULL; hashEntry = Tcl_NextHashEntry(&search)) {
		optionPtr = (FrameOption *) Tcl_GetHashValue(hashEntry);
		Tcl_DeleteHashEntry(hashEntry);
		ckfree((char *) optionPtr->name);
		ckfree((char *) optionPtr->class);
		ckfree((char *) optionPtr->defaultValue);
		ckfree((char *) optionPtr->value);
		ckfree((char *) optionPtr->optionCmd);
		ckfree((char *) optionPtr);
	    }
	} else {
	    if (argc == 3) {
		Tcl_ResetResult(interp);
		hashEntry = Tcl_FindHashEntry(&framePtr->options, argv[2]);
		if (hashEntry != NULL) {
		    optionPtr = (FrameOption *) Tcl_GetHashValue(hashEntry);
		    Tcl_DeleteHashEntry(hashEntry);
		    ckfree((char *) optionPtr->name);
		    ckfree((char *) optionPtr->class);
		    ckfree((char *) optionPtr->defaultValue);
		    ckfree((char *) optionPtr->value);
		    ckfree((char *) optionPtr->optionCmd);
		    ckfree((char *) optionPtr);
		} else {
		    Tcl_AppendResult(interp, "unknown option \"",
				     argv[2], "\"", (char *) NULL);
		    result = TCL_ERROR;
		}
	    } else {
		Tcl_AppendResult(interp, "wrong number of arguments",
				 ": must be \"", argv[1],
				 " ?optionName?\"", (char *) NULL);
		result = TCL_ERROR;
	    }
	}
    } else if ((c == 'o') && (strncmp(argv[1], "optionlist", length) == 0)
	    && (length >= 7)) {
	if (interp != framePtr->internalInterp) {
	    Tcl_AppendResult(interp, "minor command \"", argv[1],
			     "\" only available in secure interpreter",
			     (char *) NULL);
	    result = TCL_ERROR;
	    goto done;
	}
	if (argc == 2) {
	    Tcl_ResetResult(interp);
	    for (hashEntry = Tcl_FirstHashEntry(&framePtr->options, &search);
		 hashEntry != NULL; hashEntry = Tcl_NextHashEntry(&search)) {
		Tcl_AppendResult(interp, Tcl_GetHashKey(&framePtr->options,
							hashEntry),
				 " ", (char *) NULL);
	    }
	} else {
	    if (argc == 3) {
		Tcl_ResetResult(interp);
		hashEntry = Tcl_FindHashEntry(&framePtr->options,
					      argv[2]);
		if (hashEntry != NULL) {
		    optionPtr = (FrameOption *) Tcl_GetHashValue(hashEntry);
		    Tcl_AppendResult(interp,
				     Tcl_GetHashKey(&framePtr->options,
						    hashEntry), " ",
				     (char *) NULL);
		    Tcl_AppendResult(interp, "{", optionPtr->name, "} {",
				     optionPtr->class, "} {",
				     optionPtr->defaultValue, "} {",
				     optionPtr->value, "} {",
				     optionPtr->optionCmd, "} ",
				     (char *) NULL);
		    if (optionPtr->type == 0) {
			Tcl_AppendResult(interp, "normal", (char *) NULL);
		    } else {
			if (optionPtr->type == 1) {
			    Tcl_AppendResult(interp, "readonly", (char *) NULL);
			} else {
			    if (optionPtr->type == 2) {
				Tcl_AppendResult(interp, "hidden", (char *) NULL);
			    }
			}
		    }
		} else {
		    Tcl_AppendResult(interp, "unknown option \"", argv[2],
				     "\"", (char *) NULL);
		    result = TCL_ERROR;
		}
	    } else {
		Tcl_AppendResult(interp, "wrong number of arguments",
				 ": must be \"", argv[1], 
				 " ?optionName?\"", (char *) NULL);
		result = TCL_ERROR;
	    }
	}
    } else if ((c == 'r') && (strncmp(argv[1], "recursiveconfigure", length) == 0)
	    && (length >= 11)) {
	if (argc == 4) {
	    Tk_FakeWin *rootWinPtr = (Tk_FakeWin *) framePtr->tkwin;
	    
	    Tcl_VarEval(interp, rootWinPtr->pathName,
			" confinternal ", argv[2], " {", argv[3],
			"}", (char *) NULL);
	    RecursiveConfigFrame(interp, (TkWindow *) rootWinPtr,
				      argc, argv);
	    Tcl_ResetResult(interp);
	} else {
	    Tcl_AppendResult(interp, "wrong number of arguments",
			     ": must be \"", argv[1], 
			     " optionName optionValue\"", (char *) NULL);
	    result = TCL_ERROR;
	}
    } else {
        /* None of the standard minor commands matched. Now */
        /* try to evaluate a user defined minor command. */
	for (hashEntry = Tcl_FirstHashEntry(&framePtr->minors, &search);
	     hashEntry != NULL; hashEntry = Tcl_NextHashEntry(&search)) {
	    if (strcmp(Tcl_GetHashKey(&framePtr->minors,
				      hashEntry), argv[1]) == 0) {
		minorPtr = (FrameMinor *) Tcl_GetHashValue(hashEntry);
		Tcl_DStringInit(&command);
		Tcl_DStringAppend(&command, minorPtr->minorCmd, -1);
		Tcl_DStringAppend(&command, " ", -1);
		Tcl_DStringAppend(&command, argv[0], -1);
		for (counter = 2; counter < argc; counter++) {
		    Tcl_DStringAppend(&command, " {", -1);
		    Tcl_DStringAppend(&command, argv[counter], -1);
		    Tcl_DStringAppend(&command, "}", -1);
		}
		result = Tcl_GlobalEval(framePtr->internalInterp,
					command.string);
		if (result == TCL_ERROR) {
		    Tcl_AddErrorInfo(interp,
				     framePtr->internalInterp->result);
		}
		if (framePtr->internalInterp != interp) {
		    Tcl_ResetResult(interp);
		    Tcl_AppendResult(interp, framePtr->internalInterp->result,
				     (char *) NULL);
		}
		Tcl_DStringFree(&command);
		goto done;
	    }
	}
	Tcl_AppendResult(interp, "unknown minor command: \"",
			 argv[1], "\"", (char *) NULL);
	result = TCL_ERROR;
    }
/* End Sven Delmas */

    done:
    Tk_Release((ClientData) framePtr);
    return result;
}

/*
 *----------------------------------------------------------------------
 *
 * DestroyFrame --
 *
 *	This procedure is invoked by Tk_EventuallyFree or Tk_Release
 *	to clean up the internal structure of a frame at a safe time
 *	(when no-one is using it anymore).
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Everything associated with the frame is freed up.
 *
 *----------------------------------------------------------------------
 */

static void
DestroyFrame(memPtr)
    char *memPtr;             /* Info about frame widget. */
{
    register Frame *framePtr = (Frame *) memPtr;
    Tcl_HashEntry *interpHashPtr, *hashEntry;
    Tcl_HashSearch search;
    FrameInterps *interpPtr;
    FrameMinor *minorPtr;
    FrameOption *optionPtr;
    char *value;

    if (framePtr->colormap != None) {
	Tk_FreeColormap(framePtr->display, framePtr->colormap);
    }

/* Sven Delmas */
    /* Remove the mega procedure. */
    value = ckalloc(strlen(framePtr->pathName) + 20);
    strcpy(value, "mega");
    strcat(value, framePtr->pathName);
    Tcl_DeleteCommand(framePtr->internalInterp, value);
    ckfree((char *) value);

    /* Destroy interpreter. */
    if (framePtr->interpreter != NULL &&
	strlen(framePtr->interpreter) > (size_t) 0) {
	interpHashPtr = Tcl_FindHashEntry(&frameInterpsTable,
					  (char *) framePtr->interpreter);
	if (interpHashPtr != NULL) {
	    interpPtr = (FrameInterps *) Tcl_GetHashValue(interpHashPtr);
	    interpPtr->refCount--;
	    if (interpPtr->refCount <= 0) {
		value = ckalloc(30);
		strcpy(value, "update idletask");
		Tcl_Eval(interpPtr->interp, value);
		strcpy(value, "destroy .");
		Tcl_Eval(interpPtr->interp, value);
		ckfree((char *) value);
		Tcl_DeleteInterp(interpPtr->interp);
                ckfree((char *) interpPtr);
		Tcl_DeleteHashEntry(interpHashPtr);
	    }
	}
    }
    if (framePtr->pathName != NULL) {
	ckfree((char *) framePtr->pathName);
    }
#if defined(USE_DBG)
    if (debug_ccmd != NULL) {
      ckfree((char *) debug_ccmd);
      debug_ccmd = NULL;
    }
#endif

    /* Remove the user defined options. */
    for (hashEntry = Tcl_FirstHashEntry(&framePtr->options, &search);
         hashEntry != NULL; hashEntry = Tcl_NextHashEntry(&search)) {
      optionPtr = (FrameOption *) Tcl_GetHashValue(hashEntry);
      Tcl_DeleteHashEntry(hashEntry);
      ckfree((char *) optionPtr->name);
      ckfree((char *) optionPtr->class);
      ckfree((char *) optionPtr->defaultValue);
      ckfree((char *) optionPtr->value);
      ckfree((char *) optionPtr->optionCmd);
      ckfree((char *) optionPtr);
    }
    Tcl_DeleteHashTable(&framePtr->options);

    /* Remove the user defined minor commands. */
    for (hashEntry = Tcl_FirstHashEntry(&framePtr->minors, &search);
         hashEntry != NULL; hashEntry = Tcl_NextHashEntry(&search)) {
      minorPtr = (FrameMinor *) Tcl_GetHashValue(hashEntry);
      Tcl_DeleteHashEntry(hashEntry);
      ckfree((char *) minorPtr->minorCmd);
      ckfree((char *) minorPtr);
    }
    Tcl_DeleteHashTable(&framePtr->minors);
/* End Sven Delmas */

    Tk_FreeOptions(configSpecs, (char *) framePtr, framePtr->display,
	    framePtr->mask);
    ckfree((char *) framePtr);
}

/*
 *----------------------------------------------------------------------
 *
 * ConfigureFrame --
 *
 *	This procedure is called to process an argv/argc list, plus
 *	the Tk option database, in order to configure (or
 *	reconfigure) a frame widget.
 *
 * Results:
 *	The return value is a standard Tcl result.  If TCL_ERROR is
 *	returned, then interp->result contains an error message.
 *
 * Side effects:
 *	Configuration information, such as text string, colors, font,
 *	etc. get set for framePtr;  old resources get freed, if there
 *	were any.
 *
 *----------------------------------------------------------------------
 */

static int
ConfigureFrame(interp, framePtr, argc, argv, flags)
    Tcl_Interp *interp;		/* Used for error reporting. */
    register Frame *framePtr;	/* Information about widget;  may or may
				 * not already have values for some fields. */
    int argc;			/* Number of valid entries in argv. */
    char **argv;		/* Arguments. */
    int flags;			/* Flags to pass to Tk_ConfigureWidget. */
{
    int counter;
    char *oldValue;
    Tcl_HashEntry *hashEntry;
    FrameOption *optionPtr;
  
/* Sven Delmas */
    if (argc % 2 == 1) {
	Tcl_AppendResult(interp, "value for \"",
			 argv[argc-1], "\" missing", (char *) NULL);
	return TCL_ERROR;
    }
  
    Tcl_ResetResult(interp);
    /* First process all user defined options. */
    for (counter = 0; counter < argc; counter++) {
	hashEntry = Tcl_FindHashEntry(&framePtr->options,
				      argv[counter]);
	if (hashEntry != NULL) {
	    optionPtr = (FrameOption *) Tcl_GetHashValue(hashEntry);
	    if ((optionPtr->type == 3 && (flags & TK_MEGA_CONFIG_INITIAL)) ||
		(optionPtr->type == 0 && !(flags & TK_MEGA_CONFIG_HIDDEN)) ||
		(optionPtr->type == 1 && (flags & TK_MEGA_CONFIG_INTERNAL)) ||
		(optionPtr->type == 2 && (flags & TK_MEGA_CONFIG_HIDDEN))) {
		oldValue = optionPtr->value;
		optionPtr->value = (char *) ckalloc(strlen(argv[counter+1])+1);
		strcpy(optionPtr->value, argv[counter+1]);
		if (strlen(optionPtr->optionCmd) > (size_t) 0 &&
		    !(flags & TK_MEGA_CONFIG_INTERNAL)) {
		    if (Tcl_VarEval(framePtr->internalInterp,
				    optionPtr->optionCmd, " ",
				    Tk_PathName(framePtr->tkwin), " ",
				    argv[counter], " {", oldValue, "} {",
				    argv[counter+1], "}",
				    (char *) NULL) != TCL_OK) {
			ckfree((char *) oldValue);
			if (framePtr->internalInterp != interp) {
			    Tcl_AppendResult(interp,
					     framePtr->internalInterp->result,
					     (char *) NULL);
			}
			return TCL_ERROR;
		    }
		}
		ckfree((char *) oldValue);
	    } else {
		Tcl_AppendResult(interp, "not a valid option \"",
				 argv[counter], "\"", (char *) NULL);
		return TCL_ERROR;
	    }
	} else {
	    if (strcmp(argv[counter], "-background") != 0 &&
		strcmp(argv[counter], "-bd") != 0 &&
		strcmp(argv[counter], "-bg") != 0 &&
		strcmp(argv[counter], "-borderwidth") != 0 &&
		strcmp(argv[counter], "-class") != 0 &&
		strcmp(argv[counter], "-createcommand") != 0 &&
		strcmp(argv[counter], "-crypting") != 0 &&
		strcmp(argv[counter], "-cursor") != 0 &&
		strcmp(argv[counter], "-height") != 0 &&
		strcmp(argv[counter], "-interpreter") != 0 &&
		strcmp(argv[counter], "-libfile") != 0 &&
		strcmp(argv[counter], "-relief") != 0 &&
		strcmp(argv[counter], "-tksupport") != 0 &&
		strcmp(argv[counter], "-userfile") != 0 &&
		strcmp(argv[counter], "-width") != 0 &&
		strcmp(argv[counter], "-debug") != 0) {
		Tcl_AppendResult(interp, "unknown option \"",
				 argv[counter], "\"", (char *) NULL);
		return TCL_ERROR;
	    }
	}
	counter++;
    }

    /* Process the standard options. */
    Tk_ConfigureWidget(interp, framePtr->tkwin, configSpecs,
	    argc, argv, (char *) framePtr, flags | framePtr->mask);
    Tcl_ResetResult(interp);
  
    /* I don't want borders != 0... reparenting will overwrite anyway. */
    framePtr->borderWidth = 0;

#if defined(USE_DBG)
    /* enable/disable debugging. */
    if (framePtr->debug) {
	Dbg_On(framePtr->interp, 0);
	Dbg_Interactor(interp, debug_interactor);
    } else {
	Dbg_Off(framePtr->interp);
    }
#endif
/* End Sven Delmas */

    if (framePtr->border != NULL) {
	Tk_SetBackgroundFromBorder(framePtr->tkwin, framePtr->border);
    }
    if (framePtr->highlightWidth < 0) {
	framePtr->highlightWidth = 0;
    }
    Tk_SetInternalBorder(framePtr->tkwin,
	    framePtr->borderWidth + framePtr->highlightWidth);
    if ((framePtr->width > 0) || (framePtr->height > 0)) {
	Tk_GeometryRequest(framePtr->tkwin, framePtr->width,
		framePtr->height);
    }

    if (Tk_IsMapped(framePtr->tkwin)) {
	if (!(framePtr->flags & REDRAW_PENDING)) {
	    Tk_DoWhenIdle(DisplayFrame, (ClientData) framePtr);
	}
	framePtr->flags |= REDRAW_PENDING;
    }
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * DisplayFrame --
 *
 *	This procedure is invoked to display a frame widget.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Commands are output to X to display the frame in its
 *	current mode.
 *
 *----------------------------------------------------------------------
 */

static void
DisplayFrame(clientData)
    ClientData clientData;	/* Information about widget. */
{
    register Frame *framePtr = (Frame *) clientData;
    register Tk_Window tkwin = framePtr->tkwin;
    GC gc;

#if defined(DEBUGGING)
    fprintf(stderr, "display %s\n", 
            Tk_PathName(framePtr->tkwin));
#endif
    framePtr->flags &= ~REDRAW_PENDING;
    if ((framePtr->tkwin == NULL) || !Tk_IsMapped(tkwin)) {
	return;
    }

    if (framePtr->border != NULL) {
	Tk_Fill3DRectangle(tkwin, Tk_WindowId(tkwin),
		framePtr->border, framePtr->highlightWidth,
		framePtr->highlightWidth,
		Tk_Width(tkwin) - 2*framePtr->highlightWidth,
		Tk_Height(tkwin) - 2*framePtr->highlightWidth,
		framePtr->borderWidth, framePtr->relief);
    }
    if (framePtr->highlightWidth != 0) {
	if (framePtr->flags & GOT_FOCUS) {
	    gc = Tk_GCForColor(framePtr->highlightColorPtr,
		    Tk_WindowId(tkwin));
	} else {
	    gc = Tk_GCForColor(framePtr->highlightBgColorPtr,
		    Tk_WindowId(tkwin));
	}
	Tk_DrawFocusHighlight(tkwin, gc, framePtr->highlightWidth,
		Tk_WindowId(tkwin));
    }

#if defined(DEBUGGING)
    fprintf(stderr, "display...done\n");
#endif
}

/*
 *--------------------------------------------------------------
 *
 * FrameEventProc --
 *
 *	This procedure is invoked by the Tk dispatcher on
 *	structure changes to a frame.  For frames with 3D
 *	borders, this procedure is also invoked for exposures.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	When the window gets deleted, internal structures get
 *	cleaned up.  When it gets exposed, it is redisplayed.
 *
 *--------------------------------------------------------------
 */

static void
FrameEventProc(clientData, eventPtr)
    ClientData clientData;	/* Information about window. */
    register XEvent *eventPtr;	/* Information about event. */
{
    register Frame *framePtr = (Frame *) clientData;

    if ((eventPtr->type == Expose) && (eventPtr->xexpose.count == 0)) {
#if defined(DEBUGGING)
	fprintf(stderr, "public expose %s\n", 
		Tk_PathName(framePtr->tkwin));
#endif
	goto redraw;
    } else if (eventPtr->type == DestroyNotify) {
#if defined(DEBUGGING)
	fprintf(stderr, "public destroy %s\n", 
		Tk_PathName(framePtr->tkwin));
#endif
	if (framePtr->tkwin != NULL) {
	    framePtr->tkwin = NULL;
	    Tcl_DeleteCommand(framePtr->interp,
		    Tcl_GetCommandName(framePtr->interp, framePtr->widgetCmd));
	}
	if (framePtr->flags & REDRAW_PENDING) {
	    Tk_CancelIdleCall(DisplayFrame, (ClientData) framePtr);
	}
	Tk_CancelIdleCall(MapFrame, (ClientData) framePtr);
	Tk_EventuallyFree((ClientData) framePtr, DestroyFrame);
    } else if (eventPtr->type == FocusIn) {
/* Sven Delmas */
#if defined(DEBUGGING)
	fprintf(stderr, "public focusin %s\n", 
		Tk_PathName(framePtr->tkwin));
#endif
	if (eventPtr->xfocus.detail != NotifyInferior) {
	    framePtr->flags |= GOT_FOCUS;
	    if (framePtr->highlightWidth > 0) {
		goto redraw;
	    }
	}
    } else if (eventPtr->type == FocusOut) {
#if defined(DEBUGGING)
	fprintf(stderr, "public focusout %s\n", 
		Tk_PathName(framePtr->tkwin));
#endif
	if (eventPtr->xfocus.detail != NotifyInferior) {
	    framePtr->flags &= ~GOT_FOCUS;
	    if (framePtr->highlightWidth > 0) {
		goto redraw;
	    }
	}
    }
#if defined(TK_SUPPORT)
    if (eventPtr->type == ConfigureNotify &&
	framePtr->tkwin != NULL &&
	framePtr->embedWin != NULL &&
	Tk_IsMapped(framePtr->embedWin)) {
        /*  Propagate the configure event, so the */
        /* embedded window changes size. */
	char buffer[30];
	Tcl_DString command;

	framePtr->flags |= CALLED_CONFIG;
	Tcl_DStringInit(&command);
	Tcl_DStringAppend(&command, "wm geometry .", -1);
	Tcl_DStringAppend(&command, Tk_Name(framePtr->tkwin), -1);
	sprintf(buffer, " %d", Tk_Width(framePtr->tkwin));
	Tcl_DStringAppend(&command, buffer, -1);
	sprintf(buffer, "x%d", Tk_Height(framePtr->tkwin));
	Tcl_DStringAppend(&command, buffer, -1);
#if defined(DEBUGGING)
	fprintf(stderr, "public configure %s: %s\n", 
		Tk_PathName(framePtr->tkwin), command.string);
#endif
	Tcl_Eval(framePtr->internalInterp, command.string);
	Tcl_DStringFree(&command);
    }
#endif
/* End Sven Delmas */
    return;

    redraw:
    if ((framePtr->tkwin != NULL) && !(framePtr->flags & REDRAW_PENDING)) {
	Tk_DoWhenIdle(DisplayFrame, (ClientData) framePtr);
	framePtr->flags |= REDRAW_PENDING;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * FrameCmdDeletedProc --
 *
 *	This procedure is invoked when a widget command is deleted.  If
 *	the widget isn't already in the process of being destroyed,
 *	this command destroys it.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The widget is destroyed.
 *
 *----------------------------------------------------------------------
 */

static void
FrameCmdDeletedProc(clientData)
    ClientData clientData;	/* Pointer to widget record for widget. */
{
    Frame *framePtr = (Frame *) clientData;
    Tk_Window tkwin = framePtr->tkwin;

    /*
     * This procedure could be invoked either because the window was
     * destroyed and the command was then deleted (in which case tkwin
     * is NULL) or because the command was deleted, and then this procedure
     * destroys the widget.
     */

    if (tkwin != NULL) {
#if defined(DEBUGGING)
      fprintf(stderr, "delete proc\n", 
              Tk_PathName(tkwin));
#endif
	framePtr->tkwin = NULL;
	Tk_DestroyWindow(tkwin);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * MapFrame --
 *
 *	This procedure is invoked as a when-idle handler to map a
 *	newly-created top-level frame.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The frame given by the clientData argument is mapped.
 *
 *----------------------------------------------------------------------
 */

static void
MapFrame(clientData)
    ClientData clientData;		/* Pointer to frame structure. */
{
    Frame *framePtr = (Frame *) clientData;

    /*
     * Wait for all other background events to be processed before
     * mapping window.  This ensures that the window's correct geometry
     * will have been determined before it is first mapped, so that the
     * window manager doesn't get a false idea of its desired geometry.
     */

    Tk_Preserve((ClientData) framePtr);
    while (1) {
	if (Tk_DoOneEvent(TK_IDLE_EVENTS) == 0) {
	    break;
	}

	/*
	 * After each event, make sure that the window still exists
	 * and quit if the window has been destroyed.
	 */

	if (framePtr->tkwin == NULL) {
	    Tk_Release((ClientData) framePtr);
	    return;
	}
    }
    Tk_MapWindow(framePtr->tkwin);
#if defined(DEBUGGING)
    fprintf(stderr, "map %s\n", 
            Tk_PathName(framePtr->tkwin));
#endif
    Tk_Release((ClientData) framePtr);
}

/* Sven Delmas */
/*
 *--------------------------------------------------------------
 *
 * FramePrivateEventProc --
 *
 *	This procedure is invoked by the Tk dispatcher on
 *	structure changes to a window embedded in a frame.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	When the window gets deleted, internal structures get
 *	cleaned up.  When it gets exposed, it is redisplayed.
 *
 *--------------------------------------------------------------
 */

static void
FramePrivateEventProc(clientData, eventPtr)
    ClientData clientData;	/* Information about window. */
    register XEvent *eventPtr;	/* Information about event. */
{
#if defined(TK_SUPPORT)
    register Frame *framePtr = (Frame *) clientData;

    if (framePtr->flags & CALLED_CONFIG) {
	framePtr->flags &= ~CALLED_CONFIG;
	return;
    }
    if ((framePtr->embedWin == NULL) ||
	!Tk_IsMapped(framePtr->embedWin)) {
	return;
    }
    if (((eventPtr->type == Expose) && (eventPtr->xexpose.count == 0)) ||
	eventPtr->type == ConfigureNotify || eventPtr->type == MapNotify) {
        if (framePtr->tkwin != NULL) {
#if defined(DEBUGGING)
	    fprintf(stderr, "private expose %s: %d %d %d %d\n", 
		    Tk_PathName(framePtr->tkwin),
                    Tk_X(framePtr->embedWin), 
                    Tk_Y(framePtr->embedWin),
                    Tk_Width(framePtr->embedWin), 
                    Tk_Height(framePtr->embedWin));
#endif
            Tk_GeometryRequest(framePtr->tkwin,
                               Tk_Width(framePtr->embedWin), 
                               Tk_Height(framePtr->embedWin));
          }
    } else if (eventPtr->type == DestroyNotify) {
	if (framePtr->tkwin != NULL) {
#if defined(DEBUGGING)
	    fprintf(stderr, "private destroy %s\n", 
		    Tk_PathName(framePtr->tkwin));
#endif
	}
    } else if (eventPtr->type == FocusIn) {
	if (framePtr->tkwin != NULL &&
            eventPtr->xfocus.detail != NotifyInferior) {
#if defined(DEBUGGING)
	    fprintf(stderr, "private focus in %s\n", 
		    Tk_PathName(framePtr->tkwin));
#endif
	}
    } else if (eventPtr->type == FocusOut) {
	if (framePtr->tkwin != NULL &&
            eventPtr->xfocus.detail != NotifyInferior) {
#if defined(DEBUGGING)
	    fprintf(stderr, "private focus out %s\n", 
		    Tk_PathName(framePtr->tkwin));
#endif
	}
    }
#endif
}

/*
 *----------------------------------------------------------------------
 *
 * RecursiveConfigFrame --
 *
 *	This procedure is invoked to recursively configure all
 *	children in the MegaWidget widget. Errors during
 *      configuration are ignored, so even options unknown
 *      to certain widget classes can be recursively changed.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	The widgets are configured.
 *
 *----------------------------------------------------------------------
 */

static void
RecursiveConfigFrame(interp, rootWinPtr, argc, argv)
    Tcl_Interp *interp;
    TkWindow *rootWinPtr;
    int argc;
    char **argv;
{
    register TkWindow *winPtr;
  
    for (winPtr = ((TkWindow *) rootWinPtr)->childList;
	 winPtr != NULL; winPtr = winPtr->nextPtr) {
	Tcl_VarEval(interp, winPtr->pathName, " configure ",
		    argv[2], " {", argv[3], "}", (char *) NULL);
	RecursiveConfigFrame(interp, winPtr, argc, argv);
    }
}

/*
 *----------------------------------------------------------------------
 *
 * TkFrame_Init --
 *
 *      This procedure initializes the MegaWidget package, and
 *      should be called in Tcl_AppInit, or by the load feature.
 *
 * Results:
 *      Always TCL_OK.
 *
 * Side effects:
 *      Adds the new commands to the interpreter.
 *
 *----------------------------------------------------------------------
 */
int
TkFrame_Init(interp)
    Tcl_Interp *interp;         /* Interpreter for application. */
{
    /*
     * Call Tcl_CreateCommand for application-specific commands.
     */
    Tcl_CreateCommand(interp, "megawidget", Tk_FrameCmd,
		      (ClientData) Tk_MainWindow(interp),
		      (Tcl_CmdDeleteProc *) NULL);
    Tcl_CreateCommand(interp, "topmegawidget", Tk_FrameCmd,
		      (ClientData) Tk_MainWindow(interp),
		      (Tcl_CmdDeleteProc *) NULL);
  
    return TCL_OK;
}
/* End Sven Delmas */
#endif

/* eof */
