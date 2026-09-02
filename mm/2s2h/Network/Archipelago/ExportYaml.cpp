#include "ExportYaml.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <vector>

#include <libultraship/libultraship.h>
#include <libultraship/bridge/consolevariablebridge.h>

#include "2s2h/BenPort.h"
#include "2s2h/Rando/Rando.h"
#include "build.h"

namespace {

const char* gameName = "2 Ship 2 Harkinian (MM)";

// Options the apworld exposes as a Range.
const std::set<RandoOptionId> numericOptions = {
    RO_ACCESS_MAJORA_MASKS_COUNT,
    RO_ACCESS_MAJORA_REMAINS_COUNT,
    RO_ACCESS_MOON_MASKS_COUNT,
    RO_ACCESS_MOON_REMAINS_COUNT,
    RO_CLOCK_TERMINAL_TIME,
    RO_HINTS_GOSSIP_STONE_STRENGTH,
    RO_SKULLTULA_SHUFFLED,
    RO_SKULLTULA_TOKENS_REQUIRED,
    RO_STARTING_HEALTH,
    RO_STARTING_RUPEES,
    RO_STRAY_FAIRIES_MAX,
    RO_STRAY_FAIRIES_REQUIRED,
    RO_TRAP_AMOUNT,
    RO_TRIFORCE_PIECES_MAX,
    RO_TRIFORCE_PIECES_REQUIRED,
};

// The apworld's option_<name> spellings, indexed by the option's value.
const std::map<RandoOptionId, std::vector<const char*>> choiceTokens = {
    { RO_ACCESS_DUNGEONS, { "form_and_song", "form_or_song", "form_only", "song_only", "open" } },
    { RO_ACCESS_TRIALS, { "vanilla", "20_masks", "remains", "forms", "open" } },
    { RO_CLOCK_SHUFFLE_PROGRESSIVE, { "randomized", "ascending", "descending" } },
    { RO_LOGIC, { "glitchless", "no_logic", "nearly_no_logic", "vanilla" } },
    { RO_PLACEMENT_BOSS_KEYS, { "anywhere", "own_dungeon", "start_with", "vanilla" } },
    { RO_PLACEMENT_SMALL_KEYS, { "anywhere", "own_dungeon", "start_with", "vanilla" } },
    { RO_PLACEMENT_STRAY_FAIRIES, { "anywhere", "own_dungeon", "start_with", "vanilla" } },
    { RO_SHUFFLE_BOSS_REMAINS, { "vanilla", "anywhere", "own_dungeon" } },
    { RO_SHUFFLE_SONGS, { "anywhere", "song_locations", "vanilla" } },
};

const std::map<RandoOptionId, RandoItemId> shuffleWhenNotStartingWith = {
    { RO_SHUFFLE_OCARINA, RI_OCARINA },
    { RO_SHUFFLE_SHIELD, RI_SHIELD_HERO },
    { RO_SHUFFLE_SONG_TIME, RI_SONG_TIME },
    { RO_SHUFFLE_SWORD, RI_PROGRESSIVE_SWORD },
};

const std::set<std::string> smallWords = { "and", "in", "of", "on", "the", "to", "with" };

std::string Quoted(const std::string& value) {
    std::string quoted = "\"";
    for (char c : value) {
        if (c == '"' || c == '\\') {
            quoted += '\\';
        }
        quoted += c;
    }
    return quoted + "\"";
}

std::string OptionValue(RandoOptionId randoOptionId, u32 value) {
    auto choice = choiceTokens.find(randoOptionId);
    if (choice != choiceTokens.end() && value < choice->second.size()) {
        std::string token = choice->second[value];
        return std::isalpha((unsigned char)token[0]) ? token : Quoted(token);
    }

    if (value <= 1 && !numericOptions.contains(randoOptionId)) {
        return value ? "true" : "false";
    }

    return std::to_string(value);
}

// AP locations aren't exactly 1:1 with the RC enum, so we convert them
std::string ApLocationName(const std::string& checkName) {
    std::string content = checkName.rfind("RC_", 0) == 0 ? checkName.substr(3) : checkName;
    std::string result;

    std::istringstream stream(content);
    std::string word;
    while (std::getline(stream, word, '_')) {
        std::transform(word.begin(), word.end(), word.begin(), [](unsigned char c) { return std::tolower(c); });
        if (!word.empty() && (result.empty() || !smallWords.contains(word))) {
            word[0] = std::toupper((unsigned char)word[0]);
        }
        if (!result.empty()) {
            result += " ";
        }
        result += word;
    }

    return result;
}

} // namespace

