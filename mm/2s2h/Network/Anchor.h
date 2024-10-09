#ifdef ENABLE_NETWORKING
#ifndef NETWORK_ANCHOR_H
#define NETWORK_ANCHOR_H
#ifdef __cplusplus

#include "Network.h"
#include <libultraship/libultraship.h>

extern "C" {
#include "z64.h"
}

typedef struct {
    uint32_t clientId;
    std::string name;
    Color_RGB8 color;
    std::string clientVersion;
    uint32_t seed;
    bool isSaveLoaded;
    bool isGameComplete;
    int16_t sceneId;
    int32_t entrance;
    int8_t roomIndex;

    // Only available in PLAYER_UPDATE packets
    uint8_t transformation;
    PosRot posRot;
    u8 jointTable[159];
    Player player;
} AnchorClient;

class Anchor : public Network {
  private:
    static const std::string clientVersion;
    bool refreshingActors = false;

    bool IsSaveLoaded();
    void RegisterHooks();
    void RefreshClientActors();
    void HandlePacket_AllClientData(nlohmann::json payload);
    void SendPacket_UpdateClientData();
    void HandlePacket_UpdateClientData(nlohmann::json payload);
    void SendPacket_PlayerUpdate();
    void HandlePacket_PlayerUpdate(nlohmann::json payload);
    void SendPacket_RequestSaveState();
    void HandlePacket_RequestSaveState(nlohmann::json payload);
    void SendPacket_PushSaveState();
    void HandlePacket_PushSaveState(nlohmann::json payload);

  public:
    static Anchor* Instance;
    std::map<uint32_t, AnchorClient> clients;
    std::vector<uint32_t> actorIndexToClientId;

    void Enable();
    void Disable();
    void OnIncomingJson(nlohmann::json payload);
    void OnConnected();
    void OnDisconnected();
    void DrawMenu();
    void SendJsonToRemote(nlohmann::json packet);
};

#endif // __cplusplus
#endif // NETWORK_ANCHOR_H
#endif // ENABLE_NETWORKING
