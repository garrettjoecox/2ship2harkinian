#include "StaticData.h"

extern "C" {
#include "overlays/actors/ovl_En_Sth/z_en_sth.h"
}

namespace Rando {

namespace StaticData {

#define RO(id, defaultValue, apName)                             \
    {                                                            \
        id, {                                                    \
            id, #id, "gRando.Options." #id, defaultValue, apName \
        }                                                        \
    }

// clang-format off
std::map<RandoOptionId, RandoStaticOption> Options = {
    RO(RO_ACCESS_DUNGEONS,             RO_ACCESS_DUNGEONS_FORM_AND_SONG, "access_dungeons"),
    RO(RO_ACCESS_MAJORA_MASKS_COUNT,   0,                                "access_majora_masks_count"),
    RO(RO_ACCESS_MAJORA_REMAINS_COUNT, 0,                                "access_majora_remains_count"),
    RO(RO_ACCESS_MOON_MASKS_COUNT,     0,                                "access_moon_masks_count"),
    RO(RO_ACCESS_MOON_REMAINS_COUNT,   4,                                "access_moon_remains_count"),
    RO(RO_ACCESS_TRIALS,               RO_ACCESS_TRIALS_VANILLA,         "access_trials"),
    RO(RO_CLOCK_SHUFFLE_PROGRESSIVE,   RO_CLOCK_SHUFFLE_RANDOM,          "clock_shuffle_progressive"),
    RO(RO_CLOCK_SHUFFLE,               RO_GENERIC_OFF,                   "clock_shuffle"),
    RO(RO_CLOCK_TERMINAL_TIME,         350,                              "clock_terminal_time"), // Default: 05:50 (10 "minutes" before crash)
    RO(RO_EXCLUDE_TERMINA_FIELD_GRASS, RO_GENERIC_OFF,                   "exclude_termina_field_grass"),
    RO(RO_EXCLUDE_COW_GROTTO_GRASS,    RO_GENERIC_OFF,                   "exclude_cow_grotto_grass"),
    RO(RO_HINTS_BANK_SIGN,             RO_GENERIC_OFF,                   "hints_bank_sign"),
    RO(RO_HINTS_BOSS_REMAINS,          RO_GENERIC_OFF,                   "hints_boss_remains"),
    RO(RO_HINTS_GOSSIP_STONE_STRENGTH, 50,                               "hints_gossip_stone_strength"),
    RO(RO_HINTS_GOSSIP_STONES,         RO_GENERIC_OFF,                   "hints_gossip_stones"),
    RO(RO_HINTS_HOOKSHOT,              RO_GENERIC_OFF,                   "hints_hookshot"),
    RO(RO_HINTS_MOON_GOSSIP_STONES,    RO_GENERIC_OFF,                   "hints_moon_gossip_stones"),
    RO(RO_HINTS_OATH_TO_ORDER,         RO_GENERIC_OFF,                   "hints_oath_to_order"),
    RO(RO_HINTS_PURCHASEABLE,          RO_GENERIC_OFF,                   "hints_purchaseable"),
    RO(RO_HINTS_SONG_OF_SOARING,       RO_GENERIC_OFF,                   "hints_song_of_soaring"),
    RO(RO_HINTS_SPIDER_HOUSES,         RO_GENERIC_OFF,                   "hints_spider_houses"),
    RO(RO_HINTS_TRANSFORMATIONS,       RO_GENERIC_OFF,                   "hints_transformations"),
    RO(RO_LOGIC,                       RO_LOGIC_GLITCHLESS,              "logic"),
    RO(RO_PLACEMENT_BOSS_KEYS,         RO_DUNGEON_ITEM_ANYWHERE,         "placement_boss_keys"),
    RO(RO_PLACEMENT_SMALL_KEYS,        RO_DUNGEON_ITEM_ANYWHERE,         "placement_small_keys"),
    RO(RO_PLACEMENT_STRAY_FAIRIES,     RO_DUNGEON_ITEM_ANYWHERE,         "placement_stray_fairies"),
    RO(RO_PLENTIFUL_ITEMS,             RO_GENERIC_OFF,                   "plentiful_items"),
    RO(RO_PURCHASE_INFINITE_RUPEES,    RO_GENERIC_OFF,                   "purchase_infinite_rupees"),
    RO(RO_SHUFFLE_BARREL_DROPS,        RO_GENERIC_OFF,                   "shuffle_barrel_drops"),
    RO(RO_SHUFFLE_BOSS_REMAINS,        RO_REMAINS_SHUFFLE_VANILLA,       "shuffle_boss_remains"),
    RO(RO_SHUFFLE_BOSS_SOULS,          RO_GENERIC_OFF,                   "shuffle_boss_souls"),
    RO(RO_SHUFFLE_BUTTERFLIES,         RO_GENERIC_OFF,                   "shuffle_butterflies"),
    RO(RO_SHUFFLE_COWS,                RO_GENERIC_OFF,                   "shuffle_cows"),
    RO(RO_SHUFFLE_CRATE_DROPS,         RO_GENERIC_OFF,                   "shuffle_crate_drops"),
    RO(RO_SHUFFLE_ENEMY_DROPS,         RO_GENERIC_OFF,                   "shuffle_enemy_drops"),
    RO(RO_SHUFFLE_ENEMY_SOULS,         RO_GENERIC_OFF,                   "shuffle_enemy_souls"),
    RO(RO_SHUFFLE_FREESTANDING_ITEMS,  RO_GENERIC_OFF,                   "shuffle_freestanding_items"),
    RO(RO_SHUFFLE_FROGS,               RO_GENERIC_OFF,                   "shuffle_frogs"),
    RO(RO_SHUFFLE_GOLD_SKULLTULAS,     RO_GENERIC_OFF,                   "shuffle_gold_skulltulas"),
    RO(RO_SHUFFLE_GRASS_DROPS,         RO_GENERIC_OFF,                   "shuffle_grass_drops"),
    RO(RO_SHUFFLE_HIVE_DROPS,          RO_GENERIC_OFF,                   "shuffle_hive_drops"),
    RO(RO_SHUFFLE_OCARINA_BUTTONS,     RO_GENERIC_OFF,                   "shuffle_ocarina_buttons"),
    RO(RO_SHUFFLE_OCARINA,             RO_GENERIC_OFF,                   "shuffle_ocarina"),
    RO(RO_SHUFFLE_OWL_STATUES,         RO_GENERIC_OFF,                   "shuffle_owl_statues"),
    RO(RO_SHUFFLE_POT_DROPS,           RO_GENERIC_OFF,                   "shuffle_pot_drops"),
    RO(RO_SHUFFLE_SHIELD,              RO_GENERIC_OFF,                   "shuffle_shield"),
    RO(RO_SHUFFLE_SHOPS,               RO_GENERIC_OFF,                   "shuffle_shops"),
    RO(RO_SHUFFLE_SKELETON_KEY,        RO_GENERIC_OFF,                   "shuffle_skeleton_key"),
    RO(RO_SHUFFLE_SNOWBALL_DROPS,      RO_GENERIC_OFF,                   "shuffle_snowball_drops"),
    RO(RO_SHUFFLE_SONGS,               RO_SONG_SHUFFLE_ANYWHERE,         "shuffle_songs"),
    RO(RO_SHUFFLE_SONG_DOUBLE_TIME,    RO_GENERIC_OFF,                   "shuffle_song_double_time"),
    RO(RO_SHUFFLE_SONG_INVERTED_TIME,  RO_GENERIC_OFF,                   "shuffle_song_inverted_time"),
    RO(RO_SHUFFLE_SONG_SARIA,          RO_GENERIC_OFF,                   "shuffle_song_saria"),
    RO(RO_SHUFFLE_SONG_SUN,            RO_GENERIC_OFF,                   "shuffle_song_sun"),
    RO(RO_SHUFFLE_SONG_TIME,           RO_GENERIC_OFF,                   "shuffle_song_time"),
    RO(RO_SHUFFLE_SWORD,               RO_GENERIC_OFF,                   "shuffle_sword"),
    RO(RO_SHUFFLE_SWIM,                RO_GENERIC_OFF,                   "shuffle_swim"),
    RO(RO_SHUFFLE_TINGLE_SHOPS,        RO_GENERIC_OFF,                   "shuffle_tingle_shops"),
    RO(RO_SHUFFLE_TRAPS,               RO_GENERIC_OFF,                   "shuffle_traps"),
    RO(RO_SHUFFLE_TREE_DROPS,          RO_GENERIC_OFF,                   "shuffle_tree_drops"),
    RO(RO_SHUFFLE_TRIFORCE_PIECES,     RO_GENERIC_OFF,                   "shuffle_triforce_pieces"),
    RO(RO_SHUFFLE_TYCOON_WALLET,       RO_GENERIC_OFF,                   "shuffle_tycoon_wallet"),
    RO(RO_SHUFFLE_WONDER_ITEMS,        RO_GENERIC_OFF,                   "shuffle_wonder_items"),
    RO(RO_SKULLTULA_SHUFFLED,          SPIDER_HOUSE_TOKENS_REQUIRED,     "skulltula_shuffled"),
    RO(RO_SKULLTULA_TOKENS_REQUIRED,   SPIDER_HOUSE_TOKENS_REQUIRED,     "skulltula_tokens_required"),
    RO(RO_STARTING_BUNNY_HOOD,         RO_GENERIC_OFF,                   "starting_bunny_hood"),
    RO(RO_STARTING_CONSUMABLES,        RO_GENERIC_OFF,                   "starting_consumables"),
    RO(RO_STARTING_HEALTH,             3,                                "starting_health"),
    RO(RO_STARTING_MAPS_AND_COMPASSES, RO_GENERIC_OFF,                   "starting_maps_and_compasses"),
    RO(RO_STARTING_RUPEES,             RO_GENERIC_OFF,                   "starting_rupees"),
    RO(RO_STRAY_FAIRIES_MAX,           STRAY_FAIRY_SCATTERED_TOTAL,      "stray_fairies_max"),
    RO(RO_STRAY_FAIRIES_REQUIRED,      STRAY_FAIRY_SCATTERED_TOTAL,      "stray_fairies_required"),
    RO(RO_TRAP_AMOUNT,                 5,                                "trap_amount"),
    RO(RO_TRIFORCE_PIECES_MAX,         DEFAULT_TRIFORCE_PIECES_MAX,      "triforce_pieces_max"),
    RO(RO_TRIFORCE_PIECES_REQUIRED,    DEFAULT_TRIFORCE_PIECES_MAX,      "triforce_pieces_required"),
};
// clang-format on

RandoOptionId GetOptionIdFromName(const char* name) {
    for (auto& [randoOptionId, randoStaticOption] : Options) {
        if (strcmp(name, randoStaticOption.name) == 0) {
            return randoOptionId;
        }
    }
    return RO_MAX;
}

} // namespace StaticData

} // namespace Rando
