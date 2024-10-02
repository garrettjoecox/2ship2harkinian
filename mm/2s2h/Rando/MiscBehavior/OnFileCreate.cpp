#include "MiscBehavior.h"
#include <libultraship/libultraship.h>
#include <fstream>
#include "BenPort.h"
#include "Rando/Spoiler/Spoiler.h"
#include <boost_custom/container_hash/hash_32.hpp>

extern "C" {
#include "variables.h"
#include "ShipUtils.h"
}

// Very primitive randomizer implementation, when a save is created, if rando is enabled
// we set the save type to rando and shuffle all checks and persist the results to the save
void Rando::MiscBehavior::OnFileCreate(s16 fileNum) {
    if (CVarGetInteger("gRando.Enabled", 0)) {
        gSaveContext.save.shipSaveInfo.saveType = SAVETYPE_RANDO;
        memset(&gSaveContext.save.shipSaveInfo.rando, 0, sizeof(gSaveContext.save.shipSaveInfo.rando));

        try {
            if (CVarGetInteger("gRando.SpoilerFileIndex", 0) == 0) {
                bool hadInputSeed = true;
                std::string inputSeed = CVarGetString("gRando.InputSeed", "");
                if (inputSeed.empty()) {
                    inputSeed = std::to_string(Ship_Random(0, 1000000));
                    hadInputSeed = false;
                }

                uint32_t finalSeed = boost::hash_32<std::string>{}(inputSeed);
                Ship_Random_Seed(finalSeed);

                for (auto& [randoOptionId, randoStaticOption] : Rando::StaticData::Options) {
                    RANDO_SAVE_OPTIONS[randoOptionId] = CVarGetInteger(randoStaticOption.cvar, 0);
                }

                if (RANDO_SAVE_OPTIONS[RO_LOGIC] == RO_LOGIC_VANILLA) {
                    for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
                        if (randoStaticCheck.randoCheckId != RC_UNKNOWN) {
                            RANDO_SAVE_CHECKS[randoCheckId].randoItemId = randoStaticCheck.randoItemId;
                        }
                    }
                } else {
                    std::vector<RandoItemId> itemPool;
                    for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
                        if (randoStaticCheck.randoCheckId != RC_UNKNOWN) {
                            itemPool.push_back(randoStaticCheck.randoItemId);
                        }
                    }

                    for (size_t i = 0; i < itemPool.size(); i++) {
                        std::swap(itemPool[i], itemPool[Ship_Random(0, itemPool.size() - 1)]);
                    }

                    for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
                        if (randoStaticCheck.randoCheckId != RC_UNKNOWN) {
                            RANDO_SAVE_CHECKS[randoCheckId].randoItemId = itemPool.back();
                            itemPool.pop_back();
                        }
                    }
                }

                if (CVarGetInteger("gRando.GenerateSpoiler", 0)) {
                    nlohmann::json j;
                    j["type"] = "2S2H_RANDO_SPOILER";
                    j["inputSeed"] = inputSeed;
                    j["finalSeed"] = finalSeed;

                    j["options"] = nlohmann::json::object();
                    for (auto& [randoOptionId, randoStaticOption] : Rando::StaticData::Options) {
                        j["options"][randoStaticOption.name] = RANDO_SAVE_OPTIONS[randoOptionId];
                    }

                    j["checks"] = nlohmann::json::object();
                    for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
                        if (randoStaticCheck.randoCheckId != RC_UNKNOWN) {
                            j["checks"][randoStaticCheck.name] =
                                Rando::StaticData::Items[RANDO_SAVE_CHECKS[randoCheckId].randoItemId].spoilerName;
                        }
                    }

                    std::string spoilerFile = inputSeed + ".json";
                    std::string spoilerFilePath =
                        Ship::Context::GetPathRelativeToAppDirectory("randomizer/" + spoilerFile, appShortName);
                    std::ofstream fileStream(spoilerFilePath);
                    if (!fileStream.is_open()) {
                        throw std::runtime_error("Failed to open spoiler file");
                    }

                    fileStream << j.dump(4);

                    if (hadInputSeed) {
                        CVarSetString("gRando.SpoilerFile", spoilerFile.c_str());
                    }
                    Rando::Spoiler::RefreshOptions();
                }

                Audio_PlaySfx(NA_SE_SY_ATTENTION_SOUND);
            } else {
                std::string spoilerFile = CVarGetString("gRando.SpoilerFile", "");
                if (spoilerFile.empty()) {
                    throw std::runtime_error("Attempted to load spoiler file with no file selected");
                }

                std::string spoilerFilePath =
                    Ship::Context::GetPathRelativeToAppDirectory("randomizer/" + spoilerFile, appShortName);
                std::ifstream fileStream(spoilerFilePath);
                if (!fileStream.is_open()) {
                    throw std::runtime_error("Failed to open spoiler file");
                }

                nlohmann::json j;
                try {
                    fileStream >> j;
                } catch (nlohmann::json::exception& e) {
                    throw std::runtime_error("Failed to parse spoiler file: " + std::string(e.what()));
                }

                if (!j.contains("type") || j["type"] != "2S2H_RANDO_SPOILER") {
                    throw std::runtime_error("Spoiler file is not a valid spoiler file");
                }

                for (auto& [randoOptionId, randoStaticOption] : Rando::StaticData::Options) {
                    RANDO_SAVE_OPTIONS[randoOptionId] = j["options"][randoStaticOption.name].get<int>();
                }

                for (auto& [randoCheckId, randoStaticCheck] : Rando::StaticData::Checks) {
                    if (randoStaticCheck.randoCheckId != RC_UNKNOWN) {
                        RANDO_SAVE_CHECKS[randoCheckId].randoItemId = Rando::StaticData::GetItemIdFromName(
                            j["checks"][randoStaticCheck.name].get<std::string>().c_str());
                    }
                }

                Audio_PlaySfx(NA_SE_SY_ATTENTION_SOUND);
            }
        } catch (const std::exception& e) {
            SPDLOG_ERROR("Error with randomizer save creation: {}", e.what());
            Audio_PlaySfx(NA_SE_SY_QUIZ_INCORRECT);
            gSaveContext.save.shipSaveInfo.saveType = SAVETYPE_VANILLA;
            char invalidName[8] = { 18, 23, 31, 10, 21, 18, 13, 62 };
            memcpy(gSaveContext.save.saveInfo.playerData.playerName, invalidName, sizeof(invalidName));
            gSaveContext.save.saveInfo.playerData.newf[0] = '\0';
        }
    }
}
