#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

// Prevent Windows.h from pulling in WinSock.h
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include "Archipelago.h"
#include "ArchipelagoBridge.h"
#include "ArchipelagoTypes.h"

#include "BenGui/BenGui.hpp"
#include "BenGui/BenMenu.h"
#include "BenGui/Notification.h"

#include "2s2h/ShipInit.hpp"
#include "2s2h/Rando/ActorBehavior/ActorBehavior.h"

#include <apuuid.hpp>
#include <apclient.hpp>

#include <algorithm>
#include <ctime>
#include <filesystem>
#include <list>
#include <memory>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

void ArchipelagoConsole_SendMessage(const char* fmt, ...);
void ArchipelagoConsole_PrintJson(const std::vector<AP_Text::ColoredTextNode> nodes);

namespace BenGui {
extern std::shared_ptr<BenMenu> mBenMenu;
} // namespace BenGui

namespace {
namespace AP_Client_consts {
static constexpr int MAX_RETRIES = 3;

// You can change this to whatever your APWorld expects for MM/2Ship.
// (SoH uses "Ship of Harkinian")
static constexpr char const* AP_GAME_NAME = "2 Ship 2 Harkinian (MM)";

// Optional version check like SoH does (apworld_version major/minor)
// (Leave disabled until you know your APWorld versioning rules)
static constexpr char const* AP_WORLD_VERSION_MAJOR = "0";
static constexpr char const* AP_WORLD_VERSION_MINOR = "0";
} // namespace AP_Client_consts

static std::unique_ptr<APClient> sClient;

static bool sDisconnecting = false;
static bool sItemQueued = false;
static bool sIsDeathLinkedDeath = false;
static bool sSentDeathThisLife = false; // Track if we've sent death link this life
static int sRetries = 0;

static std::queue<APClient::NetworkItem> sReceiveQueue;

// Cache items with resolved names for re-sync on file load
struct CachedItem {
    APClient::NetworkItem item;
    std::string resolvedName;
};
static std::vector<CachedItem> sCachedReceivedItems;

static std::string sPassword;
static std::string sStatus = "Disconnected";

// Connection status values for UI indicator
enum ConnectionStatus {
    STATUS_NOT_CONNECTED = 0,
    STATUS_CONNECTING = 1,
    STATUS_CONNECTION_ERROR_RETRYING = 2,
    STATUS_CONNECTED = 3,
    STATUS_CONNECTED_SCOUTED = 4
};

static ConnectionStatus sLastConnectionStatus = STATUS_NOT_CONNECTED;

static void UpdateConnectionStatus(ConnectionStatus status) {
    // Show disconnection notification if we were connected and now we're not
    // (but not on first boot - only when actually disconnecting)
    if (sLastConnectionStatus >= STATUS_CONNECTED && status == STATUS_NOT_CONNECTED) {
        Notification::Emit({ .message = "Disconnected from Archipelago",
                             .messageColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f),
                             .remainingTime = 5.0f });
    }

    sLastConnectionStatus = status;
    CVarSetInteger("gArchipelago.ConnectionStatus", status);
    CVarSave();
}

// If you want a real path helper later, wire it to your app-dir helper.
// For now, this mirrors SoH�s ap_get_uuid usage but with a simple path.
static std::string GetUuidPath() {
    return "uuid";
}

static std::string GetCertPath() {
    std::filesystem::path base =
        std::filesystem::absolute(Ship::Context::GetInstance()->GetAppDirectoryPath()).lexically_normal();

    // Try: <appDir>/networking/cacert.pem
    std::filesystem::path p1 = (base / "networking" / "cacert.pem").lexically_normal();
    if (std::filesystem::exists(p1)) {
        return p1.string();
    }

    // Try: <appDir>/../networking/cacert.pem  (your confirmed existing location)
    std::filesystem::path p2 = (base.parent_path() / "networking" / "cacert.pem").lexically_normal();
    if (std::filesystem::exists(p2)) {
        return p2.string();
    }

    // On Linux, fall back to common system CA bundle locations
#if defined(__linux__)
    static const char* sLinuxCaPaths[] = { "/etc/ssl/certs/ca-certificates.crt", // Debian/Ubuntu
                                           "/etc/pki/tls/certs/ca-bundle.crt",   // Fedora/RHEL
                                           "/etc/ssl/ca-bundle.pem",             // OpenSUSE
                                           "/etc/pki/tls/cacert.pem",            // older RHEL
                                           nullptr };
    for (int i = 0; sLinuxCaPaths[i] != nullptr; i++) {
        if (std::filesystem::exists(sLinuxCaPaths[i])) {
            return sLinuxCaPaths[i];
        }
    }
#endif

    // Fall back to the original (so logs still show what we'd try)
    return p1.string();
}

