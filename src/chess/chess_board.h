
#pragma once

#include <net/packets.h>
#include "piece.h"
#include "util.h"

enum class ChessState {
  Playing,
  Stalemate,
  Checkmate
};

class ChessBoard {
 public:
  ChessBoard();
  ChessBoard(std::array<PieceData, 8 * 8> board);
  ~ChessBoard();

  Ref<Piece> GetPiece(int x, int y);
  void SetPiece(int x, int y, Ref<Piece> piece);

  bool IsValidMove(int x, int y, int to_x, int to_y);

  static int PosToIndex(int x, int y) {
    return y * 8 + x;
  }

  ChessState CheckState(PieceColor current_turn);

  bool HasValidMoves(int x, int y) {
    for (int to_x = 0; to_x < 8; ++to_x) {
      for (int to_y = 0; to_y < 8; ++to_y) {
        if (IsValidMove(x, y, to_x, to_y)) {
          return true;
        }
      }
    }
    return false;
  }

  bool HasAnyValidMoveTo(int to_x, int to_y, PieceColor other_color) {
    for (int x = 0; x < 8; x++) {
      for (int y = 0; y < 8; y++) {
        auto piece = GetPiece(x, y);
        if (piece && piece->GetColor() != other_color && IsValidMove(x, y, to_x, to_y)) {
          return true;
        }
      }
    }
    return false;
  }
 private:
  Ref<Piece> pieces_[7];
  Ref<Piece> board_[8 * 8];

  void CreatePieces();
};