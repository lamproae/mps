/*  impl.c.shield
 *
 *                         SHIELDING
 *
 *  $HopeName: MMsrc!shield.c(MMdevel_protoposm_1.1) $
 *
 *  Copyright (C) 1995 Harlequin Group, all rights reserved
 *
 *  See: idea.shield, design.mps.shield.
 *
 *  Invariant: The protected memory is a subset of the shielded memory when
 *             inside the shield, and the same set when outside.
 *
 *  HACKY HACKY JOY JOY!
 */

#include "std.h"
#include "shield.h"
#include "shieldst.h"
#include "space.h"
#include "prot.h"
#include "poolar.h"


#ifdef DEBUG_SIGN
static SigStruct ShieldSigStruct;
#endif


Bool ShieldIsValid(Shield shield, ValidationType validParam)
{
  AVER(shield != NULL);
#ifdef DEBUG_SIGN
  AVER(ISVALIDNESTED(Sig, &ShieldSigStruct));
  AVER(shield->sig == &ShieldSigStruct);
#endif /* DEBUG_SIGN */
/*  AVER(ISVALIDNESTED(Space, shield->space)); @@@@ */
/*  AVER(ISVALIDNESTED(Bool, shield->inside)); */
  return TRUE;
}


void ShieldInit(Shield shield, Space space)
{
  AVER(shield != NULL);
  AVER(ISVALID(Space, space));

  shield->inside = FALSE;
  shield->space = space;

#ifdef DEBUG_SIGN
  SigInit(&ShieldSigStruct, "Shield");
  shield->sig = &ShieldSigStruct;
#endif /* DEBUG_SIGN */

  AVER(ISVALID(Shield, shield));
}


void ShieldFinish(Shield shield)
{
  AVER(!shield->inside);

#ifdef DEBUG_SIGN
  shield->sig = SigInvalid;
#endif
}


static void protect(Arena arena, Addr base, Addr limit, ProtMode mode)
{
  if(ArenaProtMode(arena, base) != mode) {
    ProtSet(base, limit, mode);
    ArenaSetProtMode(arena, base, mode);
  }
}


void ShieldRaise(Shield shield, Addr base, Addr limit, ProtMode mode)
{
  ProtMode shieldMode;
  Arena arena;

  AVER(ISVALID(Shield, shield));

  arena = SpaceArena(shield->space);

  shieldMode = ArenaShieldMode(arena, base);
  AVER((shieldMode & mode) == ProtNONE);
  shieldMode |= mode;
  ArenaSetShieldMode(arena, base, shieldMode);

  if(shieldMode >> 2 == 0)
    protect(arena, base, limit, shieldMode);
}

void ShieldLower(Shield shield, Addr base, Addr limit, ProtMode mode)
{
  ProtMode shieldMode;
  Arena arena;

  AVER(ISVALID(Shield, shield));

  arena = SpaceArena(shield->space);

  shieldMode = ArenaShieldMode(arena, base);
  AVER((shieldMode & mode) == mode);
  shieldMode &= ~mode;
  ArenaSetShieldMode(arena, base, shieldMode);

  if(shieldMode >> 2 == 0)
    protect(arena, base, limit, shieldMode); /* will only remove protection */
}


void ShieldEnter(Shield shield)
{
  AVER(ISVALID(Shield, shield));
  AVER(!shield->inside);

  ProtShieldEnter(shield->space);
  shield->inside = TRUE;
}

void ShieldLeave(Shield shield)
{
  AVER(ISVALID(Shield, shield));
  AVER(shield->inside);

/* .opt.lazy-cover:
  for all segs {
    protect(arena, base, ArenaShieldMode(arena, base));
  }
 */
  shield->inside = FALSE;
  ProtShieldLeave(shield->space);
}


void ShieldExpose(Shield shield, Addr base, Addr limit)
{
  ProtMode shieldMode;
  Arena arena;

  AVER(ISVALID(Shield, shield));
  AVER(shield->inside);

  arena = SpaceArena(shield->space);
  shieldMode = ArenaShieldMode(arena, base);
  shieldMode += 4;
  ArenaSetShieldMode(arena, base, shieldMode);

  protect(arena, base, limit, ProtNONE);
}

void ShieldCover(Shield shield, Addr base, Addr limit)
{
  ProtMode shieldMode;
  Arena arena;

  AVER(ISVALID(Shield, shield));
  AVER(ArenaProtMode(SpaceArena(shield->space), base) == ProtNONE);

  arena = SpaceArena(shield->space);
  shieldMode = ArenaShieldMode(arena, base);
  AVER(shieldMode >= 4);
  shieldMode -= 4;
  ArenaSetShieldMode(arena, base, shieldMode);

  if(shieldMode >> 2 == 0)
    protect(arena, base, limit, shieldMode);
}
