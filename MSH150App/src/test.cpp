#include <string>
#include <iostream>
#include <list>
#include "LOTUtils.h"

int main(int argc, char* argv[])
{
	std::string version, s;
	try {
	    LOTUtils::version(version);
        std::cerr << "LOT version " << version << std::endl;	
	    LOTUtils::build_system_model("C:/Instrument/Apps/EPICS/support/MSH150/master/data/ibex_test_config.xml");
		std::list<std::string> comms_list, hardware_list, mono_items;
	    LOTUtils::get_comms_list(comms_list);
		for(const auto& c : comms_list)
		{
			std::cerr << "comms: " << "\""<< c << "\"" << std::endl;
	        LOTUtils::set(c, LOTTokens::SimulationMode, 0, 1);
		}
	    LOTUtils::initialise();
	    LOTUtils::get_hardware_list(hardware_list);
		for(const auto& h : hardware_list)
		{
			int hardware_type;
			std::string s;
			double d;
			LOTUtils::get_hardware_type(h, hardware_type);
//	        LOTUtils::get_str(h, LOTTokens::lotProductName, 0, s);
//			std::cerr << s << std::endl;
			switch(hardware_type)
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
			        std::cerr << "lotFilterWheel" << std::endl;
				    break;
                case lotMono:
			        std::cerr << "lotMono " << h << std::endl;
	                LOTUtils::get_mono_items(h, mono_items);
		            for(const auto& m : mono_items)
					{
						std::cerr << "mono " << m << std::endl;
					}
					LOTUtils::get(h, LOTTokens::MonochromatorCurrentWL, 0, d);
					std::cerr << d << std::endl;
					break;
                case lotUnknown:
			        std::cerr << "lotUnknown" << std::endl;
				    break;
				default:
			        std::cerr << "error" << std::endl;
					break;
			}
		}
		LOTUtils::close();
	}
	catch(const std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
	}
    return 0;
}