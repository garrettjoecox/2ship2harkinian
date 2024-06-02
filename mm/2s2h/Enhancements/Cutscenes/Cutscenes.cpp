#include "Cutscenes.h"

void RegisterCutscenes() {
    // MiscInteractions
    RegisterSkipDekuSalesman();
    RegisterSkipScarecrowDance();
    RegisterSkipTatlInterrupts();

    // StoryCutscenes
    RegisterSkipClockTowerOpen();
    RegisterSkipLearningSongOfTime();

    RegisterHideTitleCards();
    RegisterSkipEntranceCutscenes();
    RegisterSkipIntroSequence();
}
