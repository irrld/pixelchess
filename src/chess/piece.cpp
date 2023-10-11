#include "piece.h"

PieceColor GetOppositeColor(PieceColor color) {
  if (color == kPieceColorWhite) {
    return kPieceColorBlack;
  }
  if (color == kPieceColorBlack) {
    return kPieceColorWhite;
  }
  return kPieceColorNone;
}