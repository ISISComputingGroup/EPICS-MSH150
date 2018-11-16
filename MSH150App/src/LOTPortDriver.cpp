/*************************************************************************\
* Copyright (c) 2013 Science and Technology Facilities Council (STFC), GB.
* All rights reverved.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE.txt that is included with this distribution.
\*************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <exception>
#include <iostream>
#include <sstream>
#include <fstream>
#include <list>
#include <map>
#include <string>

#include <boost/algorithm/string.hpp>

#include <shareLib.h>
#include <epicsTypes.h>
#include <epicsExit.h>
#include <epicsTime.h>
#include <epicsThread.h>
#include <epicsString.h>
#include <epicsTimer.h>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include <errlog.h>
#include <iocsh.h>
#include <macLib.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "asynPortDriver.h"

#include <epicsExport.h>

#include "LOTUtils.h"
#include "LOTPortDriver.h"

static const char *driverName = "LOTPortDriver"; ///< Name of driver for use in message printing 

static std::map<int, std::string> TokenToName;
static std::map<int, std::string> TokenToDBName;

static void addMapping(int token, const std::string& name, const std::string& db_name = "")
{
	TokenToName[token] = name;
	TokenToDBName[token] = (db_name.size() > 0 ? db_name : name);
}

static void setupMappings()
{
	addMapping(MonochromatorScanDirection, "MonochromatorScanDirection", "SCANDIR");
	addMapping(MonochromatorCurrentWL, "MonochromatorCurrentWL", "WL");

	addMapping(MonochromatorCurrentGrating, "MonochromatorCurrentGrating", "GRATING");
	addMapping(MonochromatorInitialise, "MonochromatorInitialise");
	addMapping(MonochromatorModeSwitchNum, "MonochromatorModeSwitchNum", "MODE:SWNUM");
	addMapping(MonochromatorModeSwitchState, "MonochromatorModeSwitchState", "MODE:SWSTATE");
	addMapping(MonochromatorCanModeSwitch, "MonochromatorCanModeSwitch", "MODE:CANSWITCH");
	addMapping(MonochromatorAutoSelectWavelength, "MonochromatorAutoSelectWavelength", "AUTOWL");
	addMapping(MonochromatorZordSwitchSAM, "MonochromatorZordSwitchSAM");
	addMapping(MonochromatorNumTurrets, "MonochromatorNumTurrets", "NUMTURRETS");
	addMapping(MonochromatorCosAlpha, "MonochromatorCosAlpha");

	addMapping(TurretNumGratings, "TurretNumGratings");

	addMapping(GratingDensity, "GratingDensity");
	addMapping(GratingZord, "GratingZord");
	addMapping(GratingAlpha, "GratingAlpha");
	addMapping(GratingSwitchWL, "GratingSwitchWL");
	addMapping(GratingBlaze, "GratingBlaze");

	//-----------------------------------------------------------------------------
	// Filter wheel attributes
	//-----------------------------------------------------------------------------
	addMapping(FWheelFilter, "FWheelFilter", "FILTER");
	addMapping(FWheelPositions, "FWheelPositions", "NUMPOS");
	addMapping(FWheelCurrentPosition, "FWheelCurrentPosition", "POS");

	//-----------------------------------------------------------------------------
	// SAM attributes
	//-----------------------------------------------------------------------------
	addMapping(SAMInitialState, "SAMInitialState");
	addMapping(SAMSwitchWL, "SAMSwitchWL");
	addMapping(SAMState, "SAMState");
	addMapping(SAMCurrentState, "SAMCurrentState");
	addMapping(SAMDeflectName, "SAMDeflectName");
	addMapping(SAMNoDeflectName, "SAMNoDeflectName");

	//-----------------------------------------------------------------------------
	// MVSS attributes
	//-----------------------------------------------------------------------------
	addMapping(MVSSSwitchWL, "MVSSSwitchWL");
	addMapping(MVSSWidth, "MVSSWidth");
	addMapping(MVSSCurrentWidth, "MVSSCurrentWidth");
	addMapping(MVSSConstantBandwidth, "MVSSConstantBandwidth");
	addMapping(MVSSConstantwidth, "MVSSConstantwidth");
	addMapping(MVSSSlitMode, "MVSSSlitMode");
	addMapping(MVSSPosition, "MVSSPosition");
	addMapping(MVSSCurrentBandwidth, "MVSSCurrentBandwidth");

	//-----------------------------------------------------------------------------
	// Comms Attributes
	//-----------------------------------------------------------------------------
	addMapping(SimulationMode, "SimulationMode", "SIM");

	//-----------------------------------------------------------------------------
	// Miscellaneous attributes
	//-----------------------------------------------------------------------------
	addMapping(lotSettleDelay, "lotSettleDelay");
	addMapping(lotMoveWithWavelength, "lotMoveWithWavelength");
	addMapping(lotDescriptor, "lotDescriptor");
	addMapping(lotParkOffset, "lotParkOffset");
	addMapping(lotProductName, "lotProductName");
}

class LOTParam
{
protected:
	std::string m_lot_id;
	int m_token;
	int m_index;
	asynPortDriver* m_driver;
	int m_asyn_id; // asyn parameter id
	std::string m_asyn_name;
public:
	virtual void read() = 0;
	virtual void write() = 0;
	int id() const { return m_asyn_id; }
	const std::string& name() const { return m_asyn_name; }
	LOTParam(const std::string& lot_id, int token, int index, asynPortDriver* driver) :
		m_lot_id(lot_id), m_token(token), m_index(index), m_driver(driver), m_asyn_id(-1), m_asyn_name("") 
	{ 
		std::ostringstream oss;
		oss << lot_id << "_" << TokenToName[token];
		if (index != -1)
		{
			oss << "_" << index;
		}
		m_asyn_name = oss.str();
	}
};

class LOTStringParam : public LOTParam
{
public:
	LOTStringParam(const std::string& lot_id, int token, int index, asynPortDriver* driver) : LOTParam(lot_id, token, index, driver)
	{
		m_driver->createParam(m_asyn_name.c_str(), asynParamOctet, &m_asyn_id);
	}
	void read()
	{
		std::string s;
		LOTUtils::get_str(m_lot_id, m_token, m_index, s);
		m_driver->setStringParam(m_asyn_id, s);
	}
	void write()
	{
		std::string s;
		m_driver->getStringParam(m_asyn_id, s);
		LOTUtils::set_str(m_lot_id, m_token, m_index, s);
	}
};

class LOTRealParam : public LOTParam
{
public:
	LOTRealParam(const std::string& lot_id, int token, int index, asynPortDriver* driver) : LOTParam(lot_id, token, index, driver)
	{
		m_driver->createParam(m_asyn_name.c_str(), asynParamFloat64, &m_asyn_id);
	}
	void read()
	{
		double d;
		LOTUtils::get(m_lot_id, m_token, m_index, d);
		m_driver->setDoubleParam(m_asyn_id, d);
	}
	void write()
	{
		double d;
		m_driver->getDoubleParam(m_asyn_id, &d);
		LOTUtils::set(m_lot_id, m_token, m_index, d);
	}
};

asynStatus LOTPortDriver::writeFloat64(asynUser *pasynUser, epicsFloat64 value)
{
	static const char* functionName = "writeFloat64";
	int function = pasynUser->reason;
	asynStatus status = asynSuccess;
	const char *paramName = NULL;
	getParamName(function, &paramName);
	try
	{
		if (function == P_selectWavelength)
		{
			LOTUtils::select_wavelength(value);
		}
		else if (m_lot_params.find(function) != m_lot_params.end())
		{
			setDoubleParam(function, value);
			m_lot_params[function]->write();
		}
		asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
			"%s:%s: function=%d, name=%s, value=%f\n",
			driverName, functionName, function, paramName, value);
		return asynPortDriver::writeFloat64(pasynUser, value);
	}
	catch (const std::exception& ex)
	{
		epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
			"%s:%s: status=%d, function=%d, name=%s, value=%f, error=%s",
			driverName, functionName, status, function, paramName, value, ex.what());
		return asynError;
	}
}

asynStatus LOTPortDriver::readFloat64(asynUser *pasynUser, epicsFloat64 *value)
{
	static const char* functionName = "readFloat64";
	int function = pasynUser->reason;
	const char *paramName = NULL;
	getParamName(function, &paramName);
	try
	{
		if (m_lot_params.find(function) != m_lot_params.end())
		{
			m_lot_params[function]->read();
		}
		asynStatus status = asynPortDriver::readFloat64(pasynUser, value);
		asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
			"%s:%s: function=%d, name=%s, value=%f\n",
			driverName, functionName, function, paramName, *value);
		return status;
	}
	catch (const std::exception& ex)
	{
			epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
				"%s:%s: function=%d, name=%s, error=%s, value=%f",
				driverName, functionName, function, paramName, ex.what(), value);
			return asynError;
	}
}

asynStatus LOTPortDriver::readOctet(asynUser *pasynUser, char *value, size_t maxChars, size_t *nActual, int *eomReason)
{
	static const char *functionName = "readOctet";
	int function = pasynUser->reason;
	const char *paramName = NULL;
	getParamName(function, &paramName);

	try
	{
		if (m_lot_params.find(function) != m_lot_params.end())
		{
			m_lot_params[function]->read();
		}
		asynStatus status = asynPortDriver::readOctet(pasynUser, value, maxChars, nActual, eomReason);
		asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
			"%s:%s: function=%d, name=%s, value=%s\n",
			driverName, functionName, function, paramName, *value);
		return status;
	}
	catch (const std::exception& ex)
	{
			epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
				"%s:%s: function=%d, name=%s, error=%s",
				driverName, functionName, function, paramName, ex.what());
		*nActual = 0;
		if (eomReason) { *eomReason = ASYN_EOM_END; }
		value[0] = '\0';
		return asynError;
	}
}

asynStatus LOTPortDriver::writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual)
{
	int function = pasynUser->reason;
	asynStatus status = asynSuccess;
	const char *paramName = NULL;
	getParamName(function, &paramName);
	const char* functionName = "writeOctet";
	std::string value_s(value, maxChars);

	try
	{
		if (m_lot_params.find(function) != m_lot_params.end())
		{
			setStringParam(function, value_s);
			m_lot_params[function]->write();
		}
		asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
			"%s:%s: function=%d, name=%s, value=%s\n",
			driverName, functionName, function, paramName, value_s.c_str());
		return asynPortDriver::writeOctet(pasynUser, value, maxChars, nActual);
	}
	catch (const std::exception& ex)
	{
		epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
			"%s:%s: status=%d, function=%d, name=%s, value=%s, error=%s",
			driverName, functionName, status, function, paramName, value_s.c_str(), ex.what());
		*nActual = 0;
		return asynError;
	}
}

asynStatus LOTPortDriver::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
	static const char* functionName = "writeInt32";
	int function = pasynUser->reason;
	asynStatus status = asynSuccess;
	const char *paramName = NULL;
	getParamName(function, &paramName);
	try
	{
		if (function == P_saveSetup)
		{
			LOTUtils::save_setup();
		}
		else if (function == P_c_group)
		{
			LOTUtils::set_c_group(value);
		}
		asynPrint(pasynUser, ASYN_TRACEIO_DRIVER,
			"%s:%s: function=%d, name=%s, value=%d\n",
			driverName, functionName, function, paramName, value);
		return asynPortDriver::writeInt32(pasynUser, value);
	}
	catch (const std::exception& ex)
	{
		epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
			"%s:%s: status=%d, function=%d, name=%s, value=%d, error=%s",
			driverName, functionName, status, function, paramName, value, ex.what());
		return asynError;
	}
}

/// EPICS driver report function for iocsh dbior command
void LOTPortDriver::report(FILE* fp, int details)
{
	asynPortDriver::report(fp, details);
}

LOTParam* LOTPortDriver::addRealParam(const std::string& id, int token, bool writable, int index)
{
	LOTParam* lp = new LOTRealParam(id, token, index, this);
	char ind_str[10];
	sprintf(ind_str, "%d", index);
	int asyn_id = lp->id();
	m_lot_params[asyn_id] = lp;
	m_subst_file  << "file \"${MSH150}/db/LOT_real.template\" {\n";			
	m_subst_file  << "    { P=\"" << macEnvExpand("$(P=)") << "\",R=\"" << boost::to_upper_copy<std::string>(id) << ":" << TokenToDBName[token] << (index != -1 ? ind_str : "") << 
	         "\",PORT=\"" << portName << "\"" << ",PARAM=\"" << lp->name()  <<"\",DESC=\"" << TokenToName[token].substr(0,39) <<
			 "\",SET=\"" << (writable ? "" : "#") << "\" }\n";
	m_subst_file  << "}\n\n";
	return lp;
}

LOTParam* LOTPortDriver::addStringParam(const std::string& id, int token, bool writable, int index)
{
	LOTParam* lp = new LOTStringParam(id, token, index, this);
	char ind_str[10];
	sprintf(ind_str, "%d", index);
	int asyn_id = lp->id();
	m_lot_params[asyn_id] = lp;

	m_subst_file  << "file \"${MSH150}/db/LOT_string.template\" {\n";			
	m_subst_file  << "    { P=\"" << macEnvExpand("$(P=)") << "\",R=\"" << boost::to_upper_copy<std::string>(id) << ":" << TokenToDBName[token] << (index != -1 ? ind_str : "") << 
	         "\",PORT=\"" << portName << "\"" << ",PARAM=\"" << lp->name()  <<"\",DESC=\"" << TokenToName[token].substr(0,39) << 
			 "\",SET=\"" << (writable ? "" : "#") << "\" }\n";
	m_subst_file  << "}\n\n";
	return lp;
}

void LOTPortDriver::addHardwareParams(const std::string& item)
{
	int hardware_type;
	std::string s;
	std::list<std::string> mono_items;
	double d;
	LOTUtils::get_hardware_type(item, hardware_type);
	switch (hardware_type)
	{
	case lotInterface:
		std::cerr << "lotInterface" << std::endl;
		break;
	case lotSAM:
		std::cerr << "lotSAM" << std::endl;
		break;
	case lotSlit:
		std::cerr << "lotSlit" << std::endl;
		break;
	case lotFilterWheel:
		std::cerr << "lotFilterWheel " << item << std::endl;
		addRealParam(item, LOTTokens::FWheelPositions);
		LOTUtils::get(item, LOTTokens::FWheelPositions, 0, d);
		for (int i = 1; i <= d; ++i)
		{
			addRealParam(item, LOTTokens::FWheelFilter, false, i);
		}
		addRealParam(item, LOTTokens::FWheelCurrentPosition);
		addRealParam(item, LOTTokens::lotMoveWithWavelength);
		addStringParam(item, LOTTokens::lotDescriptor);
		break;
	case lotMono:
		std::cerr << "lotMono " << item << std::endl;
		addRealParam(item, LOTTokens::MonochromatorCurrentWL);
		addRealParam(item, LOTTokens::MonochromatorCurrentGrating);
		addRealParam(item, LOTTokens::MonochromatorModeSwitchNum);
		addRealParam(item, LOTTokens::MonochromatorModeSwitchState);
		addRealParam(item, LOTTokens::MonochromatorCanModeSwitch);
		addRealParam(item, LOTTokens::MonochromatorAutoSelectWavelength); 
		addRealParam(item, LOTTokens::MonochromatorNumTurrets);
		addStringParam(item, LOTTokens::lotDescriptor);
		LOTUtils::get_mono_items(item, mono_items);
		for (const auto& m : mono_items)
		{
			std::cerr << "mono " << item << " with " << m << std::endl;
			addHardwareParams(m);
		}
		break;
	case lotUnknown:
		std::cerr << "lotUnknown" << std::endl;
		break;
	default:
		std::cerr << "error" << std::endl;
		break;
	}
}


/// Constructor for the #NetShrVarDriver class.
/// Calls constructor for the asynPortDriver base class and sets up driver parameters.
///
/// @param[in] netvarint  interface pointer created by NetShrVarConfigure()
/// @param[in] poll_ms  @copydoc initArg0
/// @param[in] portName @copydoc initArg3
LOTPortDriver::LOTPortDriver(const char *portName, const char* config_file, const char* subst_file, bool simulate)
	: asynPortDriver(portName,
		0, /* maxAddr */
		asynInt32Mask | asynFloat64Mask | asynOctetMask | asynDrvUserMask, /* Interface mask */
		asynInt32Mask | asynFloat64Mask | asynOctetMask,  /* Interrupt mask */
		ASYN_CANBLOCK, /* asynFlags.  This driver can block but it is not multi-device */
		1, /* Autoconnect */
		0, /* Default priority */
		0)	/* Default stack size*/
{
	const char *functionName = "LOTPortDriver";

	setupMappings();

	createParam(P_configFileString, asynParamOctet, &P_configFile);
	createParam(P_saveSetupString, asynParamInt32, &P_saveSetup);
	createParam(P_selectWavelengthString, asynParamFloat64, &P_selectWavelength);
	createParam(P_versionString, asynParamOctet, &P_version);
	createParam(P_c_groupString, asynParamInt32, &P_c_group);

	setStringParam(P_configFile, config_file);
	std::string lot_version;
	LOTUtils::version(lot_version);
	setStringParam(P_version, lot_version);

	m_subst_file.open(subst_file, std::ios::out);
	
	LOTUtils::build_system_model(config_file);
	std::list<std::string> comms_list, hardware_list, mono_items;
	LOTUtils::get_comms_list(comms_list);
	for (const auto& c : comms_list)
	{
		LOTParam* lp = addRealParam(c, LOTTokens::SimulationMode);
		if (simulate)
		{
			setDoubleParam(lp->id(), 1.0);
			lp->write();
		}
	}
	LOTUtils::initialise();
	LOTUtils::get_hardware_list(hardware_list);
	for (const auto& h : hardware_list)
	{
		addHardwareParams(h);
	}
	m_subst_file.close();

	epicsAtExit(epicsExitFunc, this);

	// Create the thread for background tasks (not used at present, could be used for I/O intr scanning) 
	if (epicsThreadCreate("LOTPortDriverTask",
		epicsThreadPriorityMedium,
		epicsThreadGetStackSize(epicsThreadStackMedium),
		(EPICSTHREADFUNC)pollerTask, this) == 0)
	{
		printf("%s:%s: epicsThreadCreate failure\n", driverName, functionName);
		return;
	}
}

