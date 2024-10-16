#ifndef RANDO_TYPES_H
#define RANDO_TYPES_H

typedef enum {
    RCTYPE_UNKNOWN,
    RCTYPE_CHEST,
    RCTYPE_FREESTANDING,
    RCTYPE_NPC,
    RCTYPE_SHOP,
    RCTYPE_SKULL_TOKEN,
    RCTYPE_SONG,
    RCTYPE_STRAY_FAIRY,
    RCTYPE_POT,
    RCTYPE_MAX,
} RandoCheckType;

typedef enum {
    RC_UNKNOWN,

    // --- Verified as working --- //
    RC_ANCIENT_CASTLE_OF_IKANA_HP,
    RC_ASTRAL_OBSERVATORY_MOON_TEAR,
    RC_ASTRAL_OBSERVATORY_PASSAGE_CHEST,
    RC_BENEATH_THE_GRAVEYARD_CHEST,
    RC_BENEATH_THE_GRAVEYARD_DAMPE_CHEST,
    RC_BENEATH_THE_GRAVEYARD_HP,
    RC_BENEATH_THE_WELL_KEESE_CHEST,
    RC_BENEATH_THE_WELL_MIRROR_SHIELD,
    RC_BENEATH_THE_WELL_SKULLTULLA_CHEST,
    RC_CLOCK_TOWER_ROOF_OCARINA,
    RC_CLOCK_TOWER_ROOF_SONG_OF_TIME,
    RC_CLOCK_TOWN_DEKU_MASK,
    RC_CLOCK_TOWN_EAST_UPPER_CHEST,
    RC_CLOCK_TOWN_GREAT_FAIRY_ALT,
    RC_CLOCK_TOWN_GREAT_FAIRY,
    RC_CLOCK_TOWN_LAUNDRY_GURU_GURU,
    RC_CLOCK_TOWN_NORTH_TREE_HP,
    RC_CLOCK_TOWN_SCRUB_DEED,
    RC_CLOCK_TOWN_SONG_OF_HEALING,
    RC_CLOCK_TOWN_SOUTH_CHEST_LOWER,
    RC_CLOCK_TOWN_SOUTH_CHEST_UPPER,
    RC_CLOCK_TOWN_SOUTH_PLATFORM_HP,
    RC_CLOCK_TOWN_STRAY_FAIRY,
    RC_DEKU_PALACE_GROTTO_CHEST,
    RC_DEKU_PALACE_HP,
    RC_DOGGY_RACETRACK_CHEST,
    RC_GORON_VILLAGE_HP,
    RC_GORON_VILLAGE_SCRUB_BOMB_BAG,
    RC_GORON_VILLAGE_SCRUB_DEED,
    RC_GREAT_BAY_COAST_FISHERMAN_GROTTO,
    RC_GREAT_BAY_COAST_HP,
    RC_GREAT_BAY_GREAT_FAIRY,
    RC_GREAT_BAY_TEMPLE_BABA_CHEST,
    RC_GREAT_BAY_TEMPLE_BOSS_HC,
    RC_GREAT_BAY_TEMPLE_BOSS_KEY,
    RC_GREAT_BAY_TEMPLE_COMPASS_ROOM_UNDERWATER,
    RC_GREAT_BAY_TEMPLE_COMPASS,
    RC_GREAT_BAY_TEMPLE_ENTRANCE_CHEST,
    RC_GREAT_BAY_TEMPLE_GREEN_PIPE_1_CHEST,
    RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_LOWER_CHEST,
    RC_GREAT_BAY_TEMPLE_GREEN_PIPE_2_UPPER_CHEST,
    RC_GREAT_BAY_TEMPLE_GREEN_PIPE_3_CHEST,
    RC_GREAT_BAY_TEMPLE_ICE_ARROW,
    RC_GREAT_BAY_TEMPLE_MAP,
    RC_GREAT_BAY_TEMPLE_SF_CENTRAL_ROOM_BARREL,
    RC_GREAT_BAY_TEMPLE_SF_CENTRAL_ROOM_UNDERWATER_POT,
    RC_GREAT_BAY_TEMPLE_SF_COMPASS_ROOM_POT,
    RC_GREAT_BAY_TEMPLE_SF_GREEN_PIPE_3_BARREL,
    RC_GREAT_BAY_TEMPLE_SF_MAP_ROOM_POT,
    RC_GREAT_BAY_TEMPLE_SF_PRE_BOSS_ABOVE_WATER,
    RC_GREAT_BAY_TEMPLE_SF_PRE_BOSS_UNDERWATER,
    RC_GREAT_BAY_TEMPLE_SF_WATER_WHEEL_PLATFORM,
    RC_GREAT_BAY_TEMPLE_SF_WATER_WHEEL_SKULLTULA,
    RC_IKANA_CANYON_GROTTO,
    RC_IKANA_CANYON_SCRUB_HP,
    RC_IKANA_CANYON_SCRUB_HUGE_RUPEE,
    RC_IKANA_CANYON_SCRUB_POTION_REFILL,
    RC_IKANA_GRAVEYARD_CAPTAIN_MASK,
    RC_IKANA_GRAVEYARD_GROTTO,
    RC_IKANA_GREAT_FAIRY,
    RC_KAFEIS_HIDEOUT_PENDANT_OF_MEMORIES,
    RC_LONE_PEAK_SHRINE_BOULDER_CHEST,
    RC_LONE_PEAK_SHRINE_INVISIBLE_CHEST,
    RC_LONE_PEAK_SHRINE_LENS_CHEST,
    RC_MAYORS_OFFICE_KAFEIS_MASK,
    RC_MOON_TRIAL_DEKU_HP,
    RC_MOON_TRIAL_GORON_HP,
    RC_MOON_TRIAL_LINK_GARO_MASTER_CHEST,
    RC_MOON_TRIAL_LINK_HP,
    RC_MOON_TRIAL_LINK_IRON_KNUCKLE_CHEST,
    RC_MOON_TRIAL_ZORA_HP,
    RC_MOUNTAIN_VILLAGE_TUNNEL_GROTTO,
    RC_MOUNTAIN_VILLAGE_WATERFALL_CHEST,
    RC_OCEAN_SKULLTULA_2ND_ROOM_BEHIND_SKULL_1,
    RC_OCEAN_SKULLTULA_2ND_ROOM_BEHIND_SKULL_2,
    RC_OCEAN_SKULLTULA_2ND_ROOM_CEILING_EDGE,
    RC_OCEAN_SKULLTULA_2ND_ROOM_CEILING_PLANK,
    RC_OCEAN_SKULLTULA_2ND_ROOM_JAR,
    RC_OCEAN_SKULLTULA_2ND_ROOM_LOWER_POT,
    RC_OCEAN_SKULLTULA_2ND_ROOM_UPPER_POT,
    RC_OCEAN_SKULLTULA_2ND_ROOM_WEBBED_HOLE,
    RC_OCEAN_SKULLTULA_2ND_ROOM_WEBBED_POT,
    RC_OCEAN_SKULLTULA_COLORED_SKULLS_BEHIND_PICTURE,
    RC_OCEAN_SKULLTULA_COLORED_SKULLS_CEILING_EDGE,
    RC_OCEAN_SKULLTULA_COLORED_SKULLS_CHANDELIER_1,
    RC_OCEAN_SKULLTULA_COLORED_SKULLS_CHANDELIER_2,
    RC_OCEAN_SKULLTULA_COLORED_SKULLS_CHANDELIER_3,
    RC_OCEAN_SKULLTULA_COLORED_SKULLS_POT,
    RC_OCEAN_SKULLTULA_ENTRANCE_LEFT_WALL,
    RC_OCEAN_SKULLTULA_ENTRANCE_RIGHT_WALL,
    RC_OCEAN_SKULLTULA_ENTRANCE_WEB,
    RC_OCEAN_SKULLTULA_LIBRARY_BEHIND_BOOKCASE_1,
    RC_OCEAN_SKULLTULA_LIBRARY_BEHIND_BOOKCASE_2,
    RC_OCEAN_SKULLTULA_LIBRARY_BEHIND_PICTURE,
    RC_OCEAN_SKULLTULA_LIBRARY_CEILING_EDGE,
    RC_OCEAN_SKULLTULA_LIBRARY_HOLE_BEHIND_CABINET,
    RC_OCEAN_SKULLTULA_LIBRARY_HOLE_BEHIND_PICTURE,
    RC_OCEAN_SKULLTULA_LIBRARY_ON_CORNER_BOOKSHELF,
    RC_OCEAN_SKULLTULA_STORAGE_ROOM_BEHIND_BOAT,
    RC_OCEAN_SKULLTULA_STORAGE_ROOM_BEHIND_CRATE,
    RC_OCEAN_SKULLTULA_STORAGE_ROOM_CEILING_WEB,
    RC_OCEAN_SKULLTULA_STORAGE_ROOM_CRATE,
    RC_OCEAN_SKULLTULA_STORAGE_ROOM_JAR,
    RC_OCEAN_SPIDER_HOUSE_CHEST_HP,
    RC_PATH_TO_SNOWHEAD_GROTTO,
    RC_PATH_TO_SNOWHEAD_HP,
    RC_PINNACLE_ROCK_CHEST_1,
    RC_PINNACLE_ROCK_CHEST_2,
    RC_PIRATE_FORTRESS_ENTRANCE_CHEST_1,
    RC_PIRATE_FORTRESS_ENTRANCE_CHEST_2,
    RC_PIRATE_FORTRESS_ENTRANCE_CHEST_3,
    RC_PIRATE_FORTRESS_INTERIOR_AQUARIUM,
    RC_PIRATE_FORTRESS_INTERIOR_HOOKSHOT,
    RC_PIRATE_FORTRESS_INTERIOR_SEWERS_CHEST_1,
    RC_PIRATE_FORTRESS_INTERIOR_SEWERS_CHEST_2,
    RC_PIRATE_FORTRESS_INTERIOR_SEWERS_CHEST_3,
    RC_PIRATE_FORTRESS_INTERIOR_SEWERS_HP,
    RC_PIRATE_FORTRESS_INTERIOR_SILVER_RUPEE_CHEST,
    RC_PIRATE_FORTRESS_PLAZA_HEART_1,
    RC_PIRATE_FORTRESS_PLAZA_HEART_2,
    RC_PIRATE_FORTRESS_PLAZA_HEART_3,
    RC_PIRATE_FORTRESS_PLAZA_LOWER_CHEST,
    RC_PIRATE_FORTRESS_PLAZA_UPPER_CHEST,
    RC_ROAD_TO_IKANA_CHEST,
    RC_ROAD_TO_IKANA_GROTTO,
    RC_ROAD_TO_SOUTHERN_SWAMP_GROTTO,
    RC_ROAD_TO_SOUTHERN_SWAMP_HP,
    RC_SECRET_SHRINE_DINALFOS_CHEST,
    RC_SECRET_SHRINE_GARO_MASTER_CHEST,
    RC_SECRET_SHRINE_HP_CHEST,
    RC_SECRET_SHRINE_WART_CHEST,
    RC_SECRET_SHRINE_WIZZROBE_CHEST,
    RC_SNOWHEAD_GREAT_FAIRY,
    RC_SNOWHEAD_TEMPLE_BLOCK_ROOM_LEDGE_CHEST,
    RC_SNOWHEAD_TEMPLE_BLOCK_ROOM,
    RC_SNOWHEAD_TEMPLE_BOSS_HC,
    RC_SNOWHEAD_TEMPLE_BOSS_KEY,
    RC_SNOWHEAD_TEMPLE_BRIDGE_ROOM,
    RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_ALCOVE_CHEST,
    RC_SNOWHEAD_TEMPLE_CENTRAL_ROOM_BOTTOM_CHEST,
    RC_SNOWHEAD_TEMPLE_COMPASS_ROOM_LEDGE_CHEST,
    RC_SNOWHEAD_TEMPLE_COMPASS,
    RC_SNOWHEAD_TEMPLE_FIRE_ARROW,
    RC_SNOWHEAD_TEMPLE_ICICLE_ROOM_ALCOVE_CHEST,
    RC_SNOWHEAD_TEMPLE_ICICLE_ROOM,
    RC_SNOWHEAD_TEMPLE_MAP_ALCOVE_CHEST,
    RC_SNOWHEAD_TEMPLE_MAP,
    RC_SNOWHEAD_TEMPLE_PILLARS_ROOM_CHEST,
    RC_SNOWHEAD_TEMPLE_SF_BRIDGE_PILLAR,
    RC_SNOWHEAD_TEMPLE_SF_BRIDGE_UNDER_PLATFORM,
    RC_SNOWHEAD_TEMPLE_SF_COMPASS_ROOM_CRATE,
    RC_SNOWHEAD_TEMPLE_SF_DINOLFOS_1,
    RC_SNOWHEAD_TEMPLE_SF_DINOLFOS_2,
    RC_SNOWHEAD_TEMPLE_SF_DUAL_SWITCHES,
    RC_SNOWHEAD_TEMPLE_SF_MAP_ROOM,
    RC_SNOWHEAD_TEMPLE_SF_SNOW_ROOM,
    RC_SOUTHERN_SWAMP_GROTTO,
    RC_SOUTHERN_SWAMP_HP,
    RC_SOUTHERN_SWAMP_SCRUB_BEANS,
    RC_SOUTHERN_SWAMP_SCRUB_DEED,
    RC_STOCK_POT_INN_GRANDMA_LONG_STORY,
    RC_STOCK_POT_INN_GRANDMA_SHORT_STORY,
    RC_STOCK_POT_INN_GUEST_ROOM_CHEST,
    RC_STOCK_POT_INN_LETTER_TO_KAFEI,
    RC_STOCK_POT_INN_ROOM_KEY,
    RC_STOCK_POT_INN_STAFF_ROOM_CHEST,
    RC_STONE_TOWER_INVERTED_CHEST_1,
    RC_STONE_TOWER_INVERTED_CHEST_2,
    RC_STONE_TOWER_INVERTED_CHEST_3,
    RC_STONE_TOWER_TEMPLE_BEFORE_WATER_BRIDGE_CHEST,
    RC_STONE_TOWER_TEMPLE_CENTER_ACROSS_WATER_CHEST,
    RC_STONE_TOWER_TEMPLE_CENTER_SUN_BLOCK_CHEST,
    RC_STONE_TOWER_TEMPLE_COMPASS,
    RC_STONE_TOWER_TEMPLE_ENTRANCE_CHEST,
    RC_STONE_TOWER_TEMPLE_ENTRANCE_SWITCH_CHEST,
    RC_STONE_TOWER_TEMPLE_INVERTED_BOSS_HC,
    RC_STONE_TOWER_TEMPLE_INVERTED_BOSS_KEY,
    RC_STONE_TOWER_TEMPLE_INVERTED_DEATH_ARMOS_CHEST,
    RC_STONE_TOWER_TEMPLE_INVERTED_EAST_LOWER_CHEST,
    RC_STONE_TOWER_TEMPLE_INVERTED_EAST_MIDDLE_CHEST,
    RC_STONE_TOWER_TEMPLE_INVERTED_EAST_UPPER_CHEST,
    RC_STONE_TOWER_TEMPLE_INVERTED_ENTRANCE_CHEST,
    RC_STONE_TOWER_TEMPLE_INVERTED_GIANT_MASK,
    RC_STONE_TOWER_TEMPLE_INVERTED_WIZZROBE_CHEST,
    RC_STONE_TOWER_TEMPLE_LIGHT_ARROW,
    RC_STONE_TOWER_TEMPLE_MAP,
    RC_STONE_TOWER_TEMPLE_MIRRORS_ROOM_CENTER_CHEST,
    RC_STONE_TOWER_TEMPLE_MIRRORS_ROOM_RIGHT_CHEST,
    RC_STONE_TOWER_TEMPLE_UNDER_WEST_GARDEN_LAVA_CHEST,
    RC_STONE_TOWER_TEMPLE_UNDER_WEST_GARDEN_LEDGE_CHEST,
    RC_STONE_TOWER_TEMPLE_WATER_BRIDGE_CHEST,
    RC_STONE_TOWER_TEMPLE_WATER_SUN_SWITCH_CHEST,
    RC_STONE_TOWER_TEMPLE_WIND_ROOM_JAIL_CHEST,
    RC_STONE_TOWER_TEMPLE_WIND_ROOM_LEDGE_CHEST,
    RC_SWAMP_SKULLTULA_GOLD_ROOM_HIVE,
    RC_SWAMP_SKULLTULA_GOLD_ROOM_NEAR_CEILING,
    RC_SWAMP_SKULLTULA_GOLD_ROOM_PILLAR,
    RC_SWAMP_SKULLTULA_GOLD_ROOM_WALL,
    RC_SWAMP_SKULLTULA_MAIN_ROOM_JAR,
    RC_SWAMP_SKULLTULA_MAIN_ROOM_LOWER_LEFT_SOFT_SOIL,
    RC_SWAMP_SKULLTULA_MAIN_ROOM_LOWER_RIGHT_SOFT_SOIL,
    RC_SWAMP_SKULLTULA_MAIN_ROOM_NEAR_CEILING,
    RC_SWAMP_SKULLTULA_MAIN_ROOM_PILLAR,
    RC_SWAMP_SKULLTULA_MAIN_ROOM_UPPER_PILLAR,
    RC_SWAMP_SKULLTULA_MAIN_ROOM_UPPER_SOFT_SOIL,
    RC_SWAMP_SKULLTULA_MAIN_ROOM_WATER,
    RC_SWAMP_SKULLTULA_MONUMENT_ROOM_CRATE_1,
    RC_SWAMP_SKULLTULA_MONUMENT_ROOM_CRATE_2,
    RC_SWAMP_SKULLTULA_MONUMENT_ROOM_LOWER_WALL,
    RC_SWAMP_SKULLTULA_MONUMENT_ROOM_ON_MONUMENT,
    RC_SWAMP_SKULLTULA_MONUMENT_ROOM_TORCH,
    RC_SWAMP_SKULLTULA_POT_ROOM_BEHIND_VINES,
    RC_SWAMP_SKULLTULA_POT_ROOM_HIVE_1,
    RC_SWAMP_SKULLTULA_POT_ROOM_HIVE_2,
    RC_SWAMP_SKULLTULA_POT_ROOM_JAR,
    RC_SWAMP_SKULLTULA_POT_ROOM_POT_1,
    RC_SWAMP_SKULLTULA_POT_ROOM_POT_2,
    RC_SWAMP_SKULLTULA_POT_ROOM_WALL,
    RC_SWAMP_SKULLTULA_TREE_ROOM_GRASS_1,
    RC_SWAMP_SKULLTULA_TREE_ROOM_GRASS_2,
    RC_SWAMP_SKULLTULA_TREE_ROOM_HIVE,
    RC_SWAMP_SKULLTULA_TREE_ROOM_TREE_1,
    RC_SWAMP_SKULLTULA_TREE_ROOM_TREE_2,
    RC_SWAMP_SKULLTULA_TREE_ROOM_TREE_3,
    RC_SWORDSMAN_SCHOOL_HP,
    RC_TERMINA_FIELD_BIO_BABA_GROTTO,
    RC_TERMINA_FIELD_DODONGO_GROTTO,
    RC_TERMINA_FIELD_PEAHAT_GROTTO,
    RC_TERMINA_FIELD_PILLAR_GROTTO,
    RC_TERMINA_FIELD_TALL_GRASS_CHEST,
    RC_TERMINA_FIELD_TALL_GRASS_GROTTO,
    RC_TERMINA_FIELD_TREE_STUMP_CHEST,
    RC_TERMINA_FIELD_WATER_CHEST,
    RC_TWIN_ISLANDS_FROZEN_GROTTO_CHEST,
    RC_TWIN_ISLANDS_RAMP_GROTTO_CHEST,
    RC_TWIN_ISLANDS_UNDERWATER_CHEST_1,
    RC_TWIN_ISLANDS_UNDERWATER_CHEST_2,
    RC_WOODFALL_ENTRANCE_CHEST,
    RC_WOODFALL_GREAT_FAIRY,
    RC_WOODFALL_HP_CHEST,
    RC_WOODFALL_NEAR_OWL_CHEST,
    RC_WOODFALL_TEMPLE_BOSS_CONTAINER,
    RC_WOODFALL_TEMPLE_BOSS_KEY_CHEST,
    RC_WOODFALL_TEMPLE_BOW,
    RC_WOODFALL_TEMPLE_CENTER_CHEST,
    RC_WOODFALL_TEMPLE_COMPASS,
    RC_WOODFALL_TEMPLE_DARK_CHEST,
    RC_WOODFALL_TEMPLE_ENTRANCE_CHEST,
    RC_WOODFALL_TEMPLE_MAP,
    RC_WOODFALL_TEMPLE_SF_ENTRANCE,
    RC_WOODFALL_TEMPLE_SF_MAIN_BUBBLE,
    RC_WOODFALL_TEMPLE_SF_MAIN_DEKU_BABA,
    RC_WOODFALL_TEMPLE_SF_MAIN_POT,
    RC_WOODFALL_TEMPLE_SF_MAZE_BEEHIVE,
    RC_WOODFALL_TEMPLE_SF_MAZE_BUBBLE,
    RC_WOODFALL_TEMPLE_SF_MAZE_SKULLTULA,
    RC_WOODFALL_TEMPLE_SF_PRE_BOSS_BOTTOM_RIGHT,
    RC_WOODFALL_TEMPLE_SF_PRE_BOSS_LEFT,
    RC_WOODFALL_TEMPLE_SF_PRE_BOSS_PILLAR,
    RC_WOODFALL_TEMPLE_SF_PRE_BOSS_TOP_RIGHT,
    RC_WOODFALL_TEMPLE_SF_WATER_ROOM_BEEHIVE,
    RC_WOODFALL_TEMPLE_WATER_CHEST,
    RC_WOODS_OF_MYSTERY_GROTTO,
    RC_ZORA_CAPE_GROTTO,
    RC_ZORA_CAPE_LEDGE_CHEST_1,
    RC_ZORA_CAPE_LEDGE_CHEST_2,
    RC_ZORA_CAPE_UNDERWATER_CHEST,
    RC_ZORA_CAPE_WATERFALL_HP,
    RC_ZORA_HALL_SCRUB_DEED,
    RC_ZORA_HALL_SCRUB_HP,
    RC_ZORA_HALL_SCRUB_POTION_REFILL,

    // --- Shops --- //
    RC_TRADING_POST_SHOP_ITEM_1,
    RC_TRADING_POST_SHOP_ITEM_2,
    RC_TRADING_POST_SHOP_ITEM_3,
    RC_TRADING_POST_SHOP_ITEM_4,
    RC_TRADING_POST_SHOP_ITEM_5,
    RC_TRADING_POST_SHOP_ITEM_6,
    RC_TRADING_POST_SHOP_ITEM_7,
    RC_TRADING_POST_SHOP_ITEM_8,
    RC_BOMB_SHOP_ITEM_1,
    RC_BOMB_SHOP_ITEM_2,
    RC_BOMB_SHOP_ITEM_3,
    RC_HAGS_POTION_SHOP_ITEM_1,
    RC_HAGS_POTION_SHOP_ITEM_2,
    RC_HAGS_POTION_SHOP_ITEM_3,
    RC_GORON_SHOP_ITEM_1,
    RC_GORON_SHOP_ITEM_2,
    RC_GORON_SHOP_ITEM_3,
    RC_ZORA_SHOP_ITEM_1,
    RC_ZORA_SHOP_ITEM_2,
    RC_ZORA_SHOP_ITEM_3,

    // --- Pots --- //
    RC_ANCIENT_CASTLE_OF_IKANA_BOSS_POT_1,
    RC_ANCIENT_CASTLE_OF_IKANA_BOSS_POT_2,
    RC_ANCIENT_CASTLE_OF_IKANA_BOSS_POT_3,
    RC_ANCIENT_CASTLE_OF_IKANA_BOSS_POT_4,
    RC_ANCIENT_CASTLE_OF_IKANA_BOSS_POT_5,
    RC_ANCIENT_CASTLE_OF_IKANA_BOSS_POT_6,
    RC_ANCIENT_CASTLE_OF_IKANA_BOSS_POT_7,
    RC_ANCIENT_CASTLE_OF_IKANA_BOSS_POT_8,
    RC_ANCIENT_CASTLE_OF_IKANA_POT_ENTRANCE_1,
    RC_ANCIENT_CASTLE_OF_IKANA_POT_ENTRANCE_2,
    RC_ANCIENT_CASTLE_OF_IKANA_POT_EXTERIOR,
    RC_ANCIENT_CASTLE_OF_IKANA_POT_LEFT_FIRST_ROOM_1,
    RC_ANCIENT_CASTLE_OF_IKANA_POT_LEFT_FIRST_ROOM_2,
    RC_ANCIENT_CASTLE_OF_IKANA_POT_LEFT_SECOND_ROOM_1,
    RC_ANCIENT_CASTLE_OF_IKANA_POT_LEFT_SECOND_ROOM_2,
    RC_ANCIENT_CASTLE_OF_IKANA_POT_LEFT_SECOND_ROOM_3,
    RC_ANCIENT_CASTLE_OF_IKANA_POT_LEFT_SECOND_ROOM_4,
    RC_ANCIENT_CASTLE_OF_IKANA_POT_LEFT_THIRD_ROOM_1,
    RC_ANCIENT_CASTLE_OF_IKANA_POT_LEFT_THIRD_ROOM_2,
    RC_ANCIENT_CASTLE_OF_IKANA_POT_RIGHT_1,
    RC_ANCIENT_CASTLE_OF_IKANA_POT_RIGHT_2,
    RC_ASTRAL_OBSERVATORY_PASSAGE_POT_1,
    RC_ASTRAL_OBSERVATORY_PASSAGE_POT_2,
    RC_ASTRAL_OBSERVATORY_PASSAGE_POT_3,
    RC_ASTRAL_OBSERVATORY_PASSAGE_POT_4,
    RC_ASTRAL_OBSERVATORY_POT_1,
    RC_ASTRAL_OBSERVATORY_POT_2,
    RC_ASTRAL_OBSERVATORY_POT_3,
    RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_1,
    RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_10,
    RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_2,
    RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_3,
    RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_4,
    RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_5,
    RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_6,
    RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_7,
    RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_8,
    RC_BENEATH_THE_GRAVEYARD_POT_DAMPE_9,
    RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_AFTER_PIT_1,
    RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_AFTER_PIT_2,
    RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_AFTER_PIT_3,
    RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_AFTER_PIT_4,
    RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_BATS_1,
    RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_BATS_2,
    RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_BATS_3,
    RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_BEFORE_PIT_1,
    RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_BEFORE_PIT_2,
    RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_EARLY_1,
    RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_EARLY_2,
    RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_2_EARLY,
    RC_BENEATH_THE_WELL_POT_BIG_POE_1,
    RC_BENEATH_THE_WELL_POT_BIG_POE_2,
    RC_BENEATH_THE_WELL_POT_BIG_POE_3,
    RC_BENEATH_THE_WELL_POT_BIG_POE_4,
    RC_BENEATH_THE_WELL_POT_LEFT_SIDE_1,
    RC_BENEATH_THE_WELL_POT_LEFT_SIDE_2,
    RC_BENEATH_THE_WELL_POT_LEFT_SIDE_3,
    RC_BENEATH_THE_WELL_POT_LEFT_SIDE_4,
    RC_BENEATH_THE_WELL_POT_LEFT_SIDE_5,
    RC_BENEATH_THE_WELL_POT_MIDDLE_1,
    RC_BENEATH_THE_WELL_POT_MIDDLE_2,
    RC_BENEATH_THE_WELL_POT_MIDDLE_3,
    RC_BENEATH_THE_WELL_POT_MIDDLE_4,
    RC_BENEATH_THE_WELL_POT_MIDDLE_5,
    RC_BENEATH_THE_WELL_POT_MIDDLE_6,
    RC_BENEATH_THE_WELL_POT_MIDDLE_7,
    RC_BENEATH_THE_WELL_POT_MIDDLE_8,
    RC_BENEATH_THE_WELL_POT_MIDDLE_9,
    RC_BENEATH_THE_WELL_POT_MIDDLE_10,
    RC_CLOCK_TOWER_ROOF_POT_1,
    RC_CLOCK_TOWER_ROOF_POT_2,
    RC_CLOCK_TOWER_ROOF_POT_3,
    RC_CLOCK_TOWER_ROOF_POT_4,
    RC_CLOCK_TOWN_WEST_TRADING_POST_POT,
    RC_DEKU_PALACE_POT_1,
    RC_DEKU_PALACE_POT_2,
    RC_DEKU_SHRINE_POT_1,
    // RC_DEKU_SHRINE_POT_2,
    RC_DOGGY_RACETRACK_POT_1,
    RC_DOGGY_RACETRACK_POT_2,
    RC_DOGGY_RACETRACK_POT_3,
    RC_DOGGY_RACETRACK_POT_4,
    RC_GORON_RACETRACK_POT_1,
    RC_GORON_RACETRACK_POT_10,
    RC_GORON_RACETRACK_POT_11,
    RC_GORON_RACETRACK_POT_12,
    RC_GORON_RACETRACK_POT_13,
    RC_GORON_RACETRACK_POT_14,
    RC_GORON_RACETRACK_POT_15,
    RC_GORON_RACETRACK_POT_16,
    RC_GORON_RACETRACK_POT_17,
    RC_GORON_RACETRACK_POT_18,
    RC_GORON_RACETRACK_POT_19,
    RC_GORON_RACETRACK_POT_2,
    RC_GORON_RACETRACK_POT_20,
    RC_GORON_RACETRACK_POT_21,
    RC_GORON_RACETRACK_POT_22,
    RC_GORON_RACETRACK_POT_23,
    RC_GORON_RACETRACK_POT_24,
    RC_GORON_RACETRACK_POT_25,
    RC_GORON_RACETRACK_POT_26,
    RC_GORON_RACETRACK_POT_27,
    RC_GORON_RACETRACK_POT_28,
    RC_GORON_RACETRACK_POT_29,
    RC_GORON_RACETRACK_POT_3,
    RC_GORON_RACETRACK_POT_30,
    RC_GORON_RACETRACK_POT_4,
    RC_GORON_RACETRACK_POT_5,
    RC_GORON_RACETRACK_POT_6,
    RC_GORON_RACETRACK_POT_7,
    RC_GORON_RACETRACK_POT_8,
    RC_GORON_RACETRACK_POT_9,
    RC_GORON_SHRINE_POT_1,
    RC_GORON_SHRINE_POT_10,
    RC_GORON_SHRINE_POT_11,
    RC_GORON_SHRINE_POT_2,
    RC_GORON_SHRINE_POT_3,
    RC_GORON_SHRINE_POT_4,
    RC_GORON_SHRINE_POT_5,
    RC_GORON_SHRINE_POT_6,
    RC_GORON_SHRINE_POT_7,
    RC_GORON_SHRINE_POT_8,
    RC_GORON_SHRINE_POT_9,
    RC_GREAT_BAY_COAST_POT_1,
    RC_GREAT_BAY_COAST_POT_10,
    RC_GREAT_BAY_COAST_POT_11,
    RC_GREAT_BAY_COAST_POT_12,
    RC_GREAT_BAY_COAST_POT_2,
    RC_GREAT_BAY_COAST_POT_3,
    RC_GREAT_BAY_COAST_POT_4,
    RC_GREAT_BAY_COAST_POT_5,
    RC_GREAT_BAY_COAST_POT_6,
    RC_GREAT_BAY_COAST_POT_7,
    RC_GREAT_BAY_COAST_POT_8,
    RC_GREAT_BAY_COAST_POT_9,
    RC_GREAT_BAY_COAST_POT_LEDGE_1,
    RC_GREAT_BAY_COAST_POT_LEDGE_2,
    RC_GREAT_BAY_COAST_POT_LEDGE_3,
    RC_GREAT_BAY_TEMPLE_BOSS_POT_1,
    RC_GREAT_BAY_TEMPLE_BOSS_POT_2,
    RC_GREAT_BAY_TEMPLE_BOSS_POT_3,
    RC_GREAT_BAY_TEMPLE_BOSS_POT_4,
    RC_GREAT_BAY_TEMPLE_BOSS_POT_UNDERWATER_1,
    RC_GREAT_BAY_TEMPLE_BOSS_POT_UNDERWATER_2,
    RC_GREAT_BAY_TEMPLE_BOSS_POT_UNDERWATER_3,
    RC_GREAT_BAY_TEMPLE_BOSS_POT_UNDERWATER_4,
    RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_1,
    RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_2,
    RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_3,
    RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_4,
    RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_5,
    RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_6,
    RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_7,
    RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_8,
    RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_9,
    RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_10,
    RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_11,
    RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_12,
    RC_GREAT_BAY_TEMPLE_POT_CENTRAL_ROOM_1,
    RC_GREAT_BAY_TEMPLE_POT_CENTRAL_ROOM_2,
    RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_SURFACE_1,
    RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_SURFACE_2,
    RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_SURFACE_3,
    RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_SURFACE_4,
    RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_WATER_1,
    RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_WATER_2,
    RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_WATER_3,
    RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_1_1,
    RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_1_2,
    RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_1_3,
    RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_1_4,
    RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_1,
    RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_2,
    RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_3,
    RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_4,
    RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_5,
    RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_6,
    RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_7,
    RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_8,
    RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_3_LOWER,
    RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_3_UPPER_1,
    RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_3_UPPER_2,
    RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_SURFACE_1,
    RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_SURFACE_2,
    RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_SURFACE_3,
    RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_1,
    RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_2,
    RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_3,
    RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_4,
    RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_5,
    RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_6,
    RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_7,
    RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_8,
    RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_1,
    RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_2,
    RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_3,
    RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_4,
    RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_5,
    RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_6,
    RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_7,
    RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_8,
    RC_GREAT_BAY_TEMPLE_POT_RED_PIPE_BEFORE_WART_1,
    RC_GREAT_BAY_TEMPLE_POT_RED_PIPE_BEFORE_WART_2,
    RC_GREAT_BAY_TEMPLE_POT_RED_PIPE_BEFORE_WART_3,
    RC_GREAT_BAY_TEMPLE_POT_RED_PIPE_BEFORE_WART_4,
    RC_GREAT_BAY_TEMPLE_POT_WART_1,
    RC_GREAT_BAY_TEMPLE_POT_WART_2,
    RC_GREAT_BAY_TEMPLE_POT_WART_3,
    RC_GREAT_BAY_TEMPLE_POT_WART_4,
    RC_GREAT_BAY_TEMPLE_POT_WART_5,
    RC_GREAT_BAY_TEMPLE_POT_WART_6,
    RC_GREAT_BAY_TEMPLE_POT_WART_7,
    RC_GREAT_BAY_TEMPLE_POT_WART_8,
    RC_MOON_MAJORA_POT_1,
    RC_MOON_MAJORA_POT_2,
    RC_MOON_TRIAL_GORON_POT_1,
    RC_MOON_TRIAL_GORON_POT_2,
    RC_MOON_TRIAL_GORON_POT_3,
    RC_MOON_TRIAL_GORON_POT_4,
    RC_MOON_TRIAL_GORON_POT_5,
    RC_MOON_TRIAL_GORON_POT_6,
    RC_MOON_TRIAL_GORON_POT_7,
    RC_MOON_TRIAL_GORON_POT_8,
    RC_MOON_TRIAL_GORON_POT_9,
    RC_MOON_TRIAL_GORON_POT_10,
    RC_MOON_TRIAL_GORON_POT_11,
    RC_MOON_TRIAL_GORON_POT_EARLY_1,
    RC_MOON_TRIAL_GORON_POT_EARLY_2,
    RC_MOON_TRIAL_GORON_POT_EARLY_3,
    RC_MOON_TRIAL_GORON_POT_EARLY_4,
    RC_MOON_TRIAL_LINK_POT_1,
    RC_MOON_TRIAL_LINK_POT_2,
    RC_MOON_TRIAL_LINK_POT_3,
    RC_MOON_TRIAL_LINK_POT_4,
    RC_MOON_TRIAL_LINK_POT_5,
    RC_MOON_TRIAL_LINK_POT_6,
    RC_MOON_TRIAL_LINK_POT_7,
    RC_MOON_TRIAL_LINK_POT_8,
    RC_MOUNTAIN_VILLAGE_POT,
    RC_OCEAN_SPIDER_HOUSE_POT_COLORED_SKULLS_1,
    RC_OCEAN_SPIDER_HOUSE_POT_COLORED_SKULLS_2,
    RC_OCEAN_SPIDER_HOUSE_POT_ENTRANCE_1,
    RC_OCEAN_SPIDER_HOUSE_POT_ENTRANCE_2,
    RC_OCEAN_SPIDER_HOUSE_POT_ENTRANCE_3,
    RC_OCEAN_SPIDER_HOUSE_POT_ENTRANCE_4,
    RC_OCEAN_SPIDER_HOUSE_POT_MAIN_ROOM_1,
    RC_OCEAN_SPIDER_HOUSE_POT_MAIN_ROOM_2,
    RC_OCEAN_SPIDER_HOUSE_POT_MAIN_ROOM_BOE,
    RC_OCEAN_SPIDER_HOUSE_POT_MAIN_ROOM_WEB,
    RC_OCEAN_SPIDER_HOUSE_POT_STORAGE_1,
    RC_OCEAN_SPIDER_HOUSE_POT_STORAGE_2,
    RC_OCEAN_SPIDER_HOUSE_POT_STORAGE_3,
    RC_OCEAN_SPIDER_HOUSE_POT_STORAGE_4,
    RC_OCEAN_SPIDER_HOUSE_POT_STORAGE_TOP_1,
    RC_OCEAN_SPIDER_HOUSE_POT_STORAGE_TOP_2,
    RC_OCEAN_SPIDER_HOUSE_POT_STORAGE_TOP_3,
    RC_PINNACLE_ROCK_POT_1,
    RC_PINNACLE_ROCK_POT_2,
    RC_PINNACLE_ROCK_POT_3,
    RC_PINNACLE_ROCK_POT_4,
    RC_PINNACLE_ROCK_POT_5,
    RC_PINNACLE_ROCK_POT_6,
    RC_PINNACLE_ROCK_POT_7,
    RC_PINNACLE_ROCK_POT_8,
    RC_PINNACLE_ROCK_POT_9,
    RC_PINNACLE_ROCK_POT_10,
    RC_PINNACLE_ROCK_POT_11,
    RC_PIRATE_FORTRESS_INTERIOR_POT_BARREL_MAZE_1,
    RC_PIRATE_FORTRESS_INTERIOR_POT_BARREL_MAZE_2,
    RC_PIRATE_FORTRESS_INTERIOR_POT_BARREL_MAZE_3,
    RC_PIRATE_FORTRESS_INTERIOR_POT_BEEHIVE_1,
    RC_PIRATE_FORTRESS_INTERIOR_POT_BEEHIVE_2,
    RC_PIRATE_FORTRESS_INTERIOR_POT_CHEST_AQUARIUM_1,
    RC_PIRATE_FORTRESS_INTERIOR_POT_CHEST_AQUARIUM_2,
    RC_PIRATE_FORTRESS_INTERIOR_POT_CHEST_AQUARIUM_3,
    RC_PIRATE_FORTRESS_INTERIOR_POT_GUARDED_1,
    RC_PIRATE_FORTRESS_INTERIOR_POT_GUARDED_2,
    RC_PIRATE_FORTRESS_SEWERS_POT_END_1,
    RC_PIRATE_FORTRESS_SEWERS_POT_END_2,
    RC_PIRATE_FORTRESS_SEWERS_POT_END_3,
    RC_PIRATE_FORTRESS_SEWERS_POT_HEART_PIECE_ROOM_1,
    // RC_PIRATE_FORTRESS_SEWERS_POT_HEART_PIECE_ROOM_2,
    // RC_PIRATE_FORTRESS_SEWERS_POT_WATERWAY_1,
    RC_PIRATE_FORTRESS_SEWERS_POT_WATERWAY_2,
    RC_ROAD_TO_IKANA_POT,
    RC_SAKON_HIDEOUT_POT_FIRST_ROOM_1,
    RC_SAKON_HIDEOUT_POT_FIRST_ROOM_2,
    RC_SAKON_HIDEOUT_POT_SECOND_ROOM_1,
    RC_SAKON_HIDEOUT_POT_SECOND_ROOM_2,
    RC_SAKON_HIDEOUT_POT_THIRD_ROOM,
    RC_SECRET_SHRINE_POT_1,
    RC_SECRET_SHRINE_POT_2,
    RC_SECRET_SHRINE_POT_3,
    RC_SECRET_SHRINE_POT_4,
    RC_SECRET_SHRINE_POT_5,
    RC_SECRET_SHRINE_POT_6,
    RC_SECRET_SHRINE_POT_7,
    RC_SECRET_SHRINE_POT_8,
    RC_SECRET_SHRINE_POT_9,
    RC_SNOWHEAD_TEMPLE_BOSS_POT_1,
    RC_SNOWHEAD_TEMPLE_BOSS_POT_2,
    RC_SNOWHEAD_TEMPLE_BOSS_POT_3,
    RC_SNOWHEAD_TEMPLE_BOSS_POT_4,
    RC_SNOWHEAD_TEMPLE_BOSS_POT_5,
    RC_SNOWHEAD_TEMPLE_BOSS_POT_6,
    RC_SNOWHEAD_TEMPLE_BOSS_POT_7,
    RC_SNOWHEAD_TEMPLE_BOSS_POT_8,
    RC_SNOWHEAD_TEMPLE_BOSS_POT_9,
    RC_SNOWHEAD_TEMPLE_BOSS_POT_10,
    RC_SNOWHEAD_TEMPLE_BOSS_POT_EARLY_1,
    RC_SNOWHEAD_TEMPLE_BOSS_POT_EARLY_2,
    RC_SNOWHEAD_TEMPLE_BOSS_POT_EARLY_3,
    RC_SNOWHEAD_TEMPLE_BOSS_POT_EARLY_4,
    RC_SNOWHEAD_TEMPLE_POT_BLOCK_ROOM_1,
    RC_SNOWHEAD_TEMPLE_POT_BLOCK_ROOM_2,
    RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_1,
    RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_2,
    RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_3,
    RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_4,
    RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_5,
    RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_AFTER_1,
    RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_AFTER_2,
    RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_BOTTOM_1,
    RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_BOTTOM_2,
    RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_LEVEL_2_1,
    RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_LEVEL_2_2,
    RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_NEAR_BOSS_KEY_1,
    RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_NEAR_BOSS_KEY_2,
    RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_SCARECROW_1,
    RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_SCARECROW_2,
    RC_SNOWHEAD_TEMPLE_POT_COMPASS_ROOM_1,
    RC_SNOWHEAD_TEMPLE_POT_COMPASS_ROOM_2,
    RC_SNOWHEAD_TEMPLE_POT_COMPASS_ROOM_3,
    RC_SNOWHEAD_TEMPLE_POT_COMPASS_ROOM_4,
    RC_SNOWHEAD_TEMPLE_POT_COMPASS_ROOM_5,
    RC_SNOWHEAD_TEMPLE_POT_DUAL_SWITCHES_1,
    RC_SNOWHEAD_TEMPLE_POT_DUAL_SWITCHES_2,
    RC_SNOWHEAD_TEMPLE_POT_ENTRANCE_1,
    RC_SNOWHEAD_TEMPLE_POT_ENTRANCE_2,
    RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_1,
    RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_2,
    RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_3,
    RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_4,
    RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_5,
    RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_6,
    RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_7,
    RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_UPPER_1,
    RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_UPPER_2,
    RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_UPPER_3,
    RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_UPPER_4,
    RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_UPPER_5,
    RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_UPPER_6,
    RC_SNOWHEAD_TEMPLE_POT_WIZZROBE_1,
    RC_SNOWHEAD_TEMPLE_POT_WIZZROBE_2,
    RC_SNOWHEAD_TEMPLE_POT_WIZZROBE_3,
    RC_SNOWHEAD_TEMPLE_POT_WIZZROBE_4,
    RC_SNOWHEAD_TEMPLE_POT_WIZZROBE_5,
    RC_SOUTHERN_SWAMP_POT_1,
    RC_SOUTHERN_SWAMP_POT_2,
    RC_SOUTHERN_SWAMP_POT_3,
    RC_STONE_TOWER_INVERTED_POT_1,
    RC_STONE_TOWER_INVERTED_POT_2,
    RC_STONE_TOWER_INVERTED_POT_3,
    RC_STONE_TOWER_INVERTED_POT_4,
    RC_STONE_TOWER_INVERTED_POT_5,
    RC_STONE_TOWER_POT_CLIMB_1,
    RC_STONE_TOWER_POT_CLIMB_2,
    RC_STONE_TOWER_POT_HIGHER_SCARECROW_1,
    RC_STONE_TOWER_POT_HIGHER_SCARECROW_2,
    RC_STONE_TOWER_POT_HIGHER_SCARECROW_3,
    RC_STONE_TOWER_POT_HIGHER_SCARECROW_4,
    RC_STONE_TOWER_POT_HIGHER_SCARECROW_5,
    RC_STONE_TOWER_POT_HIGHER_SCARECROW_6,
    RC_STONE_TOWER_POT_HIGHER_SCARECROW_7,
    RC_STONE_TOWER_POT_HIGHER_SCARECROW_8,
    RC_STONE_TOWER_POT_HIGHER_SCARECROW_9,
    RC_STONE_TOWER_POT_LOWER_SCARECROW_1,
    RC_STONE_TOWER_POT_LOWER_SCARECROW_10,
    RC_STONE_TOWER_POT_LOWER_SCARECROW_11,
    RC_STONE_TOWER_POT_LOWER_SCARECROW_12,
    RC_STONE_TOWER_POT_LOWER_SCARECROW_2,
    RC_STONE_TOWER_POT_LOWER_SCARECROW_3,
    RC_STONE_TOWER_POT_LOWER_SCARECROW_4,
    RC_STONE_TOWER_POT_LOWER_SCARECROW_5,
    RC_STONE_TOWER_POT_LOWER_SCARECROW_6,
    RC_STONE_TOWER_POT_LOWER_SCARECROW_7,
    RC_STONE_TOWER_POT_LOWER_SCARECROW_8,
    RC_STONE_TOWER_POT_LOWER_SCARECROW_9,
    RC_STONE_TOWER_POT_OWL_STATUE_1,
    RC_STONE_TOWER_POT_OWL_STATUE_2,
    RC_STONE_TOWER_POT_OWL_STATUE_3,
    RC_STONE_TOWER_POT_OWL_STATUE_4,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_GOMESS_1,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_GOMESS_2,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_GOMESS_3,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_GOMESS_4,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_POE_MAZE_SIDE_1,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_POE_MAZE_SIDE_2,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_POE_WIZZROBE_SIDE_1,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_POE_WIZZROBE_SIDE_2,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_PRE_BOSS_1,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_PRE_BOSS_2,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_PRE_BOSS_3,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_PRE_BOSS_4,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_PRE_BOSS_5,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_PRE_BOSS_6,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_PRE_BOSS_7,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_PRE_BOSS_8,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_UPDRAFTS_BRIDGE_1,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_UPDRAFTS_BRIDGE_2,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_UPDRAFTS_LEDGE_1,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_UPDRAFTS_LEDGE_2,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_UPDRAFTS_LEDGE_3,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_UPDRAFTS_LEDGE_4,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_WIZZROBE_1,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_WIZZROBE_2,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_WIZZROBE_3,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_WIZZROBE_4,
    RC_STONE_TOWER_TEMPLE_INVERTED_POT_WIZZROBE_5,
    // RC_STONE_TOWER_TEMPLE_POT_BEFORE_WATER_BRIDGE_1,
    // RC_STONE_TOWER_TEMPLE_POT_BEFORE_WATER_BRIDGE_2,
    // RC_STONE_TOWER_TEMPLE_POT_BEFORE_WATER_BRIDGE_3,
    // RC_STONE_TOWER_TEMPLE_POT_BEFORE_WATER_BRIDGE_4,
    // RC_STONE_TOWER_TEMPLE_POT_BEFORE_WATER_BRIDGE_5,
    // RC_STONE_TOWER_TEMPLE_POT_BEFORE_WATER_BRIDGE_6,
    // RC_STONE_TOWER_TEMPLE_POT_BEFORE_WATER_BRIDGE_7,
    // RC_STONE_TOWER_TEMPLE_POT_BEFORE_WATER_BRIDGE_8,
    RC_STONE_TOWER_TEMPLE_POT_ENTRANCE_1,
    // RC_STONE_TOWER_TEMPLE_POT_ENTRANCE_2,
    // RC_STONE_TOWER_TEMPLE_POT_LAVA_ROOM_1,
    RC_STONE_TOWER_TEMPLE_POT_LAVA_ROOM_2,
    RC_STONE_TOWER_TEMPLE_POT_LAVA_ROOM_3,
    RC_STONE_TOWER_TEMPLE_POT_LAVA_ROOM_4,
    RC_STONE_TOWER_TEMPLE_POT_LAVA_ROOM_AFTER_BLOCK_1,
    RC_STONE_TOWER_TEMPLE_POT_LAVA_ROOM_AFTER_BLOCK_2,
    RC_STONE_TOWER_TEMPLE_POT_LAVA_ROOM_AFTER_BLOCK_3,
    RC_STONE_TOWER_TEMPLE_POT_LAVA_ROOM_AFTER_BLOCK_4,
    RC_STONE_TOWER_TEMPLE_POT_MIRROR_ROOM_1,
    RC_STONE_TOWER_TEMPLE_POT_MIRROR_ROOM_2,
    RC_STONE_TOWER_TEMPLE_POT_WATER_ROOM_BRIDGE_1,
    // RC_STONE_TOWER_TEMPLE_POT_WATER_ROOM_BRIDGE_2,
    RC_STONE_TOWER_TEMPLE_POT_WATER_ROOM_UNDERWATER_LOWER_1,
    RC_STONE_TOWER_TEMPLE_POT_WATER_ROOM_UNDERWATER_LOWER_2,
    RC_STONE_TOWER_TEMPLE_POT_WATER_ROOM_UNDERWATER_LOWER_3,
    RC_STONE_TOWER_TEMPLE_POT_WATER_ROOM_UNDERWATER_UPPER_1,
    RC_STONE_TOWER_TEMPLE_POT_WATER_ROOM_UNDERWATER_UPPER_2,
    RC_STONE_TOWER_TEMPLE_POT_WIND_ROOM_1,
    RC_STONE_TOWER_TEMPLE_POT_WIND_ROOM_2,
    RC_STONE_TOWER_TEMPLE_POT_WIND_ROOM_3,
    RC_STONE_TOWER_TEMPLE_POT_WIND_ROOM_4,
    RC_SWAMP_SPIDER_HOUSE_POT_GOLD_ROOM_LOWER_1,
    RC_SWAMP_SPIDER_HOUSE_POT_GOLD_ROOM_LOWER_2,
    RC_SWAMP_SPIDER_HOUSE_POT_GOLD_ROOM_UPPER_1,
    RC_SWAMP_SPIDER_HOUSE_POT_GOLD_ROOM_UPPER_2,
    RC_SWAMP_SPIDER_HOUSE_POT_GOLD_ROOM_UPPER_3,
    RC_SWAMP_SPIDER_HOUSE_POT_GOLD_ROOM_UPPER_4,
    RC_SWAMP_SPIDER_HOUSE_POT_JAR_ROOM_1,
    RC_SWAMP_SPIDER_HOUSE_POT_JAR_ROOM_2,
    RC_SWAMP_SPIDER_HOUSE_POT_JAR_ROOM_3,
    RC_SWAMP_SPIDER_HOUSE_POT_JAR_ROOM_4,
    RC_SWAMP_SPIDER_HOUSE_POT_JAR_ROOM_5,
    RC_SWAMP_SPIDER_HOUSE_POT_JAR_ROOM_6,
    RC_SWAMP_SPIDER_HOUSE_POT_JAR_ROOM_7,
    RC_SWAMP_SPIDER_HOUSE_POT_MAIN_ROOM_LOWER_1,
    RC_SWAMP_SPIDER_HOUSE_POT_MAIN_ROOM_LOWER_2,
    RC_SWAMP_SPIDER_HOUSE_POT_MAIN_ROOM_LOWER_3,
    RC_SWAMP_SPIDER_HOUSE_POT_MAIN_ROOM_UPPER_LEFT_1,
    RC_SWAMP_SPIDER_HOUSE_POT_MAIN_ROOM_UPPER_LEFT_2,
    RC_SWAMP_SPIDER_HOUSE_POT_MAIN_ROOM_UPPER_RIGHT_1,
    RC_SWAMP_SPIDER_HOUSE_POT_MAIN_ROOM_UPPER_RIGHT_2,
    RC_SWAMP_SPIDER_HOUSE_POT_MONUMENT_ROOM_1,
    RC_SWAMP_SPIDER_HOUSE_POT_MONUMENT_ROOM_2,
    RC_SWORDSMAN_SCHOOL_POT_1,
    RC_SWORDSMAN_SCHOOL_POT_2,
    RC_SWORDSMAN_SCHOOL_POT_3,
    RC_SWORDSMAN_SCHOOL_POT_4,
    RC_SWORDSMAN_SCHOOL_POT_5,
    RC_TERMINA_FIELD_POT,
    RC_TERMINA_FIELD_SCRUB_POT,
    RC_WOODFALL_POT_1,
    RC_WOODFALL_POT_2,
    RC_WOODFALL_POT_3,
    RC_WOODFALL_TEMPLE_POT_ENTRANCE,
    RC_WOODFALL_TEMPLE_POT_MAIN_ROOM_LOWER_1,
    RC_WOODFALL_TEMPLE_POT_MAIN_ROOM_LOWER_2,
    RC_WOODFALL_TEMPLE_POT_MAIN_ROOM_LOWER_3,
    RC_WOODFALL_TEMPLE_POT_MAIN_ROOM_LOWER_4,
    RC_WOODFALL_TEMPLE_POT_MAIN_ROOM_LOWER_5,
    RC_WOODFALL_TEMPLE_POT_MAIN_ROOM_LOWER_6,
    RC_WOODFALL_TEMPLE_POT_MAIN_ROOM_UPPER_1,
    RC_WOODFALL_TEMPLE_POT_MAIN_ROOM_UPPER_2,
    RC_WOODFALL_TEMPLE_POT_MAZE_1,
    RC_WOODFALL_TEMPLE_POT_MAZE_2,
    RC_WOODFALL_TEMPLE_POT_MINIBOSS_ROOM_1,
    RC_WOODFALL_TEMPLE_POT_MINIBOSS_ROOM_2,
    RC_WOODFALL_TEMPLE_POT_MINIBOSS_ROOM_3,
    RC_WOODFALL_TEMPLE_POT_MINIBOSS_ROOM_4,
    RC_WOODFALL_TEMPLE_POT_PREBOSS_1,
    RC_WOODFALL_TEMPLE_POT_PREBOSS_2,
    RC_WOODFALL_TEMPLE_POT_WATER_ROOM_1,
    RC_WOODFALL_TEMPLE_POT_WATER_ROOM_2,
    RC_WOODFALL_TEMPLE_POT_WATER_ROOM_3,
    RC_WOODFALL_TEMPLE_POT_WATER_ROOM_4,
    RC_ZORA_CAPE_POT_NEAR_BEAVERS_1,
    RC_ZORA_CAPE_POT_NEAR_BEAVERS_2,
    RC_ZORA_CAPE_POT_NEAR_OWL_STATUE_1,
    RC_ZORA_CAPE_POT_NEAR_OWL_STATUE_2,
    RC_ZORA_CAPE_POT_NEAR_OWL_STATUE_3,
    RC_ZORA_CAPE_POT_NEAR_OWL_STATUE_4,

    // --- Cows --- //
    RC_BENEATH_THE_WELL_COW,
    RC_GREAT_BAY_COAST_COW_BACK,
    RC_GREAT_BAY_COAST_COW_FRONT,
    RC_ROMANI_RANCH_BARN_COW_LEFT,
    RC_ROMANI_RANCH_BARN_COW_MIDDLE,
    RC_ROMANI_RANCH_BARN_COW_RIGHT,
    RC_ROMANI_RANCH_FIELD_COW_ENTRANCE,
    RC_ROMANI_RANCH_FIELD_COW_NEAR_HOUSE_BACK,
    RC_ROMANI_RANCH_FIELD_COW_NEAR_HOUSE_FRONT,
    RC_TERMINA_FIELD_COW_BACK,
    RC_TERMINA_FIELD_COW_FRONT,

    RC_MAX,
} RandoCheckId;

