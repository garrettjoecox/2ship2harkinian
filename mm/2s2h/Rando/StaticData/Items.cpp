#include "StaticData.h"

namespace Rando {

namespace StaticData {

// clang-format off
std::unordered_map<RandoItemId, RandoStaticItem> Items = {
    { RI_UNKNOWN,                 { "Unknown",                 ITEM_NONE,              GI_NONE,              GID_NONE } },
    { RI_ARROW_FIRE,              { "Fire Arrows",             ITEM_ARROW_FIRE,        GI_ARROW_FIRE,        GID_ARROW_FIRE } },
    { RI_ARROW_ICE,               { "Ice Arrows",              ITEM_ARROW_ICE,         GI_ARROW_ICE,         GID_ARROW_ICE } },
    { RI_CLOCK_TOWN_STRAY_FAIRY,  { "Clock Town Stray Fairy",  ITEM_STRAY_FAIRIES,     GI_STRAY_FAIRY,       GID_NONE } },
    { RI_DOUBLE_DEFENSE,          { "Double Defense",          ITEM_NONE,              GI_NONE,              GID_HEART_CONTAINER } },
    { RI_GREAT_BAY_BOSS_KEY,      { "Great Bay Boss Key",      ITEM_KEY_BOSS,          GI_KEY_BOSS,          GID_KEY_BOSS } },
    { RI_GREAT_BAY_COMPASS,       { "Great Bay Compass",       ITEM_COMPASS,           GI_COMPASS,           GID_COMPASS } },
    { RI_GREAT_BAY_MAP,           { "Great Bay Map",           ITEM_DUNGEON_MAP,       GI_MAP,               GID_DUNGEON_MAP } },
    { RI_GREAT_BAY_SMALL_KEY,     { "Great Bay Small Key",     ITEM_KEY_SMALL,         GI_KEY_SMALL,         GID_KEY_SMALL } },
    { RI_GREAT_BAY_STRAY_FAIRY,   { "Great Bay Stray Fairy",   ITEM_STRAY_FAIRIES,     GI_STRAY_FAIRY,       GID_NONE } },
    { RI_GREAT_FAIRY_SWORD,       { "Great Fairy's Sword",     ITEM_SWORD_GREAT_FAIRY, GI_SWORD_GREAT_FAIRY, GID_SWORD_GREAT_FAIRY } },
    { RI_GREAT_SPIN_ATTACK,       { "Great Spin Attack",       ITEM_NONE,              GI_NONE,              GID_SWORD_KOKIRI } },
    { RI_HEART_PIECE,             { "Heart Piece",             ITEM_HEART_PIECE,       GI_HEART_PIECE,       GID_HEART_PIECE } },
    { RI_HEART_CONTAINER,         { "Heart Container",         ITEM_HEART_CONTAINER,   GI_HEART_CONTAINER,   GID_HEART_CONTAINER } },
    { RI_LETTER_TO_KAFEI,         { "Letter to Kafei",         ITEM_LETTER_TO_KAFEI,   GI_LETTER_TO_KAFEI,   GID_LETTER_TO_KAFEI } },
    { RI_MASK_BREMEN,             { "Bremen Mask",             ITEM_MASK_BREMEN,       GI_MASK_BREMEN,       GID_MASK_BREMEN } },
    { RI_MASK_GREAT_FAIRY,        { "Great Fairy Mask",        ITEM_MASK_GREAT_FAIRY,  GI_MASK_GREAT_FAIRY,  GID_MASK_GREAT_FAIRY } },
    { RI_PROGRESSIVE_BOW,         { "Progressive Bow",         ITEM_NONE,              GI_NONE,              GID_NONE } },
    { RI_PROGRESSIVE_MAGIC,       { "Progressive Magic",       ITEM_NONE,              GI_NONE,              GID_MAGIC_JAR_SMALL } },
    { RI_ROOM_KEY,                { "Room Key",                ITEM_ROOM_KEY,          GI_ROOM_KEY,          GID_ROOM_KEY } },
    { RI_RUPEE_BLUE,              { "Blue Rupee",              ITEM_RUPEE_BLUE,        GI_RUPEE_BLUE,        GID_RUPEE_BLUE } },
    { RI_RUPEE_GREEN,             { "Green Rupee",             ITEM_RUPEE_GREEN,       GI_RUPEE_GREEN,       GID_RUPEE_GREEN } },
    { RI_RUPEE_PURPLE,            { "Purple Rupee",            ITEM_RUPEE_PURPLE,      GI_RUPEE_PURPLE,      GID_RUPEE_PURPLE } },
    { RI_RUPEE_RED,               { "Red Rupee",               ITEM_RUPEE_RED,         GI_RUPEE_RED,         GID_RUPEE_RED } },
    { RI_RUPEE_SILVER,            { "Silver Rupee",            ITEM_RUPEE_SILVER,      GI_RUPEE_SILVER,      GID_RUPEE_SILVER } },
    { RI_SNOWHEAD_BOSS_KEY,       { "Snowhead Boss Key",       ITEM_KEY_BOSS,          GI_KEY_BOSS,          GID_KEY_BOSS } },
    { RI_SNOWHEAD_COMPASS,        { "Snowhead Compass",        ITEM_COMPASS,           GI_COMPASS,           GID_COMPASS } },
    { RI_SNOWHEAD_MAP,            { "Snowhead Map",            ITEM_DUNGEON_MAP,       GI_MAP,               GID_DUNGEON_MAP } },
    { RI_SNOWHEAD_SMALL_KEY,      { "Snowhead Small Key",      ITEM_KEY_SMALL,         GI_KEY_SMALL,         GID_KEY_SMALL } },
    { RI_SNOWHEAD_STRAY_FAIRY,    { "Snowhead Stray Fairy",    ITEM_STRAY_FAIRIES,     GI_STRAY_FAIRY,       GID_NONE } },
    { RI_STONE_TOWER_STRAY_FAIRY, { "Stone Tower Stray Fairy", ITEM_STRAY_FAIRIES,     GI_STRAY_FAIRY,       GID_NONE } },
    { RI_SUNS_SONG,               { "Sun's Song",              ITEM_SONG_SUN,          GI_NONE,              GID_NONE } },
    { RI_WOODFALL_BOSS_KEY,       { "Woodfall Boss Key",       ITEM_KEY_BOSS,          GI_KEY_BOSS,          GID_KEY_BOSS } },
    { RI_WOODFALL_COMPASS,        { "Woodfall Compass",        ITEM_COMPASS,           GI_COMPASS,           GID_COMPASS } },
    { RI_WOODFALL_MAP,            { "Woodfall Map",            ITEM_DUNGEON_MAP,       GI_MAP,               GID_DUNGEON_MAP } },
    { RI_WOODFALL_SMALL_KEY,      { "Woodfall Small Key",      ITEM_KEY_SMALL,         GI_KEY_SMALL,         GID_KEY_SMALL } },
    { RI_WOODFALL_STRAY_FAIRY,    { "Woodfall Stray Fairy",    ITEM_STRAY_FAIRIES,     GI_STRAY_FAIRY,       GID_NONE } },
    { RI_MAX,                     { "Unknown",                 ITEM_NONE,              GI_MAX,               GID_MAXIMUM } },
};
// clang-format on

} // namespace StaticData

} // namespace Rando
