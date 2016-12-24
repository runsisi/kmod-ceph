#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.59])
AC_INIT([kmod-ceph], [3.10.0], [runsisi@hust.edu.cn])

KVERSION=`uname -r`
RPM_VERSION=$(echo $KVERSION | cut -d- -f1)
RPM_RELEASE=$(echo $KVERSION | cut -d- -f2- | sed 's/.el7.x86_64//')

AC_SUBST(KVERSION)
AC_SUBST(RPM_VERSION)
AC_SUBST(RPM_RELEASE)

# Checks for programs.
AC_PROG_CC

AC_CONFIG_FILES([kmod-ceph.spec])
AC_OUTPUT
