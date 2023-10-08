
#pragma once

#include <utility>

#include "chess/piece.h"
#include "packets.h"

class ChessConnection {
 public:
  using MoveFunction = std::function<void(int, int, int, int)>;
  using TurnChangeFunction = std::function<void(PieceColor)>;
  using OnConnectFunction = std::function<void()>;
  using StartGameFunction = std::function<void()>;
  using EndGameFunction = std::function<void(PieceColor)>;

  ChessConnection();
  virtual ~ChessConnection();

  virtual void Move(int x, int y, int to_x, int to_y, bool switch_turn) = 0;
  virtual void Promote(int x, int y, PieceType piece_type) = 0;

  virtual bool IsOpen() = 0;

  virtual bool IsHost() { return false; }

  virtual Ref<std::array<PieceData, 8 * 8>> GetBoardData() { return nullptr; }

  virtual PieceColor GetCurrentTurn() = 0;

  virtual void Ready() = 0;

  virtual void Close() = 0;

  virtual void EndGame(PieceColor winner) = 0;

  void SetOnMove(MoveFunction on_move) {
    on_move_ = std::move(on_move);
  }

  void SetOnTurnChange(TurnChangeFunction on_set_current) {
    on_turn_change_ = std::move(on_set_current);
  }

  void SetOnConnect(OnConnectFunction on_client_connect) {
    on_connect_ = std::move(on_client_connect);
  }

  void SetOnStartGame(StartGameFunction on_start_game) {
    on_start_game_ = std::move(on_start_game);
  }

  void SetOnEndGame(EndGameFunction on_end_game) {
    on_end_game_ = std::move(on_end_game);
  }
 protected:
  TurnChangeFunction on_turn_change_;
  MoveFunction on_move_;
  OnConnectFunction on_connect_;
  StartGameFunction on_start_game_;
  EndGameFunction on_end_game_;
};

class ChessConnectionLocal : public ChessConnection {
 public:
  using DisconnectFunction = std::function<void()>;

  ChessConnectionLocal(Ref<znet::Server> server);
  ~ChessConnectionLocal() override;

  void Move(int x, int y, int to_x, int to_y, bool switch_turn) override;
  void Promote(int x, int y, PieceType piece_type) override;
  bool IsOpen() override;

  bool IsHost() override { return true; }

  Ref<std::array<PieceData, 8 * 8>> GetBoardData() override { return board_data_; }

  PieceColor GetCurrentTurn() override { return current_turn_; }

  void Ready() override;

  void Close() override;

  void EndGame(PieceColor winner) override;

  void SwitchTurn();

  void SetOnOpponentDisconnect(DisconnectFunction on_disconnect) {
    on_opponent_disconnect_ = std::move(on_disconnect);
  }

 private:
  Ref<znet::Server> server_;
  bool accepting_connections_ = true;
  Ref<znet::ServerSession> opponent_session_;
  Ref<std::array<PieceData, 8 * 8>> board_data_;
  PieceColor current_turn_ = kPieceColorWhite;
  DisconnectFunction on_opponent_disconnect_;
  bool is_ready = false, received_ready = false;

 private:
  void OnEvent(znet::Event& event);
  bool OnClientConnect(znet::ServerClientConnectedEvent& event);
  bool OnClientDisconnect(znet::ServerClientDisconnectedEvent& event);
  bool OnMovePacket(znet::ConnectionSession&, Ref<MovePacket> packet);
  bool OnBoardRequestPacket(znet::ConnectionSession&, Ref<BoardRequestPacket> packet);
  bool OnClientReadyPacket(znet::ConnectionSession&, Ref<ClientReadyPacket> packet);
  bool OnEndGamePacket(znet::ConnectionSession&, Ref<EndGamePacket> packet);
  void StartGame();
};

class ChessConnectionNetwork : public ChessConnection {
 public:
  ChessConnectionNetwork(Ref<znet::Client> client);
  ~ChessConnectionNetwork() override;

  void Move(int x, int y, int to_x, int to_y, bool switch_turn) override;
  void Promote(int x, int y, PieceType piece_type) override;
  bool IsOpen() override;

  bool IsHost() override { return false; }

  Ref<std::array<PieceData, 8 * 8>> GetBoardData() override { return board_data_; }

  PieceColor GetCurrentTurn() override { return current_turn_; }

  void Ready() override;

  void Close() override;

  void EndGame(PieceColor winner) override;

 private:
  Ref<znet::Client> client_;
  Ref<znet::ClientSession> opponent_session_;
  Ref<std::array<PieceData, 8 * 8>> board_data_;
  PieceColor current_turn_ = kPieceColorNone;

 private:
  void OnEvent(znet::Event& event);
  bool OnClientConnectedToServerEvent(znet::ClientConnectedToServerEvent& event);
  bool OnMovePacket(znet::ConnectionSession&, Ref<MovePacket> packet);
  bool OnUpdateBoardPacket(znet::ConnectionSession&, Ref<UpdateBoardPacket> packet);
  bool OnSetTurnPacket(znet::ConnectionSession&, Ref<SetTurnPacket> packet);
  bool OnStartGamePacket(znet::ConnectionSession&, Ref<StartGamePacket> packet);
  bool OnEndGamePacket(znet::ConnectionSession&, Ref<EndGamePacket> packet);
};

class ChessConnectionDummy : public ChessConnection {
 public:
  ChessConnectionDummy();
  ~ChessConnectionDummy() override = default;

  void Move(int x, int y, int to_x, int to_y, bool switch_turn) override {}
  void Promote(int x, int y, PieceType piece_type) override {}
  bool IsOpen() override { return false; }

  Ref<std::array<PieceData, 8 * 8>> GetBoardData() override { return board_data_; }

  PieceColor GetCurrentTurn() override { return current_turn_; }

  void Ready() override {
    on_start_game_();
    current_turn_ = kPieceColorBlack;
    on_turn_change_(kPieceColorBlack);
  }

  void Close() override {

  }

  void EndGame(PieceColor winner) override {
    on_end_game_(winner);
  }
 private:
  PieceColor current_turn_ = kPieceColorNone;
  Ref<std::array<PieceData, 8 * 8>> board_data_;
};