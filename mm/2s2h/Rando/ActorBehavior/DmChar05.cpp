#include "ActorBehavior.h"

// These come from corresponding entries in z_message.c.
#define GORON_MASK_TEXT 0x79
#define ZORA_MASK_TEXT 0x7A

// Replace vanilla item get text with a simple message stating what randomized item has been received
void replaceGetItemText(RandoCheckId randoCheckId, u16* textId, bool* loadFromMessageTable) {
    auto randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
    auto randoStaticItem = Rando::StaticData::Items[randoSaveCheck.randoItemId];

    auto entry = CustomMessage::LoadVanillaMessageTableEntry(*textId);
    entry.autoFormat = false;
    entry.msg.assign("You received " + std::string(randoStaticItem.article) + " " + std::string(randoStaticItem.name) +
                     "!\x1C\x02\x10");
    entry.icon = 0xFE; // No icon

    CustomMessage::LoadCustomMessageIntoFont(entry);
    *loadFromMessageTable = false;
}

void Rando::ActorBehavior::InitDmChar05Behavior() {
    COND_VB_SHOULD(VB_GIVE_ITEM_FROM_DMCHAR05, IS_RANDO, {
        ItemId vanillaItemId = (ItemId)va_arg(args, int);
        switch (vanillaItemId) {
            case ITEM_MASK_GORON:
                RANDO_SAVE_CHECKS[RC_GORON_GRAVEYARD_DARMANI].eligible = true;
                break;
            case ITEM_MASK_ZORA:
                RANDO_SAVE_CHECKS[RC_GREAT_BAY_COAST_MIKAU].eligible = true;
                break;
            default:
                break;
        }
        *should = false;
    });

    /*
     * This only affects the "Get Item" model displayed above Link's head. Other instances of the items in question,
     * such as the Goron Mask when it falls to the ground, still displays that vanilla model.
     */
    COND_VB_SHOULD(VB_DRAW_ITEM_FROM_DMCHAR05, IS_RANDO, {
        GetItemDrawId vanillaItemId = (GetItemDrawId)va_arg(args, int);
        switch (vanillaItemId) {
            case GID_MASK_GORON:
                Rando::DrawItem(RANDO_SAVE_CHECKS[RC_GORON_GRAVEYARD_DARMANI].randoItemId);
                break;
            case GID_MASK_ZORA:
                Rando::DrawItem(RANDO_SAVE_CHECKS[RC_GREAT_BAY_COAST_MIKAU].randoItemId);
                break;
            default:
                break;
        }
        *should = false;
    });

    COND_ID_HOOK(OnOpenText, GORON_MASK_TEXT, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        replaceGetItemText(RC_GORON_GRAVEYARD_DARMANI, textId, loadFromMessageTable);
    });

    COND_ID_HOOK(OnOpenText, ZORA_MASK_TEXT, IS_RANDO, [](u16* textId, bool* loadFromMessageTable) {
        replaceGetItemText(RC_GREAT_BAY_COAST_MIKAU, textId, loadFromMessageTable);
    });
}