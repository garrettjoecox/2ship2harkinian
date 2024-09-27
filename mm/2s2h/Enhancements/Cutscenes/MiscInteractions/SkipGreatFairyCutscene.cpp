#include <libultraship/libultraship.h>
#include "Enhancements/GameInteractor/GameInteractor.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_En_Elfgrp/z_en_elfgrp.h"
void EnElfgrp_SpawnStrayFairies(EnElfgrp* elfgrp, PlayState* play, s32 count, s32 fairyType);
void EnElfgrp_SetFountainFairiesCount(PlayState* play, s32 type, s32 newCount);
s32 EnElfgrp_GetHeldFairiesCount(PlayState* play, s32 type);
s32 EnElfgrp_SummonStrayFairies(PlayState* play);
void func_80A3A210(EnElfgrp* elfgrp, PlayState* play);
}

#define ELFGRP_STATE_0 (1 << 0)
#define ELFGRP_STATE_3 (1 << 3)

void RegisterSkipGreatFairyCutscene() {
    REGISTER_VB_SHOULD(GI_VB_START_GREAT_FAIRY_CUTSCENE, {
        if (CVarGetInteger("gEnhancements.Cutscenes.SkipMiscInteractions", 0)) {
            EnElfgrp* elfgrp = va_arg(args, EnElfgrp*);

            if (*should && !(elfgrp->stateFlags & ELFGRP_STATE_0)) {
                elfgrp->stateFlags |= ELFGRP_STATE_0;
                if (elfgrp->type == ENELFGRP_TYPE_MAGIC) { // Clock Town
                    EnElfgrp_SpawnStrayFairies(elfgrp, gPlayState, 1, 1);
                } else {
                    EnElfgrp_SpawnStrayFairies(elfgrp, gPlayState,
                                               EnElfgrp_GetHeldFairiesCount(gPlayState, elfgrp->type), 1);
                }
                EnElfgrp_SetFountainFairiesCount(gPlayState, elfgrp->type, STRAY_FAIRY_TOTAL);
                EnElfgrp_SummonStrayFairies(gPlayState);
                elfgrp->actionFunc = func_80A3A210;
                elfgrp->stateFlags &= ~ELFGRP_STATE_3;
                elfgrp->timer = 90;

                if (GameInteractor_Should(GI_VB_GIVE_ITEM_FROM_STRAY_FAIRY_MANAGER, true, elfgrp)) {
                    switch (elfgrp->type) {
                        case ENELFGRP_TYPE_POWER:
                            if (!CHECK_WEEKEVENTREG(WEEKEVENTREG_OBTAINED_GREAT_SPIN_ATTACK)) {
                                GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                                    .showGetItemCutscene = true,
                                    .getItemText = "Great Spin Attack",
                                    .drawItem = []() { GetItem_Draw(gPlayState, GID_SWORD_KOKIRI); },
                                    .giveItem = []() { SET_WEEKEVENTREG(WEEKEVENTREG_OBTAINED_GREAT_SPIN_ATTACK); } });
                            }
                            break;
                        case ENELFGRP_TYPE_WISDOM:
                            if (!gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired) {
                                GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                                    .showGetItemCutscene = true,
                                    .getItemText = "Double Magic",
                                    .drawItem = []() { GetItem_Draw(gPlayState, GID_MAGIC_JAR_BIG); },
                                    .giveItem =
                                        []() {
                                            gSaveContext.save.saveInfo.playerData.isDoubleMagicAcquired = true;
                                            gSaveContext.magicFillTarget = MAGIC_DOUBLE_METER;
                                            gSaveContext.save.saveInfo.playerData.magicLevel = 0;
                                        } });
                            }
                            break;
                        case ENELFGRP_TYPE_COURAGE:
                            if (!gSaveContext.save.saveInfo.playerData.doubleDefense) {
                                GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                                    .showGetItemCutscene = true,
                                    .getItemText = "Double Defense",
                                    .drawItem = []() { GetItem_Draw(gPlayState, GID_HEART_CONTAINER); },
                                    .giveItem =
                                        []() {
                                            gSaveContext.save.saveInfo.playerData.doubleDefense = true;
                                            gSaveContext.save.saveInfo.inventory.defenseHearts = 20;
                                        } });
                            }
                            break;
                        case ENELFGRP_TYPE_KINDNESS:
                            if (INV_CONTENT(ITEM_SWORD_GREAT_FAIRY) != ITEM_SWORD_GREAT_FAIRY) {
                                GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                                    .showGetItemCutscene = true,
                                    .getItemText = "Great Fairy Sword",
                                    .drawItem = []() { GetItem_Draw(gPlayState, GID_SWORD_GREAT_FAIRY); },
                                    .giveItem = []() { Item_Give(gPlayState, ITEM_SWORD_GREAT_FAIRY); } });
                            }
                            break;
                        default: // ENELFGRP_TYPE_MAGIC
                            if (!gSaveContext.save.saveInfo.playerData.isMagicAcquired) {
                                GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                                    .showGetItemCutscene = true,
                                    .getItemText = "Power of Magic",
                                    .drawItem = []() { GetItem_Draw(gPlayState, GID_MAGIC_JAR_SMALL); },
                                    .giveItem =
                                        []() {
                                            gSaveContext.save.saveInfo.playerData.isMagicAcquired = true;
                                            gSaveContext.magicFillTarget = MAGIC_NORMAL_METER;
                                        } });
                            } else if (INV_CONTENT(ITEM_MASK_DEKU) == ITEM_MASK_DEKU &&
                                       INV_CONTENT(ITEM_MASK_GREAT_FAIRY) != ITEM_MASK_GREAT_FAIRY) {
                                GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                                    .showGetItemCutscene = true,
                                    .getItemText = "Great Fairy's Mask",
                                    .drawItem = []() { GetItem_Draw(gPlayState, GID_MASK_GREAT_FAIRY); },
                                    .giveItem = []() { Item_Give(gPlayState, ITEM_MASK_GREAT_FAIRY); } });
                            }
                            break;
                    }
                }

                gSaveContext.healthAccumulator = 0x140;
                Magic_Add(gPlayState, MAGIC_FILL_TO_CAPACITY);
            }

            *should = false;
        }
    });
}
