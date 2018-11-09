TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# build a support library

LOTDIR = $(ICPBINARYDIR)/LOT_monochromator/dlls

LIBRARY_IOC += MSH150

PROD_IOC += MSH150Test

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
# install MSH150.dbd into <top>/dbd
DBD += MSH150.dbd

MSH150Test_SRCS += LOTUtils.cpp test.cpp

MSH150Test_LIBS += $(EPICS_BASE_IOC_LIBS)

MSH150Test_SYS_LIBS_WIN32 += $(TOP)/implib/LotHW64

BIN_INSTALLS_WIN32 += $(LOTDIR)/LotHW64.dll

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

