#ifdef ENABLE_NETWORKING
#ifndef NETWORK_SAIL_H
#define NETWORK_SAIL_H
#ifdef __cplusplus

#include "Network.h"

class Sail : public Network {
  private:
    void RegisterHooks();

  public:
    static Sail* Instance;

    void Enable();
    void OnIncomingJson(nlohmann::json payload);
    void OnConnected();
    void OnDisconnected();
    void DrawMenu();
};

#endif // __cplusplus
#endif // NETWORK_SAIL_H
#endif // ENABLE_NETWORKING
