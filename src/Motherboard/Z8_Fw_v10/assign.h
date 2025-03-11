#ifndef ASSIGN
#define ASSIGN

///////////////////////////////////////////////////////////
// GLOBAL definitions
///////////////////////////////////////////////////////////

typedef struct {
uchar control_assign_index;				
uchar control_num_in_module_assign;
uchar current_module_assign;
} Assign ;

// state of assign_state register
enum {ASSIGN_OFF, ASSIGN_WAIT_TWEAK_CTRL, ASSIGN_ACTIVE};

///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void AssignEnter(void);
void ModuleNameAssignEnter(void);
void AssignWaitControl(void);

#endif