static void ResetQueue() {
    sItemQueued = false;
    std::queue<APClient::NetworkItem> empty;
    std::swap(sReceiveQueue, empty);
}

static bool IsSlotConnected() {
    return sClient && (sClient->get_state() == APClient::State::SLOT_CONNECTED);
}

static void InstallHandlers() {
    if (!sClient) {
        return;
    }

    sClient->set_socket_error_handler([](const std::string& msg) {
        sRetries++;
        if (sRetries >= AP_Client_consts::MAX_RETRIES) {
            ArchipelagoConsole_SendMessage("[ERROR] Could not connect to server after several tries.\n"
                                           "Are the entered server address and port correct?");
            sStatus = "Connection error";
            UpdateConnectionStatus(STATUS_NOT_CONNECTED);
            sDisconnecting = true;

            // Emit error notification
            Notification::Emit({ .message = "Failed to connect to Archipelago server",
                                 .messageColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f),
                                 .remainingTime = 5.0f });
            return;
        }
        UpdateConnectionStatus(STATUS_CONNECTION_ERROR_RETRYING);
    });

    sClient->set_room_info_handler([]() {
        std::list<std::string> tags;
        if (CVarGetInteger("gArchipelago.DeathLink", 0)) {
            tags.push_back("DeathLink");
        }

        const char* slot = CVarGetString("gArchipelago.Slot", "");
        sClient->ConnectSlot(slot ? slot : "", sPassword, 0b0111, tags, { 0, 6, 3 });
    });

    sClient->set_slot_connected_handler([](const nlohmann::json data) {
        sStatus = "Connected";
        UpdateConnectionStatus(STATUS_CONNECTED);
        ArchipelagoConsole_SendMessage("[LOG] Connected.");

        // Save the slot name to the save file if this is an Archi save
        if (IS_ARCHI && sClient) {
            std::string slotName = sClient->get_player_alias(sClient->get_player_number());
            strncpy(gSaveContext.save.shipSaveInfo.rando.archipelago.slotName, slotName.c_str(), 31);
            gSaveContext.save.shipSaveInfo.rando.archipelago.slotName[31] = '\0'; // Ensure null termination
        }

        if (!data.is_object()) {
            SPDLOG_WARN("[AP] slot_connected data is not an object!");
        }

        // Optional: SoH checks apworld_version against supported major/minor.
        // Enable this once you know your APWorld version fields/format.
        if (data.contains("apworld_version")) {
            try {
                std::string apworldVersion = data["apworld_version"];
                std::stringstream ss(apworldVersion);
                std::string segment;
                std::vector<std::string> seglist;
                while (std::getline(ss, segment, '.')) {
                    seglist.push_back(segment);
                }

                if (seglist.size() >= 2) {
                    const std::string apMajor = seglist[0];
                    const std::string apMinor = seglist[1];

                    const std::string clientMajor = AP_Client_consts::AP_WORLD_VERSION_MAJOR;
                    const std::string clientMinor = AP_Client_consts::AP_WORLD_VERSION_MINOR;

                    if (clientMajor != apMajor || clientMinor != apMinor) {
                        sDisconnecting = true;
                        ArchipelagoConsole_SendMessage("[ERROR] Client version does not match the APWorld version used "
                                                       "to generate the multiworld.\n"
                                                       "Supported version in this client is %s.%s.x.\n"
                                                       "The used APWorld is on version %s.%s.x.\n"
                                                       "Automatically disconnecting...",
                                                       clientMajor.c_str(), clientMinor.c_str(), apMajor.c_str(),
                                                       apMinor.c_str());
                        return;
                    }
                }
            } catch (...) {
                // Ignore malformed version data for now
            }
        }

        // Apply slot options from AP server
        // The slot_data IS the data itself in the connection response
        if (data.is_object() && !data.empty()) {
            ArchipelagoBridge::ApplySlotOptions(data);
            // Note: Slot options are now cached and will be reapplied on file load
            // If a save is already loaded, re-register ActorBehavior hooks with the new options
            if (IS_ARCHI) {
                Rando::ActorBehavior::OnFileLoad();
            }
        } else {
            SPDLOG_WARN("[AP] No slot_data in connection response - using default options");
        }

        // Cache server-checked locations for use when creating new save files
        // New files will use this cache in OnFileLoad to mark locations as obtained
        // BEFORE the first scene loads
        if (sClient) {
            std::set<int64_t> serverCheckedLocations = sClient->get_checked_locations();
            ArchipelagoBridge::CacheCheckedLocations(serverCheckedLocations);
        }

        // Read all shuffle options from slot_data once.
        bool shuffleBarrels = data.value("shuffle_barrel_drops", 0) != 0;
        bool shuffleBossRemains = data.value("shuffle_boss_remains", 0) != 0;
        bool shuffleCows = data.value("shuffle_cows", 0) != 0;
        bool shuffleCrates = data.value("shuffle_crate_drops", 0) != 0;
        bool shuffleEnemyDrops = data.value("shuffle_enemy_drops", 0) != 0;
        bool shuffleFreestanding = data.value("shuffle_freestanding_items", 0) != 0;
        bool shuffleFrogs = data.value("shuffle_frogs", 0) != 0;
        bool shuffleSkulltulas = data.value("shuffle_gold_skulltulas", 0) != 0;
        bool shuffleGrass = data.value("shuffle_grass_drops", 0) != 0;
        bool shuffleOwls = data.value("shuffle_owl_statues", 0) != 0;
        bool shufflePots = data.value("shuffle_pot_drops", 0) != 0;
        bool shuffleShops = data.value("shuffle_shops", 0) != 0;
        bool shuffleSnowballs = data.value("shuffle_snowball_drops", 0) != 0;
        bool shuffleTingleShops = data.value("shuffle_tingle_shops", 0) != 0;
        bool shuffleTrees = data.value("shuffle_tree_drops", 0) != 0;
        bool excludeTerminaGrass = data.value("exclude_termina_field_grass", 0) != 0;
        bool excludeCowGrottoGrass = data.value("exclude_cow_grotto_grass", 0) != 0;

        // Returns true if this check belongs in the current AP world (i.e. its
        // shuffle option is enabled).  Used for both scouting and resync so the two
        // lists are always in sync with the Python world's location_should_be_included.
        auto isCheckActive = [&](RandoCheckId checkId) -> bool {
            auto& check = Rando::StaticData::Checks[checkId];
            switch (check.randoCheckType) {
                case RCTYPE_BARREL:
                    return shuffleBarrels;
                case RCTYPE_COW:
                    return shuffleCows;
                case RCTYPE_CRATE:
                    return shuffleCrates;
                case RCTYPE_ENEMY_DROP:
                    return shuffleEnemyDrops;
                case RCTYPE_FREESTANDING:
                    return shuffleFreestanding;
                case RCTYPE_FROG:
                    return shuffleFrogs;
                case RCTYPE_GRASS: {
                    if (!shuffleGrass)
                        return false;
                    if (excludeTerminaGrass && checkId >= RC_TERMINA_FIELD_GRASS_01 &&
                        checkId <= RC_TERMINA_FIELD_GRASS_216)
                        return false;
                    if (excludeCowGrottoGrass && ((checkId >= RC_TERMINA_FIELD_COW_GROTTO_GRASS_01 &&
                                                   checkId <= RC_TERMINA_FIELD_COW_GROTTO_GRASS_72) ||
                                                  (checkId >= RC_GREAT_BAY_COAST_COW_GROTTO_GRASS_01 &&
                                                   checkId <= RC_GREAT_BAY_COAST_COW_GROTTO_GRASS_72)))
                        return false;
                    return true;
                }
                case RCTYPE_OWL:
                    return shuffleOwls;
                case RCTYPE_POT:
                    return shufflePots;
                case RCTYPE_REMAINS:
                    return shuffleBossRemains;
                case RCTYPE_SHOP:
                    return shuffleShops;
                case RCTYPE_SKULL_TOKEN:
                    return shuffleSkulltulas;
                case RCTYPE_SNOWBALL:
                    return shuffleSnowballs;
                case RCTYPE_TINGLE_SHOP:
                    return shuffleTingleShops;
                case RCTYPE_TREE:
                    return shuffleTrees;
                // Always-active types (chest, NPC, song, stray fairy, heart, minigame)
                default:
                    return true;
            }
        };

        // Build list of locations to scout (only active ones).
        // NOTE: Map/compass locations are always scouted even when
        // starting_maps_and_compasses is ON — items are given at start but
        // chest locations still contain randomized items.
        std::list<int64_t> locationsToScout;
        for (int rc = RC_UNKNOWN + 1; rc < RC_MAX; rc++) {
            RandoCheckId checkId = static_cast<RandoCheckId>(rc);
            if (!isCheckActive(checkId))
                continue;
            uint64_t apLocId = ArchipelagoBridge::GetLocationIdFromRandoCheck(checkId);
            if (apLocId != 0) {
                locationsToScout.push_back(static_cast<int64_t>(apLocId));
            }
        }

        // APClient::LocationScouts() - this sends the request
        // The response comes back via set_location_info_handler
        sClient->LocationScouts(locationsToScout);

        // Resync any location checks that were marked while disconnected.
        // Apply the same isCheckActive filter so we never send a location ID
        // the server doesn't know about (which would crash with "No location X").
        if (IS_ARCHI) {
            std::list<int64_t> locationsToResync;
            for (int rc = RC_UNKNOWN + 1; rc < RC_MAX; rc++) {
                RandoCheckId checkId = static_cast<RandoCheckId>(rc);
                if (!isCheckActive(checkId))
                    continue;
                uint64_t apLocId = ArchipelagoBridge::GetLocationIdFromRandoCheck(checkId);
                if (apLocId != 0 && ArchipelagoBridge::IsLocationChecked(apLocId)) {
                    locationsToResync.push_back(static_cast<int64_t>(apLocId));
                }
            }
            if (!locationsToResync.empty()) {
                sClient->LocationChecks(locationsToResync);
            }
        }

        // TODO (MM): Save/load of AP state (use ArchipelagoBridge::SaveState/LoadState)
    });

    sClient->set_slot_refused_handler([](const std::list<std::string>& msgs) {
        sDisconnecting = true;

        // Collect all error messages
        std::string allErrors;
        for (const std::string& msg : msgs) {
            ArchipelagoConsole_SendMessage("[ERROR] %s", msg.c_str());
            if (!allErrors.empty()) {
                allErrors += "; ";
            }
            allErrors += msg;
        }

        // Emit error notification with the error messages
        if (!allErrors.empty()) {
            Notification::Emit({ .message = allErrors.c_str(),
                                 .messageColor = ImVec4(1.0f, 0.5f, 0.5f, 1.0f),
                                 .remainingTime = 7.0f });
        }
    });

    sClient->set_items_received_handler([](const std::list<APClient::NetworkItem>& items) {
        if (sDisconnecting) {
            return;
        }

        // The AP server sends ReceivedItems in two ways:
        //   Full sync  (first item index == 0): all items from the beginning, on initial connect.
        //   Delta      (first item index  > 0): only newly unlocked items, e.g. after a location check.
        //
        // We must NOT clear the cache on a delta — doing so would wipe the full history and leave
        // ResyncItems with only the 1 new item, breaking re-sync when a new save is created.
        // Only clear when it's a full sync (index 0).
        const bool isFullSync = !items.empty() && (items.front().index == 0);
        if (isFullSync) {
            sCachedReceivedItems.clear();
        }
        sCachedReceivedItems.reserve(sCachedReceivedItems.size() + items.size());

        for (const APClient::NetworkItem& item : items) {
            // Try resolving from local game first (items sent TO us), then fall back to player's game
            std::string itemName = sClient->get_item_name((int64_t)item.item, "2 Ship 2 Harkinian (MM)");
            if (itemName == "Unknown" || itemName.empty()) {
                itemName = sClient->get_item_name((int64_t)item.item, sClient->get_player_game(item.player));
            }

            // Cache the item with resolved name
            sCachedReceivedItems.push_back({ item, itemName });

            // Enqueue for processing
            ArchipelagoBridge::EnqueueItem((uint64_t)item.item, (int)item.player, (int64_t)item.index,
                                           (uint32_t)item.flags, itemName);
        }
    });

    sClient->set_location_info_handler([](const std::list<APClient::NetworkItem>& items) {
        if (sDisconnecting) {
            return;
        }

        // Build a JSON object mapping location ID -> item info
        nlohmann::json locationInfo = nlohmann::json::object();

        for (const auto& item : items) {
            nlohmann::json itemData;
            itemData["item"] = item.item;
            itemData["player"] = item.player;
            itemData["flags"] = item.flags;

            // Get the item name
            std::string itemName =
                sClient->get_item_name(static_cast<int64_t>(item.item), sClient->get_player_game(item.player));
            itemData["item_name"] = itemName;

            // Store by location ID
            locationInfo[std::to_string(item.location)] = itemData;
        }

        // Pass to bridge to populate RANDO_SAVE_CHECKS
        ArchipelagoBridge::PopulateLocationRewards(locationInfo);

        // Mark as fully connected + scouted (ready for gameplay)
        UpdateConnectionStatus(STATUS_CONNECTED_SCOUTED);

        // Emit success notification with slot name
        if (sClient) {
            std::string slotName = sClient->get_player_alias(sClient->get_player_number());
            std::string message = "Connected to slot: " + slotName;
            Notification::Emit(
                { .message = message.c_str(), .messageColor = ImVec4(0.5f, 1.0f, 0.5f, 1.0f), .remainingTime = 5.0f });
        }
    });

    sClient->set_location_checked_handler([](const std::list<int64_t> locations) {
        if (sDisconnecting) {
            return;
        }

        // For now, forward directly to bridge (bridge can decide to enqueue/persist).
        for (const int64_t apLoc : locations) {
            ArchipelagoBridge::MarkLocationChecked((uint64_t)apLoc);
        }
    });

    sClient->set_print_json_handler([](const APClient::PrintJSONArgs& arg) {
        if (sDisconnecting) {
            return;
        }

        std::vector<AP_Text::ColoredTextNode> coloredNodes;
        coloredNodes.reserve(arg.data.size());

        for (const APClient::TextNode& node : arg.data) {
            AP_Text::TextColor color = AP_Text::TextColor::COLOR_DEFAULT;
            std::string text;

            // This mapping is ported from SoH. It depends on APClient�s TextNode shape.
            // If your APClient version differs, adjust fields accordingly.
            if (node.type == "player_id") {
                int id = std::stoi(node.text);
                if (color == AP_Text::TextColor::COLOR_DEFAULT && id == sClient->get_player_number()) {
                    color = AP_Text::TextColor::COLOR_MAGENTA;
                } else if (color == AP_Text::TextColor::COLOR_DEFAULT) {
                    color = AP_Text::TextColor::COLOR_YELLOW;
                }
                text = sClient->get_player_alias(id);
            } else if (node.type == "item_id") {
                int64_t id = std::stoll(node.text);
                if (color == AP_Text::TextColor::COLOR_DEFAULT) {
                    if (node.flags & APClient::ItemFlags::FLAG_ADVANCEMENT) {
                        color = AP_Text::TextColor::COLOR_PLUM;
                    } else if (node.flags & APClient::ItemFlags::FLAG_NEVER_EXCLUDE) {
                        color = AP_Text::TextColor::COLOR_SLATEBLUE;
                    } else if (node.flags & APClient::ItemFlags::FLAG_TRAP) {
                        color = AP_Text::TextColor::COLOR_SALMON;
                    } else {
                        color = AP_Text::TextColor::COLOR_CYAN;
                    }
                }
                text = sClient->get_item_name(id, sClient->get_player_game(node.player));
            } else if (node.type == "location_id") {
                int64_t id = std::stoll(node.text);
                if (color == AP_Text::TextColor::COLOR_DEFAULT) {
                    color = AP_Text::TextColor::COLOR_BLUE;
                }
                text = sClient->get_location_name(id, sClient->get_player_game(node.player));
            } else if (node.type == "hint_status") {
                text = node.text;
                if (node.hintStatus == APClient::HINT_FOUND) {
                    color = AP_Text::TextColor::COLOR_GREEN;
                } else if (node.hintStatus == APClient::HINT_UNSPECIFIED) {
                    color = AP_Text::TextColor::COLOR_GRAY;
                } else if (node.hintStatus == APClient::HINT_NO_PRIORITY) {
                    color = AP_Text::TextColor::COLOR_SLATEBLUE;
                } else if (node.hintStatus == APClient::HINT_AVOID) {
                    color = AP_Text::TextColor::COLOR_SALMON;
                } else if (node.hintStatus == APClient::HINT_PRIORITY) {
                    color = AP_Text::TextColor::COLOR_PLUM;
                } else {
                    color = AP_Text::TextColor::COLOR_RED;
                }
            } else if (node.type == "ERROR") {
                color = AP_Text::TextColor::COLOR_ERROR;
                text = node.text;
            } else if (node.type == "LOG") {
                color = AP_Text::TextColor::COLOR_LOG;
                text = node.text;
            } else {
                color = AP_Text::TextColor::COLOR_WHITE;
                text = node.text;
            }

            AP_Text::ColoredTextNode out;
            out.color = color;
            out.text = text;
            coloredNodes.push_back(out);
        }

        ArchipelagoConsole_PrintJson(coloredNodes);
    });

    sClient->set_bounced_handler([](const nlohmann::json data) {
        if (sDisconnecting || !data.contains("tags")) {
            return;
        }

        std::list<std::string> tags = data["tags"];
        const bool deathLink = (std::find(tags.begin(), tags.end(), "DeathLink") != tags.end());
        if (!deathLink || !data.contains("data")) {
            return;
        }

        // Mirror SoH logic shape, but keep gameplay in Bridge.
        // NOTE: AP "source" in SoH is compared vs apClient->get_slot() (string).
        try {
            const std::string source = data["data"]["source"];
            const std::string cause = data["data"]["cause"];

            // Don�t self-kill if it�s our own bounce (SoH checks this).
            if (sClient && source != sClient->get_slot()) {
                // Apply only if in-game (SoH does).
                if (ArchipelagoBridge::IsInGame()) {
                    // Queue to bridge; bridge applies on main thread via Tick()
                    ArchipelagoBridge::EnqueueDeathLink(source, cause);

                    ArchipelagoConsole_SendMessage("[LOG] Received death link from %s. Cause: %s", source.c_str(),
                                                   cause.c_str());
                    sIsDeathLinkedDeath = true;
                }
            }
        } catch (...) {
            // ignore malformed bounce payload
        }
    });
}

} // namespace

