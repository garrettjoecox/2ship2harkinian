from __future__ import annotations

from typing import NamedTuple, TYPE_CHECKING

from BaseClasses import MultiWorld, Region

from .Enums import Regions, Locations
from .Locations import MM2ShipLocation, location_table
from .LocationData import LOCATION_RCTYPE

if TYPE_CHECKING:
    from . import MM2ShipWorld


class MM2ShipRegionData(NamedTuple):
    connecting_regions: list[str] = []


class MM2ShipRegion(Region):
    game = "2 Ship 2 Harkinian (MM)"

    def __init__(self, name: str, player: int, multiworld: MultiWorld, hint: str | None = None):
        super().__init__(name, player, multiworld, hint)


# Maps C++ RandoCheckType → the MM2ShipOptions attribute that enables it.
# Types absent from this dict are always active (RCTYPE_CHEST, RCTYPE_NPC,
# RCTYPE_SONG, RCTYPE_STRAY_FAIRY, RCTYPE_HEART, RCTYPE_MINIGAME, etc.).
_RCTYPE_OPTION: dict[str, str] = {
    "RCTYPE_BARREL":      "shuffle_barrel_drops",
    "RCTYPE_COW":         "shuffle_cows",
    "RCTYPE_CRATE":       "shuffle_crate_drops",
    "RCTYPE_ENEMY_DROP":  "shuffle_enemy_drops",
    "RCTYPE_FREESTANDING":"shuffle_freestanding_items",
    "RCTYPE_FROG":        "shuffle_frogs",
    "RCTYPE_GRASS":       "shuffle_grass_drops",
    "RCTYPE_OWL":         "shuffle_owl_statues",
    "RCTYPE_POT":         "shuffle_pot_drops",
    # "RCTYPE_REMAINS":     "shuffle_boss_remains", 
    "RCTYPE_SHOP":        "shuffle_shops",
    "RCTYPE_SKULL_TOKEN": "shuffle_gold_skulltulas",
    "RCTYPE_SNOWBALL":    "shuffle_snowball_drops",
    "RCTYPE_TINGLE_SHOP": "shuffle_tingle_shops",
    "RCTYPE_TREE":        "shuffle_tree_drops",
}


def location_should_be_included(world: "MM2ShipWorld", loc: Locations) -> bool:
    """
    Return True if this location belongs in the item pool given the current world options.

    Filtering is type-based: each location's C++ RandoCheckType (stored in
    LocationData.LOCATION_RCTYPE) is mapped to the option that controls it.
    This avoids fragile name-pattern matching and stays in sync with C++.

    Called from both generate_early (to filter location_name_to_id) and
    create_regions_and_locations (to filter Location objects).  Both must stay
    in sync — always go through this function rather than duplicating the logic.
    """
    name = loc.name  # UPPER_SNAKE_CASE enum key

    rctype = LOCATION_RCTYPE.get(name)

    # Look up which option controls this RCTYPE (None → always active)
    option_name = _RCTYPE_OPTION.get(rctype) if rctype else None
    if option_name is not None:
        option = getattr(world.options, option_name, None)
        if option is not None and not option.value:
            return False

    # Sub-exclusions for grass (only reached when shuffle_grass_drops is ON)
    if rctype == "RCTYPE_GRASS":
        if world.options.exclude_termina_field_grass.value and name.startswith("TERMINA_FIELD_GRASS_"):
            return False
        if world.options.exclude_cow_grotto_grass.value and (
            "TERMINA_FIELD_COW_GROTTO_GRASS_" in name
            or "GREAT_BAY_COAST_COW_GROTTO_GRASS_" in name
        ):
            return False

    return True


def create_regions_and_locations(world: "MM2ShipWorld") -> None:
    region_data_table: dict[str, MM2ShipRegionData] = {}

    # Regions enum -> region name strings
    for entry in Regions:
        region_data_table[entry.value] = MM2ShipRegionData([])

    # Start region (not part of enum)
    region_data_table.setdefault("Menu", MM2ShipRegionData(["Clock Town South"]))

    # Safety: ensure the hub exists
    region_data_table.setdefault("Clock Town South", MM2ShipRegionData([]))

    # Create regions
    for region_name, data in region_data_table.items():
        region = MM2ShipRegion(region_name, world.player, world.multiworld)
        world.multiworld.regions.append(region)
        region.add_exits(data.connecting_regions)

    hub = world.get_region("Clock Town South")

    # Create locations and attach to hub, filtering based on options.
    # Inactive locations are skipped entirely — no AP item is placed there and they
    # won't appear in the spoiler log.  The C++ resync loop applies the same filter
    # so it never sends a location ID the server doesn't know about.
    # NOTE: Map/compass locations are always created even when
    # starting_maps_and_compasses is ON — items are removed from the pool,
    # not the locations.
    from .Locations import location_data_table
    from .VanillaItems import vanilla_items
    for loc in Locations:
        if not location_should_be_included(world, loc):
            continue

        loc_name = loc.value
        address = location_data_table[loc]
        # Event locations (address=None) are created but not given a network ID
        loc_obj = MM2ShipLocation(world.player, loc_name, address, hub)
        hub.locations.append(loc_obj)

        # Place Victory event item at Victory location
        if loc == Locations.VICTORY:
            loc_obj.place_locked_item(world.create_item("Victory", create_as_event=True))

        # Boss warp locations are always created. When not shuffled, pre-fill
        # with the vanilla item so the player receives it from the server on check.
        elif LOCATION_RCTYPE.get(loc.name) == "RCTYPE_REMAINS" and not world.options.shuffle_boss_remains.value:
            vanilla_item = vanilla_items.get(loc)
            if vanilla_item is not None:
                loc_obj.place_locked_item(world.create_item(vanilla_item.value))

    if not hasattr(world, "included_locations") or world.included_locations is None:
        world.included_locations = {}
