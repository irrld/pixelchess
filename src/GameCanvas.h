
#pragma once

#include <net/ChessConnection.h>
#include "app/gApp.h"
#include "chess/ChessBoard.h"
#include "gBaseCanvas.h"
#include "gImage.h"
#include "net/Packets.h"

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
  void Move(int x, int y, int to_x, int to_y);
  void DrawBoard();
private:
  static const int kBoardThemes = 5;
  gApp* root;
  gImage cursor;
  gImage board_textures[kBoardThemes];
  gImage outliner;
  int board_theme;
  int board_pos_x, board_pos_y;
  int board_width, board_height;
  gImage white_pieces;
  gImage black_pieces;
  ChessBoard* chess_board;
  int hover_piece_x;
  int hover_piece_y;
  int move_piece_x;
  int move_piece_y;
  bool show_moves;
  PieceColor player_color;
  gFont font;
  bool animate;
  int animate_move_piece_x;
  int animate_move_piece_y;
  float animate_from_pos_x;
  float animate_from_pos_y;
  float animate_to_pos_x;
  float animate_to_pos_y;
  int piece_offset;
  bool input_lock;
  double game_time;
  double fell_anim_dist;
  std::string info_text;
  int info_pos_x;
  int info_pos_y;
  bool wait_turn;
  Ref<ChessConnection> connection;
  Ref<std::thread> thread;

 private:
  void SetInfoText(const std::string& text);
};
