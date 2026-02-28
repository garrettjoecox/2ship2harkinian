from __future__ import annotations

from typing import TYPE_CHECKING

from BaseClasses import ItemClassification as IC

if TYPE_CHECKING:
    from . import MM2ShipWorld


def create_item_pool(world: "MM2ShipWorld") -> None:
    """
    Core item pool creation matching 2Ship's GeneratePools.cpp logic.

    This loops through all enabled locations and adds their vanilla items to the pool,
    then adds items that have no vanilla location (Hero's Shield, etc.).
    """
    from .Items import Items
    from .Enums import Locations
    from .VanillaItems import vanilla_items
    from .LocationData import LOCATION_RCTYPE
    from collections import Counter

    # Define map and compass items (excluded from pool if starting_maps_and_compasses is ON)
    # This includes dungeon maps/compasses AND Tingle maps
    map_compass_items = {
        Items.GREAT_BAY_COMPASS,
        Items.GREAT_BAY_MAP,
        Items.SNOWHEAD_COMPASS,
        Items.SNOWHEAD_MAP,
        Items.STONE_TOWER_COMPASS,
        Items.STONE_TOWER_MAP,
        Items.WOODFALL_COMPASS,
        Items.WOODFALL_MAP,
        Items.TINGLE_MAP_CLOCK_TOWN,
        Items.TINGLE_MAP_GREAT_BAY,
        Items.TINGLE_MAP_ROMANI_RANCH,
        Items.TINGLE_MAP_SNOWHEAD,
        Items.TINGLE_MAP_STONE_TOWER,
        Items.TINGLE_MAP_WOODFALL,
    }

    # Step 1: Add vanilla items from all enabled locations
    # This matches GeneratePools.cpp lines 28-153 where it loops through all checks
    # and adds their vanilla items to the item pool
    for location in world.multiworld.get_locations(world.player):
        # Convert location name back to enum
        try:
            loc_enum = Locations(location.name)
        except ValueError:
            continue  # Skip event locations or unknown locations

        # Look up the vanilla item for this location
        if loc_enum in vanilla_items:
            vanilla_item = vanilla_items[loc_enum]

            # Skip maps/compasses if starting with them
            # (The locations still exist, but the items are not in the pool)
            if world.options.starting_maps_and_compasses.value and vanilla_item in map_compass_items:
                continue

            # Skip Bunny Hood if starting with it
            if world.options.starting_bunny_hood.value and vanilla_item == Items.MASK_BUNNY:
                continue

            # Skip Ocarina if not shuffled (you start with it instead)
            if not world.options.shuffle_ocarina.value and vanilla_item == Items.OCARINA:
                continue

            # Skip boss remains if not shuffled — those locations are pre-filled
            # with locked items and must not also appear in the random pool.
            if not world.options.shuffle_boss_remains.value and LOCATION_RCTYPE.get(loc_enum.name) == "RCTYPE_REMAINS":
                continue

            world.multiworld.itempool.append(world.create_item(vanilla_item.value))

    # Step 2: Add items with no vanilla location
    # These match GeneratePools.cpp lines 156-231

    # Add sword and shield if shuffled (line 158-159 in GeneratePools.cpp)
    # If not shuffled, these are given as starting items (see StartingItems.cpp)
    if world.options.shuffle_sword.value:
        world.multiworld.itempool.append(world.create_item("Progressive Sword"))
    if world.options.shuffle_shield.value:
        world.multiworld.itempool.append(world.create_item("Hero's Shield"))

    # Add boss souls if shuffled
    if world.options.shuffle_boss_souls.value:
        boss_souls = [
            "Soul of Goht",
            "Soul of Gyorg",
            "Soul of Odolwa",
            "Soul of Twinmold",
        ]
        # Skip Majora soul if triforce pieces are shuffled
        if not world.options.shuffle_triforce_pieces.value:
            boss_souls.append("Soul of Majora")

        for soul in boss_souls:
            world.multiworld.itempool.append(world.create_item(soul))

    # Add enemy souls if shuffled
    if world.options.shuffle_enemy_souls.value:
        enemy_souls = [
            "Soul of Aliens", "Soul of Armos", "Soul of Bad Bats", "Soul of Beamos",
            "Soul of Boes", "Soul of Bubbles", "Soul of Captain Keeta", "Soul of Chuchus",
            "Soul of Death Armos", "Soul of Deep Pythons", "Soul of Deku Babas", "Soul of Dexihands",
            "Soul of Dinolfos", "Soul of Dodongos", "Soul of Dragonflies", "Soul of Eenos",
            "Soul of Eyegores", "Soul of Freezards", "Soul of Garos", "Soul of Gekkos",
            "Soul of Giant Bees", "Soul of Gomess", "Soul of Guays", "Soul of Hiploops",
            "Soul of Igos du Ikana", "Soul of Iron Knuckles", "Soul of Keese", "Soul of Leevers",
            "Soul of Like Likes", "Soul of Mad Scrubs", "Soul of Nejirons", "Soul of Octoroks",
            "Soul of Peahats", "Soul of Pirates", "Soul of Poes", "Soul of Redeads",
            "Soul of Shellblades", "Soul of Skullfish", "Soul of Skulltulas", "Soul of Snappers",
            "Soul of Stalchildren", "Soul of Takkuri", "Soul of Tektites", "Soul of Wallmasters",
            "Soul of Warts", "Soul of Wizrobes", "Soul of Wolfos",
        ]
        for soul in enemy_souls:
            world.multiworld.itempool.append(world.create_item(soul))

    # Add clock shuffle items if shuffled
    if world.options.clock_shuffle.value:
        time_items = [
            "Time (Day 1)", "Time (Night 1)", "Time (Day 2)",
            "Time (Night 2)", "Time (Day 3)", "Time (Night 3)",
        ]
        for time_item in time_items:
            world.multiworld.itempool.append(world.create_item(time_item))

    # Add swim ability if shuffled
    if world.options.shuffle_swim.value:
        world.multiworld.itempool.append(world.create_item("Ability to Swim"))

    # Add ocarina buttons if shuffled
    if world.options.shuffle_ocarina_buttons.value:
        buttons = ["A Button", "C Down Button", "C Right Button", "C Left Button", "C Up Button"]
        for button in buttons:
            world.multiworld.itempool.append(world.create_item(button))

    # Add songs if shuffled
    if world.options.shuffle_song_sun.value:
        world.multiworld.itempool.append(world.create_item("Sun's Song"))
    if world.options.shuffle_song_time.value:
        world.multiworld.itempool.append(world.create_item("Song of Time"))
    if world.options.shuffle_song_double_time.value:
        world.multiworld.itempool.append(world.create_item("Song of Double Time"))
    if world.options.shuffle_song_inverted_time.value:
        world.multiworld.itempool.append(world.create_item("Inverted Song of Time"))
    if world.options.shuffle_song_saria.value:
        world.multiworld.itempool.append(world.create_item("Saria's Song"))

    # Add triforce pieces if shuffled
    if world.options.shuffle_triforce_pieces.value:
        for _ in range(world.options.triforce_pieces_max.value):
            world.multiworld.itempool.append(world.create_item("Piece of the Triforce"))

    # Step 3: Trim stray fairies and skulltula tokens to max counts
    # This matches GeneratePools.cpp lines 233-269
    # IMPORTANT: Only count and trim items for THIS player
    item_counts = Counter(item.name for item in world.multiworld.itempool if item.player == world.player)

    # Stray fairies - trim to max count
    stray_fairy_items = [
        "Stone Tower Stray Fairy",
        "Great Bay Stray Fairy",
        "Snowhead Stray Fairy",
        "Woodfall Stray Fairy",
    ]
    max_fairies = world.options.stray_fairies_max.value
    for fairy_item in stray_fairy_items:
        while item_counts[fairy_item] > max_fairies:
            # Remove one instance of THIS PLAYER's item
            for item in world.multiworld.itempool:
                if item.name == fairy_item and item.player == world.player:
                    world.multiworld.itempool.remove(item)
                    item_counts[fairy_item] -= 1
                    break

    # Skulltula tokens - trim to max count
    skulltula_items = [
        "Swamp Gold Skulltula Token",
        "Ocean Gold Skulltula Token",
    ]
    max_skulltulas = world.options.skulltula_tokens_max.value
    for skulltula_item in skulltula_items:
        while item_counts[skulltula_item] > max_skulltulas:
            # Remove one instance of THIS PLAYER's item
            for item in world.multiworld.itempool:
                if item.name == skulltula_item and item.player == world.player:
                    world.multiworld.itempool.remove(item)
                    item_counts[skulltula_item] -= 1
                    break

    # Track how many items we've added for THIS player (for filler calculation)
    world.items_added = len([item for item in world.multiworld.itempool if item.player == world.player])


