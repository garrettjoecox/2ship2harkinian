#include "Enhancements.h"

void InitEnhancements() {
    // Camera
    RegisterCameraInterpolationFixes();
    RegisterCameraFreeLook();
    RegisterDebugCam();

    // Cheats
    RegisterInfiniteCheats();
    RegisterLongerFlowerGlide();
    RegisterMoonJumpOnL();
    RegisterUnbreakableRazorSword();
    RegisterUnrestrictedItems();
    RegisterTimeStopInTemples();
    RegisterElegyAnywhere();
    RegisterHookshotAnywhere();

    // Clock
    RegisterTextBasedClock();
    Register3DSClock();

    // Cycle & Saving
    RegisterEndOfCycleSaveHooks();
    RegisterSavingEnhancements();
    RegisterAutosave();
    RegisterKeepExpressMail();

    // Dialogue
    RegisterFastBankSelection();

    // Equipment
    RegisterBetterPictoMessage();
    RegisterSkipMagicArrowEquip();
    RegisterInstantRecall();

    // Fixes
    RegisterFierceDeityZTargetMovement();
    RegisterTwoHandedSwordSpinAttack();
    RegisterCompletedHeartContainerAudio();
    RegisterFixEponaStealingSword();

    // Graphics
    RegisterDisableBlackBars();
    RegisterHyruleWarriorsStyledLink();
    Register3DItemDrops();

    // Masks
    RegisterFastTransformation();
    RegisterFierceDeityAnywhere();
    RegisterBlastMaskKeg();
    RegisterNoBlastMaskCooldown();
    RegisterPersistentMasks();

    // Minigames
    RegisterAlwaysWinDoggyRace();
    RegisterCremiaHugs();
    RegisterPowderKegCertification();
    RegisterCuccoShackCuccoCount();
    RegisterSwordsmanSchool();

    // Player
    RegisterClimbSpeed();
    RegisterFastFlowerLaunch();
    RegisterInstantPutaway();
    RegisterFierceDeityPutaway();

    // Songs
    RegisterBetterSongOfDoubleTime();
    RegisterEnableSunsSong();
    RegisterFasterSongPlayback();
    RegisterPauseOwlWarp();
    RegisterZoraEggCount();
    RegisterSkipScarecrowSong();

    // Restorations
    RegisterPowerCrouchStab();
    RegisterSideRoll();
    RegisterTatlISG();
    RegisterVariableFlipHop();
    RegisterWoodfallMountainAppearance();

    // Cutscenes
    RegisterCutscenes();

    // Modes
    RegisterPlayAsKafei();
    RegisterTimeMovesWhenYouMove();

    // Difficulty Options
    RegisterCustomBankRewardThresholds();
    RegisterDisableTakkuriSteal();
    RegisterShowDekuGuardSearchBalls();

    // Uncomment to enable the demo behavior, this shows of different modding capabilities
    // void RegisterDemoBehavior();
    // RegisterDemoBehavior();
}
