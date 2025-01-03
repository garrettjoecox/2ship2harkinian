#include "ActorBehavior.h"
#include <libultraship/libultraship.h>
#include "2s2h/ShipUtils.h"

#include <vector>

extern "C" {
#include "functions.h"
#include "variables.h"

#include "overlays/actors/ovl_En_Gs/z_en_gs.h"
}

std::unordered_map<RandoCheckId, std::string> readableCheckNamesForGs;

#define FIRST_GS_MESSAGE 0x20D1
#define SECOND_GS_MESSAGE 0x20C0

s32 GetObtainedChecksAmount() {
    s32 obtainedChecks = 0;
    for (auto& [randoCheckId, _] : Rando::StaticData::Checks) {
        RandoSaveCheck saveCheck = RANDO_SAVE_CHECKS[randoCheckId];
        if (saveCheck.shuffled && saveCheck.obtained) {
            obtainedChecks++;
        }
    }
    return obtainedChecks;
}

RandoCheckId GetRandomCheck(bool repeatableOnlyObtained = false) {
    Player* player = GET_PLAYER(gPlayState);
    if (player->talkActor == nullptr || player->talkActor->id != ACTOR_EN_GS) {
        return RC_UNKNOWN;
    }
    EnGs* enGs = (EnGs*)player->talkActor;

    std::vector<RandoCheckId> availableChecks;
    for (auto& [randoCheckId, _] : Rando::StaticData::Checks) {
        RandoSaveCheck saveCheck = RANDO_SAVE_CHECKS[randoCheckId];
        if (saveCheck.shuffled && (!repeatableOnlyObtained || !saveCheck.obtained)) {
            availableChecks.push_back(randoCheckId);
        }
    }

    if (availableChecks.empty()) {
        return RC_UNKNOWN;
    }

    if (repeatableOnlyObtained) {
        Ship_Random_Seed(gGameState->frames);
    } else {
        Ship_Random_Seed(gSaveContext.save.shipSaveInfo.rando.finalSeed + enGs->unk_195);
    }
    return availableChecks[Ship_Random(0, availableChecks.size() - 1)];
}

void Rando::ActorBehavior::InitEnGsBehavior() {
    readableCheckNamesForGs.clear();
    for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
        readableCheckNamesForGs[randoCheckId] = convertEnumToReadableName(randoStaticCheck.name);
    }

    COND_VB_SHOULD(VB_GS_CONSIDER_MASK_OF_TRUTH_EQUIPPED, IS_RANDO, { *should = true; });

    // Override the message ID so that we can control the text
    COND_VB_SHOULD(VB_GS_CONTINUE_TEXTBOX, IS_RANDO, {
        *should = false;
        Message_ContinueTextbox(gPlayState, SECOND_GS_MESSAGE);
    });

    COND_ID_HOOK(OnOpenText, FIRST_GS_MESSAGE, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        RandoCheckId randoCheckId = GetRandomCheck();
        if (randoCheckId == RC_UNKNOWN) {
            return;
        }

        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
        entry.autoFormat = false;
        auto& saveCheck = RANDO_SAVE_CHECKS[randoCheckId];

        entry.msg = "They say %g{{article}}{{item}}%w is hidden at %y{{location}}%w.\x11";

        if (Rando::StaticData::Items[saveCheck.randoItemId].article != "") {
            CustomMessage::Replace(&entry.msg, "{{article}}",
                                   std::string(Rando::StaticData::Items[saveCheck.randoItemId].article) + " ");
        } else {
            CustomMessage::Replace(&entry.msg, "{{article}}", "");
        }

        CustomMessage::Replace(&entry.msg, "{{item}}", Rando::StaticData::Items[saveCheck.randoItemId].name);
        CustomMessage::Replace(&entry.msg, "{{location}}", readableCheckNamesForGs[randoCheckId]);

        CustomMessage::AddLineBreaks(&entry.msg);

        // Eventually this part should be opt-in, but for now it's always on
        entry.msg += "\x13\x12...\x13\x12Trade %r{{rupees}} Rupees%w for another hint?\x11\xC2Yes\x11No";
        s32 cost = MAX(10, MIN(500, GetObtainedChecksAmount() * 2));
        CustomMessage::Replace(&entry.msg, "{{rupees}}", std::to_string(cost));

        CustomMessage::ReplaceColorChars(&entry.msg);
        CustomMessage::EnsureMessageEnd(&entry.msg);

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });

    COND_ID_HOOK(OnOpenText, SECOND_GS_MESSAGE, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        MessageContext* msgCtx = &gPlayState->msgCtx;

        auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);

        if (msgCtx->choiceIndex == 0) {
            s32 cost = MAX(10, MIN(500, GetObtainedChecksAmount() * 2));

            RandoCheckId randoCheckId = GetRandomCheck(true);
            if (gSaveContext.save.saveInfo.playerData.rupees < cost) {
                entry.msg = "Foolish... You don't have enough rupees...";
            } else if (randoCheckId == RC_UNKNOWN) {
                entry.msg = "I have no more hints for you...";
            } else {
                RandoSaveCheck saveCheck = RANDO_SAVE_CHECKS[randoCheckId];

                entry.msg = "Wise choice... They say %g{{article}}{{item}}%w is hidden at %y{{location}}%w.";

                if (Rando::StaticData::Items[saveCheck.randoItemId].article != "") {
                    CustomMessage::Replace(&entry.msg, "{{article}}",
                                           std::string(Rando::StaticData::Items[saveCheck.randoItemId].article) + " ");
                } else {
                    CustomMessage::Replace(&entry.msg, "{{article}}", "");
                }

                CustomMessage::Replace(&entry.msg, "{{item}}", Rando::StaticData::Items[saveCheck.randoItemId].name);
                CustomMessage::Replace(&entry.msg, "{{location}}", readableCheckNamesForGs[randoCheckId]);

                gSaveContext.rupeeAccumulator -= cost;
                cost *= 2;
            }
        } else {
            entry.msg = "Foolish... Come back later when you have more sense.";
        }

        CustomMessage::LoadCustomMessageIntoFont(entry);
        *loadFromMessageTable = false;
    });
}
