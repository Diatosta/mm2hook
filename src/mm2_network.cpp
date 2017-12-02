#include "mm2_network.h"
using namespace MM2;

int asNetwork::Initialize(int a2, int a3, int gameVersion) {
    if (pDPlay)
        return 1;

    pGUID = &MM2_GUID;
    float34 = (float)gameVersion;
    MaxPlayers = a2;
    dword44 = a3;
    CoInitialize(0);

    if (!CreateInterface())
        return 1;

    SessionOpen = 0;
    return 0;
}

int asNetwork::InitializeLobby(int maxPlayers, int a3) {
    CoInitialize(0);
    if (pLobby || !CoCreateInstance(CLSID_DirectPlayLobby, NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlayLobby3A, (LPVOID *)this + 7)) {
        MaxPlayers = maxPlayers;
        dword44 = a3;
        Displayf("Lobby interface created or already created.");
        return 1;
    }
    else {
        InLobby = 0;
        CoUninitialize();
        return 0;
    }
}

void asNetwork::WaitForLobbyConnection(int a2) {
    HRESULT hr;

    if (pLobby) {
        if (a2)
            hr = pLobby->WaitForConnectionSettings(0);

        else
            hr = pLobby->WaitForConnectionSettings(1);
        
        if (hr == DPERR_INVALIDFLAGS)
            Errorf("DPLAY: WaitForConnectionSettings failed DPERR_INVALIDFLAGS");

        else if (hr = DPERR_UNAVAILABLE)
            Errorf("DPLAY: WaitForConnectSettings failed DPERR_UNAVAILABLE");
    }
}

int asNetwork::JoinLobbySession(void) {
    HRESULT hr;
    DWORD lpdwDataSize;
    int result;
    DPLCONNECTION *lpData;
    DPLCONNECTION *v6;
    IDirectPlay4A *v7;

    Displayf("asNetwork::JoinLobbySession()");
    if (pLobby) {
        //Get session details
        hr = pLobby->GetConnectionSettings(0, 0, &lpdwDataSize);

        if (hr == DPERR_NOTLOBBIED) { //Not in a lobby session
            InLobby = 0;
            result = 0;
        }
        else if (hr == DPERR_BUFFERTOOSMALL) {
            if (pConnection)
                operator delete(pConnection);

            lpData = (DPLCONNECTION *)operator new(lpdwDataSize);

            pConnection = lpData;

            if (lpData) {
                if (FAILED(pLobby->GetConnectionSettings(0, lpData, &lpdwDataSize))) {
                    Errorf("JoinLobbySession: GetConnectionSettings failed.");
                    InLobby = 0;
                    result = 0;
                }
                else {
                    v6 = pConnection;
                    if (v6->dwFlags & 2)
                        IsHost = true;
                    else
                        IsHost = false;

                    MaxPlayers = 8;
                    dword44 = 0;
                    v6->lpSessionDesc->dwFlags = 68;
                    pConnection->lpSessionDesc->dwMaxPlayers = MaxPlayers;

                    if (FAILED(pLobby->SetConnectionSettings(0, 0, pConnection))) {
                        Errorf("asNetwork::JoinLobbySession: couldn't set lobby connection.");
                        InLobby = 0;
                        result = 0;
                    }
                    else {
                        v7 = pDPlay;
                        if (v7) {
                            pDPlay->Release();
                            pDPlay = 0;
                        }

                        hr = pLobby->ConnectEx(0, IID_IDirectPlay4A, (LPVOID *)&pDPlay, 0);

                        if (hr) {
                            Errorf("asNetwork::JoinLobbySession: couldn't join lobby. %x", hr);
                            if (hr == DPERR_CONNECTING) {
                                Errorf("asNetwork::JoinLobbySession:DPERR_CONNECTING");
                                InLobby = 0;
                                result = 0;
                            }
                            else if (hr == DPERR_INVALIDFLAGS) {
                                Errorf("asNetwork::JoinLobbySession:DPERR_INVALIDFLAGS");
                                InLobby = 0;
                                result = 0;
                            }
                            else {
                                if (hr == DPERR_INVALIDPARAMS)
                                    Errorf("asNetwork::JoinLobbySession:DPERR_INVALIDPARAMS");
                                InLobby = 0;
                                result = 0;
                            }
                        }
                        else {
                            InLobby = 1;
                            SessionOpen = 1;
                            Displayf("asNetwork::JoinLobbySession successful.");
                            result = 1;
                        }
                    }
                }
            }
            else {
                Errorf("Out of memory.");
                InLobby = 0;
                result = 0;
            }
        }
        else {
            result = 0;
        }
    }
    else {
        Errorf("asNetwork::JoinLobbySession -- couldn't get lobby interface.");
        result = 0;
    }
    return result;
}

