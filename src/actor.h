//----------------------------------------------------------------------------

#ifndef _ACTOR_H_
#define _ACTOR_H_

namespace Fitd {

struct ZVStruct;
struct sceZoneStruct;
struct roomDataStruct;

int processActor1Sub1(int actorIdx, ZVStruct *zvPtr);
void processActor1();

sceZoneStruct *processActor2Sub(int x, int y, int z, roomDataStruct *pRoomData);
void processActor2();


} // end of namespace Fitd

#endif
