
#include "promote_screen.h"
#include "game_canvas.h"

PromoteScreen::PromoteScreen(gApp* root, GameCanvas* canvas, PieceColor player_color) : root_(root), canvas_(canvas), player_color_(player_color) {
  renderer_ = canvas_->getRenderer();
}

void PromoteScreen::Setup() {
  x_ = (renderer_->getWidth() - width_) / 2;
  y_ = (renderer_->getHeight() - height_) / 2;
  mouse_x_ = 0;
  mouse_y_ = 0;
  anim_time_ = 0.0f;
}

void PromoteScreen::Draw() {
  const gColor& og_color = renderer_->getColor();
  double alpha_anim = std::min(popup_time_ * 4.0f, 1.0);
  float t = anim_time_ * 4.0f;
  if (t > 0) {
    alpha_anim = 1.0f - t;
  }
  renderer_->setColor(0, 0, 0, (int) (alpha_anim * 100));
  gDrawRectangle(0, 0, renderer_->getWidth(), renderer_->getHeight(), true);
  renderer_->setColor(255, 255, 255, (int) (alpha_anim * 255));
  RenderUtil::DrawRect(x_, y_,
                       width_,
                       height_);
  for (int i = 0; i < 4; i++) {
    PieceType type = (PieceType) (kPieceTypeKnight + i);
    int scale = RenderUtil::kPixelScale;
    int dx = (padding_ * scale) + (space_ * i * scale) + (piece_width_ * i * scale);
    int dy = 4 * scale;
    int x = x_ + dx;
    int y = y_ - dy;
    if (type == selected_type_) {
      continue;
    }
    bool hover = mouse_x_ >= x && mouse_x_ <= x + piece_width_ * scale &&
                 mouse_y_ >= y && mouse_y_ <= y + piece_height_ * scale;
    if (selected_type_ == kPieceTypeNone && hover) {
      const gColor& og_color = renderer_->getColor();
      renderer_->setColor(0.0f, 0.545f, 0.796f);
      canvas_->DrawOutline(type, x, y, scale);
      renderer_->setColor(og_color);
    }
    canvas_->DrawPiece(type, player_color_, x, y, scale);
  }
  renderer_->setColor(og_color);
  if (selected_type_ != kPieceTypeNone) {
    int i = selected_type_ - kPieceTypeKnight;
    int scale = RenderUtil::kPixelScale;
    int dx = (padding_ * scale) + (space_ * i * scale) + (piece_width_ * i * scale);
    int dy = 4 * scale;
    int x = x_ + dx;
    int y = y_ - dy;
    canvas_->DrawPieceAnimatingUnbound(selected_type_, player_color_, x, y, promote_x_, promote_y_, t);
    if (t >= 1.0f) {
      callback_fn_(selected_type_);
      selected_type_ = kPieceTypeNone;
    }
  }
}

void PromoteScreen::Update() {
  if (selected_type_ != kPieceTypeNone) {
    anim_time_ += appmanager->getElapsedTime();
  }
  popup_time_ += appmanager->getElapsedTime();
}

void PromoteScreen::OnMouseMoved(int mouse_x, int mouse_y) {
  mouse_x_ = mouse_x;
  mouse_y_ = mouse_y;
}

void PromoteScreen::OnMousePressed(int mouse_x, int mouse_y) {
  mouse_x_ = mouse_x;
  mouse_y_ = mouse_y;
}

void PromoteScreen::OnMouseReleased(int mouse_x, int mouse_y) {
  mouse_x_ = mouse_x;
  mouse_y_ = mouse_y;
  if (selected_type_ != kPieceTypeNone) {
    return;
  }
  for (int i = 0; i < 4; i++) {
    PieceType type = (PieceType) (kPieceTypeKnight + i);
    int scale = RenderUtil::kPixelScale;
    int dx = (padding_ * scale) + (space_ * i * scale) + (piece_width_ * i * scale);
    int dy = 4 * scale;
    int x = x_ + dx;
    int y = y_ - dy;
    bool hover = mouse_x_ >= x && mouse_x_ <= x + piece_width_ * scale &&
                 mouse_y_ >= y && mouse_y_ <= y + piece_height_ * scale;
    if (hover) {
      selected_type_ = type;
      break;
    }
  }
}

void PromoteScreen::SetSelectCallback(SelectCallback callback_fn) {
  callback_fn_ = std::move(callback_fn);
}

void PromoteScreen::SetPromotePosition(int x, int y) {
  promote_x_ = x;
  promote_y_ = y;
}

void PromoteScreen::Reset() {
  selected_type_ = kPieceTypeNone;
  anim_time_ = 0.0f;
  popup_time_ = 0.0f;
}