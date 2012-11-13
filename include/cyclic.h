#include "ethercattype.h"
#include "ethercatmain.h"
#include "ethercatbase.h"
#include "ethercatcoe.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>




uint8 PrepareCycPacket(uint8 FrameIndex);
void SendCycFrame (void);
int ec_receive_cyclic_packet( int timeout);