void Archipelago::OnFileLoad(s16 fileNum) {
    (void)fileNum;

    // Re-initialize enhancements that depend on whether we're archi or not
    ShipInit::Init("IS_ARCHI");

    // Clear session-only item dedupe so items can be re-applied for this file
    ArchipelagoBridge::OnFileLoad();

    // If this save is NOT Archipelago, ensure we shut down cleanly
    if (!IS_ARCHI) {
        sDisconnecting = true;
        return;
    }

    // Re-sync all cached items for this save file
    // This ensures items are received even if we were already connected when loading the file
    ResyncItems();
}

void Archipelago::SendChat(const char* msg) {
    if (msg == nullptr || msg[0] == '\0') {
        return;
    }

    if (!sClient || !IsSlotConnected()) {
        return;
    }

    sClient->Say(std::string(msg));
}

void Archipelago::Init() {
    // Keep this light. Any defaults you want to guarantee can be set here.
    // NOTE: We use Host + Port as primary connection target.

    // Initialize connection status to NOT_CONNECTED
    CVarSetInteger("gArchipelago.ConnectionStatus", STATUS_NOT_CONNECTED);
    CVarSave();

    int port = CVarGetInteger("gArchipelago.Port", 38281);
    if (port <= 0 || port > 65535) {
        CVarSetInteger("gArchipelago.Port", 38281);
    }

    const char* host = CVarGetString("gArchipelago.Host", "");
    if (host == nullptr || host[0] == '\0') {
        CVarSetString("gArchipelago.Host", "archipelago.gg");
    }

    const char* slot = CVarGetString("gArchipelago.Slot", "");
    if (slot == nullptr || slot[0] == '\0') {
        CVarSetString("gArchipelago.Slot", "");
    }

    const char* pass = CVarGetString("gArchipelago.Password", "");
    if (pass == nullptr || pass[0] == '\0') {
        CVarSetString("gArchipelago.Password", "");
    }
}

