#include "chess_board.h"

#include <utility>

bool CheckOccupation(ChessBoard* board, int ox, int oy, int to_x, int to_y, int x, int y, PieceColor team) {
  if (ox < 0 || ox >= 8 || oy < 0 || oy >= 8) {
    return true;
  }
  Ref<Piece> piece = board->GetPiece(ox, oy);
  if (!piece) {
    return true;
  }
  if (piece->GetColor() != team && ox == to_x && oy == to_y) {
    return true;
  }
  return false;
}

ChessBoard::ChessBoard() {
  CreatePieces();
}

ChessBoard::ChessBoard(std::array<PieceData, 8 * 8> board) {
  CreatePieces();
  for (int x = 0; x < 8; ++x) {
    for (int y = 0; y < 8; ++y) {
      PieceData data = board[PosToIndex(x, y)];
      if (data.exists) {
        SetPiece(x, y, pieces_[data.type]->CopyTeam(data.color));
      }
    }
  }
}

ChessBoard::~ChessBoard() {
}

Ref<Piece> ChessBoard::GetPiece(int x, int y) {
  return board_[PosToIndex(x, y)];
}

void ChessBoard::SetPiece(int x, int y, Ref<Piece> piece) {
  board_[PosToIndex(x, y)] = std::move(piece);
}

void ChessBoard::SetPiece(int x, int y, PieceType type, PieceColor color) {
  SetPiece(x, y, pieces_[type]->CopyTeam(color));
}

bool ChessBoard::IsValidMove(int x, int y, int to_x, int to_y) {
  if (x < 0 || x > 8 || y < 0 || y > 8 || (x == to_x && y == to_y)) {
    return false;
  }
  Ref<Piece> piece = GetPiece(x, y);
  if (piece == nullptr) {
    return false;
  }
  Ref<Piece> to_piece = GetPiece(to_x, to_y);
  if (to_piece != nullptr && to_piece->GetColor() == piece->GetColor()) {
    return false;
  }
  if (piece->IsValidMove(x, y, to_x, to_y)) {
    SetPiece(to_x, to_y, piece);
    SetPiece(x, y, nullptr);
    auto state = CheckState(GetOppositeColor(piece->GetColor()));
    SetPiece(x, y, piece);
    SetPiece(to_x, to_y, to_piece);
    return state == ChessState::Playing;
  }
  return false;
}

ChessState ChessBoard::CheckState(PieceColor current_turn) {
  if (current_turn == kPieceColorNone) {
    return ChessState::Playing;
  }
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      Ref<Piece> piece = GetPiece(x, y);
      if (piece == nullptr) {
        continue;
      }
      if (piece->GetType() == kPieceTypeKing && piece->GetColor() == current_turn) {
        if (HasAnyValidMoveTo(x, y, piece->GetColor())) {
          return ChessState::Checkmate;
        }
        return ChessState::Playing;
      }
    }
  }
  return ChessState::Playing;
}

