from dataclasses import dataclass
from Options import (
    Choice,
    Toggle,
    DefaultOnToggle,
    Range,
    PerGameCommonOptions,
    StartInventoryPool,
    Visibility,
    OptionGroup,
)

# -----------------------------
# Minimal, MM2Ship-safe options
# -----------------------------

class TrueNoLogic(Toggle):
    """
    Turn off logic completely.
    Generation may fail if allow_true_no_logic is not set to true in the host.yaml.
    """
    display_name = "True No Logic"
    true_no_logic = 1
    # visibility = Visibility.spoiler


# -----------------------------------------
# 2Ship Randomizer option mirror (no logic)
# These mirror Options.cpp defaults.
# -----------------------------------------

class AccessDungeons(Choice):
    display_name = "Access Dungeons"
    option_form_and_song = 0
    option_form_or_song = 1
    option_form_only = 2
    option_song_only = 3
    option_open = 4
    default = 0  # RO_ACCESS_DUNGEONS_FORM_AND_SONG


class AccessMajoraMasksCount(Range):
    display_name = "Access Majora Masks Count"
    range_start = 0
    range_end = 24
    default = 0


class AccessMajoraRemainsCount(Range):
    display_name = "Access Majora Remains Count"
    range_start = 0
    range_end = 4
    default = 0


class AccessMoonMasksCount(Range):
    display_name = "Access Moon Masks Count"
    range_start = 0
    range_end = 20
    default = 0


class AccessMoonRemainsCount(Range):
    display_name = "Access Moon Remains Count"
    range_start = 0
    range_end = 4
    default = 4


class AccessTrials(Choice):
    display_name = "Access Trials"
    option_20_masks = 0
    option_remains = 1
    option_forms = 2
    option_open = 3
    default = 0  # RO_ACCESS_TRIALS_20_MASKS


class ClockShuffleProgressive(Choice):
    # Values come from RO_CLOCK_SHUFFLE_* in 2Ship
    display_name = "Clock Shuffle Progressive"
    option_randomized = 0
    option_ascending = 1
    option_descending = 2
    default = 0  # RO_CLOCK_SHUFFLE_RANDOM


class ClockShuffle(Toggle):
    display_name = "Clock Shuffle"
    default = 0  # RO_GENERIC_OFF


class ClockTerminalTime(Range):
    display_name = "Clock Terminal Time"
    range_start = 0
    range_end = 359
    default = 0


class ExcludeTerminaFieldGrass(Toggle):
    """
    Exclude Termina Field grass checks from the location pool.
    Only applies when Shuffle Grass Drops is enabled.
    Does not exclude Termina Field grotto grass.
    """
    display_name = "Exclude Termina Field Grass"
    default = 0


class ExcludeCowGrottoGrass(Toggle):
    """
    Exclude cow grotto grass checks from the location pool.
    Only applies when Shuffle Grass Drops is enabled.
    Excludes 72 Termina Field Cow Grotto grass and 72 Great Bay Cow Grotto grass (144 total).
    """
    display_name = "Exclude Cow Grotto Grass"
    default = 0


class HintsBossRemains(Toggle):
    display_name = "Hints: Boss Remains"
    default = 0


class HintsGossipStones(Toggle):
    display_name = "Hints: Gossip Stones"
    default = 0


class HintsHookshot(Toggle):
    display_name = "Hints: Hookshot"
    default = 0


class HintsOathToOrder(Toggle):
    display_name = "Hints: Oath to Order"
    default = 0


class HintsPurchaseable(Toggle):
    display_name = "Hints: Purchaseable"
    default = 0


class HintsSongOfSoaring(Toggle):
    display_name = "Hints: Song of Soaring"
    default = 0


class HintsSpiderHouses(Toggle):
    display_name = "Hints: Spider Houses"
    default = 0


class Logic(Choice):
    """ Logic is not currently implemented """
    display_name = "Logic"
    option_glitchless = 0
    option_vanilla = 1
    option_nearly_no_logic = 2
    option_no_logic = 3
    default = 0  # RO_LOGIC_GLITCHLESS


