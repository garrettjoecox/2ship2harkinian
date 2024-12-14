#include <libultraship/bridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
#include "overlays/actors/ovl_En_Bigokuta/z_en_bigokuta.h"
#include "overlays/actors/ovl_En_Clear_Tag/z_en_clear_tag.h"
#include "overlays/actors/ovl_En_Firefly/z_en_firefly.h"
#include "overlays/actors/ovl_En_Fz/z_en_fz.h"
#include "overlays/actors/ovl_En_Neo_Reeba/z_en_neo_reeba.h"
}

#define HIT_BY_SWORD_BEAM 1
#define NOT_HIT_BY_SWORD_BEAM 0

#define CVAR_NAME "gEnhancements.Masks.FierceDeitysAnywhere"
#define CVAR CVarGetInteger(CVAR_NAME, 0)

void RegisterFierceDeityAnywhere() {
    COND_VB_SHOULD(VB_DISABLE_FD_MASK, CVAR, { *should = false; });

    COND_VB_SHOULD(VB_DAMAGE_MULTIPLIER, CVAR, {
        int index = va_arg(args, int);
        DamageTable* damageTable = va_arg(args, DamageTable*);
        f32* damage = va_arg(args, f32*);
        f32* damageMultipliers = va_arg(args, f32*);

        /*
         * 25 is the index of the sword beam damage effect.
         */
        if (index == 25) {
            *should = false;
            /*
             * If the existing damage multiplier for sword beams is 0, use a damage multiplier of 1.0 instead.
             */
            u8 defaultMultiplier = damageTable->attack[index] & 0xF;
            *damage *= damageMultipliers[defaultMultiplier == 0 ? 1 : defaultMultiplier];
        }
    });

    COND_VB_SHOULD(VB_DAMAGE_EFFECT, CVAR, {
        int index = va_arg(args, int);
        DamageTable* damageTable = va_arg(args, DamageTable*);
        u32* effect = va_arg(args, u32*);
        Actor* actor = va_arg(args, Actor*);
        /*
         * 25 is the index of the sword beam damage effect.
         */
        if (index == 25) {
            *should = false;
            /*
             * If the sword beam effect is 0, use the light arrow effect instead.
             */
            u8 defaultEffect = (damageTable->attack[index] >> 4) & 0xF;
            if (defaultEffect == 0) {
                // 13 is the index of the light arrow damage effect
                *effect = (damageTable->attack[13] >> 4) & 0xF;
            } else {
                *effect = defaultEffect;
            }
            /*
             * shape.face is unused for any actor besides the player. We are hijacking this because we need to have
             * some variable connected to the specific actor to indicate that the damage they received comes from a
             * sword beam. Each stage of the pipeline (update, draw) goes through all actors in a batch.
             */
            actor->shape.face = HIT_BY_SWORD_BEAM;
        } else if (index != 9) {
            /*
             * 9 is the index of the sword damage effect. With how FD plays, it is possible for the sword to connect
             * after sword beams have dealt damage. Without this check, the damage effect would revert back to the
             * light arrows effect upon sword collision.
             */
            actor->shape.face = NOT_HIT_BY_SWORD_BEAM;
        }
    });

    /*
     * Keese, Freezards, and Leevers are unique in that they call Actor_DrawDamageEffects() with NULL for the actor.
     * Normally, that method only uses the actor for playing positional sound effects. For sword beams only, we
     * overwrite these calls and pass in the actor so that the sword beam draws can be handled properly in the
     * VB_DRAW_DAMAGE_EFFECT hook.
     */
    COND_VB_SHOULD(VB_USE_NULL_FOR_DRAW_DAMAGE_EFFECTS, CVAR, {
        Actor* actor = va_arg(args, Actor*);
        // Only change the call if there is a sword beam collision
        if (actor->shape.face & HIT_BY_SWORD_BEAM) {
            *should = false;
            Vec3f* bodyPartsPos = va_arg(args, Vec3f*);
            int bodyPartsCount = va_arg(args, int);
            if (actor->id == ACTOR_EN_FIREFLY) { // Keese
                EnFirefly* enFireFly = (EnFirefly*)actor;
                Actor_DrawDamageEffects(gPlayState, actor, bodyPartsPos, bodyPartsCount,
                                        enFireFly->drawDmgEffScale * actor->scale.y * 200.0f,
                                        enFireFly->drawDmgEffFrozenSteamScale, enFireFly->drawDmgEffAlpha,
                                        enFireFly->drawDmgEffType);
            } else if (actor->id == ACTOR_EN_FZ) { // Freezard
                EnFz* enFz = (EnFz*)actor;
                Actor_DrawDamageEffects(gPlayState, actor, bodyPartsPos, bodyPartsCount, enFz->drawDmgEffScale * 4.0f,
                                        0.5f, enFz->drawDmgEffAlpha, ACTOR_DRAW_DMGEFF_LIGHT_ORBS);
            } else if (actor->id == ACTOR_EN_NEO_REEBA) { // Leever
                EnNeoReeba* enNeoReeba = (EnNeoReeba*)actor;
                Actor_DrawDamageEffects(gPlayState, actor, bodyPartsPos, bodyPartsCount, enNeoReeba->drawEffectScale,
                                        0.5f, enNeoReeba->drawEffectAlpha, enNeoReeba->drawEffectType);
            }
        }
    });

    /*
     * If we're drawing the light arrow damage effect, but we know it's from a sword beam, then quietly change the type
     * to the blue lights effect.
     */
    COND_VB_SHOULD(VB_DRAW_DAMAGE_EFFECT, CVAR, {
        Actor* actor = va_arg(args, Actor*);
        if (actor != nullptr && actor->shape.face & HIT_BY_SWORD_BEAM) {
            u8* type = va_arg(args, u8*);
            if (*type == ACTOR_DRAW_DMGEFF_LIGHT_ORBS) {
                *type = ACTOR_DRAW_DMGEFF_BLUE_LIGHT_ORBS;
            }
        }
    });

    /*
     * If this is a sword beam collision, just hand wave it as a valid collision. This allows for sword beams to hit
     * enemies in a damaging way, such as Skulltulas and Big Octos.
     */
    COND_VB_SHOULD(VB_CHECK_BUMPER_COLLISION, CVAR, {
        ColliderInfo* toucher = va_arg(args, ColliderInfo*);
        ColliderInfo* bumper = va_arg(args, ColliderInfo*);
        if (toucher->toucher.dmgFlags & DMG_SWORD_BEAM) {
            bumper->bumper.dmgFlags |= DMG_SWORD_BEAM;
            *should = false;
        }
    });

    /*
     * Define a custom damage effect for sword beams for the Big Octo, which handles drawing damage effects differently
     * from most enemies. We cannot easily piggyback off of the light arrows effect like we do for everybody else.
     */
    COND_ID_HOOK(ShouldActorUpdate, ACTOR_EN_BIGOKUTA, CVAR, [](Actor* actor, bool* result) {
        EnBigokuta* enBigOkuta = (EnBigokuta*)actor;
        if (enBigOkuta->bodyCollider.base.acFlags & AC_HIT &&
            enBigOkuta->bodyCollider.info.acHitInfo->toucher.dmgFlags & DMG_SWORD_BEAM) {
            enBigOkuta->drawDmgEffType = ACTOR_DRAW_DMGEFF_BLUE_LIGHT_ORBS;
            enBigOkuta->drawDmgEffScale = 1.2f;
            enBigOkuta->drawDmgEffAlpha = 4.0f;
            Actor_Spawn(&gPlayState->actorCtx, gPlayState, ACTOR_EN_CLEAR_TAG,
                        enBigOkuta->bodyCollider.info.bumper.hitPos.x, enBigOkuta->bodyCollider.info.bumper.hitPos.y,
                        enBigOkuta->bodyCollider.info.bumper.hitPos.z, 0, 0, 0,
                        CLEAR_TAG_PARAMS(CLEAR_TAG_LARGE_LIGHT_RAYS));
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterFierceDeityAnywhere, { CVAR_NAME });
