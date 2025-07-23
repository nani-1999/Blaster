// Copyright Na9i Studio.

#pragma once

#define NANI_LOG(Verbosity, Str, ...) \
	switch (GetWorld()->GetNetMode()) { \
		case ENetMode::NM_Client: \
			UE_LOG(LogTemp, Verbosity, TEXT("Client: %d | " ## Str), GPlayInEditorID, ##__VA_ARGS__); \
			break; \
		case ENetMode::NM_DedicatedServer: \
		case ENetMode::NM_ListenServer: \
			UE_LOG(LogTemp, Verbosity, TEXT("Server | " ## Str), ##__VA_ARGS__); \
			break; \
		case ENetMode::NM_Standalone: \
			UE_LOG(LogTemp, Verbosity, TEXT("Standalone | " ## Str), ##__VA_ARGS__); \
			break; \
		default : \
			UE_LOG(LogTemp, Verbosity, TEXT("MAX? | " ## Str), ##__VA_ARGS__); \
	} \

template<typename T>
T GetNetModeStr(int Val) {
	switch (Val) {
		case 0:
			return "Standalone";
			break;
		case 1:
		case 2:
			return "Server";
			break;
		case 3:
			return "Client";
			break;
		default:
			return "MAX?";
	}
}
template<typename T>
T GetNetRoleStr(int Val) {
	switch (Val) {
		case 0:
			return "None";
			break;
		case 1:
			return "Simulated Proxy";
			break;
		case 2:
			return "Autonomous Proxy";
			break;
		case 3:
			return "Authority";
			break;
		default:
			return "MAX?";
	}
}
