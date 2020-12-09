
#pragma once

#include "chess/Piece.h"
#include "Packets.h"

class ChessConnection {
 public:
  using MoveFunction = std::function<void(int, int, int, int, PieceType, bool)>;
  using SetCurrentFunction = std::function<void(PieceColor)>;

  ChessConnection();
  virtual ~ChessConnection();

  virtual void Move(int x, int y, int to_x, int to_y, PieceType piece_type, bool is_capture) = 0;
  virtual void Promote(int x, int y, PieceType piece_type) = 0;

  virtual bool IsOpen() = 0;

  virtual bool IsLocal() { return false; }

  virtual PieceData* GetBoardData() { return nullptr; }

  virtual PieceColor GetCurrentTurn() = 0;

  void SetOnMove(MoveFunction on_move) {
    on_move_ = std::move(on_move);
  }

  void SetOnSetCurrent(SetCurrentFunction on_set_current) {
    on_set_current_ = std::move(on_set_current);
  }

 protected:
  SetCurrentFunction on_set_current_;
  MoveFunction on_move_;
};

class ChessConnectionLocal : public ChessConnection {
 public:
  using ClientConnectFunction = std::function<void()>;

  ChessConnectionLocal(Ref<znet::Server> server);
  ~ChessConnectionLocal() override;

  void Move(int x, int y, int to_x, int to_y, PieceType piece_type, bool is_capture) override;
  void Promote(int x, int y, PieceType piece_type) override;
  bool IsOpen() override;

  bool IsLocal() override { return true; }

  PieceData* GetBoardData() override { return board_data_; }

  PieceColor GetCurrentTurn() override { return current_turn_; }

  void SetOnClientConnect(ClientConnectFunction on_client_connect) {
    on_client_connect_ = std::move(on_client_connect);
  }
 private:
  Ref<znet::Server> server_;
  ClientConnectFunction on_client_connect_;
  bool accepting_connections_ = true;
  Ref<znet::ClientSession> opponent_session_;
  PieceData* board_data_;
  PieceColor current_turn_ = kPieceColorWhite;

 private:
  void OnEvent(znet::Event& event);
  bool OnClientConnectEvent(znet::ClientConnectedToServerEvent& event);
  bool OnMovePacket(znet::ConnectionSession&, Ref<MovePacket> packet);
  bool OnBoardRequestPacket(znet::ConnectionSession&, Ref<BoardRequestPacket> packet);
};

class ChessConnectionNetwork : public ChessConnection {
 public:
  ChessConnectionNetwork(std::string host, int port);
  ~ChessConnectionNetwork() override;

  void Move(int x, int y, int to_x, int to_y, PieceType piece_type, bool is_capture) override;
  void Promote(int x, int y, PieceType piece_type) override;
  bool IsOpen() override;

 private:
  Ref<znet::Client> client_;
};