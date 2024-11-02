#include "Cutscenes.h"

void RegisterCutscenes() {
    // MiscInteractions
    RegisterSkipDekuSalesman();
    RegisterSkipFallingMoonsTear();
    RegisterSkipGreatFairyCutscene();
    RegisterSkipKafeiReveal();
    RegisterSkipScarecrowDance();
    RegisterSkipTatlInterrupts();
    RegisterSkipMayorsOfficeCutscene();
    RegisterSkipKamaroTeachDance();

    // StoryCutscenes
    RegisterSkipClockTowerOpen();
    SkipClockTowerSkullKidEncounter();
    RegisterSkipLearningSongOfHealing();
    RegisterSkipLearningSongOfTime();
    RegisterSkipSongOfTimeCutscene();
    RegisterSkipMilkRunCutscenes();
    RegisterSkipBombBagTheftCutscene();

    RegisterHideTitleCards();
    RegisterSkipEntranceCutscenes();
    RegisterSkipIntroSequence();
    RegisterSkipToFileSelect();
}
