#include <string>
#include <iostream>
#include "LOTUtils.h"

int main(int argc, char* argv[])
{
	std::string version;
	LOTUtils::version(version);
    std::cerr << "LOT version " << version << std::endl;	
//	LOTUtils::build_system_model("something");
    return 0;
}