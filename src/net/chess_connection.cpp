#include "chess_connection.h"
#include "chess/chess_board.h"

ChessConnection::ChessConnection() {

}

ChessConnection::~ChessConnection() {

}

ChessConnectionLocal::ChessConnectionLocal(Ref<znet::Server> server) : server_(server) {
  server->SetEventCallback(ZNET_BIND_FN(OnEvent));
  board_data_ = CreateRef<std::array<PieceData, 8 * 8>>();
  std::array<PieceData, 8 * 8>& board_data = *board_data_.get();
  for (int x = 0; x < 8; ++x) {
    board_data[ChessBoard::PosToIndex(x, 1)] = {true, kPieceTypePawn, kPieceColorWhite};
    board_data[ChessBoard::PosToIndex(x, 6)] = {true, kPieceTypePawn, kPieceColorBlack};
  }
  board_data[ChessBoard::PosToIndex(0, 0)] = {true, kPieceTypeRook, kPieceColorWhite};
  board_data[ChessBoard::PosToIndex(7, 0)] = {true, kPieceTypeRook, kPieceColorWhite};
  board_data[ChessBoard::PosToIndex(0, 7)] = {true, kPieceTypeRook, kPieceColorBlack};
  board_data[ChessBoard::PosToIndex(7, 7)] = {true, kPieceTypeRook, kPieceColorBlack};

  board_data[ChessBoard::PosToIndex(1, 0)] = {true, kPieceTypeKnight, kPieceColorWhite};
  board_data[ChessBoard::PosToIndex(6, 0)] = {true, kPieceTypeKnight, kPieceColorWhite};
  board_data[ChessBoard::PosToIndex(1, 7)] = {true, kPieceTypeKnight, kPieceColorBlack};
  board_data[ChessBoard::PosToIndex(6, 7)] = {true, kPieceTypeKnight, kPieceColorBlack};

  board_data[ChessBoard::PosToIndex(2, 0)] = {true, kPieceTypeBishop, kPieceColorWhite};
  board_data[ChessBoard::PosToIndex(5, 0)] = {true, kPieceTypeBishop, kPieceColorWhite};
  board_data[ChessBoard::PosToIndex(2, 7)] = {true, kPieceTypeBishop, kPieceColorBlack};
  board_data[ChessBoard::PosToIndex(5, 7)] = {true, kPieceTypeBishop, kPieceColorBlack};

  board_data[ChessBoard::PosToIndex(4, 0)] = {true, kPieceTypeKing, kPieceColorWhite};
  board_data[ChessBoard::PosToIndex(3, 0)] = {true, kPieceTypeQueen, kPieceColorWhite};

  board_data[ChessBoard::PosToIndex(4, 7)] = {true, kPieceTypeKing, kPieceColorBlack};
  board_data[ChessBoard::PosToIndex(3, 7)] = {true, kPieceTypeQueen, kPieceColorBlack};
}

ChessConnectionLocal::~ChessConnectionLocal() {
}

void ChessConnectionLocal::Move(int x, int y, int to_x, int to_y, bool switch_turn) {
#ifdef DEBUG
  if (x < 0 || x > 7 || y < 0 || y > 7 || to_x < 0 || to_x > 7 || to_y < 0 || to_y > 7) {
    throw std::runtime_error("Invalid move");
  }
#endif
  auto pk = CreateRef<MovePacket>();
  pk->x_ = x;
  pk->y_ = y;
  pk->to_x_ = to_x;
  pk->to_y_ = to_y;
  opponent_session_->SendPacket(pk);
  if (switch_turn) {
    SwitchTurn();
  }
}

void ChessConnectionLocal::Promote(int x, int y, PieceType piece_type, PieceColor piece_color) {
  auto pk = CreateRef<SetPiecePacket>();
  pk->x_ = x;
  pk->y_ = y;
  pk->type_ = piece_type;
  pk->color_ = piece_color;
  opponent_session_->SendPacket(pk);
  on_piece_change_(x, y, piece_type, piece_color);
  SwitchTurn();
}

bool ChessConnectionLocal::IsOpen() {
  return true;
}

void ChessConnectionLocal::Ready() {
  is_ready = true;
  if (received_ready) {
    StartGame();
  }
}

void ChessConnectionLocal::Close() {
  server_->Stop();
}

void ChessConnectionLocal::SwitchTurn() {
  current_turn_ = current_turn_ == kPieceColorWhite ? kPieceColorBlack : kPieceColorWhite;
  auto turn_pk = CreateRef<SetTurnPacket>();
  turn_pk->color_ = current_turn_;
  opponent_session_->SendPacket(turn_pk);
  on_turn_change_(current_turn_);
}

