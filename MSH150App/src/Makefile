TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================

#==================================================
# build a support library

LOTDIR = $(ICPBINARYDIR)/LOT_monochromator

LIBRARY_IOC += MSH150

# xxxRecord.h will be created from xxxRecord.dbd
#DBDINC += xxxRecord
# install MSH150.dbd into <top>/dbd
DBD += MSH150.dbd

MSH150_SRCS += LOTUtils.cpp LOTPortDriver.cpp
MSH150_LIBS += asyn
MSH150_LIBS += $(EPICS_BASE_IOC_LIBS)

MSH150_SYS_LIBS_WIN32 += $(TOP)/implib/LotHW64

BIN_INSTALLS_WIN32 += $(LOTDIR)/dlls/LotHW64.dll
DATA += ibex_test_config.xml

#===========================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

ibex_test_config.xml : $(LOTDIR)/Configurations/ibex_test_config.xml
	$(CP) $< $@