class PlentifulItems(Toggle):
    display_name = "Plentiful Items"
    default = 0


class ShuffleBarrelDrops(Toggle):
    display_name = "Shuffle Barrel Drops"
    default = 0


class ShuffleBossRemains(Toggle):
    display_name = "Shuffle Boss Remains"
    default = 0


class ShuffleBossSouls(Toggle):
    display_name = "Shuffle Boss Souls"
    default = 0


class ShuffleCows(Toggle):
    display_name = "Shuffle Cows"
    default = 0


class ShuffleCrateDrops(Toggle):
    display_name = "Shuffle Crate Drops"
    default = 0


class ShuffleEnemyDrops(Toggle):
    display_name = "Shuffle Enemy Drops"
    default = 0


class ShuffleEnemySouls(Toggle):
    display_name = "Shuffle Enemy Souls"
    default = 0


class ShuffleFreestandingItems(Toggle):
    display_name = "Shuffle Freestanding Items"
    default = 0


class ShuffleFrogs(Toggle):
    display_name = "Shuffle Frogs"
    default = 0


class ShuffleGoldSkulltulas(Toggle):
    display_name = "Shuffle Gold Skulltulas"
    default = 0


class ShuffleGrassDrops(Toggle):
    display_name = "Shuffle Grass Drops"
    default = 0


class ShuffleOcarinaButtons(Toggle):
    display_name = "Shuffle Ocarina Buttons"
    default = 0


class ShuffleOcarina(Toggle):
    """Shuffle the Ocarina into the item pool. If disabled, you start with an Ocarina."""
    display_name = "Shuffle Ocarina"
    default = 0


class ShuffleOwlStatues(Toggle):
    display_name = "Shuffle Owl Statues"
    default = 0


class ShufflePotDrops(Toggle):
    display_name = "Shuffle Pot Drops"
    default = 0


class ShuffleShops(Toggle):
    display_name = "Shuffle Shops"
    default = 0


class ShuffleShield(Toggle):
    """Shuffle shields into the item pool. If disabled, you start with a Hero's Shield."""
    display_name = "Shuffle Shield"
    default = 0


class ShuffleSnowballDrops(Toggle):
    display_name = "Shuffle Snowball Drops"
    default = 0


class ShuffleSongDoubleTime(Toggle):
    display_name = "Shuffle Song of Double Time"
    default = 0


class ShuffleSongInvertedTime(Toggle):
    display_name = "Shuffle Inverted Song of Time"
    default = 0


class ShuffleSongSaria(Toggle):
    display_name = "Shuffle Saria's Song"
    default = 0


class ShuffleSongSun(Toggle):
    display_name = "Shuffle Sun's Song"
    default = 0


class ShuffleSongTime(Toggle):
    """Shuffle the Song of Time into the item pool. If disabled, you start with the Song of Time."""
    display_name = "Shuffle Song of Time"
    default = 0


class ShuffleSword(Toggle):
    """Shuffle sword upgrades into the item pool. If disabled, you start with a Kokiri Sword."""
    display_name = "Shuffle Sword"
    default = 0


class ShuffleSwim(Toggle):
    display_name = "Shuffle Swim"
    default = 0


class ShuffleTingleShops(Toggle):
    display_name = "Shuffle Tingle Shops"
    default = 0


class ShuffleTraps(Toggle):
    display_name = "Shuffle Traps"
    default = 0


class ShuffleTreeDrops(Toggle):
    display_name = "Shuffle Tree Drops"
    default = 0


class ShuffleTriforcePieces(Toggle):
    display_name = "Shuffle Triforce Pieces"
    default = 0


class SkulltulaTokensMax(Range):
    display_name = "Skulltula Tokens Max"
    range_start = 0
    range_end = 30
    default = 30  # sane MM default; matches typical spider house totals


class SkulltulaTokensRequired(Range):
    display_name = "Skulltula Tokens Required"
    range_start = 0
    range_end = 30
    default = 30


class StartingConsumables(Toggle):
    display_name = "Starting Consumables"
    default = 0


