#include <string>
#include <sstream>
#include <exception>
#include "LOTHw.h"
#include "LOTUtils.h"

#define LOT_CHECK(__val) \
{ \
	if (__val != LOT_OK) \
	{ \
		std::stringstream message; \
        int errCode, address; \
	    char id[128]; \
	    id[sizeof(id) - 1] = '\0'; \
	    if (LOT_get_last_error(&errCode, id, &address) == LOT_OK) \
		{ \
	        id[sizeof(id) - 1] = '\0'; \
			message << "LOT: " << id << " " << errCode << " " << address; \
		} \
		else \
		{ \
			message << "unknown error"; \
		} \
		throw std::runtime_error(message.str()); \
	} \
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

void LOTUtils::get_comms_list(std::string& list)
{
	char buffer[128];
	buffer[sizeof(buffer) - 1] = '\0';
	LOT_CHECK(LOT_get_comms_list(buffer));
	buffer[sizeof(buffer) - 1] = '\0';
	list = buffer;
}

void LOTUtils::initialise()
{
	LOT_CHECK(LOT_initialise());
}

void LOTUtils::get_hardware_list(std::string& list)
{
	char buffer[128];
	buffer[sizeof(buffer) - 1] = '\0';
	LOT_CHECK(LOT_get_hardware_list(buffer));
	buffer[sizeof(buffer) - 1] = '\0';
	list = buffer;
}

void LOTUtils::get_hardware_type(const std::string& id, int& HardwareType)
{
	LOT_CHECK(LOT_get_hardware_type(id.c_str(), &HardwareType));
}

void LOTUtils::get_mono_items(const std::string& monoID, std::string& ItemIDs)
{
	char buffer[128];
	buffer[sizeof(buffer) - 1] = '\0';
	LOT_CHECK(LOT_get_mono_items(monoID.c_str(), buffer));
	buffer[sizeof(buffer) - 1] = '\0';
	ItemIDs = buffer;
}

void LOTUtils::get_str(const std::string& id, int token, int _index, std::string& s)
{
	char buffer[128];
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
	char buffer[128];
	buffer[sizeof(buffer) - 1] = '\0';
	LOT_CHECK(LOT_version(buffer));
	buffer[sizeof(buffer) - 1] = '\0';
	version = buffer;
}