void ChessConnectionLocal::StartGame() {
  auto turn_packet = CreateRef<SetTurnPacket>();
  turn_packet->color_ = current_turn_;
  opponent_session_->SendPacket(turn_packet);
  auto start_packet = CreateRef<StartGamePacket>();
  opponent_session_->SendPacket(start_packet);
  on_turn_change_(current_turn_);
  on_start_game_();
}

void ChessConnectionLocal::EndGame(PieceColor winner) {
  auto end_packet = CreateRef<EndGamePacket>();
  end_packet->winner_ = winner;
  opponent_session_->SendPacket(end_packet);
  on_end_game_(winner);
}

void ChessConnectionLocal::OnEvent(znet::Event& event) {
  znet::EventDispatcher dispatcher{event};
  dispatcher.Dispatch<znet::ServerClientConnectedEvent>(ZNET_BIND_FN(OnClientConnect));
  dispatcher.Dispatch<znet::ServerClientDisconnectedEvent>(ZNET_BIND_FN(OnClientDisconnect));
}

bool ChessConnectionLocal::OnClientConnect(znet::ServerClientConnectedEvent& event) {
  if (!accepting_connections_) {
    event.session()->Close();
    return false;
  }
  accepting_connections_ = false;
  opponent_session_ = event.session();
  auto& layer = opponent_session_->handler_layer();
  layer.AddPacketHandler(CreateRef<znet::PacketHandler<UpdateBoardPacket, UpdateBoardPacketSerializerV1>>());
  auto move_packet_handler =
      CreateRef<znet::PacketHandler<MovePacket, MovePacketSerializerV1>>();
  move_packet_handler->AddReceiveCallback(ZNET_BIND_FN(OnMovePacket));
  layer.AddPacketHandler(move_packet_handler);
  auto board_request_handler =
      CreateRef<znet::PacketHandler<BoardRequestPacket, BoardRequestPacketSerializerV1>>();
  board_request_handler->AddReceiveCallback(ZNET_BIND_FN(OnBoardRequestPacket));
  layer.AddPacketHandler(board_request_handler);
  layer.AddPacketHandler(CreateRef<znet::PacketHandler<SetTurnPacket, SetTurnPacketSerializerV1>>());
  auto client_ready_handler = CreateRef<znet::PacketHandler<ClientReadyPacket, ClientReadyPacketSerializerV1>>();
  client_ready_handler->AddReceiveCallback(ZNET_BIND_FN(OnClientReadyPacket));
  layer.AddPacketHandler(client_ready_handler);
  layer.AddPacketHandler(CreateRef<znet::PacketHandler<StartGamePacket, StartGamePacketSerializerV1>>());
  auto end_game_handler = CreateRef<znet::PacketHandler<EndGamePacket, EndGamePacketSerializerV1>>();
  end_game_handler->AddReceiveCallback(ZNET_BIND_FN(OnEndGamePacket));
  layer.AddPacketHandler(end_game_handler);
  auto set_piece_handler = CreateRef<znet::PacketHandler<SetPiecePacket, SetPiecePacketSerializerV1>>();
  set_piece_handler->AddReceiveCallback(ZNET_BIND_FN(OnSetPiecePacket));
  layer.AddPacketHandler(set_piece_handler);
  on_connect_();
  return false;
}

bool ChessConnectionLocal::OnClientDisconnect(znet::ServerClientDisconnectedEvent& event) {
  on_opponent_disconnect_();
  return false;
}

bool ChessConnectionLocal::OnBoardRequestPacket(znet::ConnectionSession&, Ref<BoardRequestPacket> packet) {
  auto pk = CreateRef<UpdateBoardPacket>();
  pk->board_ = board_data_;
  opponent_session_->SendPacket(pk);
  return true;
}

bool ChessConnectionLocal::OnClientReadyPacket(znet::ConnectionSession&, Ref<ClientReadyPacket> packet) {
  received_ready = true;
  if (is_ready) {
    StartGame();
  }
  return false;
}

bool ChessConnectionLocal::OnEndGamePacket(znet::ConnectionSession&, Ref<EndGamePacket> packet) {
  on_end_game_(packet->winner_);
  return true;
}

bool ChessConnectionLocal::OnMovePacket(znet::ConnectionSession&, Ref<MovePacket> packet) {
  on_move_(packet->x_, packet->y_, packet->to_x_, packet->to_y_);
  SwitchTurn();
  return true;
}

