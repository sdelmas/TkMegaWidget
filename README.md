The TkMegaWidget extension

Copyright (c) 1995 by Sven Delmas
All rights reserved.
See the file COPYRIGHT for the copyright notes.

Changes:
--------
	- Tk 4.0 adaptions
	- confvalue was renamed to cget
        - debug support (not fully functional)
        - Enhanced configuration
        - Some minor bug fixes


What is TkMegaWidget ?
----------------------
This small extension to Tk allows the creation of widgets
that are combined by other widgets. It is possible to
attach options and minor commands to this new widget. The
advantage of TkMegaWidget is that the parsing and evaluation
of the resources and the activation of the methods are done
in C, which makes the implementation faster.

It is also possible to use a private interpreter attached to
the megawidget or groups of megawidgets. These private
interpreters can load encrypted files, and can also use a
private widget tree. 

I would not recommend to use DES encryption. The reason is
that this feature requires a newly built wish for each used
encryption key. So you have to build a specific wish for a
specific Tcl source file. The default key is unique to all
TkMegaWidget wishs and is part of the source file. So
everyone can read the code... (to change it search for the
function des_string_to_key in the tkMegaWidget.c file). If
you plan to really use the DES encryption, please contact me.


Requirements:
-------------
        - The Tk-4.0 package.

        - The libdes library (part of the distribution).


