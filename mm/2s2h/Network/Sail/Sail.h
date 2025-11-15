#ifndef NETWORK_SAIL_H
#define NETWORK_SAIL_H
#ifdef __cplusplus

#include "2s2h/Network/Network.h"

class Sail : public Network {
  private:
    void UnregisterAllHooks();

  public:
    static Sail* Instance;

    void Enable();
    void OnIncomingJson(nlohmann::json payload);
    void OnConnected();
    void OnDisconnected();
};

#endif // __cplusplus
#endif // NETWORK_SAIL_H
