
struct LOTUtils
{
    static void build_system_model(const std::string& xmlfile);

    static void close();

    static void get(const std::string& id, int token, int _index, double &value);
	
	static void get_comms_list(std::string& list);

    static void initialise();
	
	static void get_hardware_list(std::string& list);
	 
	static void get_hardware_type(const std::string& id, int& HardwareType);
			 
	static void get_mono_items(const std::string& monoID, std::string& ItemIDs);
				 
	static void get_str(const std::string& id, int token, int _index, std::string& s);

    static void recalibrate(const std::string& id, int _index, double Wavelength, double CorrectWavelength, int& OldZord, int& NewZord);
				 
    static void save_setup();

    static void select_wavelength(double wl);
	
    static void set(const std::string& id, int token, int _index, double value);

    static void set_str(const std::string& id, int token, int _index, const std::string& s);
	
    static void set_c_group(int group);

	static void version(std::string& version);

};
