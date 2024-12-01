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
    RegisterSkipLearningElegyOfEmptiness();
    RegisterSkipLearningSongOfHealing();
    RegisterSkipLearningSongOfTime();
    RegisterSkipSongOfTimeCutscene();
    RegisterSkipMilkRunCutscenes();
    RegisterSkipBombBagTheftCutscene();
    RegisterSkipRosaSistersDance();
    RegisterSkipLearningSonataOfAwakening();
    RegisterSkipBossCutscenes();
    RegisterSkipGiantsChamber();
    RegisterSkipHealingDarmani();
    RegisterSkipHealingMikau();

    RegisterHideTitleCards();
    RegisterSkipEntranceCutscenes();
    RegisterSkipIntroSequence();
    RegisterSkipToFileSelect();
}
