
#pragma once

#include "util.h"
#include "app/app.h"
#include "chess/piece.h"

class GameCanvas;

class PromoteScreen {
 public:
  using SelectCallback = std::function<void(PieceType)>;

  PromoteScreen(gApp* root, GameCanvas* canvas, PieceColor player_color);

  void Setup();
  void Draw();
  void Update();
  void OnMouseMoved(int x, int y);
  void OnMousePressed(int x, int y);
  void OnMouseReleased(int x, int y);

  void SetPromotePosition(int x, int y);

  void SetSelectCallback(SelectCallback callback_fn);

  void Reset();
 private:
  gApp* root_;
  GameCanvas* canvas_;
  gRenderer* renderer_;
  PieceColor player_color_;
  int space_ = 3;
  int padding_ = 6;
  int piece_width_ = 16;
  int piece_height_ = 32;
  int piece_num_ = 4;
  int width_ = (piece_width_ * piece_num_ +
                space_ * (piece_num_ - 1) + padding_ * 2) * RenderUtil::kPixelScale;
  int height_ = 35 * RenderUtil::kPixelScale;
  int x_;
  int y_;
  int mouse_x_;
  int mouse_y_;
  SelectCallback callback_fn_;
  int promote_x_;
  int promote_y_;
  PieceType selected_type_ = PieceType::kPieceTypeNone;
  double anim_time_ = 0.0f;
  double popup_time_ = 0.0f;
};