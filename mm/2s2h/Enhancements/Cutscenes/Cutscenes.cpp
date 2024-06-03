#include "Cutscenes.h"

void RegisterCutscenes() {
    // MiscInteractions
    RegisterSkipDekuSalesman();
    RegisterSkipScarecrowDance();
    RegisterSkipTatlInterrupts();

    // StoryCutscenes
    RegisterSkipClockTowerOpen();

    RegisterHideTitleCards();
    RegisterSkipEntranceCutscenes();
    RegisterSkipGetItemCutscene();
    RegisterSkipIntroSequence();
}