def create_plentiful_and_trap_items(world: "MM2ShipWorld") -> None:
    """
    Apply plentiful items logic if enabled, add traps, then fill remaining with filler.
    Matches 2Ship's plentiful logic from GeneratePools.cpp lines 279-312.
    """
    # Plentiful items: duplicate major items if enabled
    if world.options.plentiful_items.value:
        plentiful_candidates = []

        # Only look at items for THIS player
        for item in world.multiworld.itempool:
            if item.player != world.player:
                continue

            # Skip triforce pieces (user specifies exact count)
            if item.name == "Piece of the Triforce":
                continue
            if item.name == "Swamp Gold Skulltula Token":
                continue
            if item.name == "Ocean Gold Skulltula Token":
                continue
            if item.name == "Woodfall Stray Fairy":
                continue
            if item.name == "Snowhead Stray Fairy":
                continue
            if item.name == "Great Bay Stray Fairy":
                continue
            if item.name == "Stone Tower Stray Fairy":
                continue
            if item.name == "Heart Container":
                continue
            if item.name == "Heart Piece":
                continue

            # Add based on classification
            if item.classification in (IC.progression, IC.useful):
                # Skip maps and compasses (they're IC.useful but we don't duplicate them)
                if "Map" not in item.name and "Compass" not in item.name and "Tingle Map" not in item.name:
                    plentiful_candidates.append(item.name)

        # Add duplicates
        for item_name in plentiful_candidates:
            world.multiworld.itempool.append(world.create_item(item_name))

    # Add traps if shuffled
    if world.options.shuffle_traps.value:
        for _ in range(world.options.trap_amount.value):
            world.multiworld.itempool.append(world.create_item("Knockoff Item"))

    # Now fill remaining locations with filler items
    # Count how many items we've added for THIS player
    items_for_this_player = sum(1 for item in world.multiworld.itempool if item.player == world.player)
    locations_for_this_player = len(world.multiworld.get_unfilled_locations(world.player))
    filler_needed = locations_for_this_player - items_for_this_player

    if filler_needed > 0:
        for _ in range(filler_needed):
            filler_name = get_filler_item(world)
            world.multiworld.itempool.append(world.create_item(filler_name))


def get_filler_item(world: "MM2ShipWorld") -> str:
    """
    Choose a filler item name. Prefers rupees and consumables.
    """
    filler_options = [
        "Junk",
    ]
    return world.random.choice(filler_options)
