#ifndef RANDO_SPOILER_H
#define RANDO_SPOILER_H

#include "Rando/Rando.h"

namespace Rando {

namespace Spoiler {

extern std::vector<std::string> spoilerOptions;
void RefreshOptions();
void GenerateFromSaveContext();
void SaveToFile(std::string filePath);
void LoadFromFile(std::string filePath);
void ApplyToSaveContext();
bool HandleFileDropped(std::string filePath);

} // namespace Spoiler

} // namespace Rando

#endif
