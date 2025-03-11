#ifndef REALTIME
#define REALTIME

#define REALTIME_BUFFER_LENGHT	8

enum {BACKWARD_TRANSPORT, FORWARD_TRANSPORT, PLAY_TRANSPORT, STOP_TRANSPORT, REC_TRANSPORT, PAUSE_TRANSPORT};

///////////////////////////////////////////////////////////
// Function Prototypes
///////////////////////////////////////////////////////////
void CheckRealtimeMidi(void);
void UpdateInternalRealtime(uchar next_state);

#endif