class StartingBunnyHood(Toggle):
    """Start with the Bunny Hood. The Bunny Hood will not be in the item pool."""
    display_name = "Starting Bunny Hood"
    default = 0


class StartingHealth(Range):
    display_name = "Starting Health"
    range_start = 1
    range_end = 20
    default = 3


class StartingMapsAndCompasses(Toggle):
    display_name = "Starting Maps and Compasses"
    default = 0


class StartingRupees(Range):
    display_name = "Starting Rupees"
    range_start = 0
    range_end = 500
    default = 99


class StrayFairiesMax(Range):
    display_name = "Stray Fairies Max"
    range_start = 0
    range_end = 15
    default = 15


class StrayFairiesRequired(Range):
    display_name = "Stray Fairies Required"
    range_start = 0
    range_end = 15
    default = 15


class TrapAmount(Range):
    display_name = "Trap Amount"
    range_start = 0
    range_end = 100
    default = 5


class TriforcePiecesMax(Range):
    display_name = "Triforce Pieces Max"
    range_start = 1
    range_end = 1000
    default = 30


class TriforcePiecesRequired(Range):
    display_name = "Triforce Pieces Required"
    range_start = 1
    range_end = 1000
    default = 20


# -----------------------------
# Per-game options dataclass
# -----------------------------

@dataclass
class MM2ShipOptions(PerGameCommonOptions):
    # -----------------------------
    # Generation
    # -----------------------------
    true_no_logic: TrueNoLogic
    logic: Logic

    # -----------------------------
    # Randomizer Settings
    # -----------------------------
    access_dungeons: AccessDungeons
    access_majora_masks_count: AccessMajoraMasksCount
    access_majora_remains_count: AccessMajoraRemainsCount
    access_moon_masks_count: AccessMoonMasksCount
    access_moon_remains_count: AccessMoonRemainsCount
    access_trials: AccessTrials

    plentiful_items: PlentifulItems

    shuffle_triforce_pieces: ShuffleTriforcePieces
    triforce_pieces_max: TriforcePiecesMax
    triforce_pieces_required: TriforcePiecesRequired

    skulltula_tokens_max: SkulltulaTokensMax
    skulltula_tokens_required: SkulltulaTokensRequired

    stray_fairies_max: StrayFairiesMax
    stray_fairies_required: StrayFairiesRequired

    shuffle_traps: ShuffleTraps
    trap_amount: TrapAmount

    # -----------------------------
    # Starting Stuff
    # -----------------------------
    starting_health: StartingHealth
    starting_rupees: StartingRupees
    starting_consumables: StartingConsumables
    starting_maps_and_compasses: StartingMapsAndCompasses
    starting_bunny_hood: StartingBunnyHood

    # -----------------------------
    # Item Shuffles
    # -----------------------------
    shuffle_sword: ShuffleSword
    shuffle_shield: ShuffleShield
    shuffle_ocarina: ShuffleOcarina
    shuffle_ocarina_buttons: ShuffleOcarinaButtons
    shuffle_swim: ShuffleSwim

    shuffle_song_time: ShuffleSongTime
    shuffle_song_double_time: ShuffleSongDoubleTime
    shuffle_song_inverted_time: ShuffleSongInvertedTime
    shuffle_song_saria: ShuffleSongSaria
    shuffle_song_sun: ShuffleSongSun

    clock_shuffle: ClockShuffle
    clock_shuffle_progressive: ClockShuffleProgressive
    clock_terminal_time: ClockTerminalTime

    # -----------------------------
    # Location Shuffle
    # -----------------------------
    exclude_termina_field_grass: ExcludeTerminaFieldGrass
    exclude_cow_grotto_grass: ExcludeCowGrottoGrass

    shuffle_shops: ShuffleShops
    shuffle_tingle_shops: ShuffleTingleShops
    shuffle_owl_statues: ShuffleOwlStatues
    shuffle_gold_skulltulas: ShuffleGoldSkulltulas
    shuffle_frogs: ShuffleFrogs
    shuffle_cows: ShuffleCows

    shuffle_grass_drops: ShuffleGrassDrops
    shuffle_pot_drops: ShufflePotDrops
    shuffle_crate_drops: ShuffleCrateDrops
    shuffle_barrel_drops: ShuffleBarrelDrops
    shuffle_freestanding_items: ShuffleFreestandingItems
    shuffle_snowball_drops: ShuffleSnowballDrops
    shuffle_tree_drops: ShuffleTreeDrops
    shuffle_enemy_drops: ShuffleEnemyDrops

    shuffle_boss_remains: ShuffleBossRemains
    shuffle_boss_souls: ShuffleBossSouls
    shuffle_enemy_souls: ShuffleEnemySouls

    # -----------------------------
    # Hints
    # -----------------------------
    hints_boss_remains: HintsBossRemains
    hints_oath_to_order: HintsOathToOrder
    hints_gossip_stones: HintsGossipStones
    hints_purchaseable: HintsPurchaseable
    hints_hookshot: HintsHookshot
    hints_song_of_soaring: HintsSongOfSoaring
    hints_spider_houses: HintsSpiderHouses

    # -----------------------------
    # AP common (if you insist it appears somewhere specific)
    # -----------------------------
    start_inventory_from_pool: StartInventoryPool

