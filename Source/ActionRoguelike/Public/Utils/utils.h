#pragma once
#ifndef _UTILS_H_
#define _UTILS_H_

#include "CoreMinimal.h"

namespace AwUtils
{
#define cast(_Ty,toObj) Cast<_Ty>(toObj)

#define GapFrame(deltaT, TimeToGap) if(deltaT < TimeToGap) return;

#define Radian2Angle(r) r * 180 / PI

#define TraceClimbaleSur(Comp) AwMoveSystem::TraceClimbaleSurface(Comp, Comp->ClimbReachDistance, Comp->ClimbDectHeight)
	
}


#endif
