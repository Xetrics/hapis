#pragma once

#include <string>
#include <cstdint>
#include <cassert>

#pragma comment(lib, "libprotobuf.lib")
#pragma comment(lib, "libprotobuf-lite.lib")
#pragma comment(lib, "libprotoc.lib")

#include "rrapi.h"

namespace Rust
{
	enum MessageType
	{
		First = 141,
		Welcome = 141,
		Auth = 142,
		Approved = 143,
		Ready = 144,
		Entities = 145,
		EntityDestroy = 146,
		GroupChange = 147,
		GroupDestroy = 148,
		RPCMessage = 149,
		EntityPosition = 150,
		ConsoleMessage = 151,
		ConsoleCommand = 152,
		Effect = 153,
		DisconnectReason = 154,
		Tick = 155,
		Message = 156,
		RequestUserInformation = 157,
		GiveUserInformation = 158,
		GroupEnter = 159,
		GroupLeave = 160,
		VoiceData = 161,
		Last = 161,
	};

	class Message
	{
	public:
		/* NETSND_Start should be called before this while NETSND_Send should be called after */
		virtual void Serialize(void* pointer) = 0;
		/* no bytes should be read before calling this */
		virtual void Deserialize(void* pointer) = 0;

		/* NOTE: this should be called before anything is read from the packet */
		/* return value must be deallocated if not null */
		static Message* MessageFromPacket(void* pointer, unsigned char* data, uint32_t size)
		{
			Message* message = 0;
			MessageType type = (MessageType)data[0];

			switch (type)
			{
			case EntityPosition:
				//message = new EntityPositionMessage;
				//break;
			/* ... */
			default:
				printf("MessageFromPacket: Unhandled Rust packet ID %d\n", data[0]);
				break;
			}

			if (message)
			{
				message->type = type;
				message->Deserialize(pointer);
			}

			return message;
		}

		/* NOTE: this should be called before anything is read from the packet */
		/* return value must be deallocated if not null */
		static Message* MessageFromPacket(void* pointer)
		{
			return MessageFromPacket(pointer, (unsigned char*)API::NETRCV_RawData(pointer), API::NETRCV_LengthBits(pointer) / 8);
		}

		static const char* TypeToName(MessageType type)
		{
			if (type < MessageType::First || type > MessageType::Last) return 0;
			static const char* MessageNames[21] = { "Welcome",	"Auth", "Approved", "Ready", "Entities", "EntityDestroy", "GroupChange", "GroupDestroy", "RPCMessage", "EntityPosition", "ConsoleMessage",
				"ConsoleCommand", "Effect", "DisconnectReason", "Tick", "Message", "RequestUserInformation", "GiveUserInformation", "GroupEnter", "GroupLeave", "VoiceData" };
			return MessageNames[type - MessageType::First];
		}

		const char* ToString()
		{
			return TypeToName(type);
		}

		MessageType type;
	};
	
	/* messages */

	class EntityPositionMessage : public Message
	{
	public:
		uint32_t entity_id;
		Vector3 position;
		Vector3 rotation;
	public:
		virtual void Serialize(void* pointer)
		{
			Write<uint8_t>(pointer, MessageType::EntityPosition);
			Write<uint32_t>(pointer, entity_id);
			Write<Vector3>(pointer, position);
			Write<Vector3>(pointer, rotation);
		}

		virtual void Deserialize(void* pointer)
		{
			assert((type = (MessageType)Read<uint8_t>(pointer)) == MessageType::EntityPosition);
			entity_id = Read<uint32_t>(pointer);
			position = Read<Vector3>(pointer);
			rotation = Read<Vector3>(pointer);
		}
	};

	class EntityDestroyMessage : public Message
	{
	public:
		uint32_t entity_id;
	public:
		virtual void Serialize(void* pointer)
		{
			Write<uint8_t>(pointer, MessageType::EntityDestroy);
			Write<uint32_t>(pointer, entity_id);
		}

		virtual void Deserialize(void* pointer)
		{
			assert((type = (MessageType)Read<uint8_t>(pointer)) == MessageType::EntityDestroy);
			entity_id = Read<uint32_t>(pointer);
		}
	};

	class RPCMessageClass : public Message {
	public:
		uint32_t entity_id;
		uint32_t func_name;
		void* objects;
	public:
		virtual void Serialize(void* pointer)
		{
			Write<uint8_t>(pointer, MessageType::RPCMessage);
			Write<uint32_t>(pointer, entity_id);
			Write<uint32_t>(pointer, func_name);
		}

		virtual void Deserialize(void* pointer)
		{
			assert((type = (MessageType)Read<uint8_t>(pointer)) == MessageType::RPCMessage);
			entity_id = Read<uint32_t>(pointer);
			func_name = Read<uint32_t>(pointer);
			
		}
	};

	class ConsoleCommandMessage : public Message
	{
	public:
		std::string command;

	public:
		virtual void Serialize(void* pointer)
		{
			Write<uint8_t>(pointer, MessageType::ConsoleCommand);
			WriteString(pointer, command);
		}

		virtual void Deserialize(void* pointer)
		{
			assert((type = (MessageType)Read<uint8_t>(pointer)) == MessageType::ConsoleCommand);
			command = ReadString(pointer);
		}
	};
}