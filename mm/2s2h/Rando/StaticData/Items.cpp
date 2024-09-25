#include "StaticData.h"

namespace Rando {

namespace StaticData {

// clang-format off
std::unordered_map<RandoItemId, RandoStaticItem> Items = {
    { RI_UNKNOWN,                 { "Unknown",                   ITEM_NONE,              GI_NONE,              GID_NONE } },
    { RI_ARROW_FIRE,              { "Fire Arrows",               ITEM_ARROW_FIRE,        GI_ARROW_FIRE,        GID_ARROW_FIRE } },
    { RI_ARROW_ICE,               { "Ice Arrows",                ITEM_ARROW_ICE,         GI_ARROW_ICE,         GID_ARROW_ICE } },
    { RI_ARROW_LIGHT,             { "Light Arrows",              ITEM_ARROW_LIGHT,       GI_ARROW_LIGHT,       GID_ARROW_LIGHT } },
    { RI_BOMBCHU_10,              { "10 Bombchus",               ITEM_BOMBCHUS_10,       GI_BOMBCHUS_10,       GID_BOMBCHU } },
    { RI_BOW,                     { "a Bow",                     ITEM_BOW,               GI_NONE,              GID_BOW } },
    { RI_CLOCK_TOWN_STRAY_FAIRY,  { "a Clock Town Stray Fairy",  ITEM_STRAY_FAIRIES,     GI_STRAY_FAIRY,       GID_NONE } },
    { RI_DOUBLE_DEFENSE,          { "Double Defense",            ITEM_NONE,              GI_NONE,              GID_HEART_CONTAINER } },
    { RI_DOUBLE_MAGIC,            { "a Magic Upgrade",           ITEM_NONE,              GI_NONE,              GID_MAGIC_JAR_BIG } },
    { RI_GREAT_BAY_BOSS_KEY,      { "the Great Bay Boss Key",    ITEM_KEY_BOSS,          GI_KEY_BOSS,          GID_KEY_BOSS } },
    { RI_GREAT_BAY_COMPASS,       { "the Great Bay Compass",     ITEM_COMPASS,           GI_COMPASS,           GID_COMPASS } },
    { RI_GREAT_BAY_MAP,           { "the Great Bay Map",         ITEM_DUNGEON_MAP,       GI_MAP,               GID_DUNGEON_MAP } },
    { RI_GREAT_BAY_SMALL_KEY,     { "a Great Bay Small Key",     ITEM_KEY_SMALL,         GI_KEY_SMALL,         GID_KEY_SMALL } },
    { RI_GREAT_BAY_STRAY_FAIRY,   { "a Great Bay Stray Fairy",   ITEM_STRAY_FAIRIES,     GI_STRAY_FAIRY,       GID_NONE } },
    { RI_GREAT_FAIRY_SWORD,       { "the Great Fairy's Sword",   ITEM_SWORD_GREAT_FAIRY, GI_SWORD_GREAT_FAIRY, GID_SWORD_GREAT_FAIRY } },
    { RI_GREAT_SPIN_ATTACK,       { "the Great Spin Attack",     ITEM_NONE,              GI_NONE,              GID_SWORD_KOKIRI } },
    { RI_HEART_CONTAINER,         { "a Heart Container",         ITEM_HEART_CONTAINER,   GI_HEART_CONTAINER,   GID_HEART_CONTAINER } },
    { RI_HEART_PIECE,             { "a Heart Piece",             ITEM_HEART_PIECE,       GI_HEART_PIECE,       GID_HEART_PIECE } },
    { RI_LETTER_TO_KAFEI,         { "the Letter to Kafei",       ITEM_LETTER_TO_KAFEI,   GI_LETTER_TO_KAFEI,   GID_LETTER_TO_KAFEI } },
    { RI_MAGIC_BEAN,              { "a Magic Bean",              ITEM_MAGIC_BEANS,       GI_MAGIC_BEANS,       GID_MAGIC_BEANS } },
    { RI_MASK_BREMEN,             { "the Bremen Mask",           ITEM_MASK_BREMEN,       GI_MASK_BREMEN,       GID_MASK_BREMEN } },
    { RI_MASK_GREAT_FAIRY,        { "the Great Fairy Mask",      ITEM_MASK_GREAT_FAIRY,  GI_MASK_GREAT_FAIRY,  GID_MASK_GREAT_FAIRY } },
    { RI_MASK_GIANT,              { "the Giant's Mask",          ITEM_MASK_GIANT,        GI_MASK_GIANT,        GID_MASK_GIANT } },
    { RI_PROGRESSIVE_BOW,         { "a Progressive Bow",         ITEM_NONE,              GI_NONE,              GID_NONE } },
    { RI_PROGRESSIVE_MAGIC,       { "Progressive Magic",         ITEM_NONE,              GI_NONE,              GID_MAGIC_JAR_SMALL } },
    { RI_QUIVER_40,               { "a Medium Quiver",           ITEM_QUIVER_40,         GI_QUIVER_40,         GID_QUIVER_40 } },
    { RI_QUIVER_50,               { "a Large Quiver",            ITEM_QUIVER_50,         GI_QUIVER_50,         GID_QUIVER_50 } },
    { RI_ROOM_KEY,                { "the Room Key",              ITEM_ROOM_KEY,          GI_ROOM_KEY,          GID_ROOM_KEY } },
    { RI_RUPEE_BLUE,              { "a Blue Rupee",              ITEM_RUPEE_BLUE,        GI_RUPEE_BLUE,        GID_RUPEE_BLUE } },
    { RI_RUPEE_GREEN,             { "a Green Rupee",             ITEM_RUPEE_GREEN,       GI_RUPEE_GREEN,       GID_RUPEE_GREEN } },
    { RI_RUPEE_PURPLE,            { "a Purple Rupee",            ITEM_RUPEE_PURPLE,      GI_RUPEE_PURPLE,      GID_RUPEE_PURPLE } },
    { RI_RUPEE_RED,               { "a Red Rupee",               ITEM_RUPEE_RED,         GI_RUPEE_RED,         GID_RUPEE_RED } },
    { RI_RUPEE_SILVER,            { "a Silver Rupee",            ITEM_RUPEE_SILVER,      GI_RUPEE_SILVER,      GID_RUPEE_SILVER } },
    { RI_SINGLE_MAGIC,            { "the Power of Magic",        ITEM_NONE,              GI_NONE,              GID_MAGIC_JAR_SMALL } },
    { RI_SNOWHEAD_BOSS_KEY,       { "the Snowhead Boss Key",     ITEM_KEY_BOSS,          GI_KEY_BOSS,          GID_KEY_BOSS } },
    { RI_SNOWHEAD_COMPASS,        { "the Snowhead Compass",      ITEM_COMPASS,           GI_COMPASS,           GID_COMPASS } },
    { RI_SNOWHEAD_MAP,            { "the Snowhead Map",          ITEM_DUNGEON_MAP,       GI_MAP,               GID_DUNGEON_MAP } },
    { RI_SNOWHEAD_SMALL_KEY,      { "a Snowhead Small Key",      ITEM_KEY_SMALL,         GI_KEY_SMALL,         GID_KEY_SMALL } },
    { RI_SNOWHEAD_STRAY_FAIRY,    { "a Snowhead Stray Fairy",    ITEM_STRAY_FAIRIES,     GI_STRAY_FAIRY,       GID_NONE } },
    { RI_STONE_TOWER_BOSS_KEY,    { "the Stone Tower Boss Key",  ITEM_KEY_BOSS,          GI_KEY_BOSS,          GID_KEY_BOSS } },
    { RI_STONE_TOWER_COMPASS,     { "the Stone Tower Compass",   ITEM_COMPASS,           GI_COMPASS,           GID_COMPASS } },
    { RI_STONE_TOWER_MAP,         { "the Stone Tower Map",       ITEM_DUNGEON_MAP,       GI_MAP,               GID_DUNGEON_MAP } },
    { RI_STONE_TOWER_SMALL_KEY,   { "a Stone Tower Small Key",   ITEM_KEY_SMALL,         GI_KEY_SMALL,         GID_KEY_SMALL } },
    { RI_STONE_TOWER_STRAY_FAIRY, { "a Stone Tower Stray Fairy", ITEM_STRAY_FAIRIES,     GI_STRAY_FAIRY,       GID_NONE } },
    { RI_SUNS_SONG,               { "the Sun's Song",            ITEM_SONG_SUN,          GI_NONE,              GID_NONE } },
    { RI_WOODFALL_BOSS_KEY,       { "the Woodfall Boss Key",     ITEM_KEY_BOSS,          GI_KEY_BOSS,          GID_KEY_BOSS } },
    { RI_WOODFALL_COMPASS,        { "the Woodfall Compass",      ITEM_COMPASS,           GI_COMPASS,           GID_COMPASS } },
    { RI_WOODFALL_MAP,            { "the Woodfall Map",          ITEM_DUNGEON_MAP,       GI_MAP,               GID_DUNGEON_MAP } },
    { RI_WOODFALL_SMALL_KEY,      { "a Woodfall Small Key",      ITEM_KEY_SMALL,         GI_KEY_SMALL,         GID_KEY_SMALL } },
    { RI_WOODFALL_STRAY_FAIRY,    { "a Woodfall Stray Fairy",    ITEM_STRAY_FAIRIES,     GI_STRAY_FAIRY,       GID_NONE } },
    { RI_MAX,                     { "Unknown",                   ITEM_NONE,              GI_MAX,               GID_MAXIMUM } },
};
// clang-format on

} // namespace StaticData

} // namespace Rando
