#!/bin/sh
bindir=$(pwd)
cd /home/ioanna/Desktop/reportsGraphics/SolarSystem/project/
export 

if test "x$1" = "x--debugger"; then
	shift
	if test "x" = "xYES"; then
		echo "r  " > $bindir/gdbscript
		echo "bt" >> $bindir/gdbscript
		GDB_COMMAND-NOTFOUND -batch -command=$bindir/gdbscript  /home/ioanna/Desktop/reportsGraphics/SolarSystem/build/project 
	else
		"/home/ioanna/Desktop/reportsGraphics/SolarSystem/build/project"  
	fi
else
	"/home/ioanna/Desktop/reportsGraphics/SolarSystem/build/project"  
fi