typedef enum {
    RI_UNKNOWN,
    RI_ARROW_FIRE,
    RI_ARROW_ICE,
    RI_ARROW_LIGHT,
    RI_ARROWS_10,
    RI_ARROWS_30,
    RI_ARROWS_50,
    RI_BLUE_POTION_REFILL,
    RI_BOMB_BAG_20,
    RI_BOMB_BAG_30,
    RI_BOMB_BAG_40,
    RI_BOMBCHU_10,
    RI_BOMBCHU_5,
    RI_BOMBCHU,
    RI_BOMBS_10,
    RI_BOMBS_5,
    RI_BOTTLE_EMPTY,
    RI_BOW,
    RI_CLOCK_TOWN_STRAY_FAIRY,
    RI_DEED_LAND,
    RI_DEED_MOUNTAIN,
    RI_DEED_OCEAN,
    RI_DEED_SWAMP,
    RI_DEKU_NUT,
    RI_DEKU_NUTS_10,
    RI_DEKU_NUTS_5,
    RI_DEKU_STICK,
    RI_DOUBLE_DEFENSE,
    RI_DOUBLE_MAGIC,
    RI_FAIRY_REFILL,
    RI_GREAT_BAY_BOSS_KEY,
    RI_GREAT_BAY_COMPASS,
    RI_GREAT_BAY_MAP,
    RI_GREAT_BAY_SMALL_KEY,
    RI_GREAT_BAY_STRAY_FAIRY,
    RI_GREAT_FAIRY_SWORD,
    RI_GREAT_SPIN_ATTACK,
    RI_GREEN_POTION_REFILL,
    RI_GS_TOKEN_OCEAN,
    RI_GS_TOKEN_SWAMP,
    RI_HEART_CONTAINER,
    RI_HEART_PIECE,
    RI_HOOKSHOT,
    RI_LENS,
    RI_LETTER_TO_KAFEI,
    RI_LETTER_TO_MAMA,
    RI_MAGIC_BEAN,
    RI_MAGIC_JAR_BIG,
    RI_MAGIC_JAR_SMALL,
    RI_MASK_BREMEN,
    RI_MASK_CAPTAIN,
    RI_MASK_DEKU,
    RI_MASK_GIANT,
    RI_MASK_GREAT_FAIRY,
    RI_MASK_KAFEIS_MASK,
    RI_MILK_REFILL,
    RI_MOONS_TEAR,
    RI_NONE,
    RI_NUTS_10,
    RI_OCARINA,
    RI_PENDANT_OF_MEMORIES,
    RI_PROGRESSIVE_BOMB_BAG,
    RI_PROGRESSIVE_BOW,
    RI_PROGRESSIVE_MAGIC,
    RI_QUIVER_40,
    RI_QUIVER_50,
    RI_RECOVERY_HEART,
    RI_RED_POTION_REFILL,
    RI_ROOM_KEY,
    RI_RUPEE_BLUE,
    RI_RUPEE_GREEN,
    RI_RUPEE_HUGE,
    RI_RUPEE_PURPLE,
    RI_RUPEE_RED,
    RI_RUPEE_SILVER,
    RI_SHIELD_HERO,
    RI_SHIELD_MIRROR,
    RI_SINGLE_MAGIC,
    RI_SNOWHEAD_BOSS_KEY,
    RI_SNOWHEAD_COMPASS,
    RI_SNOWHEAD_MAP,
    RI_SNOWHEAD_SMALL_KEY,
    RI_SNOWHEAD_STRAY_FAIRY,
    RI_SONG_OF_HEALING,
    RI_SONG_OF_TIME,
    RI_STONE_TOWER_BOSS_KEY,
    RI_STONE_TOWER_COMPASS,
    RI_STONE_TOWER_MAP,
    RI_STONE_TOWER_SMALL_KEY,
    RI_STONE_TOWER_STRAY_FAIRY,
    RI_SUNS_SONG,
    RI_WOODFALL_BOSS_KEY,
    RI_WOODFALL_COMPASS,
    RI_WOODFALL_MAP,
    RI_WOODFALL_SMALL_KEY,
    RI_WOODFALL_STRAY_FAIRY,
    RI_MAX,
} RandoItemId;