void Archipelago::Shutdown() {
    // Request disconnect; Update() will cleanly reset.
    sDisconnecting = true;
}

void Archipelago::Update() {
    // If we don't even have a client, nothing to do.
    if (!sClient) {
        return;
    }

    // Handle requested disconnect no matter what state we're in (menu/in-game/etc).
    if (sDisconnecting) {
        sClient->reset();
        sClient = nullptr;

        ArchipelagoBridge::Reset();

        ResetQueue();
        sDisconnecting = false;
        sRetries = 0;
        sStatus = "Disconnected";
        UpdateConnectionStatus(STATUS_NOT_CONNECTED);
        sIsDeathLinkedDeath = false;
        sSentDeathThisLife = false;
        return;
    }

    // Only apply gameplay effects when we're actually in an Archipelago save.
    if (IS_ARCHI) {
        ArchipelagoBridge::Tick();

        // Death link sending: check if player died (and it wasn't from receiving a death link)
        if (CVarGetInteger("gArchipelago.DeathLink", 0) && ArchipelagoBridge::IsInGame()) {
            s16 currentHealth = gSaveContext.save.saveInfo.playerData.health;

            // Player died (health reached 0)
            if (currentHealth == 0 && !sSentDeathThisLife && !sIsDeathLinkedDeath) {
                // Send death link to other players
                if (sClient && sClient->get_state() == APClient::State::SLOT_CONNECTED) {
                    nlohmann::json deathLinkData;
                    deathLinkData["time"] = (double)time(NULL);
                    // "source" is WHO died (the player's slot name)
                    // "cause" is HOW they died (description)
                    deathLinkData["source"] = CVarGetString("gArchipelago.Slot", "Unknown");
                    deathLinkData["cause"] = "Met with a terrible fate.";

                    sClient->Bounce(deathLinkData, {}, {}, { "DeathLink" });

                    ArchipelagoConsole_SendMessage("[AP] You died! Sending death link to other players...");
                    sSentDeathThisLife = true;
                }
            }
            // Player is alive - reset death tracking
            else if (currentHealth > 0) {
                sSentDeathThisLife = false;
                sIsDeathLinkedDeath = false; // Reset the "received death link" flag when player is alive again
            }
        }
    }

    // Always poll so connections can complete in menus / file select too.
    sClient->poll();
}

