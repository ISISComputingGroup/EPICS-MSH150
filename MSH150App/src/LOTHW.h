#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllimport) int LOT_build_system_model(const char* xmlfile);

__declspec(dllimport) int LOT_close();

__declspec(dllimport) int LOT_get(const char* id, int token, int _index, double *value);

/* list needs to be pre-allocated and null terminated */
__declspec(dllimport) int LOT_get_comms_list(char* list);
	 
/* list needs to be pre-allocated and null terminated */
__declspec(dllimport) int LOT_get_hardware_list(char* list);
			 
__declspec(dllimport) int LOT_get_hardware_type(const char* id, int *HardwareType);
				 
/* ID needs to be pre-allocated and null terminated */
__declspec(dllimport) int LOT_get_last_error(int *ErrorCode, char* ID, int *Address);

/* ItemIDs needs to be pre-allocated and null terminated */
__declspec(dllimport) int LOT_get_mono_items(const char* monoID, char* ItemIDs);

/* s needs to be pre-allocated and null terminated */
__declspec(dllimport) int LOT_get_str(const char* id, int token, int _index, char* s);

__declspec(dllimport) int LOT_initialise();

__declspec(dllimport) int LOT_recalibrate(const char* ID, int _index, double Wavelength, double CorrectWavelength, int *OldZord, int *NewZord);
				 
__declspec(dllimport) int LOT_save_setup();

__declspec(dllimport) int LOT_select_wavelength(double wl);

__declspec(dllimport) int LOT_set(const char* id, int token, int _index, const double *value);

__declspec(dllimport) int LOT_set_str(const char* id, int token, int _index, const char* s);

__declspec(dllimport) int LOT_set_c_group(int group);

/* Version needs to be pre-allocated and null terminated */
__declspec(dllimport) int LOT_version(char* Version);


//-----------------------------------------------------------------------------
// Monochromator attributes
//-----------------------------------------------------------------------------

typedef enum MonochromatorAttributes {
  MonochromatorScanDirection = 10,
  MonochromatorCurrentWL = 11,
  MonochromatorCurrentGrating = 12,
  MonochromatorInitialise = 13,
  MonochromatorModeSwitchNum = 14,
  MonochromatorModeSwitchState = 15,
  MonochromatorCanModeSwitch = 16,
  MonochromatorAutoSelectWavelength = 17,
  MonochromatorZordSwitchSAM = 18,
  MonochromatorNumTurrets = 19,
  MonochromatorCosAlpha = 21,
  
  TurretNumGratings = 20,

  GratingDensity = 30,
  GratingZord = 31,
  GratingAlpha = 32,
  GratingSwitchWL = 33,
  GratingBlaze = 34,

//-----------------------------------------------------------------------------
// Filter wheel attributes
//-----------------------------------------------------------------------------
  FWheelFilter = 100,
  FWheelPositions = 101,
  FWheelCurrentPosition = 102,

//-----------------------------------------------------------------------------
// SAM attributes
//-----------------------------------------------------------------------------
  SAMInitialState = 300,
  SAMSwitchWL = 301,
  SAMState = 302,
  SAMCurrentState = 303,
  SAMDeflectName = 304,
  SAMNoDeflectName = 305,

//-----------------------------------------------------------------------------
// MVSS attributes
//-----------------------------------------------------------------------------
  MVSSSwitchWL = 401,
  MVSSWidth = 402,
  MVSSCurrentWidth = 403,
  MVSSConstantBandwidth = 404,
  MVSSConstantwidth = 405,
  MVSSSlitMode = 406,
  MVSSPosition = 407,
  MVSSCurrentBandwidth = 408,  

//-----------------------------------------------------------------------------
// Comms Attributes
//-----------------------------------------------------------------------------
  SimulationMode = 500,

//-----------------------------------------------------------------------------
// Miscellaneous attributes
//-----------------------------------------------------------------------------
  lotSettleDelay = 1000,
  lotMoveWithWavelength = 1001,
  lotDescriptor = 1002,
  lotParkOffset = 1003,
  lotProductName = 1004,

//-----------------------------------------------------------------------------
// LOT Hardware Types
//-----------------------------------------------------------------------------
  lotInterface          = 10000,
  lotSAM                = 10001,
  lotSlit               = 10002,
  lotFilterWheel        = 10003,
  lotMono               = 10004,

  lotUnknown            = 10011
} MonochromatorAttributes;

typedef enum LOTErrorCodes {
  LOT_OK                                = 0,
  LOT_Error                             = -1,

  // System Related
  LOT_File_Not_Found                    = 1,
  LOT_Invalid_System_Model              = 2,
  LOT_Invalid_Group                     = 3,
  LOT_Invalid_Slave                     = 4,
  LOT_No_Groups_Exist                   = 5,
  LOT_Duplicate_ID                      = 6,
  LOT_System_Not_Initialised            = 7,

  // Get/Set Attribute Related
  LOT_Invalid_ID                        = 8,
  LOT_Invalid_Token                     = 9,
  LOT_Invalid_Attribute                 = 10,
  LOT_Invalid_Index                     = 11,
  LOT_Invalid_Setup                     = 12,
  LOT_Invalid_Value                     = 13,

  // Comms Related
  LOT_Error_USB_Disconnected            = 14,
  LOT_USB_Comms_Not_Initialised         = 15,
  LOT_Invalid_SMDType                   = 16,
  LOT_Invalid_Comms                     = 17,
  LOT_Error_IMAC_timeout                = 18,
  LOT_USB_Comms_Error                   = 19,

  // Hardware Related
  LOT_Invalid_Turret_Wavelength         = 20,
  LOT_Invalid_Filter_Pos                = 21,
  LOT_Invalid_Slit_Width                = 22,
  LOT_Invalid_Hardware_Type             = 23,
  LOT_Invalid_Slit_Type                 = 24,
  LOT_Invalid_Grating                   = 25,
  LOT_Invalid_Turret                    = 26,
  LOT_Invalid_SAM_State                 = 27,

  LOT_Error_Undefined                   = 100
} LOTErrorCodes;

#ifdef __cplusplus
}
#endif