bool LOTPortDriver::m_shutdown_requested = false;

void LOTPortDriver::updateValues()
{
	lock();
	for (auto it = m_lot_params.begin(); it != m_lot_params.end(); ++it)
	{
		it->second->read();
	}
	callParamCallbacks();
	unlock();
}

void LOTPortDriver::epicsExitFunc(void* arg)
{
	LOTPortDriver* driver = static_cast<LOTPortDriver*>(arg);
	if (driver == NULL)
	{
		return;
	}
	driver->m_shutdown_requested = true;
	epicsThreadSleep(0.1);
	LOTUtils::close();
}

void LOTPortDriver::pollerTask(void* arg)
{
	LOTPortDriver* driver = (LOTPortDriver*)arg;
	while (!m_shutdown_requested)
	{
		driver->updateValues();
		epicsThreadSleep(0.1);
	}
}

extern "C" {

	/// EPICS iocsh callable function to call constructor of NetShrVarInterface().
	/// The function is registered via NetShrVarRegister().
	///
	/// @param[in] portName @copydoc initArg0
	/// @param[in] configSection @copydoc initArg1
	/// @param[in] configFile @copydoc initArg2
	/// @param[in] pollPeriod @copydoc initArg3
	/// @param[in] options @copydoc initArg4
	int LOTConfigure(const char *portName, const char* configFile, const char* substFile, int simulate)
	{
		try
		{
			LOTPortDriver* pd = new LOTPortDriver(portName, configFile, substFile, (simulate != 0));
			return(asynSuccess);
		}
		catch (const std::exception& ex)
		{
			errlogSevPrintf(errlogFatal, "LOTConfigure failed: %s\n", ex.what());
			return(asynError);
		}
	}

	// EPICS iocsh shell commands 

	static const iocshArg initArg0 = { "portName", iocshArgString };			///< The name of the asyn driver port we will create
	static const iocshArg initArg1 = { "configFile", iocshArgString };		///< Path to the XML input file to load configuration information from
	static const iocshArg initArg2 = { "substFile", iocshArgString };		///< Path to the XML input file to load configuration information from
	static const iocshArg initArg3 = { "simulate", iocshArgInt };			///< poll period (ms) for BufferedReaders

	static const iocshArg * const initArgs[] = { &initArg0,
		&initArg1,
		&initArg2,
		&initArg3 };

	static const iocshFuncDef initFuncDef = { "LOTConfigure", sizeof(initArgs) / sizeof(iocshArg*), initArgs };

	static void initCallFunc(const iocshArgBuf *args)
	{
		LOTConfigure(args[0].sval, args[1].sval, args[2].sval, args[3].ival);
	}

	/// Register new commands with EPICS IOC shell
	static void LOTRegister(void)
	{
		iocshRegister(&initFuncDef, initCallFunc);
	}

	epicsExportRegistrar(LOTRegister);

}