void Archipelago::SendLocationCheck(uint64_t locationId) {
    if (!sClient || !IsSlotConnected()) {
        return;
    }

    std::list<int64_t> locs;
    locs.push_back((int64_t)locationId);
    sClient->LocationChecks(locs);
}

static bool ParseHostPort(const char* in, std::string& outHost, int& outPort) {
    if (in == nullptr || in[0] == '\0') {
        return false;
    }

    std::string s(in);

    // Trim spaces
    auto isSpace = [](unsigned char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; };
    while (!s.empty() && isSpace((unsigned char)s.front()))
        s.erase(s.begin());
    while (!s.empty() && isSpace((unsigned char)s.back()))
        s.pop_back();

    if (s.empty()) {
        return false;
    }

    // If it contains a colon, treat as host:port
    size_t colon = s.rfind(':');
    if (colon == std::string::npos) {
        outHost = s;
        return true;
    }

    std::string host = s.substr(0, colon);
    std::string portStr = s.substr(colon + 1);

    if (host.empty() || portStr.empty()) {
        return false;
    }

    int port = 0;
    try {
        port = std::stoi(portStr);
    } catch (...) { return false; }

    if (port <= 0 || port > 65535) {
        return false;
    }

    outHost = host;
    outPort = port;
    return true;
}

