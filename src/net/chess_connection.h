
#pragma once

#include <utility>

#include "chess/piece.h"
#include "packets.h"

class ChessConnection {
 public:
  using MoveFunction = std::function<void(int, int, int, int)>;
  using TurnChangeFunction = std::function<void(PieceColor)>;
  using OnConnectFunction = std::function<void()>;
  using OnDisconnectFunction = std::function<void(bool)>;
  using StartGameFunction = std::function<void()>;
  using EndGameFunction = std::function<void(PieceColor)>;
  using PieceChangeFunction = std::function<void(int, int, PieceType, PieceColor)>;

  ChessConnection();
  virtual ~ChessConnection();

  virtual void Move(int x, int y, int to_x, int to_y, bool switch_turn) = 0;
  virtual void Promote(int x, int y, PieceType piece_type, PieceColor piece_color) = 0;

  virtual bool IsOpen() = 0;

  virtual bool IsHost() { return false; }

  virtual Ref<std::array<PieceData, 8 * 8>> GetBoardData() { return nullptr; }

  virtual PieceColor GetCurrentTurn() = 0;

  virtual void Ready() = 0;

  virtual void Close() = 0;

  virtual void Wait() = 0;

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

  void SetOnDisconnect(OnDisconnectFunction on_client_disconnect) {
    on_disconnect_ = std::move(on_client_disconnect);
  }

  void SetOnStartGame(StartGameFunction on_start_game) {
    on_start_game_ = std::move(on_start_game);
  }

  void SetOnEndGame(EndGameFunction on_end_game) {
    on_end_game_ = std::move(on_end_game);
  }

  void SetOnPieceChance(PieceChangeFunction on_piece_change) {
    on_piece_change_ = std::move(on_piece_change);
  }
 protected:
  TurnChangeFunction on_turn_change_;
  MoveFunction on_move_;
  OnConnectFunction on_connect_;
  OnDisconnectFunction on_disconnect_;
  StartGameFunction on_start_game_;
  EndGameFunction on_end_game_;
  PieceChangeFunction on_piece_change_;
};

class ChessConnectionLocal : public ChessConnection {
 public:
  ChessConnectionLocal(Ref<znet::Server> server);
  ~ChessConnectionLocal() override;

  void Move(int x, int y, int to_x, int to_y, bool switch_turn) override;
  void Promote(int x, int y, PieceType piece_type, PieceColor piece_color) override;
  bool IsOpen() override;

  bool IsHost() override { return true; }

  Ref<std::array<PieceData, 8 * 8>> GetBoardData() override { return board_data_; }

  PieceColor GetCurrentTurn() override { return current_turn_; }

  void Ready() override;

  void Close() override;

  void Wait() override;

  void EndGame(PieceColor winner) override;

  void SwitchTurn();

 private:
  Ref<znet::Server> server_;
  bool accepting_connections_ = true;
  Ref<znet::PeerSession> opponent_session_;
  Ref<std::array<PieceData, 8 * 8>> board_data_;
  PieceColor current_turn_ = kPieceColorWhite;
  bool is_ready = false, received_ready = false;

 private:
  void OnEvent(znet::Event& event);
  bool OnClientConnect(znet::ServerClientConnectedEvent& event);
  bool OnClientDisconnect(znet::ServerClientDisconnectedEvent& event);
  bool OnMovePacket(znet::PeerSession&, Ref<MovePacket> packet);
  bool OnBoardRequestPacket(znet::PeerSession&, Ref<BoardRequestPacket> packet);
  bool OnClientReadyPacket(znet::PeerSession&, Ref<ClientReadyPacket> packet);
  bool OnEndGamePacket(znet::PeerSession&, Ref<EndGamePacket> packet);
  bool OnSetPiecePacket(znet::PeerSession&, Ref<SetPiecePacket> packet);
  void StartGame();
};

class ChessConnectionNetwork : public ChessConnection {
 public:
  ChessConnectionNetwork(Ref<znet::Client> client);
  ~ChessConnectionNetwork() override;

  void Move(int x, int y, int to_x, int to_y, bool switch_turn) override;
  void Promote(int x, int y, PieceType piece_type, PieceColor piece_color) override;
  bool IsOpen() override;

  bool IsHost() override { return false; }

  Ref<std::array<PieceData, 8 * 8>> GetBoardData() override { return board_data_; }

  PieceColor GetCurrentTurn() override { return current_turn_; }

  void Ready() override;

  void Close() override;

  void Wait() override;

  void EndGame(PieceColor winner) override;

 private:
  Ref<znet::Client> client_;
  Ref<znet::PeerSession> opponent_session_;
  Ref<std::array<PieceData, 8 * 8>> board_data_;
  PieceColor current_turn_ = kPieceColorNone;

 private:
  void OnEvent(znet::Event& event);
  bool OnClientConnectedToServerEvent(znet::ClientConnectedToServerEvent& event);
  bool OnClientDisconnectedFromServerEvent(znet::ClientDisconnectedFromServerEvent& event);
  bool OnMovePacket(znet::PeerSession&, Ref<MovePacket> packet);
  bool OnUpdateBoardPacket(znet::PeerSession&, Ref<UpdateBoardPacket> packet);
  bool OnSetTurnPacket(znet::PeerSession&, Ref<SetTurnPacket> packet);
  bool OnStartGamePacket(znet::PeerSession&, Ref<StartGamePacket> packet);
  bool OnEndGamePacket(znet::PeerSession&, Ref<EndGamePacket> packet);
  bool OnSetPiecePacket(znet::PeerSession&, Ref<SetPiecePacket> packet);
};
