#ifndef NETWORK_ARCHIPELAGO_H
#define NETWORK_ARCHIPELAGO_H

#define IS_ARCHI (IS_RANDO && gSaveContext.save.shipSaveInfo.rando.isArchiSave)

#ifdef __cplusplus

#include <unordered_map>
#include <set>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "2s2h/Rando/Rando.h"

// Forward declare from APClient
struct NetworkItem {
    int64_t item;
    int64_t location;
    int player;
    unsigned flags;
    int index = -1;
};

struct ForeignItemLocation {
    std::string playerName;
    std::string locationName;
    std::string hintText;
    bool obtained;
};

class Archipelago {
  private:
    int connectionRetryCount;
    bool hasEverConnected;
    bool isConnectionReady;
    bool isSlotDataReady;
    bool isCheckInfoReady;
    bool isSaveSynced;
    bool isItemQueued;
    bool isBuildVersionRejected;
    // Slot data which includes options and price info
    nlohmann::json slotData;
    // Checked Locations, size compared with gSaveContext...archipelago.checkedLocationCount
    std::set<int64_t> incomingCheckedLocations;
    // Items indexed by gSaveContext...archipelago.receivedItemCount
    std::vector<NetworkItem> incomingItems;

    // Lifecycle
    void RegisterHooks();
    void OnConnected();
    void OnDisconnected();
    void OnGameTick();
    void ProcessItemQueue();
    void GrantPendingItemsImmediately();
    bool VerifyBuildVersion();
    void Reset();

  public:
    static bool IsAPItem(RandoItemId randoItemId);
    static Archipelago* Instance;

    // Map of actually shuffled checks, their item and player
    std::unordered_map<RandoCheckId, NetworkItem> checkInfo;

    void Enable();
    void Disable();
    void DrawMenu();
    u8 GetState();
    bool IsConnected();
    bool EnsureConnected();

    void SendChat(const char* msg);
    void SendLocationCheck(RandoCheckId randoCheckId);
    void GetArchipelagoItemInfo(RandoCheckId checkId, std::string& playerName, std::string& itemName);
    RandoItemId GetRandoItemIdFromNetworkItem(NetworkItem networkItem, bool convertOtherPlayerItems = false);
    std::vector<ForeignItemLocation> GetForeignItemLocations(RandoItemId randoItemId);
    bool IsCheckForSameGame(RandoCheckId checkId);
    void UpdateDeathLinkTag();
};

#endif // __cplusplus
#endif // NETWORK_ARCHIPELAGO_H
