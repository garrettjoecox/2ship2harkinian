#include "ActorBehavior.h"
#include <libultraship/libultraship.h>
#include "CustomItem/CustomItem.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Geg/z_en_geg.h"
void Player_TalkWithPlayer(PlayState* play, Actor* actor);
void func_80BB27D4(EnGeg* enGeg, PlayState* play);
}

void Rando::ActorBehavior::InitEnGegBehavior() {
    if (IS_RANDO) {
        SET_WEEKEVENTREG(WEEKEVENTREG_35_40);
    }

    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_OFFER, IS_RANDO, {
        GetItemId* item = va_arg(args, GetItemId*);
        EnGeg* refActor = va_arg(args, EnGeg*);
        Player* player = GET_PLAYER(gPlayState);

        if (refActor->actor.id != ACTOR_EN_GEG) {
            return;
        }

        *should = false;

        refActor->actor.parent = &player->actor;
        player->talkActor = &refActor->actor;
        player->talkActorDistance = refActor->actor.xzDistToPlayer;
        player->exchangeItemAction = PLAYER_IA_MINUS1;
        Player_TalkWithPlayer(gPlayState, &refActor->actor);
    });

    COND_ID_HOOK(OnOpenText, 0xd74, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        RandoItemId randoItemId = RANDO_SAVE_CHECKS[RC_MOUNTAIN_VILLAGE_DON_GERO_MASK].randoItemId;

        CustomMessage::Replace(&entry.msg, "Rupee", Rando::StaticData::Items[randoItemId].name);
        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });
}