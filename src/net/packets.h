
#pragma once

#include <chess/piece.h>
#include "znet/znet.h"

struct PieceData {
  bool exists = false;
  PieceType type;
  PieceColor color;
};

class UpdateBoardPacket : public znet::Packet {
 public:
  UpdateBoardPacket() : Packet(1) { }

  Ref<std::array<PieceData, 8 * 8>> board_;
};

class MovePacket : public znet::Packet {
 public:
  MovePacket() : Packet(2) { }

  int x_;
  int y_;
  int to_x_;
  int to_y_;
};

class BoardRequestPacket : public znet::Packet {
 public:
  BoardRequestPacket() : znet::Packet(3) { }
};

class SetTurnPacket : public znet::Packet {
 public:
  SetTurnPacket() : znet::Packet(4) { }

  PieceColor color_;
};

class StartGamePacket : public znet::Packet {
 public:
  StartGamePacket() : znet::Packet(5) { }
};

class ClientReadyPacket : public znet::Packet {
 public:
  ClientReadyPacket() : znet::Packet(6) { }
};

class EndGamePacket : public znet::Packet {
 public:
  EndGamePacket() : znet::Packet(7) { }

  PieceColor winner_;
};

class SetPiecePacket : public znet::Packet {
 public:
  SetPiecePacket() : znet::Packet(8) { }

  int x_;
  int y_;
  PieceType type_;
  PieceColor color_;
};

class UpdateBoardPacketSerializerV1 : public znet::PacketSerializer<UpdateBoardPacket> {
 public:
  UpdateBoardPacketSerializerV1() : PacketSerializer<UpdateBoardPacket>(1) {}

  znet::Ref<znet::Buffer> Serialize(Ref<UpdateBoardPacket> packet, Ref<znet::Buffer> buffer) override {
    // todo bitmap
    for (int i = 0; i < 8 * 8; i++) {
      auto& data = packet->board_->at(i);
      buffer->WriteBool(data.exists);
      if (data.exists) {
        buffer->WriteInt((uint8_t)data.type);
        buffer->WriteInt((uint8_t)data.color);
      }
    }
    return buffer;
  }

  Ref<UpdateBoardPacket> Deserialize(Ref<znet::Buffer> buffer) override {
    auto packet = CreateRef<UpdateBoardPacket>();
    packet->board_ = CreateRef<std::array<PieceData, 8 * 8>>();
    for (int i = 0; i < 8 * 8; i++) {
      auto& data = packet->board_->at(i);
      data.exists = buffer->ReadBool();
      if (data.exists) {
        data.type = (PieceType) buffer->ReadInt<uint8_t>();
        data.color = (PieceColor) buffer->ReadInt<uint8_t>();
      }
    }
    return packet;
  }
};

class MovePacketSerializerV1 : public znet::PacketSerializer<MovePacket> {
 public:
  MovePacketSerializerV1() : PacketSerializer<MovePacket>(2) {}

  Ref<znet::Buffer> Serialize(Ref<MovePacket> packet, Ref<znet::Buffer> buffer) override {
    buffer->WriteInt(packet->x_);
    buffer->WriteInt(packet->y_);
    buffer->WriteInt(packet->to_x_);
    buffer->WriteInt(packet->to_y_);
    return buffer;
  }

  Ref<MovePacket> Deserialize(Ref<znet::Buffer> buffer) override {
    auto packet = CreateRef<MovePacket>();
    packet->x_ = buffer->ReadInt<int>();
    packet->y_ = buffer->ReadInt<int>();
    packet->to_x_ = buffer->ReadInt<int>();
    packet->to_y_ = buffer->ReadInt<int>();
    return packet;
  }
};

class BoardRequestPacketSerializerV1 : public znet::PacketSerializer<BoardRequestPacket> {
 public:
  BoardRequestPacketSerializerV1() : PacketSerializer<BoardRequestPacket>(3) {}

  Ref<znet::Buffer> Serialize(Ref<BoardRequestPacket> packet, Ref<znet::Buffer> buffer) override {
    return buffer;
  }

  Ref<BoardRequestPacket> Deserialize(Ref<znet::Buffer> buffer) override {
    return CreateRef<BoardRequestPacket>();
  }
};

class SetTurnPacketSerializerV1 : public znet::PacketSerializer<SetTurnPacket> {
 public:
  SetTurnPacketSerializerV1() : PacketSerializer<SetTurnPacket>(4) {}

  Ref<znet::Buffer> Serialize(Ref<SetTurnPacket> packet, Ref<znet::Buffer> buffer) override {
    buffer->WriteInt((uint8_t) packet->color_);
    return buffer;
  }

  Ref<SetTurnPacket> Deserialize(Ref<znet::Buffer> buffer) override {
    auto packet = CreateRef<SetTurnPacket>();
    packet->color_ = (PieceColor) buffer->ReadInt<uint8_t>();
    return packet;
  }
};

class StartGamePacketSerializerV1 : public znet::PacketSerializer<StartGamePacket> {
 public:
  StartGamePacketSerializerV1() : PacketSerializer<StartGamePacket>(5) {}

  Ref<znet::Buffer> Serialize(Ref<StartGamePacket> packet, Ref<znet::Buffer> buffer) override {
    return buffer;
  }

  Ref<StartGamePacket> Deserialize(Ref<znet::Buffer> buffer) override {
    return CreateRef<StartGamePacket>();
  }
};

class ClientReadyPacketSerializerV1 : public znet::PacketSerializer<ClientReadyPacket> {
 public:
  ClientReadyPacketSerializerV1() : PacketSerializer<ClientReadyPacket>(6) {}

  Ref<znet::Buffer> Serialize(Ref<ClientReadyPacket> packet, Ref<znet::Buffer> buffer) override {
    return buffer;
  }

  Ref<ClientReadyPacket> Deserialize(Ref<znet::Buffer> buffer) override {
    return CreateRef<ClientReadyPacket>();
  }
};

class EndGamePacketSerializerV1 : public znet::PacketSerializer<EndGamePacket> {
 public:
  EndGamePacketSerializerV1() : PacketSerializer<EndGamePacket>(7) {}

  Ref<znet::Buffer> Serialize(Ref<EndGamePacket> packet, Ref<znet::Buffer> buffer) override {
    buffer->WriteInt((uint8_t) packet->winner_);
    return buffer;
  }

  Ref<EndGamePacket> Deserialize(Ref<znet::Buffer> buffer) override {
    auto packet = CreateRef<EndGamePacket>();
    packet->winner_ = (PieceColor) buffer->ReadInt<uint8_t>();
    return packet;
  }
};

class SetPiecePacketSerializerV1 : public znet::PacketSerializer<SetPiecePacket> {
 public:
  SetPiecePacketSerializerV1() : PacketSerializer<SetPiecePacket>(8) {}

  Ref<znet::Buffer> Serialize(Ref<SetPiecePacket> packet, Ref<znet::Buffer> buffer) override {
    buffer->WriteInt(packet->x_);
    buffer->WriteInt(packet->y_);
    buffer->WriteInt((uint8_t) packet->type_);
    buffer->WriteInt((uint8_t) packet->color_);
    return buffer;
  }

  Ref<SetPiecePacket> Deserialize(Ref<znet::Buffer> buffer) override {
    auto packet = CreateRef<SetPiecePacket>();
    packet->x_ = buffer->ReadInt<int>();
    packet->y_ = buffer->ReadInt<int>();
    packet->type_ = (PieceType) buffer->ReadInt<uint8_t>();
    packet->color_ = (PieceColor) buffer->ReadInt<uint8_t>();
    return packet;
  }
};