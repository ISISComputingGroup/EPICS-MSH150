#include <string>
#include <sstream>
#include <list>
#include <exception>
#include <iostream>
#include <boost/algorithm/string.hpp>

#include "LOTHw.h"

#include <epicsExport.h>

#include "LOTUtils.h"

#define BUFFER_SIZE 256

static const char* lookupError(int code)
{
	switch (code)
	{
	case LOT_OK:
		return "LOT_OK";
	case LOT_Error:
		return "LOT_Error";

		// System Related
	case LOT_File_Not_Found:
		return "LOT_File_Not_Found";
	case LOT_Invalid_System_Model:
		return "LOT_Invalid_System_Model";
	case LOT_Invalid_Group:
		return "LOT_Invalid_Group";
	case LOT_Invalid_Slave:
		return "LOT_Invalid_Slave";
	case LOT_No_Groups_Exist:
		return "LOT_No_Groups_Exist";
	case LOT_Duplicate_ID:
		return "LOT_Duplicate_ID";
	case LOT_System_Not_Initialised:
		return "LOT_System_Not_Initialised";

		// Get/Set Attribute Related
	case LOT_Invalid_ID:
		return "LOT_Invalid_ID";
	case LOT_Invalid_Token:
		return "LOT_Invalid_Token";
	case LOT_Invalid_Attribute:
		return "LOT_Invalid_Attribute";
	case LOT_Invalid_Index:
		return "LOT_Invalid_Index";
	case LOT_Invalid_Setup:
		return "LOT_Invalid_Setup";
	case LOT_Invalid_Value:
		return "LOT_Invalid_Value";

		// Comms Related
	case LOT_Error_USB_Disconnected:
		return "LOT_Error_USB_Disconnected";
	case LOT_USB_Comms_Not_Initialised:
		return "LOT_USB_Comms_Not_Initialised";
	case LOT_Invalid_SMDType:
		return "LOT_Invalid_SMDType";
	case LOT_Invalid_Comms:
		return "LOT_Invalid_Comms";
	case LOT_Error_IMAC_timeout:
		return "LOT_Error_IMAC_timeout";
	case LOT_USB_Comms_Error:
		return "LOT_USB_Comms_Error";

		// Hardware Related
	case LOT_Invalid_Turret_Wavelength:
		return "LOT_Invalid_Turret_Wavelength";
	case LOT_Invalid_Filter_Pos:
		return "LOT_Invalid_Filter_Pos";
	case LOT_Invalid_Slit_Width:
		return "LOT_Invalid_Slit_Width";
	case LOT_Invalid_Hardware_Type:
		return "LOT_Invalid_Hardware_Type";
	case LOT_Invalid_Slit_Type:
		return "LOT_Invalid_Slit_Type";
	case LOT_Invalid_Grating:
		return "LOT_Invalid_Grating";
	case LOT_Invalid_Turret:
		return "LOT_Invalid_Turret";
	case LOT_Invalid_SAM_State:
		return "LOT_Invalid_SAM_State";

	case LOT_Error_Undefined:
		return "LOT_Error_Undefined";
	default:
		return "LOT_UNKNOWN";
	}
} 

#define LOT_CHECK(__val) \
{ \
    if ( (__val) != LOT_OK ) \
	{ \
		std::stringstream message; \
        int errCode, address; \
	    char myid[BUFFER_SIZE]; \
	    myid[sizeof(myid) - 1] = '\0'; \
		message << "LOT: " << __FUNCTION__ << "[\"" << __FILE__ << "\"/" << __LINE__ << "] "; \
	    if (LOT_get_last_error(&errCode, myid, &address) == LOT_OK) \
		{ \
	        myid[sizeof(myid) - 1] = '\0'; \
			message << "id='" << myid << "' error=" << errCode << " (" << lookupError(errCode) << ") address=" << address; \
		} \
		else \
		{ \
			message << "unknown error"; \
		} \
        std::cerr << message.str() << std::endl; \
		throw std::runtime_error(message.str()); \
	} \
}

static void split_string(const std::string& s, std::list<std::string>& splits)
{
	boost::split(splits, s, boost::is_any_of(","), boost::token_compress_on);
	splits.remove(""); // remove blanks	
}

void LOTUtils::build_system_model(const std::string& xmlfile)
{
	LOT_CHECK(LOT_build_system_model(xmlfile.c_str()));
}

void LOTUtils::close()
{
	LOT_CHECK(LOT_close());
}

void LOTUtils::get(const std::string& id, int token, int _index, double &value)
{
	LOT_CHECK(LOT_get(id.c_str(), token, _index, &value));
}

void LOTUtils::get_comms_list(std::list<std::string>& list)
{
	char buffer[BUFFER_SIZE];
	buffer[sizeof(buffer) - 1] = '\0';
	LOT_CHECK(LOT_get_comms_list(buffer));
	buffer[sizeof(buffer) - 1] = '\0';
	split_string(buffer, list);
}

void LOTUtils::initialise()
{
	LOT_CHECK(LOT_initialise());
}

void LOTUtils::get_hardware_list(std::list<std::string>& list)
{
	char buffer[BUFFER_SIZE];
	buffer[sizeof(buffer) - 1] = '\0';
	LOT_CHECK(LOT_get_hardware_list(buffer));
	buffer[sizeof(buffer) - 1] = '\0';
	split_string(buffer, list);
}

void LOTUtils::get_hardware_type(const std::string& id, int& HardwareType)
{
	LOT_CHECK(LOT_get_hardware_type(id.c_str(), &HardwareType));
}

void LOTUtils::get_mono_items(const std::string& monoID, std::list<std::string>& ItemIDs)
{
	char buffer[BUFFER_SIZE];
	buffer[sizeof(buffer) - 1] = '\0';
	LOT_CHECK(LOT_get_mono_items(monoID.c_str(), buffer));
	buffer[sizeof(buffer) - 1] = '\0';
	split_string(buffer, ItemIDs);
}

void LOTUtils::get_str(const std::string& id, int token, int _index, std::string& s)
{
	char buffer[BUFFER_SIZE];
	buffer[sizeof(buffer) - 1] = '\0';
	LOT_CHECK(LOT_get_str(id.c_str(), token, _index, buffer));
	buffer[sizeof(buffer) - 1] = '\0';
	s = buffer;
}

void LOTUtils::recalibrate(const std::string& id, int _index, double Wavelength, double CorrectWavelength, int& OldZord, int& NewZord)
{
	LOT_CHECK(LOT_recalibrate(id.c_str(), _index, Wavelength, CorrectWavelength, &OldZord, &NewZord));
}

void LOTUtils::save_setup()
{
	LOT_CHECK(LOT_save_setup());
}

void LOTUtils::select_wavelength(double wl)
{
	LOT_CHECK(LOT_select_wavelength(wl));
}

void LOTUtils::set(const std::string& id, int token, int _index, double value)
{
	LOT_CHECK(LOT_set(id.c_str(), token, _index, &value));
}

void LOTUtils::set_str(const std::string& id, int token, int _index, const std::string& s)
{
	LOT_CHECK(LOT_set_str(id.c_str(), token, _index, s.c_str()));
}

void LOTUtils::set_c_group(int group)
{
	LOT_CHECK(LOT_set_c_group(group));
}

void LOTUtils::version(std::string& version)
{
	char buffer[BUFFER_SIZE];
	buffer[sizeof(buffer) - 1] = '\0';
	LOT_CHECK(LOT_version(buffer));
	buffer[sizeof(buffer) - 1] = '\0';
	version = buffer;
}