Calling configure:
------------------
When you call ./configure in the temporary TkMegaWidget
build directory (see below, several additional
parameters allow the selection of new packages etc..
Several packages allow the setting of additional 
subparameters. Setting one of these subparameters
means that all default settings for the subparameters
are ignored. So you have to list all required
subparameters by hand. Most parameters are directly
mapped to compiler options, so the configuration can
also be adapted by changing the Makefile directly. The
configuration reads the file "config.init", where the
several default settings are defined. The current
parameters are stored in the file "config.local". This
file can be used by calling ./configure with
-with-config-local.

IMPORTANT!!!!
If you call ./configure with new parameters (compared to
a previous call of ./configure), you should call "make
clean" before you build the new wish.

        -with-blt (default=off)
                This adds the BLT extension to the
                wish. Adding this feature makes it
                necessary to build and install the
                BLT package before you build this
                package. If there is no globally
                installed library, configure looks
                in the directory ../blt for a local
                copy.

        -with-config-local (default=off)
                Call ./configure with the parameters
                of the last ./configure call.

        -with-expect (default=off)
                This adds the expect extension to the
                wish. Adding this feature makes it
                necessary to build and install the
                expect package before you build this
                package. If there is no globally
                installed library, configure looks
                in the directory ../expect for a local
                copy.

        -with-itcl (default=off)
                This adds the itcl extension to the
                wish. Adding this feature makes it
                necessary to build and install the
                itcl package before you build this
                package. If there is no globally
                installed library, configure looks
                in the directory ../itcl for a local
                copy.

        -with-kanji (default=off)
                This allows the building of a Kanji
                wish. You have to provide the already
                patched Tk source tree under the name
                ../tk4.0-jp (or ../tk4.0jp) and the
                patched Tcl source tree under the name
                ../tcl7.4-jp (or ../tcl7.4jp).

        -with-nogcc (default=off)
                This disables the usage of gcc, irrespective
                of the existance of gcc.

        -with-noshlib (default=off)
                Even if the system supports shared
                libraries, don't build them.

        -with-tcldp (default=off)
                This adds the Tcl-DP package to the
                wish. Adding this feature makes it
                necessary to build and install the
                tcl-dp package before you build this
                package. If there is no globally
                installed library, configure looks
                in the directory ../tcl-dp for a local
                copy.

        -with-tclload (default=off)
                This adds the dynamic loading package to
                the wish. Adding this feature makes it
                necessary to build the shells package
                before you build this package. The shells
                package must be located in the same
                directory as the other extension packages
                under the name "shells".

        -with-tclX (default=off)
                This adds the TclX extension to the
                wish. Adding this feature makes it
                necessary to build and install the
                extended Tcl package before you build
                this package. If there is no globally
                installed library, configure looks
                in the directory ../tclX for a local
                copy.

        -with-tix (default=off)
                This adds the Tix extension to the
                wish. Adding this feature makes it
                necessary to build and install the
                Tix package before you build this
                package. If there is no globally
                installed library, configure looks
                in the directory ../tix for a local
                copy.

        -with-tkicon (default=off)
                This adds the TkIcon extension to
                the wish.

        -with-tklibrary=pathName (default=/usr/local/tk...)
                This specifies a new TK_LIBRARY path.
                The new Tk library searches for the
                Tcl library files in this directory.

        -with-tkmegawidget (default=on)
                This adds the TkMegaWidget extension to
                the wish. There are several 
                subparameters for this parameter:

                -with-tkmegawidget-des (default=off)
                        adds the DES encryption library
                        to the TkMegaWidget.
                -with-tkmegawidget-localdes (default=off)
                        causes the use of the local des
                        library. Otherwise, a globally 
                        installed des library is used, if
                        there is any.
                -with-tkmegawidget-debug (default=on)
                        adds the debugger code. This
                        means that it is possible to run
                        two separate interpreters where
                        one interpreter is used to debug
                        the second one.

        -with-tkmobal (default=off)
                This adds the TkMobal extension to
                the wish.

        -with-tksteal (default=off)
                This adds the TkSteal extension to
                the wish.

        -with-turndial (default=off)
                This adds the tkTurndial widget to the
                wish. Adding this feature makes it
                necessary to build and install the
                tkTurndial package before you build
                this package. If there is no globally
                installed library, configure looks
                in the directory ../tkTurndial for a
                local copy named "libtkTurndial.a".

        -with-xpm (default=off)
                This adds the Xpm3 library to the link
                command in case Tk was extended with the
                externally available Pixmap support. If
                there is no globally installed library,
                configure looks in the directory ../xpm
                for a local copy.


Building a patched wish:
------------------------
         1) Please take a look into the "Portability"
            section at the bottom first. The installation
            may require a special handling for your system.

         2) Unpack the Tk distribution. Build the Tk
            distribution as described in the Tk README. If
            your system supports shared libraries, and
            you want to use them, you should build the
            Tcl and the Tk distribution with the
            tcltk_shlib package published by Xiaokun Zhu.

         3) Unpack the TkMegaWidget distribution in the
            same directory where you have unpacked the Tk
            distribution.

         4) The resulting directory structure will look
            about like that:

            .../
               |
               +-- tcl7.4
               |
               +-- tk4.0
               |
               +-- TkMegaWidget
               |
               +-- TkSteal
               |
               ....

            Now change into the new TkMegaWidget directory.

         5) Configure the distribution by running ./configure.
            And do all the things you usually do to get a
            working Tk. To select certain additional 
            packages and perform other changes of the new
            wish, you can call ./configure with several
            parameters. Take a look into the configure
            section above for a detailed list. Certain
            extensions have to be built and installed before
            you can use them. You should take care that the
            TK_LIBRARY path is valid. This means that you
            either use the path from a previous Tk
            installation, or you create the new path, and
            copy all Tk library files to that directory.

         6) If you have selected DES support, the libdes
            library is built automatically. The library
            is not installed. This can be done manually.

         7) A new libtk will be built, as there are some
            minor problems with certain packages (especially
            with tcl-DP). This libtk is totally compatible with
            the previous libtk (the static and the shared
            version). So, I decided to install the library
            under the same name. If you do not want this,
            change the TK_LIB_NAME in the toplevel Makefile
            (go to 9)).

         8) Build the new wish (to build a new wish with
            a manually adapted Makefile, go to 9)). This
            will copy the required files to the local
            directory. Then these files are patched (the 
            protocol can be found in the files *.patch.out).
            The new local libtk and the required local
            extension libraries are built. Finally the new
            wish is linked. If your system supports shared
            libraries, they will be generated automatically.
            As the position of shared libraries is usually
            very critical, the libraries are automatically 
            installed in the final library directory when
            they are built. This makes it necessary that you 
            have write permission for that directory. To
            build the new wish, type:

                make

            Now go to 10) to continue the installation.

         9) If you want to change the toplevel Makefile
            manually (usually this is not required), first,
            you have to call:

                make prepare

            Adapt the Makefile to your personal needs. Now
            you can build the new wish (don't call "make"
            as you would normally do (see 8)), but call:

                make wish
        
        10) You can now install everything by typing this:

                make install

            This will install the new wish, the new library
            files and the new doc. By default, the library
            files are placed in the TK_LIBRARY directory.

        11) To encrypt or decrypt files, you can use the small
            program called demo.TkMegaWidget/tkencrypt. 

        12) To see what the widgets can do for you, change to
            the directory named ./demo.TkMegaWidget and type
            "../wish/mega-wish -f tkMegaWidget1.tcl" (do this
            for all tcl files in the directory).

        13) After that, you should test the new installed
            wish. If everything works fine, you can remove
            the temporary build directories.

        14) To prevent problems, please don't use this directory
            structure for totally different configurations. The
            subdirectories may contain newly copied files that
            are not removed automatically when you reconfigure
            the distribution. So you will end up in a totally
            corrupted distribution (especially the libtk directory
            will be corrupted, and will create libtk.a versions
            that cannot work). So, if you decide to reconfigure
            this package, and to drop certain packages from the
            new wish, please delete the current directorytree.
            Use a newly unpacked distribution to prevent trouble.