namespace ArchipelagoYaml {

bool Export(std::string& filePath, std::string& error) {
    std::string slotName = CVarGetString("gArchipelago.Slot", "");
    // Archipelago caps player names at 16 characters
    if (slotName.length() > 16) {
        slotName = slotName.substr(0, 16);
    }

    std::string fileName;
    for (char c : slotName) {
        fileName += (std::isalnum((unsigned char)c) || c == '-' || c == '_') ? c : '_';
    }
    if (fileName.empty()) {
        fileName = "2ship";
    }

    const std::string buildVersion = std::to_string(gBuildVersionMajor) + "." + std::to_string(gBuildVersionMinor) +
                                     "." + std::to_string(gBuildVersionPatch);

    auto saveInfo = std::make_unique<RandoSaveInfo>();
    memset(saveInfo.get(), 0, sizeof(RandoSaveInfo));
    for (const auto& [randoOptionId, option] : Rando::StaticData::Options) {
        saveInfo->randoSaveOptions[randoOptionId] = CVarGetInteger(option.cvar, option.defaultValue);
    }

    auto startingItems = Rando::GetStartingItemsFromConfig();
    auto startsWith = [&startingItems](RandoItemId randoItemId) {
        return std::find(startingItems.begin(), startingItems.end(), randoItemId) != startingItems.end();
    };

    for (const auto& [randoOptionId, randoItemId] : shuffleWhenNotStartingWith) {
        saveInfo->randoSaveOptions[randoOptionId] = startsWith(randoItemId) ? RO_GENERIC_NO : RO_GENERIC_YES;
    }
    saveInfo->randoSaveOptions[RO_STARTING_BUNNY_HOOD] = startsWith(RI_MASK_BUNNY) ? RO_GENERIC_ON : RO_GENERIC_OFF;

    // TODO: Handle if they start with more wallets
    if (saveInfo->randoSaveOptions[RO_STARTING_RUPEES]) {
        saveInfo->randoSaveOptions[RO_STARTING_RUPEES] = 99;
    }

    std::ostringstream yaml;
    yaml << "# Archipelago options exported from 2 Ship 2 Harkinian " << buildVersion << ".\n"
         << "#\n"
         << "# Copy this file into your Archipelago installation's Players folder before you\n"
         << "# generate, or upload it at https://archipelago.gg/uploads\n"
         << "# It needs the mm_2ship apworld built for 2S2H " << buildVersion << ".\n"
         << "\n"
         << "name: " << Quoted(slotName.empty() ? "Player{number}" : slotName) << "\n"
         << "description: Exported from the 2 Ship 2 Harkinian Archipelago menu\n"
         << "game: " << gameName << "\n"
         << "requires:\n"
         << "  game:\n"
         << "    " << gameName << ": " << buildVersion << "\n"
         << "\n"
         << gameName << ":\n";

    for (const auto& [randoOptionId, option] : Rando::StaticData::Options) {
        u32 value = saveInfo->randoSaveOptions[randoOptionId];
        yaml << "  " << option.apName << ": " << OptionValue(randoOptionId, value);
        if (randoOptionId == RO_LOGIC && (value == RO_LOGIC_NO_LOGIC || value == RO_LOGIC_NEARLY_NO_LOGIC)) {
            yaml << " # the host has to allow this with allow_true_no_logic in host.yaml";
        }
        yaml << "\n";
    }

    for (RandoItemId computedItem : Rando::GetComputedStartingItems(*saveInfo, true)) {
        auto it = std::find(startingItems.begin(), startingItems.end(), computedItem);
        if (it != startingItems.end()) {
            startingItems.erase(it);
        }
    }

    std::map<std::string, int> startingItemCounts;
    for (RandoItemId randoItemId : startingItems) {
        startingItemCounts[Rando::StaticData::Items[randoItemId].name]++;
    }

    if (!startingItemCounts.empty()) {
        yaml << "\n  start_inventory_from_pool:\n";
        for (const auto& [itemName, count] : startingItemCounts) {
            yaml << "    " << Quoted(itemName) << ": " << count << "\n";
        }
    }

    auto excludedChecks = Rando::GetExcludedChecksFromConfig();
    if (!excludedChecks.empty()) {
        yaml << "\n  exclude_locations:\n";
        for (RandoCheckId randoCheckId : excludedChecks) {
            yaml << "    - " << Quoted(ApLocationName(Rando::StaticData::Checks[randoCheckId].name)) << "\n";
        }
    }

    const std::filesystem::path directory =
        std::filesystem::absolute(Ship::Context::GetPathRelativeToAppDirectory("archipelago", appShortName))
            .lexically_normal();

    std::error_code ec;
    std::filesystem::create_directories(directory, ec);

    const std::filesystem::path path = directory / (fileName + ".yaml");
    std::ofstream fileStream(path);
    if (!fileStream.is_open()) {
        error = "Could not write to " + path.string();
        return false;
    }

    fileStream << yaml.str();
    filePath = path.string();
    return true;
}

} // namespace ArchipelagoYaml