void ChessBoard::CreatePieces() {
  pieces_[kPieceTypePawn] = CreateRef<Piece>(kPieceTypePawn, kPieceColorWhite, [this](int x, int y, int to_x, int to_y, PieceColor team) -> bool {
    if (team == kPieceColorWhite) {
      bool double_jump = y == 1;
      if ((to_y == y + 1 || (double_jump && to_y == y + 2)) && x == to_x) {
        Ref<Piece> front_piece = GetPiece(to_x, to_y);
        if (front_piece) {
          return false;
        }
        if (double_jump && to_y == y + 2) {
          // check if trying to jump over from a friendly piece
          Ref<Piece> piece = GetPiece(to_x, y + 1);
          if (piece && piece->GetColor() == team) {
            return false;
          }
        }
        return true;
      }
      if (x - 1 == to_x && y + 1 == to_y) {
        Ref<Piece> left_piece = GetPiece(x - 1, y + 1);
        if (left_piece && left_piece->GetColor() != team) {
          return true;
        }
      }
      if (x + 1 == to_x && y + 1 == to_y) {
        Ref<Piece> right_piece = GetPiece(x + 1, y + 1);
        if (right_piece && right_piece->GetColor() != team) {
          return true;
        }
      }
    } else {
      bool double_jump = y == 6;
      if ((to_y == y - 1 || (double_jump && to_y == y - 2)) && x == to_x) {
        Ref<Piece> front_piece = GetPiece(to_x, to_y);
        if (front_piece) {
          return false;
        }
        if (double_jump && to_y == y - 2) {
          // check if trying to jump over from a friendly piece
          Ref<Piece> piece = GetPiece(to_x, y - 1);
          if (piece && piece->GetColor() == team) {
            return false;
          }
        }
        return true;
      }
      if (x - 1 == to_x && y - 1 == to_y) {
        Ref<Piece> left_piece = GetPiece(x - 1, y - 1);
        if (left_piece && left_piece->GetColor() != team) {
          return true;
        }
      }
      if (x + 1 == to_x && y - 1 == to_y) {
        Ref<Piece> right_piece = GetPiece(x + 1, y - 1);
        if (right_piece && right_piece->GetColor() != team) {
          return true;
        }
      }
    }
    return false;
  });
  pieces_[kPieceTypeKnight] = CreateRef<Piece>(kPieceTypeKnight, kPieceColorWhite, [](int x, int y, int to_x, int to_y, PieceColor team) -> bool {
    if (to_x == x + 2 && to_y == y + 1) {
      return true;
    }
    if (to_x == x + 1 && to_y == y + 2) {
      return true;
    }
    if (to_x == x + 2 && to_y == y - 1) {
      return true;
    }
    if (to_x == x + 1 && to_y == y - 2) {
      return true;
    }
    if (to_x == x - 2 && to_y == y + 1) {
      return true;
    }
    if (to_x == x - 1 && to_y == y + 2) {
      return true;
    }
    if (to_x == x - 2 && to_y == y - 1) {
      return true;
    }
    if (to_x == x - 1 && to_y == y - 2) {
      return true;
    }
    return false;
  });
  pieces_[kPieceTypeBishop] = CreateRef<Piece>(kPieceTypeBishop, kPieceColorWhite, [this](int x, int y, int to_x, int to_y, PieceColor team) -> bool {
    int rx = to_x - x;
    int ry = to_y - y;

    if (std::abs(rx) != std::abs(ry)) {
      return false;
    }
    int r = std::max(std::abs(rx), std::abs(ry));
    for (int i = 1; i < r; i++) {
      if (rx < 0 && ry < 0 && !CheckOccupation(this, x - i, y - i, to_x, to_y, x, y, team)) {
        return false;
      }
      if (rx > 0 && ry < 0 && !CheckOccupation(this, x + i, y - i, to_x, to_y, x, y, team)) {
        return false;
      }
      if (rx > 0 && ry > 0 && !CheckOccupation(this, x + i, y + i, to_x, to_y, x, y, team)) {
        return false;
      }
      if (rx < 0 && ry > 0 && !CheckOccupation(this, x - i, y + i, to_x, to_y, x, y, team)) {
        return false;
      }
    }
    return true;
  });
  pieces_[kPieceTypeRook] = CreateRef<Piece>(kPieceTypeRook, kPieceColorWhite, [this](int x, int y, int to_x, int to_y, PieceColor team) -> bool {
    bool match = (x == to_x && y != to_y) || (x != to_x && y == to_y);
    if (!match) {
      return false;
    }
    int rx = to_x - x;
    int ry = to_y - y;
    int r = std::max(std::abs(rx), std::abs(ry));
    for (int i = 1; i < r; i++) {
      if (rx > 0 && ry == 0 && !CheckOccupation(this, x + i, y, to_x, to_y, x, y, team)) {
        return false;
      }
      if (rx < 0 && ry == 0 && !CheckOccupation(this, x - i, y, to_x, to_y, x, y, team)) {
        return false;
      }
      if (rx == 0 && ry > 0 && !CheckOccupation(this, x, y + i, to_x, to_y, x, y, team)) {
        return false;
      }
      if (rx == 0 && ry < 0 && !CheckOccupation(this, x, y - i, to_x, to_y, x, y, team)) {
        return false;
      }
    }
    return true;
  });
  pieces_[kPieceTypeQueen] = CreateRef<Piece>(kPieceTypeQueen, kPieceColorWhite, [this](int x, int y, int to_x, int to_y, PieceColor team) -> bool {
    if ((to_x == x + 1 || to_x == x - 1 || to_x == x) && (to_y == y + 1 || to_y == y - 1 || to_y == y)) {
      return true;
    }
    int rx = to_x - x;
    int ry = to_y - y;
    if (std::abs(rx) != std::abs(ry)) {
      bool match = (x == to_x && y != to_y) || (x != to_x && y == to_y);
      if (!match) {
        return false;
      }
      int r = std::max(std::abs(rx), std::abs(ry));
      for (int i = 1; i < r; i++) {
        if (rx > 0 && ry == 0 && !CheckOccupation(this, x + i, y, to_x, to_y, x, y, team)) {
          return false;
        }
        if (rx < 0 && ry == 0 && !CheckOccupation(this, x - i, y, to_x, to_y, x, y, team)) {
          return false;
        }
        if (rx == 0 && ry > 0 && !CheckOccupation(this, x, y + i, to_x, to_y, x, y, team)) {
          return false;
        }
        if (rx == 0 && ry < 0 && !CheckOccupation(this, x, y - i, to_x, to_y, x, y, team)) {
          return false;
        }
      }
      return true;
    }
    int r = std::max(std::abs(rx), std::abs(ry));
    for (int i = 1; i < r; i++) {
      if (rx < 0 && ry < 0 && !CheckOccupation(this, x - i, y - i, to_x, to_y, x, y, team)) {
        return false;
      }
      if (rx > 0 && ry < 0 && !CheckOccupation(this, x + i, y - i, to_x, to_y, x, y, team)) {
        return false;
      }
      if (rx > 0 && ry > 0 && !CheckOccupation(this, x + i, y + i, to_x, to_y, x, y, team)) {
        return false;
      }
      if (rx < 0 && ry > 0 && !CheckOccupation(this, x - i, y + i, to_x, to_y, x, y, team)) {
        return false;
      }
    }
    return true;
  });
  pieces_[kPieceTypeKing] = CreateRef<Piece>(kPieceTypeKing, kPieceColorWhite, [](int x, int y, int to_x, int to_y, PieceColor team) -> bool {
    if ((to_x == x + 1 || to_x == x - 1 || to_x == x) && (to_y == y + 1 || to_y == y - 1 || to_y == y)) {
      return true;
    }
    return false;
  });
}