typedef enum {
    RO_LOGIC,
    RO_SHUFFLE_GOLD_SKULLTULAS,
    RO_SHUFFLE_POTS,
    RO_SHUFFLE_SHOPS,
    RO_MAX,
} RandoOptionId;

typedef enum {
    RO_GENERIC_OFF,
    RO_GENERIC_ON,
} RandoOptionGenericOffOn;

typedef enum {
    RO_GENERIC_NO,
    RO_GENERIC_YES,
} RandoOptionGenericNoYes;

typedef enum {
    RO_LOGIC_NO_LOGIC,
    RO_LOGIC_VANILLA,
    RO_LOGIC_GLITCHLESS,
    RO_LOGIC_GLITCHED,
} RandoOptionLogic;

typedef enum {
    RANDO_INF_PURCHASED_BEANS_FROM_SOUTHERN_SWAMP_SCRUB,
    RANDO_INF_PURCHASED_BOMB_BAG_FROM_GORON_VILLAGE_SCRUB,
    RANDO_INF_PURCHASED_POTION_FROM_IKANA_CANYON_SCRUB,
    RANDO_INF_PURCHASED_POTION_FROM_ZORA_HALL_SCRUB,
    RANDO_INF_OBTAINED_MOONS_TEAR,
    RANDO_INF_OBTAINED_DEED_LAND,
    RANDO_INF_OBTAINED_DEED_SWAMP,
    RANDO_INF_OBTAINED_DEED_MOUNTAIN,
    RANDO_INF_OBTAINED_DEED_OCEAN,
    RANDO_INF_OBTAINED_ROOM_KEY,
    RANDO_INF_OBTAINED_LETTER_TO_MAMA,
    RANDO_INF_OBTAINED_LETTER_TO_KAFEI,
    RANDO_INF_OBTAINED_PENDANT_OF_MEMORIES,
    RANDO_INF_MAX,
} RandoInf;

#endif // RANDO_TYPES_H