void Archipelago::ConnectFromCvars() {
    // Reset prior client
    sClient.reset();
    ResetQueue();
    sDisconnecting = false;
    sIsDeathLinkedDeath = false;
    sSentDeathThisLife = false;
    sRetries = 0;

    const char* pass = CVarGetString("gArchipelago.Password", "");

    // If ServerAddress exists (legacy UI), migrate it into Host/Port once.
    {
        const char* server = CVarGetString("gArchipelago.ServerAddress", "");
        if (server && server[0] != '\0') {
            std::string migratedHost;
            int migratedPort = CVarGetInteger("gArchipelago.Port", 38281);

            if (ParseHostPort(server, migratedHost, migratedPort)) {
                CVarSetString("gArchipelago.Host", migratedHost.c_str());
                CVarSetInteger("gArchipelago.Port", migratedPort);

                // Keep ServerAddress in sync for UI display (host:port)
                std::string display = migratedHost + ":" + std::to_string(migratedPort);
                CVarSetString("gArchipelago.ServerAddress", display.c_str());
            }
        }
    }

    const char* host = CVarGetString("gArchipelago.Host", "archipelago.gg");
    int port = CVarGetInteger("gArchipelago.Port", 38281);
    if (port <= 0 || port > 65535) {
        port = 38281;
        CVarSetInteger("gArchipelago.Port", port);
    }
    if (host == nullptr || host[0] == '\0') {
        host = "archipelago.gg";
        CVarSetString("gArchipelago.Host", host);
    }

    const std::string uri = std::string(host) + ":" + std::to_string(port);
    sPassword = (pass ? pass : "");

    const std::string uuid = ap_get_uuid(GetUuidPath());
    const std::string cert = GetCertPath();

    sClient = std::unique_ptr<APClient>(new APClient(uuid, AP_Client_consts::AP_GAME_NAME, uri, cert));

    sStatus = "Connecting...";
    UpdateConnectionStatus(STATUS_CONNECTING);

    InstallHandlers();
}

