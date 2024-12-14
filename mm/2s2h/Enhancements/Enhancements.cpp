#include "Enhancements.h"

void InitEnhancements() {
    // Cycle & Saving
    RegisterEndOfCycleSaveHooks();
    RegisterSavingEnhancements();
    RegisterAutosave();
    RegisterKeepExpressMail();

    // Uncomment to enable the demo behavior, this shows of different modding capabilities
    // void RegisterDemoBehavior();
    // RegisterDemoBehavior();
}
