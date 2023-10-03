
#pragma once

#include <utility>
#include <vector>
#include "Util.h"

enum PieceType : uint8_t {
  kPieceTypeNone,
  kPieceTypePawn,
  kPieceTypeKnight,
  kPieceTypeRook,
  kPieceTypeBishop,
  kPieceTypeQueen,
  kPieceTypeKing,
};

enum PieceColor : uint8_t {
  kPieceColorNone,
  kPieceColorWhite,
  kPieceColorBlack
};

using MoveFunction = std::function<bool(int, int, int, int, PieceColor)>;

class Piece {
 public:
  Piece(PieceType type, PieceColor color, MoveFunction move_function) : type_(type), color_(color), move_function_(std::move(move_function)) {
  }

  PieceType GetType() const { return type_; }
  PieceColor GetColor() const { return color_; }

  bool IsValidMove(int x, int y, int to_x, int to_y) {
    return move_function_(x, y, to_x, to_y, color_);
  }

  MoveFunction GetMoveFunction() const { return move_function_; }

  Ref<Piece> CopyTeam(PieceColor color) {
    return CreateRef<Piece>(type_, color, move_function_);
  }
 private:
  PieceType type_;
  PieceColor color_;
  MoveFunction move_function_;
};