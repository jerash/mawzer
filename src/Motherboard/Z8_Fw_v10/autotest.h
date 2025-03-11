#ifndef AUTOTEST
#define AUTOTEST

///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void CheckBootStatus(void);
void FactoryDefault(void);
void AutoAssignAllModules(void);
void CreateStdModuleNameFromCurrentModule(void);
void SendFullStructureToModule(uchar current_control_in_module);
void GetModuleEEAdressFromIndexGroup(uchar current_control_in_module);

#endif