//TODO find out what NullCallback is
void asNetwork::Logout(void) {
    datCallback NullCallback;

    if (pLobby)
        pLobby->Release();
    pLobby = NULL;

    if (pDPlay) {
        if (nRecvBufSize) {
            operator delete(nRecvBufSize);
            nRecvBufSize = NULL;
        }
        if (pConnection) {
            operator delete(pConnection);
            pConnection = NULL;
        }
        if (!pDPlay->DestroyPlayer(PlayerID))
            Warningf("DPLAY: DestroyPlayer");
        if (!pDPlay->Close())
            Warningf("DPLAY: Closing session");
        if (!pDPlay->Release())
            Warningf("DPLAY: DirectPlay object pointer released.");
        pDPlay = NULL;
        CoUninitialize();
    }
    PlayerID = 0;
    SessionOpen = 0;
    InLobby = 0;
    IsHost = 0;
    SysMessageCB = NullCallback;
    AppMessageCB = NullCallback;
    dword48 = 0;
    Elapsed = 0.0;
    memset(&SEND_BUFFER, 0, 0x400);
}

void asNetwork::Deallocate(void) {
    if (nRecvBufSize) {
        operator delete(nRecvBufSize);
        nRecvBufSize = NULL;
    }
    if (pConnection) {
        operator delete(pConnection);
        pConnection = NULL;
    }
}

int asNetwork::CreateInterface(void) {
    return CoCreateInstance(CLSID_DirectPlay, NULL, CLSCTX_INPROC_SERVER, IID_IDirectPlay4A, (LPVOID *)&pDPlay);
}

int asNetwork::GetNetworkCaps(void) {
    int v7, v8, v9, v10, v11;
    NETCOMMPACK *v6;
    char v12, v13;

    v6 = 0;
    v7 = 9600;
    v8 = 1;
    v9 = v10 = v11 = v12 = v13 = 0;

    if (!dword48) {
        GetProtocols();
        if (SetProtocol(0)) {
            dword48 = dword48 | 1;
            dword48 = LOBYTE(dword48);
            Disconnect();
        }
        if (SetProtocol(1, v6)) {
            dword48 = dword48 | 2;
            dword48 = LOBYTE(dword48);
            Disconnect();
        }
        QueryModems();
        if (GetNumModems()) {
            dword48 = dword48 | 4;
            LOBYTE(dword48);
        }
        if (pDPlay)
            Disconnect();
    }
    return dword48;
}

void asNetwork::Disconnect(void) {
    if (pDPlay) {
        if (pConnection) {
            operator delete(pConnection);
            pConnection = 0;
        }
        if (!pDPlay->Release())
            Warningf("DPLAY: DirectPlay object pointer released.");
    }
    SessionOpen = 0;
    CreateInterface();
}

int asNetwork::GetNumPlayers(void) {
    DWORD lpdwDataSize;
    BYTE *lpData = NULL;
    HRESULT hr;
    DPSESSIONDESC2 *pdpsd;

    pDPlay->GetSessionDesc(NULL, &lpdwDataSize);

    lpData = new BYTE[lpdwDataSize];
    if (lpData == NULL) {
        Errorf("DPLAY: DPERR_OUTOFMEMORY");
        return -1;
    }

    hr = pDPlay->GetSessionDesc(lpData, &lpdwDataSize);

    switch (hr) {
    case DPERR_INVALIDOBJECT:
        Errorf("DPLAY: DPERR_INVALIDOBJECT");
        return -1;
    case DPERR_INVALIDPARAM:
        Errorf("DPLAY: DPERR_INVALIDPARAM");
        return -1;
    case DPERR_NOCONNECTION:
        Errorf("DPLAY: DPERR_NOCONNECTION");
        return -1;
    default:
        pdpsd = (DPSESSIONDESC2 *)lpData;
        return pdpsd->dwCurrentPlayers;
    }
}