void Archipelago::SetDeathLinkTag() {
    if (!Archipelago::IsConnected()) {
        return;
    }
    std::list<std::string> tags;
    if (CVarGetInteger("gArchipelago.DeathLink", 0)) {
        tags.push_back("DeathLink");
    }
    sClient->ConnectUpdate(false, 1, true, tags);
}

void Archipelago::Disconnect() {
    // Mirror SoH behavior: request disconnect and let Update() tear down cleanly
    sDisconnecting = true;
}

bool Archipelago::IsConnected() {
    return IsSlotConnected();
}

bool Archipelago::IsConnecting() {
    return sClient && (sClient->get_state() != APClient::State::SLOT_CONNECTED) && !sDisconnecting;
}

const char* Archipelago::GetStatusText() {
    return sStatus.c_str();
}

std::string Archipelago::GetPlayerAlias(int playerId) {
    if (!sClient) {
        return "Player " + std::to_string(playerId);
    }
    return sClient->get_player_alias(playerId);
}

int Archipelago::GetPlayerNumber() {
    if (!sClient) {
        return -1;
    }
    return sClient->get_player_number();
}

std::string Archipelago::GetItemName(int64_t itemId, const std::string& game) {
    if (!sClient) {
        return "Unknown";
    }
    return sClient->get_item_name(itemId, game);
}