# -----------------------------
# Option groups (UI organization)
# -----------------------------

mm2ship_option_groups = [
    OptionGroup("Generation", [
        TrueNoLogic,
        # Logic,
    ]),

    OptionGroup("Randomizer Settings", [
        # access / gating
        AccessDungeons,
        AccessMajoraMasksCount,
        AccessMajoraRemainsCount,
        AccessMoonMasksCount,
        AccessMoonRemainsCount,
        AccessTrials,

        # core logic + density
        PlentifulItems,

        # goal / endgame
        ShuffleTriforcePieces,
        TriforcePiecesMax,
        TriforcePiecesRequired,

        # collectibles requirements/caps
        SkulltulaTokensMax,
        SkulltulaTokensRequired,
        StrayFairiesMax,
        StrayFairiesRequired,

        # traps
        ShuffleTraps,
        TrapAmount,
    ]),

    OptionGroup("Starting Stuff", [
        StartingBunnyHood,
        StartingConsumables,
        StartingMapsAndCompasses,
        StartingHealth,
        StartingRupees,
    ]),

    OptionGroup("Item Shuffles", [
        # items/abilities/gear/songs/buttons themselves
        ShuffleSword,
        ShuffleShield,
        ShuffleOcarina,

        ShuffleOcarinaButtons,
        ShuffleSwim,

        ShuffleSongTime,
        ShuffleSongDoubleTime,
        ShuffleSongInvertedTime,
        ShuffleSongSaria,
        ShuffleSongSun,

        # clock behavior/settings
        ClockShuffle,
        ClockShuffleProgressive,
        ClockTerminalTime,

        # souls
        ShuffleBossSouls,
        ShuffleEnemySouls,
    ]),

    OptionGroup("Location Shuffles", [
        # exclusions belong with the location pools they affect
        ExcludeTerminaFieldGrass,
        ExcludeCowGrottoGrass,

        # “shuffle checks/drops/locations”
        ShuffleBossRemains,

        ShuffleShops,
        ShuffleTingleShops,
        ShuffleOwlStatues,
        ShuffleGoldSkulltulas,
        ShuffleFrogs,
        ShuffleCows,

        ShuffleGrassDrops,
        ShufflePotDrops,
        ShuffleCrateDrops,
        ShuffleBarrelDrops,
        ShuffleFreestandingItems,
        ShuffleSnowballDrops,
        ShuffleTreeDrops,
        ShuffleEnemyDrops,
    ]),

    OptionGroup("Hints", [
        # hints are location/knowledge distribution
        HintsBossRemains,
        HintsOathToOrder,
        HintsGossipStones,
        HintsPurchaseable,
        HintsHookshot,
        HintsSongOfSoaring,
        HintsSpiderHouses,
    ]),
]