void asNetwork::Install() {
    InstallCallback("asNetwork::Initialize", "Hijack the initialize function",
        &Initialize, {
            cbHook<CALL>(0x40FFA1),     //mmInterface::InitNetwork
        }
    );
    InstallCallback("asNetwork::InitializeLobby", "Hijack the initialize lobby function",
        &InitializeLobby, {
            cbHook<CALL>(0x410113),     //mmInterface::InitLobby
        }
    );
    InstallCallback("asNetwork::WaitForLobbyConnection", "Hijack the waiting for lobby function",
        &WaitForLobbyConnection, {
            cbHook<CALL>(0x40B045),     //mmInterface::Update
        }
    );
    InstallCallback("asNetwork::JoinLobbySession", "Hijack the join lobby session function",
        &JoinLobbySession, {
            cbHook<CALL>(0x409C6F),     //mmInterface::MessageCallback
            cbHook<CALL>(0x410126),     //mmInterface::InitLobby
        }
    );
    InstallCallback("asNetwork::Logout", "Hijack the logout function",
        &Logout, {
            cbHook<CALL>(0x401616),     //Main
            cbHook<CALL>(0x409A83),     //mmInterface::MessageCallback
            cbHook<CALL>(0x411E2E),     //sub_411DD0
            cbHook<CALL>(0x56FD96),     //asNetwork::~asNetwork
        }
    );
    InstallCallback("asNetwork::Deallocate", "Hijack the deallocate function",
        &Deallocate, {
            cbHook<CALL>(0x4094FB),     //mmInterface::~mmInterface
            cbHook<JMP>(0x43B163),      //mmGameMulti::QuitNetwork
        }
    );
    InstallCallback("asNetwork::GetNetworkCaps", "Hijack the network cap getter function",
        &GetNetworkCaps, {
            cbHook<CALL>(0x41004B),     //mmInterface::InitNetwork
            cbHook<CALL>(0x504943),     //NetSelectMenu::PreSetup
        }
    );
    InstallCallback("asNetwork::Disconnect", "Hijack the network disconnect function",
        &Disconnect, {
            cbHook<CALL>(0x4096C4),     //mmInterface::MessageCallback
            cbHook<CALL>(0x409A5B),     //mmInterface::MessageCallback
            cbHook<CALL>(0x409E55),     //mmInterface::MessageCallback2
            cbHook<CALL>(0x40ACEA),     //mmInterface::Update
            cbHook<CALL>(0x40B10F),     //mmInterface::Update
            cbHook<CALL>(0x40B183),     //mmInterface::Update
            cbHook<CALL>(0x40B25A),     //mmInterface::Update
            cbHook<CALL>(0x40B276),     //mmInterface::Update
            cbHook<CALL>(0x40B2E9),     //mmInterface::Update
            cbHook<CALL>(0x40B373),     //mmInterface::Update
            cbHook<CALL>(0x40B6BB),     //mmInterface::Update
            cbHook<CALL>(0x40B973),     //mmInterface::Update
            cbHook<CALL>(0x40B989),     //mmInterface::Update
            cbHook<CALL>(0x40C247),     //mmInterface::JoinLAN
            cbHook<CALL>(0x40C2FB),     //mmInterface::JoinSerial
            cbHook<CALL>(0x40C411),     //mmInterface::JoinModem
            cbHook<CALL>(0x40D394),     //mmInterface::Switch
            cbHook<CALL>(0x410938),     //mmInterface::SetHostProtocol
            cbHook<CALL>(0x4109D8),     //mmInterface::SetProtocol
            cbHook<CALL>(0x411E16),     //sub_411DD0
        }
    );
    InstallCallback("asNetwork::GetNumPlayers", "Hijack the get num of players function",
        &GetNumPlayers, {
            cbHook<CALL>(0x40C782),     //mmInterface::UpdateLobby
            cbHook<CALL>(0x4111B1),     //mmInterface::RefreshPlayers
            cbHook<CALL>(0x411374),     //mmInterface::RefreshPlayers
            cbHook<CALL>(0x420910),     //mmMultiBlitz::Reset
            cbHook<CALL>(0x4224F2),     //mmMultiCircuit::Reset
            cbHook<CALL>(0x4226D4),     //mmMultiCircuit::UpdateGame
            cbHook<CALL>(0x422B86),     //mmMultiCircuit::UpdateGame
            cbHook<CALL>(0x42333E),     //mmMultiCircuit::GameMessage
            cbHook<CALL>(0x425F11),     //mmMultiCR::FillResults
            cbHook<CALL>(0x428B84),     //mmMultiRace::Reset
            cbHook<CALL>(0x428D4B),     //mmMultiRace::UpdateGame
            cbHook<CALL>(0x429235),     //mmMultiRace::UpdateGame
            cbHook<CALL>(0x4299E0),     //mmMultiRace::GameMessage
            cbHook<CALL>(0x429A59),     //mmMultiRace::GameMessage
            cbHook<CALL>(0x43A7FC),     //mmGameMulti::SendPosition
            cbHook<CALL>(0x43B801),     //mmGameMulti:SendLobbyResults
        }
    );
}