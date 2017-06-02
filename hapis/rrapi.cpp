#include <Windows.h>
#include <fstream>
#include <sstream>

#include "rrapi.h"
#include "md5.h"

bool CalculateMD5OfFile(const std::string& path, std::string& out)
{
	std::ifstream file(path, std::ios::in | std::ios::binary);
	if (file.is_open())
	{
		std::stringstream buffer;
		buffer << file.rdbuf();
		out = md5(buffer.str());
		return true;
	}

	return false;
}

namespace Rust
{
	namespace API
	{
		void* Pointer = 0;

		NETRCV_AddressFn NETRCV_Address = 0;
		NETRCV_GUIDFn NETRCV_GUID = 0;
		NETRCV_LengthBitsFn NETRCV_LengthBits = 0;
		NETRCV_PortFn NETRCV_Port = 0;
		NETRCV_RawDataFn NETRCV_RawData = 0;
		NETRCV_ReadBytesFn NETRCV_ReadBytes = 0;
		NETRCV_SetReadPointerFn NETRCV_SetReadPointer = 0;
		NETRCV_UnreadBitsFn NETRCV_UnreadBits = 0;
		NETSND_BroadcastFn NETSND_Broadcast = 0;
		NETSND_ReadCompressedFloatFn NETSND_ReadCompressedFloat = 0;
		NETSND_ReadCompressedInt32Fn NETSND_ReadCompressedInt32 = 0;
		NETSND_ReadCompressedInt64Fn NETSND_ReadCompressedInt64 = 0;
		NETSND_SendFn NETSND_Send = 0;
		NETSND_SizeFn NETSND_Size = 0;
		NETSND_StartFn NETSND_Start = 0;
		NETSND_WriteBytesFn NETSND_WriteBytes = 0;
		NETSND_WriteCompressedFloatFn NETSND_WriteCompressedFloat = 0;
		NETSND_WriteCompressedInt32Fn NETSND_WriteCompressedInt32 = 0;
		NETSND_WriteCompressedInt64Fn NETSND_WriteCompressedInt64 = 0;
		NET_CloseFn NET_Close = 0;
		NET_CloseConnectionFn NET_CloseConnection = 0;
		NET_CreateFn NET_Create = 0;
		NET_GetAddressFn NET_GetAddress = 0;
		NET_GetAveragePingFn NET_GetAveragePing = 0;
		NET_GetGUIDFn NET_GetGUID = 0;
		NET_GetLastPingFn NET_GetLastPing = 0;
		NET_GetLowestPingFn NET_GetLowestPing = 0;
		NET_GetStatisticsFn NET_GetStatistics = 0;
		NET_GetStatisticsStringFn NET_GetStatisticsString = 0;
		NET_LastStartupErrorFn NET_LastStartupError = 0;
		NET_LimitBandwidthFn NET_LimitBandwidth = 0;
		NET_ReceiveFn NET_Receive = 0;
		NET_SendMessageFn NET_SendMessage = 0;
		NET_StartClientFn NET_StartClient = 0;
		NET_StartServerFn NET_StartServer = 0;