Portability:
------------
General:
        - If the widgets do not work as expected under the
          new wish, maybe you have forgotten to adapt the
          TK_LIBRARY path in the Makefile, or you have not
          created the new TK_LIBRARY path with all Tk
          library files.

        - If you have installed the Tcl/Tk related libraries
          in non standard directories (i.e. directories that
          are not automatically searched by the linker), you
          have to set the environment variable LD_LIBRARY_PATH
          to contain these additional directories.

        - If you plan to use dynamically loaded libraries
          together with DES encryption, you have to add a
          -ldes somewhere in the TkSteal creation code
          (Makefile). I did not add this because on different
          systems the concrete command differs very much. On
          certain system the -ldes must be part of the library
          link command. On other systems the wish link command
          must contain it. As DES encription is not very easy
          to use (actually I don't believe someone out there
          really knows how to use it under TkMegaWidget), I
          would recommend to not use this feature.

Linux:
        - I have made some small changes to the libdes code
          to make it portable to Linux.

OSF:
        - It seems that the command "sed" (which is used by GNU's
          configure package) accepts no long commandline strings
          (at least not long enough for this configure program).
          The error message you get will look about like this:

                "Too many commands, last: s%@srcdir@%.%g"

          Please install the GNU sed package, and use this "sed"
          command (make sure the new sed is found before the
          original sed).

Extended Tcl:
        - If you build the wish with extended Tcl, you have
          to set the TCL_LIBRARY path to the extended Tcl
          library path before you call the new wish.


Availability:
-------------
The TkMegaWidget distribution file is named
TkMegaWidget4.0*.tar.gz or TkMegaWidget4.0*.tar.Z.
The * represents the current version.
TkMegaWidget is available via anonymous ftp at:
        ftp.aud.alcatel.com:        tcl/extensions/
        ftp.ibp.fr:                 pub/tcl/contrib/extensions/
        nic.funet.fi:               pub/languages/tcl/extensions/
        syd.dit.csiro.au:           pub/tk/contrib/extensions/
        ftp.cs.tu-berlin.de:        pub/tcl/contrib/extensions/

To get a copy of it, just type the following:

        ftp ftp.cs.tu-berlin.de
        bin
        cd pub/tcl/extensions
        get TkMegaWidget4.0*.tar.gz

To get a copy via ftpmail, just send the following mail to:

        mail-server@cs.tu-berlin.de

To get a help message, send the following body:

        send HELP
        end

To retrieve TkMegaWidget, send the following body:

        send pub/tcl/contrib/extensions/TkMegaWidget4.0*.tar.gz
        end


Feedback (Author):
-----------------
Sven Delmas
sven@ignoranceisbliss.com