bool ChessConnectionLocal::OnSetPiecePacket(znet::ConnectionSession&, Ref<SetPiecePacket> packet) {
  on_piece_change_(packet->x_, packet->y_, packet->type_, packet->color_);
  return true;
}

ChessConnectionNetwork::ChessConnectionNetwork(Ref<znet::Client> client) : client_(client) {
  board_data_ = nullptr;
  client->SetEventCallback(ZNET_BIND_FN(OnEvent));
}

ChessConnectionNetwork::~ChessConnectionNetwork() {

}

void ChessConnectionNetwork::Move(int x, int y, int to_x, int to_y, bool switch_turn) {
#ifdef DEBUG
  if (x < 0 || x > 7 || y < 0 || y > 7 || to_x < 0 || to_x > 7 || to_y < 0 || to_y > 7) {
    throw std::runtime_error("Invalid move");
  }
#endif
  auto pk = CreateRef<MovePacket>();
  pk->x_ = x;
  pk->y_ = y;
  pk->to_x_ = to_x;
  pk->to_y_ = to_y;
  client_->client_session()->SendPacket(pk);
  if (switch_turn) {
    current_turn_ = kPieceColorNone;
  }
}

void ChessConnectionNetwork::Promote(int x, int y, PieceType piece_type, PieceColor piece_color) {
  auto pk = CreateRef<SetPiecePacket>();
  pk->x_ = x;
  pk->y_ = y;
  pk->type_ = piece_type;
  pk->color_ = piece_color;
  client_->client_session()->SendPacket(pk);
  on_piece_change_(x, y, piece_type, piece_color);
}

bool ChessConnectionNetwork::IsOpen() {
  return client_->client_session()->IsAlive();
}

void ChessConnectionNetwork::Ready() {
  auto pk = CreateRef<ClientReadyPacket>();
  client_->client_session()->SendPacket(pk);
}

void ChessConnectionNetwork::Close() {
  client_->Disconnect();
}

void ChessConnectionNetwork::EndGame(PieceColor winner) {
  auto pk = CreateRef<EndGamePacket>();
  pk->winner_ = winner;
  client_->client_session()->SendPacket(pk);
  on_end_game_(winner);
}

void ChessConnectionNetwork::OnEvent(znet::Event& event) {
  znet::EventDispatcher dispatcher{event};
  dispatcher.Dispatch<znet::ClientConnectedToServerEvent>(ZNET_BIND_FN(OnClientConnectedToServerEvent));
}

bool ChessConnectionNetwork::OnClientConnectedToServerEvent(znet::ClientConnectedToServerEvent& event) {
  auto session = event.session();
  auto& layer = session->handler_layer();
  auto update_board_handler = CreateRef<znet::PacketHandler<UpdateBoardPacket, UpdateBoardPacketSerializerV1>>();
  update_board_handler->AddReceiveCallback(ZNET_BIND_FN(OnUpdateBoardPacket));
  layer.AddPacketHandler(update_board_handler);
  auto move_packet_handler =
      CreateRef<znet::PacketHandler<MovePacket, MovePacketSerializerV1>>();
  move_packet_handler->AddReceiveCallback(ZNET_BIND_FN(OnMovePacket));
  layer.AddPacketHandler(move_packet_handler);
  layer.AddPacketHandler(CreateRef<znet::PacketHandler<BoardRequestPacket, BoardRequestPacketSerializerV1>>());
  auto set_turn_handler = CreateRef<znet::PacketHandler<SetTurnPacket, SetTurnPacketSerializerV1>>();
  set_turn_handler->AddReceiveCallback(ZNET_BIND_FN(OnSetTurnPacket));
  layer.AddPacketHandler(set_turn_handler);
  auto start_game_handler = CreateRef<znet::PacketHandler<StartGamePacket, StartGamePacketSerializerV1>>();
  start_game_handler->AddReceiveCallback(ZNET_BIND_FN(OnStartGamePacket));
  layer.AddPacketHandler(start_game_handler);
  layer.AddPacketHandler(CreateRef<znet::PacketHandler<ClientReadyPacket, ClientReadyPacketSerializerV1>>());
  auto end_game_handler = CreateRef<znet::PacketHandler<EndGamePacket, EndGamePacketSerializerV1>>();
  end_game_handler->AddReceiveCallback(ZNET_BIND_FN(OnEndGamePacket));
  layer.AddPacketHandler(end_game_handler);
  auto set_piece_handler = CreateRef<znet::PacketHandler<SetPiecePacket, SetPiecePacketSerializerV1>>();
  set_piece_handler->AddReceiveCallback(ZNET_BIND_FN(OnSetPiecePacket));
  layer.AddPacketHandler(set_piece_handler);

  auto pk = CreateRef<BoardRequestPacket>();
  session->SendPacket(pk);
  on_connect_();
  return false;
}

