
#pragma once

#include <net/chess_connection.h>
#include "app/app.h"
#include "chess/chess_board.h"
#include "gBaseCanvas.h"
#include "gImage.h"
#include "net/packets.h"


class GameCanvas : public gBaseCanvas {
 public:
  GameCanvas(gApp* root, Ref<ChessConnection> connection, Ref<std::thread> thread);
  virtual ~GameCanvas();

  void setup();
  void update();
  void draw();

  void keyPressed(int key);
  void keyReleased(int key);
  void charPressed(unsigned int codepoint);
  void mouseMoved(int x, int y );
  void mouseDragged(int x, int y, int button);
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  void mouseScrolled(int x, int y);
  void mouseEntered();
  void mouseExited();
  void windowResized(int w, int h);

  void showNotify();
  void hideNotify();

  void HighlightPosition(int board_x, int board_y, const gColor& color);
  bool Move(int x, int y, int to_x, int to_y);
  void DrawBoard();
  void DrawInfoText();
  void DrawPiece(int x, int y);
  int ConvertY(int y);
  void ResetSelection();

  void UpdateInfoText();
  void UpdateFallAnimation();
 private:
  gApp* root_;
  gImage board_textures_[2];
  gImage outliner_;
  int board_pos_x, board_pos_y;
  int board_width_, board_height_;
  gImage white_pieces_;
  gImage black_pieces_;
  Ref<ChessBoard> chess_board_;
  int hover_piece_x_, hover_piece_y_;
  int move_piece_x_, move_piece_y_;
  bool show_moves_;
  PieceColor player_color_;
  gFont font_;
  bool animate_ = false;
  int animate_move_piece_x_, animate_move_piece_y_;
  float animate_from_pos_x_, animate_from_pos_y_;
  float animate_to_pos_x_, animate_to_pos_y_;
  int piece_offset_;
  bool input_lock_;
  double game_time_ = 0.0f;
  double fall_anim_dist_ = 1.0f;
  std::string info_text_;
  int info_pos_x_, info_pos_y_;
  Ref<ChessConnection> connection_;
  Ref<std::thread> thread_;
  std::string next_info_text_;
  bool has_next_text_ = false;
  bool flip_board_;
  bool game_started_;
  bool promoting_ = false;
  bool ended_ = false;
  PieceColor winner_color_;
  double dance_anim_time_ = 0.0f;
 private:
  void SetInfoText(const std::string& text);
};
