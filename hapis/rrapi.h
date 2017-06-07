#pragma once

#include <cstdint>
#include <cassert>
#include <string>

/* Rust RakNet API */

#define RAKNET_DLL	"RakNet.dll"
#define RAKNET_HASH	"9925fc6b745d94469a9c530fa5785438" // 5/31/2017

/* Namespace for Rust network related things */
namespace Rust
{
	namespace API
	{
		typedef int* RakPeer;

		/* http://www.jenkinssoftware.com/raknet/manual/Doxygen/structRakNet_1_1RakNetStatistics.html */
		struct RaknetStats
		{
			uint64_t valueOverLastSecond[7];
			uint64_t runningTotal[7];
			uint64_t connectionStartTime;
			bool isLimitedByCongestionControl;
			uint64_t BPSLimitByCongestionControl;
			bool isLimitedByOutgoingBandwidthLimit;
			uint64_t BPSLimitByOutgoingBandwidthLimit;
			uint32_t messageInSendBuffer[4];
			double bytesInSendBuffer[4];
			uint32_t messagesInResendBuffer;
			uint64_t bytesInResendBuffer;
			float packetlossLastSecond;
			float packetlossTotal;
		};

		/* this literally took me 2 hours to reverse all the exports and write their definitions accurately */
		typedef uint64_t(__fastcall *NETRCV_AddressFn)(void* ptr);
		typedef uint64_t(__fastcall *NETRCV_GUIDFn)(void* ptr); // incomingGuid
		typedef uint32_t(__fastcall *NETRCV_LengthBitsFn)(void* ptr);
		typedef uint32_t(__fastcall *NETRCV_PortFn)(void* ptr);
		typedef void*(__fastcall *NETRCV_RawDataFn)(void* ptr);
		typedef bool(__fastcall *NETRCV_ReadBytesFn)(void* ptr, unsigned char* data, uint32_t length);
		typedef bool(__fastcall *NETRCV_SetReadPointerFn)(void* ptr, uint32_t bitsOffset);
		typedef uint32_t(__fastcall *NETRCV_UnreadBitsFn)(void* ptr);
		typedef uint32_t(__fastcall *NETSND_BroadcastFn)(void* ptr, uint32_t priority, uint32_t reliability, uint32_t channel); // NOTE: IDA only shows 3 arguments?
		typedef float(__fastcall *NETSND_ReadCompressedFloatFn)(void* ptr);
		typedef uint32_t(__fastcall *NETSND_ReadCompressedInt32Fn)(void* ptr);
		typedef uint64_t(__fastcall *NETSND_ReadCompressedInt64Fn)(void* ptr);
		typedef uint32_t(__fastcall *NETSND_SendFn)(void* ptr, uint64_t connectionID, uint32_t priority, uint32_t reliability, uint32_t channel);
		typedef uint32_t(__fastcall *NETSND_SizeFn)(void* ptr);
		typedef void(__fastcall *NETSND_StartFn)(void* ptr);
		typedef void(__fastcall *NETSND_WriteBytesFn)(void* ptr, unsigned char* data, uint32_t length);
		typedef void(__fastcall *NETSND_WriteCompressedFloatFn)(void* ptr, float data);
		typedef void(__fastcall *NETSND_WriteCompressedInt32Fn)(void* ptr, uint32_t data);
		typedef void(__fastcall *NETSND_WriteCompressedInt64Fn)(void* ptr, uint64_t data);
		typedef void(__fastcall *NET_CloseFn)(void* ptr);
		typedef void(__fastcall *NET_CloseConnectionFn)(void* ptr, uint64_t connectionID);
		typedef int*(__fastcall *NET_CreateFn)();
		typedef char*(__fastcall *NET_GetAddressFn)(void* ptr, uint64_t connectionID);
		typedef uint32_t(__fastcall *NET_GetAveragePingFn)(void* ptr, uint64_t connectionID);
		typedef uint64_t(__fastcall *NET_GetGUIDFn)(void* ptr); // unused?
		typedef uint32_t(__fastcall *NET_GetLastPingFn)(void* ptr, uint64_t connectionID);
		typedef uint32_t(__fastcall *NET_GetLowestPingFn)(void* ptr, uint64_t connectionID);
		typedef bool(__fastcall *NET_GetStatisticsFn)(void* ptr, uint64_t connectionID, RaknetStats* out, uint32_t length); // length should be sizeof(RaknetStats) or 224
		typedef char*(__fastcall *NET_GetStatisticsStringFn)(void* ptr, uint64_t connectionID);
		typedef char*(__fastcall *NET_LastStartupErrorFn)();
		typedef void(__fastcall *NET_LimitBandwidthFn)(void* ptr, uint32_t maxBitsPerSecond); // unused?
		typedef bool(__fastcall *NET_ReceiveFn)(void* ptr);
		typedef void(__fastcall *NET_SendMessageFn)(void* ptr, unsigned char* data, uint64_t length, uint32_t ip, uint16_t port);
		typedef uint32_t(__fastcall *NET_StartClientFn)(void* ptr, const char* host, uint16_t port, uint32_t retries, uint32_t retryDelay, uint32_t timeout);
		typedef uint32_t(__fastcall *NET_StartServerFn)(void* ptr, const char* host, uint16_t port, uint32_t maxConnections);