		void Init()
		{
			std::string FileHash;
			if (!CalculateMD5OfFile(RAKNET_DLL, FileHash))
				throw std::exception("unable to open " RAKNET_DLL);

			if (FileHash != RAKNET_HASH) // in case rust modifies RakNet.dll idk im paranoid
				throw std::exception("raknet hash mismatch");

			HMODULE Module = LoadLibraryA(RAKNET_DLL);
			if (!Module)
				throw std::exception("unable to load " RAKNET_DLL);

			/* this probably isn't necessary (we could just let the compiler link and load RakNet.dll) but oh well */
			NETRCV_Address = (NETRCV_AddressFn)GetProcAddress(Module, "NETRCV_Address");
			NETRCV_GUID = (NETRCV_GUIDFn)GetProcAddress(Module, "NETRCV_GUID");
			NETRCV_LengthBits = (NETRCV_LengthBitsFn)GetProcAddress(Module, "NETRCV_LengthBits");
			NETRCV_Port = (NETRCV_PortFn)GetProcAddress(Module, "NETRCV_Port");
			NETRCV_RawData = (NETRCV_RawDataFn)GetProcAddress(Module, "NETRCV_RawData");
			NETRCV_ReadBytes = (NETRCV_ReadBytesFn)GetProcAddress(Module, "NETRCV_ReadBytes");
			NETRCV_SetReadPointer = (NETRCV_SetReadPointerFn)GetProcAddress(Module, "NETRCV_SetReadPointer");
			NETRCV_UnreadBits = (NETRCV_UnreadBitsFn)GetProcAddress(Module, "NETRCV_UnreadBits");
			NETSND_Broadcast = (NETSND_BroadcastFn)GetProcAddress(Module, "NETSND_Broadcast");
			NETSND_ReadCompressedFloat = (NETSND_ReadCompressedFloatFn)GetProcAddress(Module, "NETSND_ReadCompressedFloat");
			NETSND_ReadCompressedInt32 = (NETSND_ReadCompressedInt32Fn)GetProcAddress(Module, "NETSND_ReadCompressedInt32");
			NETSND_ReadCompressedInt64 = (NETSND_ReadCompressedInt64Fn)GetProcAddress(Module, "NETSND_ReadCompressedInt64");
			NETSND_Send = (NETSND_SendFn)GetProcAddress(Module, "NETSND_Send");
			NETSND_Size = (NETSND_SizeFn)GetProcAddress(Module, "NETSND_Size");
			NETSND_Start = (NETSND_StartFn)GetProcAddress(Module, "NETSND_Start");
			NETSND_WriteBytes = (NETSND_WriteBytesFn)GetProcAddress(Module, "NETSND_WriteBytes");
			NETSND_WriteCompressedFloat = (NETSND_WriteCompressedFloatFn)GetProcAddress(Module, "NETSND_WriteCompressedFloat");
			NETSND_WriteCompressedInt32 = (NETSND_WriteCompressedInt32Fn)GetProcAddress(Module, "NETSND_WriteCompressedInt32");
			NETSND_WriteCompressedInt64 = (NETSND_WriteCompressedInt64Fn)GetProcAddress(Module, "NETSND_WriteCompressedInt64");
			NET_Close = (NET_CloseFn)GetProcAddress(Module, "NET_Close");
			NET_CloseConnection = (NET_CloseConnectionFn)GetProcAddress(Module, "NET_CloseConnection");
			NET_Create = (NET_CreateFn)GetProcAddress(Module, "NET_Create");
			NET_GetAddress = (NET_GetAddressFn)GetProcAddress(Module, "NET_GetAddress");
			NET_GetAveragePing = (NET_GetAveragePingFn)GetProcAddress(Module, "NET_GetAveragePing");
			NET_GetGUID = (NET_GetGUIDFn)GetProcAddress(Module, "NET_GetGUID");
			NET_GetLastPing = (NET_GetLastPingFn)GetProcAddress(Module, "NET_GetLastPing");
			NET_GetLowestPing = (NET_GetLowestPingFn)GetProcAddress(Module, "NET_GetLowestPing");
			NET_GetStatistics = (NET_GetStatisticsFn)GetProcAddress(Module, "NET_GetStatistics");
			NET_GetStatisticsString = (NET_GetStatisticsStringFn)GetProcAddress(Module, "NET_GetStatisticsString");
			NET_LastStartupError = (NET_LastStartupErrorFn)GetProcAddress(Module, "NET_LastStartupError");
			NET_LimitBandwidth = (NET_LimitBandwidthFn)GetProcAddress(Module, "NET_LimitBandwidth");
			NET_Receive = (NET_ReceiveFn)GetProcAddress(Module, "NET_Receive");
			NET_SendMessage = (NET_SendMessageFn)GetProcAddress(Module, "NET_SendMessage");
			NET_StartClient = (NET_StartClientFn)GetProcAddress(Module, "NET_StartClient");
			NET_StartServer = (NET_StartServerFn)GetProcAddress(Module, "NET_StartServer");
		}
	}
}