/*************************************************************************\ 
* Copyright (c) 2013 Science and Technology Facilities Council (STFC), GB. 
* All rights reverved. 
* This file is distributed subject to a Software License Agreement found 
* in the file LICENSE.txt that is included with this distribution. 
\*************************************************************************/ 

#ifndef LOTPORTDRIVER_H
#define LOTPORTDRIVER_H

class LOTParam;
 
/// EPICS Asyn port driver class. 
class LOTPortDriver : public asynPortDriver 
{
public:
	LOTPortDriver(const char *portName, const char* config_file, const char* subst_file, bool simulate);

	// These are the methods that we override from asynPortDriver
	virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
	virtual asynStatus writeFloat64(asynUser *pasynUser, epicsFloat64 value);
	virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual);
	virtual asynStatus readFloat64(asynUser *pasynUser, epicsFloat64 *value);
	virtual asynStatus readOctet(asynUser *pasynUser, char *value, size_t maxChars, size_t *nActual, int *eomReason);
	virtual void report(FILE* fp, int details);
	static void epicsExitFunc(void* arg);
	void updateValues();

private:

	static void pollerTask(void* arg);
    LOTParam* addRealParam(const std::string& id, int token, bool writable = false, int index = -1);
    LOTParam* addStringParam(const std::string& id, int token, bool writable = false, int index = -1);
    void addHardwareParams(const std::string& item);

    int P_configFile; // string
	int P_saveSetup; // int
	int P_selectWavelength; // double
	int P_version; // string
	int P_c_group; // int
	
	static bool m_shutdown_requested;
	
	std::map<int,LOTParam*> m_lot_params;
    std::fstream m_subst_file;
};

#define P_configFileString 				"CONFIGFILE"
#define P_saveSetupString 				"SAVESETUP"
#define P_selectWavelengthString 		"SELECTWAVELENGTH"
#define P_versionString 				"VERSION"
#define P_c_groupString 				"GROUP"

#endif /* LOTPORTDRIVER_H */
