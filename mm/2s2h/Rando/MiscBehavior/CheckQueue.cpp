#include "MiscBehavior.h"
#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/CustomMessage/CustomMessage.h"
#include "2s2h/BenGui/Notification.h"
#include "2s2h/Rando/StaticData/StaticData.h"
#include "2s2h/ShipUtils.h"
#include "Traps.h"
#include "2s2h/Network/Archipelago/Archipelago.h"
#include "2s2h/Network/Archipelago/ArchipelagoBridge.h"

extern "C" {
#include "variables.h"
#include <functions.h>
extern TexturePtr gItemIcons[131];
extern s16 D_801CFF94[250];
}

static bool queued = false;

// This function handles queuing up item gives that the player has been marked as eligible for. If you are looking for
// the behavior of the actual giving itself, the heavy lifting is done by the GameInteractor queue. This function is
// currently called every frame, and loops through the entire list of checks, this works for now but as the check list
// grows we should keep an eye on performance.
//
// Though it seems counter-intuitive, we currently only allow one thing from randommizer to be queued at a time,
// primarily because of the way an item can be converted may change as you queue up multiple items. This is actually
// fine for both the giving/drawing, as we can call ConvertItem() inside the Give/Draw lambda, but the message we
// pass to the queue is static and would need to be updated if we allowed multiple items to be queued at once.
void Rando::MiscBehavior::CheckQueue() {
    if (queued) {
        return;
    }

    for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
        auto randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];

        if (randoSaveCheck.eligible) {
            // Archipelago save: give item locally AND send LocationCheck to AP server (if connected)
            if (IS_ARCHI) {
                const uint64_t apLocationId = ArchipelagoBridge::GetLocationIdFromRandoCheck(randoCheckId);

                if (apLocationId == 0) {
                    return;
                }

                // Mark location as checked in save file (even when disconnected)
                // This allows us to resync on reconnect
                if (!ArchipelagoBridge::IsLocationChecked(apLocationId)) {
                    ArchipelagoBridge::MarkLocationChecked(apLocationId);
                }

                // Send to server if connected, otherwise it will be synced on reconnect
                if (Archipelago::IsConnected()) {
                    Archipelago::SendLocationCheck(apLocationId);

                    // For our own items, the server will send them back via ApplyOneItem (with animation).
                    // Only skip local give for non-placeholder items to avoid double-giving.
                    // AP placeholder items (going to other players) fall through to GIEventGiveItem
                    // so the animation still plays locally.
                    bool isApPlaceholder = (randoSaveCheck.randoItemId == RI_ARCHIPELAGO_PROGRESSIVE ||
                                            randoSaveCheck.randoItemId == RI_ARCHIPELAGO_USEFUL ||
                                            randoSaveCheck.randoItemId == RI_ARCHIPELAGO_JUNK);
                    if (!isApPlaceholder) {
                        // Own item - mark obtained and let server send it back with animation.
                        auto& saveCheckRef = RANDO_SAVE_CHECKS[randoCheckId];
                        saveCheckRef.cycleObtained = true;
                        saveCheckRef.obtained = true;
                        saveCheckRef.eligible = false;
                        return;
                    }
                    // AP placeholder - fall through to GIEventGiveItem so animation plays locally.
                }

                // Offline fallback: give item locally (server will receive location on reconnect)

                // Safety check: if item data wasn't populated, don't queue to avoid crash
                if (randoSaveCheck.randoItemId == RI_UNKNOWN) {
                    // Mark as obtained so we don't try again
                    auto& saveCheck = RANDO_SAVE_CHECKS[randoCheckId];
                    saveCheck.cycleObtained = true;
                    saveCheck.obtained = true;
                    saveCheck.eligible = false;

                    return;
                }
            }

            queued = true;

            GameInteractor::Instance->events.emplace_back(GIEventGiveItem{
                .showGetItemCutscene =
                    Rando::StaticData::ShouldShowGetItemCutscene(ConvertItem(randoSaveCheck.randoItemId, randoCheckId)),
                .param = (int16_t)randoCheckId,
                .giveItem =
                    [](Actor* actor, PlayState* play) {
                        auto& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
                        RandoItemId randoItemId =
                            Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)CUSTOM_ITEM_PARAM);
                        std::string prefix = "You found";
                        std::string message =
                            Rando::StaticData::GetItemName(randoItemId, true, (RandoCheckId)CUSTOM_ITEM_PARAM);

                        if (randoItemId == RI_JUNK) {
                            randoItemId = Rando::CurrentJunkItem((RandoCheckId)CUSTOM_ITEM_PARAM);
                        }
                        if (randoItemId == RI_TRIFORCE_PIECE) {
                            if (gSaveContext.save.shipSaveInfo.rando.foundTriforcePieces + 1 >=
                                RANDO_SAVE_OPTIONS[RO_TRIFORCE_PIECES_REQUIRED]) {
                                prefix = "You";
                                message = "completed the Triforce";
                            }
                            randoItemId = RI_TRIFORCE_PIECE_PREVIOUS;
                        }

                        if (randoItemId == RI_TRAP) {
                            prefix = "";
                            message = GetTrapMessage();
                            // We need to remove the Color Codes if the player is skipping Item Get Cutscenes as the
                            // Notification Emit doesnt support it.
                            if (CVarGetInteger("gEnhancements.Cutscenes.SkipGetItemCutscenes", 0) >= 2) {
                                message = CustomMessage::RemoveColorCodes(message);
                            }
                        }

                        // For AP placeholder items (another player's item at this location),
                        // color the item name green and the player name yellow in textboxes.
                        bool isArchiPlaceholder = (randoItemId == RI_ARCHIPELAGO_PROGRESSIVE ||
                                                   randoItemId == RI_ARCHIPELAGO_USEFUL ||
                                                   randoItemId == RI_ARCHIPELAGO_JUNK);
                        if (isArchiPlaceholder) {
                            std::string apPlayerName, apItemName;
                            ArchipelagoBridge::GetArchipelagoItemComponents(
                                (RandoCheckId)CUSTOM_ITEM_PARAM, apPlayerName, apItemName);
                            if (!apPlayerName.empty()) {
                                message = "%g" + apItemName + "%w for %y" + apPlayerName + "%w";
                            } else {
                                message = "%g" + apItemName + "%w";
                            }
                        }

                        CustomMessage::Entry entry = {
                            .textboxType = 2,
                            .icon = Rando::StaticData::GetIconForZMessage(randoItemId),
                            .msg = (prefix == "" ? "" : prefix + " ") + message + (randoItemId == RI_TRAP ? "" : "!"),
                        };

                        if (CUSTOM_ITEM_FLAGS & CustomItem::GIVE_ITEM_CUTSCENE) {
                            CustomMessage::SetActiveCustomMessage(entry.msg, entry);
                        } else if (Rando::StaticData::ShouldShowGetItemCutscene(randoItemId)) {
                            CustomMessage::StartTextbox(entry.msg + "\x1C\x02\x10", entry);
                        } else {
                            if (Rando::StaticData::Items[randoItemId].randoItemType != RITYPE_JUNK) {
                                Notification::Emit({
                                    .itemIcon = Rando::StaticData::GetIconTexturePath(randoItemId),
                                    .message = prefix,
                                    .suffix = message,
                                });
                            }
                        }
                        Rando::GiveItem(randoItemId);
                        randoSaveCheck.cycleObtained = true;
                        randoSaveCheck.obtained = true;
                        randoSaveCheck.eligible = false;
                        queued = false;
                        CUSTOM_ITEM_PARAM = randoItemId;
                    },
                .drawItem =
                    [](Actor* actor, PlayState* play) {
                        RandoItemId randoItemId;

                        // If the item has been given, the CUSTOM_ITEM_PARAM is set to the RI, prior to that it's the RC
                        if (CUSTOM_ITEM_FLAGS & CustomItem::CALLED_ACTION) {
                            if ((RandoItemId)CUSTOM_ITEM_PARAM == RI_TRAP) {
                                randoItemId = RI_MAX_TRAP;
                            } else {
                                randoItemId = (RandoItemId)CUSTOM_ITEM_PARAM;
                            }
                        } else {
                            auto& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
                            randoItemId =
                                Rando::ConvertItem(randoSaveCheck.randoItemId, (RandoCheckId)CUSTOM_ITEM_PARAM);

                            // Safety: for Archi saves, if item is unknown, use a default to avoid crash
                            if (randoItemId == RI_UNKNOWN && IS_ARCHI) {
                                // Use the vanilla item for this check as fallback
                                auto staticCheck = Rando::StaticData::Checks.find((RandoCheckId)CUSTOM_ITEM_PARAM);
                                if (staticCheck != Rando::StaticData::Checks.end()) {
                                    randoItemId = staticCheck->second.randoItemId;
                                } else {
                                    randoItemId = RI_RECOVERY_HEART; // Ultimate fallback
                                }
                            }
                        }

                        Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                        Rando::DrawItem(randoItemId, (RandoCheckId)CUSTOM_ITEM_PARAM, actor);
                    } });
            return;
        }
    }
}

void Rando::MiscBehavior::CheckQueueReset() {
    queued = false;
    GameInteractor::Instance->currentEvent = GIEventNone{};
    GameInteractor::Instance->events.clear();
}