std::string Archipelago::GetPlayerGame(int playerId) {
    if (!sClient) {
        return "";
    }
    return sClient->get_player_game(playerId);
}

void Archipelago::ResyncItems() {
    if (!sClient) {
        return;
    }

    if (sCachedReceivedItems.empty()) {
        return;
    }

    // Clear session dedupe so items can be re-enqueued
    ArchipelagoBridge::ClearSessionDedupe();

    // Re-enqueue all cached items - EnqueueItem will filter based on current save's receivedItemCount
    for (const auto& cached : sCachedReceivedItems) {
        // Use the cached resolved name instead of trying to re-resolve
        ArchipelagoBridge::EnqueueItem((uint64_t)cached.item.item, (int)cached.item.player, (int64_t)cached.item.index,
                                       (uint32_t)cached.item.flags, cached.resolvedName);
    }
}

void Archipelago::RegisterMenu() {
    BenGui::mBenMenu->AddMenuEntry("Archipelago", "gSettings.Menu.ArchipelagoSidebarSection");

    // Make this sidebar page have 2 columns
    BenGui::mBenMenu->AddSidebarEntry("Archipelago", "Windows", 2);

    // Left column: Settings
    {
        WidgetPath left = { "Archipelago", "Windows", SECTION_COLUMN_1 };
        BenGui::mBenMenu->AddWidget(left, "Settings", WIDGET_WINDOW_BUTTON)
            .CVar("gWindows.ArchipelagoSettings")
            .WindowName("Archipelago Settings");
    }

    // Right column: Console
    {
        WidgetPath right = { "Archipelago", "Windows", SECTION_COLUMN_2 };
        BenGui::mBenMenu->AddWidget(right, "Console", WIDGET_WINDOW_BUTTON)
            .CVar("gWindows.ArchipelagoConsole")
            .WindowName("Archipelago Console");
    }
}

static RegisterMenuInitFunc initFunc(Archipelago::RegisterMenu);

static void OnArchipelagoSaveLoadHandler(s16 fileNum) {
    Archipelago::OnFileLoad(fileNum);
}

static void OnArchipelagoGameCompletionHandler() {
    if (!IS_ARCHI) {
        return;
    }

    if (sClient && sClient->get_state() == APClient::State::SLOT_CONNECTED) {
        sClient->StatusUpdate(APClient::ClientStatus::GOAL);
    }
}

static void RegisterArchipelagoTick() {
    COND_HOOK(GameInteractor::OnGameStateUpdate, true, []() { Archipelago::Update(); });

    // Mirror Rando: register a save-load hook once on boot
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnSaveLoad>(OnArchipelagoSaveLoadHandler);

    // Register game completion hook to notify AP server when game is beaten
    GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameCompletion>(OnArchipelagoGameCompletionHandler);

    Archipelago::Init();
}

static RegisterShipInitFunc apInit(RegisterArchipelagoTick);

// C-callable wrapper for IsConnected
extern "C" int Archipelago_IsConnected(void) {
    return Archipelago::IsConnected() ? 1 : 0;
}

extern "C" void Archipelago_ConnectFromCvars(void) {
    Archipelago::ConnectFromCvars();
}