		extern NETRCV_AddressFn NETRCV_Address;
		extern NETRCV_GUIDFn NETRCV_GUID;
		extern NETRCV_LengthBitsFn NETRCV_LengthBits;
		extern NETRCV_PortFn NETRCV_Port;
		extern NETRCV_RawDataFn NETRCV_RawData;
		extern NETRCV_ReadBytesFn NETRCV_ReadBytes;
		extern NETRCV_SetReadPointerFn NETRCV_SetReadPointer;
		extern NETRCV_UnreadBitsFn NETRCV_UnreadBits;
		extern NETSND_BroadcastFn NETSND_Broadcast;
		extern NETSND_ReadCompressedFloatFn NETSND_ReadCompressedFloat;
		extern NETSND_ReadCompressedInt32Fn NETSND_ReadCompressedInt32;
		extern NETSND_ReadCompressedInt64Fn NETSND_ReadCompressedInt64;
		extern NETSND_SendFn NETSND_Send;
		extern NETSND_SizeFn NETSND_Size;
		extern NETSND_StartFn NETSND_Start;
		extern NETSND_WriteBytesFn NETSND_WriteBytes;
		extern NETSND_WriteCompressedFloatFn NETSND_WriteCompressedFloat;
		extern NETSND_WriteCompressedInt32Fn NETSND_WriteCompressedInt32;
		extern NETSND_WriteCompressedInt64Fn NETSND_WriteCompressedInt64;
		extern NET_CloseFn NET_Close;
		extern NET_CloseConnectionFn NET_CloseConnection;
		extern NET_CreateFn NET_Create;
		extern NET_GetAddressFn NET_GetAddress;
		extern NET_GetAveragePingFn NET_GetAveragePing;
		extern NET_GetGUIDFn NET_GetGUID;
		extern NET_GetLastPingFn NET_GetLastPing;
		extern NET_GetLowestPingFn NET_GetLowestPing;
		extern NET_GetStatisticsFn NET_GetStatistics;
		extern NET_GetStatisticsStringFn NET_GetStatisticsString;
		extern NET_LastStartupErrorFn NET_LastStartupError;
		extern NET_LimitBandwidthFn NET_LimitBandwidth;
		extern NET_ReceiveFn NET_Receive;
		extern NET_SendMessageFn NET_SendMessage;
		extern NET_StartClientFn NET_StartClient;
		extern NET_StartServerFn NET_StartServer;

		void Init();
	}

	/* utility for reading/writing */

	struct Vector3
	{
		float x, y, z;

		Vector3 operator+(const Vector3 &v2) {
			this->x = this->x + v2.x;
			this->y = this->y + v2.y;
			this->z = this->z + v2.z;

			return *this;
		 }

		Vector3 operator-(const Vector3 &v2) {
			this->x = this->x - v2.x;
			this->y = this->y - v2.y;
			this->z = this->z - v2.z;

			return *this;
		}
	};

	template <typename T>
	T Read(void* pointer)
	{
		T data;
		API::NETRCV_ReadBytes(pointer, (unsigned char*)&data, sizeof(T));
		return data;
	}

	template <typename T>
	void Write(void* pointer, T value)
	{
		API::NETSND_WriteBytes(pointer, (unsigned char*)&value, sizeof(T));
		return;
	}

	std::string ReadString(void* pointer);
	void WriteString(void* pointer, const std::string& string);
}
