#!/bin/sh

if [ $# -lt 3 ]
then
    echo "Usage: $0 srcdir builddir compiler"
    exit 1
fi

uname=`uname -a`
date=`date`
srcdir=$1
builddir=$2
compiler=$3

if [ ! "x$USER" = "x" ]; then
    whoami=$USER
else
    if [ ! "x$LOGNAME" = "x" ]; then
	whoami=$LOGNAME
    else
	whoami=`whoami || id -un`
    fi
fi

host=`hostname`

cat <<EOF
/* This file is automatically generated with "make include/build_env.h". DO NOT EDIT */

#ifndef _BUILD_ENV_H
#define _BUILD_ENV_H

#define BUILD_ENV_UNAME "${uname}"
#define BUILD_ENV_DATE "${date}"
#define BUILD_ENV_SRCDIR "${srcdir}"
#define BUILD_ENV_BUILDDIR "${builddir}"
#define BUILD_ENV_USER "${whoami}"
#define BUILD_ENV_HOST "${host}"
#define BUILD_ENV_COMPILER "${compiler}"

/* added by HM3 */
#define SBINDIR "$SBINDIR"
#define BINDIR "$BINDIR"
#define CONFIGFILE "$CONFIGFILE"
#define LOGFILEBASE "$LOGFILEBASE"
#define LMHOSTSFILE "$LMHOSTSFILE"
#define LIBDIR "$LIBDIR"
#define LOCKDIR "$LOCKDIR"
#define PIDDIR "$PIDDIR"
#define SMB_PASSWD_FILE "$SMB_PASSWD_FILE"
#define PRIVATE_DIR "$PRIVATE_DIR"

#endif /* _BUILD_ENV_H */
EOF