bool ChessConnectionNetwork::OnMovePacket(znet::ConnectionSession&, Ref<MovePacket> packet) {
  on_move_(packet->x_, packet->y_, packet->to_x_, packet->to_y_);
  return true;
}

bool ChessConnectionNetwork::OnUpdateBoardPacket(znet::ConnectionSession&, Ref<UpdateBoardPacket> packet) {
  board_data_ = packet->board_;
  // todo board changed event
  return true;
}

bool ChessConnectionNetwork::OnSetTurnPacket(znet::ConnectionSession&, Ref<SetTurnPacket> packet) {
  current_turn_ = packet->color_;
  on_turn_change_(current_turn_);
  return true;
}

bool ChessConnectionNetwork::OnStartGamePacket(znet::ConnectionSession&, Ref<StartGamePacket> packet) {
  on_start_game_();
  return true;
}

bool ChessConnectionNetwork::OnEndGamePacket(znet::ConnectionSession&, Ref<EndGamePacket> packet) {
  on_end_game_(packet->winner_);
  return true;
}

bool ChessConnectionNetwork::OnSetPiecePacket(znet::ConnectionSession&, Ref<SetPiecePacket> packet) {
  on_piece_change_(packet->x_, packet->y_, packet->type_, packet->color_);
  return true;
}

ChessConnectionDummy::ChessConnectionDummy() {
  board_data_ = CreateRef<std::array<PieceData, 8 * 8>>();
  std::array<PieceData, 8 * 8>& board_data = *board_data_.get();
  for (int x = 0; x < 8; ++x) {
    board_data[ChessBoard::PosToIndex(x, 1)] = {true, kPieceTypePawn, kPieceColorWhite};
    board_data[ChessBoard::PosToIndex(x, 6)] = {true, kPieceTypePawn, kPieceColorBlack};
  }
  board_data[ChessBoard::PosToIndex(0, 0)] = {true, kPieceTypeRook, kPieceColorWhite};
  board_data[ChessBoard::PosToIndex(7, 0)] = {true, kPieceTypeRook, kPieceColorWhite};
  board_data[ChessBoard::PosToIndex(0, 7)] = {true, kPieceTypeRook, kPieceColorBlack};
  board_data[ChessBoard::PosToIndex(7, 7)] = {true, kPieceTypeRook, kPieceColorBlack};

  board_data[ChessBoard::PosToIndex(1, 0)] = {true, kPieceTypeKnight, kPieceColorWhite};
  board_data[ChessBoard::PosToIndex(6, 0)] = {true, kPieceTypeKnight, kPieceColorWhite};
  board_data[ChessBoard::PosToIndex(1, 7)] = {true, kPieceTypeKnight, kPieceColorBlack};
  board_data[ChessBoard::PosToIndex(6, 7)] = {true, kPieceTypeKnight, kPieceColorBlack};

  board_data[ChessBoard::PosToIndex(2, 0)] = {true, kPieceTypeBishop, kPieceColorWhite};
  board_data[ChessBoard::PosToIndex(5, 0)] = {true, kPieceTypeBishop, kPieceColorWhite};
  board_data[ChessBoard::PosToIndex(2, 7)] = {true, kPieceTypeBishop, kPieceColorBlack};
  board_data[ChessBoard::PosToIndex(5, 7)] = {true, kPieceTypeBishop, kPieceColorBlack};

  board_data[ChessBoard::PosToIndex(4, 0)] = {true, kPieceTypeKing, kPieceColorWhite};
  board_data[ChessBoard::PosToIndex(3, 0)] = {true, kPieceTypeQueen, kPieceColorWhite};

  board_data[ChessBoard::PosToIndex(4, 7)] = {true, kPieceTypeKing, kPieceColorBlack};
  board_data[ChessBoard::PosToIndex(3, 7)] = {true, kPieceTypeQueen, kPieceColorBlack};
}

void ChessConnectionDummy::Promote(int x, int y, PieceType piece_type, PieceColor piece_color) {
  on_piece_change_(x, y, piece_type, piece_color);
}