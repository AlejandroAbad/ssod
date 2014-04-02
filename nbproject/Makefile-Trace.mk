#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Trace
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/datastructs/error.o \
	${OBJECTDIR}/datastructs/iface.o \
	${OBJECTDIR}/datastructs/listener.o \
	${OBJECTDIR}/datastructs/realm.o \
	${OBJECTDIR}/datastructs/ssod.o \
	${OBJECTDIR}/datastructs/thread.o \
	${OBJECTDIR}/debug.o \
	${OBJECTDIR}/inih/ini.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/watchers/network.o \
	${OBJECTDIR}/wpa_ctrl/wpa_ctrl.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lfnasso -lpthread -lcom_err

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ssod

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ssod: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ssod ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/datastructs/error.o: datastructs/error.c 
	${MKDIR} -p ${OBJECTDIR}/datastructs
	${RM} "$@.d"
	$(COMPILE.c) -g -DTRACE_MODE -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/datastructs/error.o datastructs/error.c

${OBJECTDIR}/datastructs/iface.o: datastructs/iface.c 
	${MKDIR} -p ${OBJECTDIR}/datastructs
	${RM} "$@.d"
	$(COMPILE.c) -g -DTRACE_MODE -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/datastructs/iface.o datastructs/iface.c

${OBJECTDIR}/datastructs/listener.o: datastructs/listener.c 
	${MKDIR} -p ${OBJECTDIR}/datastructs
	${RM} "$@.d"
	$(COMPILE.c) -g -DTRACE_MODE -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/datastructs/listener.o datastructs/listener.c

${OBJECTDIR}/datastructs/realm.o: datastructs/realm.c 
	${MKDIR} -p ${OBJECTDIR}/datastructs
	${RM} "$@.d"
	$(COMPILE.c) -g -DTRACE_MODE -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/datastructs/realm.o datastructs/realm.c

${OBJECTDIR}/datastructs/ssod.o: datastructs/ssod.c 
	${MKDIR} -p ${OBJECTDIR}/datastructs
	${RM} "$@.d"
	$(COMPILE.c) -g -DTRACE_MODE -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/datastructs/ssod.o datastructs/ssod.c

${OBJECTDIR}/datastructs/thread.o: datastructs/thread.c 
	${MKDIR} -p ${OBJECTDIR}/datastructs
	${RM} "$@.d"
	$(COMPILE.c) -g -DTRACE_MODE -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/datastructs/thread.o datastructs/thread.c

${OBJECTDIR}/debug.o: debug.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -DTRACE_MODE -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/debug.o debug.c

${OBJECTDIR}/inih/ini.o: inih/ini.c 
	${MKDIR} -p ${OBJECTDIR}/inih
	${RM} "$@.d"
	$(COMPILE.c) -g -DTRACE_MODE -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/inih/ini.o inih/ini.c

${OBJECTDIR}/main.o: main.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -g -DTRACE_MODE -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.c

${OBJECTDIR}/watchers/network.o: watchers/network.c 
	${MKDIR} -p ${OBJECTDIR}/watchers
	${RM} "$@.d"
	$(COMPILE.c) -g -DTRACE_MODE -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/watchers/network.o watchers/network.c

${OBJECTDIR}/wpa_ctrl/wpa_ctrl.o: wpa_ctrl/wpa_ctrl.c 
	${MKDIR} -p ${OBJECTDIR}/wpa_ctrl
	${RM} "$@.d"
	$(COMPILE.c) -g -DTRACE_MODE -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/wpa_ctrl/wpa_ctrl.o wpa_ctrl/wpa_ctrl.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ssod

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
