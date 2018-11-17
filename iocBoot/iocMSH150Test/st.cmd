#!../../bin/win32-x86/MSH150Test

## You may have to change MSH150Test to something else
## everywhere it appears in this file

< envPaths

## Register all support components
dbLoadDatabase("../../dbd/MSH150Test.dbd",0,0)
MSH150Test_registerRecordDeviceDriver(pdbbase) 

# used in generating substitutions file
epicsEnvSet("P","$(MYPVPREFIX)MSH150:")

LOTConfigure("L0", "$(TOP)/data/ibex_test_config.xml", "$(TOP)/db/LOT.substitutions", 1)

## Load record instances
dbLoadRecords("$(TOP)/db/MSH150.db","P=$(MYPVPREFIX)MSH150:")
dbLoadTemplate("$(TOP)/db/LOT.substitutions")

iocInit()

## Start any sequence programs
#seq sncMSH150Test,"user=faa59"
