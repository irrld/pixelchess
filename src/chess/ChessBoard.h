
#pragma once

#include <net/Packets.h>
#include "Piece.h"
#include "Util.h"

class ChessBoard {
 public:
  ChessBoard();
  ChessBoard(PieceData board[8 * 8]);
  ~ChessBoard();

  Ref<Piece> GetPiece(int x, int y);
  void SetPiece(int x, int y, Ref<Piece> piece);

  bool IsValidMove(int x, int y, int to_x, int to_y);

  static int PosToIndex(int x, int y) {
    return y * 8 + x;
  }
 private:
  Ref<Piece> pieces_[6];
  Ref<Piece> board_[8 * 8];

  void CreatePieces();
};