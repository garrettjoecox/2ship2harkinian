#include "ActorBehavior.h"
#include <libultraship/libultraship.h>

#include "2s2h/CustomItem/CustomItem.h"
#include "2s2h/Rando/Rando.h"

extern "C" {
#include "variables.h"
#include "overlays/actors/ovl_Obj_Tsubo/z_obj_tsubo.h"
}

#define IS_AT(xx, zz) (actor->home.pos.x == xx && actor->home.pos.z == zz)

RandoCheckId IdentifyPot(Actor* actor) {
    auto randoStaticCheck =
        Rando::StaticData::GetCheckFromFlag(FLAG_CYCL_SCENE_COLLECTIBLE, OBJ_TSUBO_PFE00(actor), gPlayState->sceneId);
    if (randoStaticCheck.randoCheckId != RC_UNKNOWN) {
        return randoStaticCheck.randoCheckId;
    }

    RandoCheckId randoCheckId = RC_UNKNOWN;

    switch (gPlayState->sceneId) {
        case SCENE_F40:
            if (IS_AT(1204.0f, 1871.0f)) {
                randoCheckId = RC_STONE_TOWER_POT_HIGHER_SCARECROW_2;
            }
            if (IS_AT(1364.0f, 2391.0f)) {
                randoCheckId = RC_STONE_TOWER_POT_HIGHER_SCARECROW_8;
            }
            if (IS_AT(-1116.0f, 2231.0f)) {
                randoCheckId = RC_STONE_TOWER_POT_LOWER_SCARECROW_2;
            }
            if (IS_AT(-1116.0f, 2031.0f)) {
                randoCheckId = RC_STONE_TOWER_POT_LOWER_SCARECROW_4;
            }
            if (IS_AT(-1116.0f, 1511.0f)) {
                randoCheckId = RC_STONE_TOWER_POT_LOWER_SCARECROW_11;
            }
            break;
        case SCENE_HAKASHITA:
            if (IS_AT(1270.0f, 380.0f)) {
                randoCheckId = RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_BATS_3;
            }
            if (IS_AT(-1121.0f, -879.0f)) {
                randoCheckId = RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_AFTER_PIT_1;
            }
            if (IS_AT(-1200.0f, -1400.0f)) {
                randoCheckId = RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_AFTER_PIT_3;
            }
            if (IS_AT(-1120.0f, -1400.0f)) {
                randoCheckId = RC_BENEATH_THE_GRAVEYARD_POT_NIGHT_1_AFTER_PIT_4;
            }
            break;
        case SCENE_HAKUGIN_BS:
            if (IS_AT(-1250.0f, -1250.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_POT_1;
            }
            if (IS_AT(-1400.0f, -60.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_POT_2;
            }
            if (IS_AT(-1600.0f, -60.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_POT_3;
            }
            if (IS_AT(-1250.0f, 1250.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_POT_4;
            }
            if (IS_AT(61.0f, 1440.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_POT_5;
            }
            if (IS_AT(61.0f, 1740.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_POT_6;
            }
            if (IS_AT(1250.0f, 1250.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_POT_7;
            }
            if (IS_AT(1460.0f, -60.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_POT_8;
            }
            if (IS_AT(1660.0f, -60.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_POT_9;
            }
            if (IS_AT(1250.0f, -1250.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_POT_10;
            }
            if (IS_AT(150.0f, -1170.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_POT_EARLY_1;
            }
            if (IS_AT(-150.0f, -1170.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_POT_EARLY_2;
            }
            if (IS_AT(0.0f, -1400.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_POT_EARLY_3;
            }
            if (IS_AT(0.0f, -1600.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_BOSS_POT_EARLY_4;
            }
            break;
        case SCENE_HAKUGIN:
            if (IS_AT(-1052.0f, 338.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_BLOCK_ROOM_1;
            }
            if (IS_AT(-1091.0f, 327.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_BLOCK_ROOM_2;
            }
            if (IS_AT(930.0f, 1065.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_1;
            }
            if (IS_AT(810.0f, 1065.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_2;
            }
            if (IS_AT(870.0f, 1005.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_3;
            }
            if (IS_AT(870.0f, 1125.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_4;
            }
            if (IS_AT(870.0f, 1065.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_5;
            }
            if (IS_AT(795.0f, -60.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_AFTER_1;
            }
            if (IS_AT(825.0f, -75.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_BRIDGE_ROOM_AFTER_2;
            }
            if (IS_AT(-585.0f, -645.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_BOTTOM_1;
            }
            if (IS_AT(-645.0f, -585.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_BOTTOM_2;
            }
            if (IS_AT(570.0f, -90.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_LEVEL_2_1;
            }
            if (IS_AT(510.0f, -90.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_LEVEL_2_2;
            }
            if (IS_AT(435.0f, -570.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_NEAR_BOSS_KEY_1;
            }
            if (IS_AT(410.0f, -526.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_NEAR_BOSS_KEY_2;
            }
            if (IS_AT(473.0f, 458.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_SCARECROW_1;
            }
            if (IS_AT(519.0f, 535.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_CENTRAL_ROOM_SCARECROW_2;
            }
            if (IS_AT(-1543.0f, 703.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_COMPASS_ROOM_1;
            }
            if (IS_AT(-1507.0f, 655.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_COMPASS_ROOM_2;
            }
            if (IS_AT(-351.0f, 1153.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_COMPASS_ROOM_3;
            }
            if (IS_AT(-378.0f, 1169.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_COMPASS_ROOM_4;
            }
            if (IS_AT(-1525.0f, 679.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_COMPASS_ROOM_5;
            }
            if (IS_AT(-727.0f, 993.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_DUAL_SWITCHES_1;
            }
            if (IS_AT(-675.0f, 920.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_DUAL_SWITCHES_2;
            }
            if (IS_AT(225.0f, 975.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_ENTRANCE_1;
            }
            if (IS_AT(225.0f, 1065.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_ENTRANCE_2;
            }
            if (IS_AT(-452.0f, -995.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_1;
            }
            if (IS_AT(-473.0f, -958.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_2;
            }
            if (IS_AT(-493.0f, -921.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_3;
            }
            if (IS_AT(-420.0f, -930.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_4;
            }
            if (IS_AT(-1275.0f, -600.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_5;
            }
            if (IS_AT(-1305.0f, -623.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_6;
            }
            if (IS_AT(-1335.0f, -645.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_LOWER_7;
            }
            if (IS_AT(-870.0f, -105.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_UPPER_1;
            }
            if (IS_AT(-915.0f, -105.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_UPPER_2;
            }
            if (IS_AT(135.0f, -1050.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_UPPER_3;
            }
            if (IS_AT(150.0f, -990.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_UPPER_4;
            }
            if (IS_AT(-1184.0f, -1316.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_UPPER_5;
            }
            if (IS_AT(-1116.0f, -1374.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_PILLARS_ROOM_UPPER_6;
            }
            if (IS_AT(1320.0f, -690.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_WIZZROBE_1;
            }
            if (IS_AT(1440.0f, -690.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_WIZZROBE_2;
            }
            if (IS_AT(1380.0f, -690.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_WIZZROBE_3;
            }
            if (IS_AT(1440.0f, -630.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_WIZZROBE_4;
            }
            if (IS_AT(1440.0f, -570.0f)) {
                randoCheckId = RC_SNOWHEAD_TEMPLE_POT_WIZZROBE_5;
            }
            break;
        case SCENE_LAST_BS:
            if (IS_AT(80.0f, 800.0f)) {
                randoCheckId = RC_MOON_MAJORA_POT_1;
            }
            if (IS_AT(-80.0f, 800.0f)) {
                randoCheckId = RC_MOON_MAJORA_POT_2;
            }
            break;
        case SCENE_LAST_GORON:
            if (IS_AT(-5670.0f, -2280.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_1;
            }
            if (IS_AT(-5610.0f, -2280.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_2;
            }
            if (IS_AT(-2700.0f, -2490.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_3;
            }
            if (IS_AT(-2640.0f, -2490.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_4;
            }
            if (IS_AT(-2580.0f, -2490.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_5;
            }
            if (IS_AT(-7200.0f, -1290.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_6;
            }
            if (IS_AT(-7260.0f, -1290.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_7;
            }
            if (IS_AT(-7200.0f, -1410.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_8;
            }
            if (IS_AT(-7260.0f, -1410.0)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_9;
            }
            if (IS_AT(-4890.0f, -1380.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_10;
            }
            if (IS_AT(-4890.0f, -1320.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_11;
            }
            if (IS_AT(-420.0f, -1425.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_EARLY_1;
            }
            if (IS_AT(-480.0f, -1425.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_EARLY_2;
            }
            if (IS_AT(-420.0f, -1275.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_EARLY_3;
            }
            if (IS_AT(-480.0f, -1275.0f)) {
                randoCheckId = RC_MOON_TRIAL_GORON_POT_EARLY_4;
            }
            break;
        case SCENE_LAST_LINK:
            if (IS_AT(-270.0f, -90.0f)) {
                randoCheckId = RC_MOON_TRIAL_LINK_POT_1;
            }
            if (IS_AT(-270.0f, -150.0f)) {
                randoCheckId = RC_MOON_TRIAL_LINK_POT_2;
            }
            if (IS_AT(-210.0f, -90.0f)) {
                randoCheckId = RC_MOON_TRIAL_LINK_POT_3;
            }
            if (IS_AT(-210.0f, -150.0f)) {
                randoCheckId = RC_MOON_TRIAL_LINK_POT_4;
            }
            if (IS_AT(210.0f, -90.0f)) {
                randoCheckId = RC_MOON_TRIAL_LINK_POT_5;
            }
            if (IS_AT(210.0f, -150.0f)) {
                randoCheckId = RC_MOON_TRIAL_LINK_POT_6;
            }
            if (IS_AT(270.0f, -90.0f)) {
                randoCheckId = RC_MOON_TRIAL_LINK_POT_7;
            }
            if (IS_AT(270.0f, -150.0f)) {
                randoCheckId = RC_MOON_TRIAL_LINK_POT_8;
            }
            break;
        case SCENE_REDEAD:
            if (IS_AT(1620.0f, -960.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_POT_BIG_POE_1;
            }
            if (IS_AT(1620.0f, -1560.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_POT_BIG_POE_2;
            }
            if (IS_AT(2280.0f, -1560.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_POT_BIG_POE_3;
            }
            if (IS_AT(2280.0f, -960.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_POT_BIG_POE_4;
            }
            if (IS_AT(1185.0f, -915.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_POT_MIDDLE_1;
            }
            if (IS_AT(1185.0f, -945.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_POT_MIDDLE_2;
            }
            if (IS_AT(1185.0f, -975.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_POT_MIDDLE_3;
            }
            if (IS_AT(1185.0f, -1005.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_POT_MIDDLE_4;
            }
            if (IS_AT(1185.0f, -1035.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_POT_MIDDLE_5;
            }
            if (IS_AT(1185.0f, -1065.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_POT_MIDDLE_6;
            }
            if (IS_AT(1185.0f, -1095.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_POT_MIDDLE_7;
            }
            if (IS_AT(1185.0f, -1125.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_POT_MIDDLE_8;
            }
            if (IS_AT(1185.0f, -1155.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_POT_MIDDLE_9;
            }
            if (IS_AT(1185.0f, -1185.0f)) {
                randoCheckId = RC_BENEATH_THE_WELL_POT_MIDDLE_10;
            }
            break;
        case SCENE_MITURIN:
            if (IS_AT(120.0f, 2100.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_POT_ENTRANCE;
            }
            if (IS_AT(660.0f, -120.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_POT_MAIN_ROOM_LOWER_1;
            }
            if (IS_AT(660.0f, 120.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_POT_MAIN_ROOM_LOWER_2;
            }
            if (IS_AT(630.0f, 510.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_POT_MAIN_ROOM_LOWER_3;
            }
            if (IS_AT(570.0f, 570.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_POT_MAIN_ROOM_LOWER_4;
            }
            if (IS_AT(510.0f, 630.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_POT_MAIN_ROOM_LOWER_5;
            }
            if (IS_AT(510.0f, 510.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_POT_MAIN_ROOM_LOWER_6;
            }
            if (IS_AT(675.0f, 255.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_POT_MAIN_ROOM_UPPER_1;
            }
            if (IS_AT(645.0f, 255.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_POT_MAIN_ROOM_UPPER_2;
            }
            if (IS_AT(-780.0f, -165.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_POT_MAZE_1;
            }
            if (IS_AT(-780.0f, -105.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_POT_MAZE_2;
            }
            if (IS_AT(-120.0f, -2220.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_POT_PREBOSS_1;
            }
            if (IS_AT(120.0f, -2220.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_POT_PREBOSS_2;
            }
            if (IS_AT(1335.0f, 585.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_POT_WATER_ROOM_1;
            }
            if (IS_AT(1335.0f, 615.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_POT_WATER_ROOM_2;
            }
            if (IS_AT(1335.0f, 645.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_POT_WATER_ROOM_3;
            }
            if (IS_AT(1335.0f, 675.0f)) {
                randoCheckId = RC_WOODFALL_TEMPLE_POT_WATER_ROOM_4;
            }
            break;
        case SCENE_KINSTA1:
            if (IS_AT(-30.0f, -1650.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_POT_GOLD_ROOM_LOWER_1;
            }
            if (IS_AT(30.0f, -1650.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_POT_GOLD_ROOM_LOWER_2;
            }
            if (IS_AT(330.0f, -1651.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_POT_GOLD_ROOM_UPPER_1;
            }
            if (IS_AT(330.0f, -1590.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_POT_GOLD_ROOM_UPPER_2;
            }
            if (IS_AT(330.0f, -1531.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_POT_GOLD_ROOM_UPPER_3;
            }
            if (IS_AT(330.0f, -1471.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_POT_GOLD_ROOM_UPPER_4;
            }
            if (IS_AT(544.0f, -569.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_POT_JAR_ROOM_1;
            }
            if (IS_AT(483.0f, -569.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_POT_JAR_ROOM_2;
            }
            if (IS_AT(575.0f, -538.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_POT_JAR_ROOM_3;
            }
            if (IS_AT(514.0f, -539.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_POT_JAR_ROOM_4;
            }
            if (IS_AT(454.0f, -539.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_POT_JAR_ROOM_5;
            }
            if (IS_AT(451.0f, -1232.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_POT_JAR_ROOM_6;
            }
            if (IS_AT(451.0f, -1290.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_POT_JAR_ROOM_7;
            }
            if (IS_AT(90.0f, -209.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_POT_MAIN_ROOM_LOWER_1;
            }
            if (IS_AT(89.0f, -269.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_POT_MAIN_ROOM_LOWER_2;
            }
            if (IS_AT(-90.0f, -270.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_POT_MAIN_ROOM_LOWER_3;
            }
            if (IS_AT(330.0f, -210.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_POT_MAIN_ROOM_UPPER_LEFT_1;
            }
            if (IS_AT(270.0f, -210.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_POT_MAIN_ROOM_UPPER_LEFT_2;
            }
            if (IS_AT(-270.0f, -210.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_POT_MAIN_ROOM_UPPER_RIGHT_1;
            }
            if (IS_AT(-330.0f, -210.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_POT_MAIN_ROOM_UPPER_RIGHT_2;
            }
            if (IS_AT(-871.0f, -872.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_POT_MONUMENT_ROOM_1;
            }
            if (IS_AT(-809.0f, -870.0f)) {
                randoCheckId = RC_SWAMP_SPIDER_HOUSE_POT_MONUMENT_ROOM_2;
            }
            break;
        case SCENE_KINDAN2:
            if (IS_AT(64.0f, -1650.0)) {
                randoCheckId = RC_OCEAN_SPIDER_HOUSE_POT_MAIN_ROOM_1;
            }
            if (IS_AT(2.0f, -1722.0f)) {
                randoCheckId = RC_OCEAN_SPIDER_HOUSE_POT_MAIN_ROOM_WEB;
            }
            if (IS_AT(-569.0f, -1200.0f)) {
                randoCheckId = RC_OCEAN_SPIDER_HOUSE_POT_STORAGE_2;
            }
            if (IS_AT(-630.f, -1200.0f)) {
                randoCheckId = RC_OCEAN_SPIDER_HOUSE_POT_STORAGE_3;
            }
            break;
        case SCENE_SEA_BS:
            if (IS_AT(150.0f, 150.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_BOSS_POT_1;
            }
            if (IS_AT(-150.0f, 150.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_BOSS_POT_2;
            }
            if (IS_AT(-150.0f, -150.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_BOSS_POT_3;
            }
            if (IS_AT(150.0f, -150.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_BOSS_POT_4;
            }
            if (IS_AT(1200.0f, -1200.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_BOSS_POT_UNDERWATER_1;
            }
            if (IS_AT(1200.0f, 1200.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_BOSS_POT_UNDERWATER_2;
            }
            if (IS_AT(-1200.0f, 1200.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_BOSS_POT_UNDERWATER_3;
            }
            if (IS_AT(-1200.0f, -1200.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_BOSS_POT_UNDERWATER_4;
            }
            break;
        case SCENE_SEA:
            if (IS_AT(-1905.0f, -1200.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_3;
            }
            if (IS_AT(-1755.0f, -1200.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_6;
            }
            if (IS_AT(-1755.0f, -1680.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_7;
            }
            if (IS_AT(-1755.0f, -1725.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_8;
            }
            if (IS_AT(-1755.0f, -1770.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_9;
            }
            if (IS_AT(-1905.0f, -1680.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_10;
            }
            if (IS_AT(-1905.0f, -1725.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_11;
            }
            if (IS_AT(-1905.0f, -1770.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_BEFORE_WART_12;
            }
            if (IS_AT(600.0f, -30.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_CENTRAL_ROOM_1;
            }
            if (IS_AT(600.0f, 30.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_CENTRAL_ROOM_2;
            }
            if (IS_AT(1265.0f, -785.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_SURFACE_1;
            }
            if (IS_AT(1235.0f, -815.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_SURFACE_2;
            }
            if (IS_AT(1575.0f, -1155.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_SURFACE_3;
            }
            if (IS_AT(1605.0f, -1125.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_SURFACE_4;
            }
            if (IS_AT(1170.0f, -1078.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_WATER_1;
            }
            if (IS_AT(1310.0f, -1078.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_WATER_2;
            }
            if (IS_AT(1310.0f, -1218.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_COMPASS_ROOM_WATER_3;
            }
            if (IS_AT(540.0f, -930.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_1_1;
            }
            if (IS_AT(-540.0f, -930.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_1_2;
            }
            if (IS_AT(-540.0f, -2070.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_1_3;
            }
            if (IS_AT(540.0f, -2070.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_1_4;
            }
            if (IS_AT(-479.0f, 2141.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_1;
            }
            if (IS_AT(-524.0f, 2141.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_2;
            }
            if (IS_AT(-391.0f, 1959.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_3;
            }
            if (IS_AT(-436.0f, 1959.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_4;
            }
            if (IS_AT(-391.0f, 1810.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_5;
            }
            if (IS_AT(-436.0f, 1810.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_6;
            }
            if (IS_AT(-524.0f, 1628.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_7;
            }
            if (IS_AT(-479.0f, 1628.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_2_8;
            }
            if (IS_AT(2970.0f, 1020.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_3_UPPER_1;
            }
            if (IS_AT(2970.0f, 960.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_GREEN_PIPE_3_UPPER_2;
            }
            if (IS_AT(1500.0f, 540.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_SURFACE_1;
            }
            if (IS_AT(1590.0f, 540.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_SURFACE_2;
            }
            if (IS_AT(1712.0f, 1215.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_SURFACE_3;
            }
            if (IS_AT(1500.0f, 1030.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_3;
            }
            if (IS_AT(1590.0f, 1250.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_4;
            }
            if (IS_AT(1635.0f, 1620.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_5;
            }
            if (IS_AT(1470.0f, 1665.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_7;
            }
            if (IS_AT(1455.0f, 1620.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_MAP_ROOM_WATER_8;
            }
            if (IS_AT(2835.0f, -555.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_1;
            }
            if (IS_AT(2835.0f, -495.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_2;
            }
            if (IS_AT(2745.0f, -495.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_3;
            }
            if (IS_AT(2745.0f, -555.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_4;
            }
            if (IS_AT(2730.0f, -975.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_5;
            }
            if (IS_AT(2775.0f, -975.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_6;
            }
            if (IS_AT(3555.0f, -720.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_7;
            }
            if (IS_AT(3555.0f, -675.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_PRE_BOSS_8;
            }
            if (IS_AT(-1530.0f, 60.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_RED_PIPE_BEFORE_WART_1;
            }
            if (IS_AT(-1560.0f, 30.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_RED_PIPE_BEFORE_WART_2;
            }
            if (IS_AT(-2130.0f, 60.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_RED_PIPE_BEFORE_WART_3;
            }
            if (IS_AT(-2100.0f, 30.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_RED_PIPE_BEFORE_WART_4;
            }
            if (IS_AT(-2250.0f, -1890.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_WART_1;
            }
            if (IS_AT(-2280.0f, -1920.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_WART_2;
            }
            if (IS_AT(-2280.0f, -2760.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_WART_3;
            }
            if (IS_AT(-2250.0f, -2790.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_WART_4;
            }
            if (IS_AT(-1410.0f, -2790.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_WART_5;
            }
            if (IS_AT(-1380.0f, -2760.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_WART_6;
            }
            if (IS_AT(-1380.0f, -1920.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_WART_7;
            }
            if (IS_AT(-1410.0f, -1890.0f)) {
                randoCheckId = RC_GREAT_BAY_TEMPLE_POT_WART_8;
            }
            break;
        case SCENE_INISIE_N:
            if (IS_AT(2070.0f, -1620.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POT_GOMESS_1;
            }
            if (IS_AT(2790.0f, -1620.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POT_GOMESS_2;
            }
            if (IS_AT(2790.0f, -900.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POT_GOMESS_3;
            }
            if (IS_AT(2070.0f, -900.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POT_GOMESS_4;
            }
            if (IS_AT(-735.0f, -1305.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POT_POE_MAZE_SIDE_1;
            }
            if (IS_AT(-735.0f, -1305.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POT_POE_MAZE_SIDE_2;
            }
            if (IS_AT(-1605.0f, -1395.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POT_POE_WIZZROBE_SIDE_1;
            }
            if (IS_AT(-1605.0f, -1305.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POT_POE_WIZZROBE_SIDE_2;
            }
            if (IS_AT(-30.0f, -1995.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POT_PRE_BOSS_1;
            }
            if (IS_AT(-30.0f, -1965.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POT_PRE_BOSS_2;
            }
            if (IS_AT(-30.0f, -1935.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POT_PRE_BOSS_3;
            }
            if (IS_AT(-30.0f, -1905.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POT_PRE_BOSS_4;
            }
            if (IS_AT(30.0f, -1995.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POT_PRE_BOSS_5;
            }
            if (IS_AT(30.0f, -1965.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POT_PRE_BOSS_6;
            }
            if (IS_AT(30.0f, -1935.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POT_PRE_BOSS_7;
            }
            if (IS_AT(30.0f, -1905.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POT_PRE_BOSS_8;
            }
            if (IS_AT(1350.0f, -630.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POT_UPDRAFTS_BRIDGE_1;
            }
            if (IS_AT(1230.0f, -630.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POT_UPDRAFTS_BRIDGE_2;
            }
            if (IS_AT(735.0f, -1305.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POT_UPDRAFTS_LEDGE_3;
            }
            if (IS_AT(-1290.0f, -510.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POT_WIZZROBE_1;
            }
            if (IS_AT(-1290.0f, -450.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POT_WIZZROBE_2;
            }
            if (IS_AT(-1410.0f, -450.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POT_WIZZROBE_3;
            }
            if (IS_AT(-1410.0f, -510.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_INVERTED_POT_WIZZROBE_4;
            }
            // if (IS_AT(-30.0f, -1995.0f)) {
            //     randoCheckId = RC_STONE_TOWER_TEMPLE_POT_BEFORE_WATER_BRIDGE_1;
            // }
            // if (IS_AT(-30.0f, -1965.0f)) {
            //     randoCheckId = RC_STONE_TOWER_TEMPLE_POT_BEFORE_WATER_BRIDGE_2;
            // }
            // if (IS_AT(-30.0f, -1935.0f)) {
            //     randoCheckId = RC_STONE_TOWER_TEMPLE_POT_BEFORE_WATER_BRIDGE_3;
            // }
            // if (IS_AT(-30.0f, -1905.0f)) {
            //     randoCheckId = RC_STONE_TOWER_TEMPLE_POT_BEFORE_WATER_BRIDGE_4;
            // }
            // if (IS_AT(30.0f, -1995.0f)) {
            //     randoCheckId = RC_STONE_TOWER_TEMPLE_POT_BEFORE_WATER_BRIDGE_5;
            // }
            // if (IS_AT(30.0f, -1965.0f)) {
            //     randoCheckId = RC_STONE_TOWER_TEMPLE_POT_BEFORE_WATER_BRIDGE_6;
            // }
            // if (IS_AT(30.0f, -1935.0f)) {
            //     randoCheckId = RC_STONE_TOWER_TEMPLE_POT_BEFORE_WATER_BRIDGE_7;
            // }
            // if (IS_AT(30.0f, -1905.0f)) {
            //     randoCheckId = RC_STONE_TOWER_TEMPLE_POT_BEFORE_WATER_BRIDGE_8;
            // }
            if (IS_AT(45.0f, -690.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_POT_ENTRANCE_1;
            }
            if (IS_AT(-1275.0f, -1305.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_POT_LAVA_ROOM_2;
            }
            if (IS_AT(-1425.0f, -1395.0)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_POT_LAVA_ROOM_3;
            }
            if (IS_AT(-1425.0f, -1305.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_POT_LAVA_ROOM_4;
            }
            if (IS_AT(-1275.0f, -615.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_POT_LAVA_ROOM_AFTER_BLOCK_2;
            }
            if (IS_AT(1425.0f, -705.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_POT_LAVA_ROOM_AFTER_BLOCK_3;
            }
            if (IS_AT(-1425.0f, -615.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_POT_LAVA_ROOM_AFTER_BLOCK_4;
            }
            if (IS_AT(1350.0f, -1770.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_POT_MIRROR_ROOM_1;
            }
            if (IS_AT(1230.0f, -1770.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_POT_MIRROR_ROOM_2;
            }
            if (IS_AT(810.0f, -1260.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_POT_WATER_ROOM_UNDERWATER_LOWER_1;
            }
            if (IS_AT(750.0f, -1260.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_POT_WATER_ROOM_UNDERWATER_LOWER_2;
            }
            if (IS_AT(810.0f, -1320.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_POT_WATER_ROOM_UNDERWATER_LOWER_3;
            }
            if (IS_AT(1800.0f, -900.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_POT_WATER_ROOM_UNDERWATER_UPPER_1;
            }
            if (IS_AT(1800.0f, -840.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_POT_WATER_ROOM_UNDERWATER_UPPER_2;
            }
            if (IS_AT(615.0f, -3375.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_POT_WIND_ROOM_1;
            }
            if (IS_AT(585.0f, -3375.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_POT_WIND_ROOM_2;
            }
            if (IS_AT(615.0f, -3465.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_POT_WIND_ROOM_3;
            }
            if (IS_AT(585.0f, -3465.0f)) {
                randoCheckId = RC_STONE_TOWER_TEMPLE_POT_WIND_ROOM_4;
            }
            break;
        default:
            break;
    }
    return randoCheckId;
}

void Rando::ActorBehavior::InitObjTsuboBehavior() {
    static uint32_t onActorInit = 0;
    static uint32_t shouldHookId1 = 0;
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorInit>(onActorInit);
    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::ShouldVanillaBehavior>(shouldHookId1);

    onActorInit = 0;
    shouldHookId1 = 0;

    if (!IS_RANDO) {
        return;
    }

    onActorInit =
        GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorInit>(ACTOR_OBJ_TSUBO, [](Actor* actor) {
            RandoCheckId randoCheckId = IdentifyPot(actor);
            if (randoCheckId == RC_UNKNOWN) {
                return;
            }

            if (!RANDO_SAVE_CHECKS[randoCheckId].shuffled || RANDO_SAVE_CHECKS[randoCheckId].eligible) {
                return;
            }

            actor->home.rot.x = randoCheckId;
        });

    shouldHookId1 = REGISTER_VB_SHOULD(VB_POT_DROP_COLLECTIBLE, {
        Actor* actor = va_arg(args, Actor*);
        RandoCheckId randoCheckId = (RandoCheckId)actor->home.rot.x;
        if (randoCheckId == RC_UNKNOWN) {
            return;
        }

        RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[randoCheckId];
        if (randoSaveCheck.eligible) {
            return;
        }

        CustomItem::Spawn(
            actor->world.pos.x, actor->world.pos.y, actor->world.pos.z, 0,
            CustomItem::KILL_ON_TOUCH | CustomItem::TOSS_ON_SPAWN, randoCheckId,
            [](Actor* actor, PlayState* play) {
                RandoSaveCheck& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
                if (randoSaveCheck.eligible) {
                    return;
                }
                randoSaveCheck.eligible = true;
            },
            [](Actor* actor, PlayState* play) {
                auto& randoSaveCheck = RANDO_SAVE_CHECKS[CUSTOM_ITEM_PARAM];
                RandoItemId randoItemId = Rando::ConvertItem(randoSaveCheck.randoItemId);
                Matrix_Scale(30.0f, 30.0f, 30.0f, MTXMODE_APPLY);
                Rando::DrawItem(randoItemId);
            });
        *should = false;
    });
}
