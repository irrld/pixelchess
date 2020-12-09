
#pragma once

#include <chess/Piece.h>
#include "znet/znet.h"

struct PieceData {
  bool exists = false;
  PieceType type;
  PieceColor color;
};

class UpdateBoardPacket : public znet::Packet {
 public:
  UpdateBoardPacket() : Packet(1) { }

  PieceData* board_;
};

class MovePacket : public znet::Packet {
 public:
  MovePacket() : Packet(2) { }

  int x_;
  int y_;
  int to_x_;
  int to_y_;
  PieceType piece_type_;
  bool is_capture_;
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

class UpdateBoardPacketSerializerV1 : public znet::PacketSerializer<UpdateBoardPacket> {
 public:
  UpdateBoardPacketSerializerV1() : PacketSerializer<UpdateBoardPacket>(1) {}

  znet::Ref<znet::Buffer> Serialize(Ref<UpdateBoardPacket> packet, Ref<znet::Buffer> buffer) override {
    // todo bitmap
    for (int i = 0; i < 8 * 8; i++) {
      auto& data = packet->board_[i];
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
    for (int i = 0; i < 8 * 8; i++) {
      auto& data = packet->board_[i];
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
    buffer->WriteInt((uint8_t) packet->piece_type_);
    buffer->WriteBool(packet->is_capture_);
    return buffer;
  }

  Ref<MovePacket> Deserialize(Ref<znet::Buffer> buffer) override {
    auto packet = CreateRef<MovePacket>();
    packet->x_ = buffer->ReadInt<int>();
    packet->y_ = buffer->ReadInt<int>();
    packet->to_x_ = buffer->ReadInt<int>();
    packet->to_y_ = buffer->ReadInt<int>();
    packet->piece_type_ = (PieceType) buffer->ReadInt<uint8_t>();
    packet->is_capture_ = buffer->ReadBool();
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