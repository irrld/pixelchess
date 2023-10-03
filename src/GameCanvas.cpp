#include "GameCanvas.h"

GameCanvas::GameCanvas(gApp* root, Ref<ChessConnection> connection, Ref<std::thread> thread) : gBaseCanvas(root), connection(connection), thread(thread) {
  this->root = root;
}

GameCanvas::~GameCanvas() {
}

void GameCanvas::setup() {
  if (!connection->IsLocal()) {
    player_color = kPieceColorBlack;
    flip_board = true;
  } else {
    player_color = kPieceColorWhite;
    flip_board = false;
  }
  board_textures[0].loadImage("boards/board_persp_01.png");
  board_textures[0].setFiltering(2, 2);
  board_textures[1].loadImage("boards/board_persp_01_flipped.png");
  board_textures[1].setFiltering(2, 2);

  board_width = board_textures[0].getWidth() * RenderUtil::kPixelScale;
  board_height = board_textures[0].getHeight() * RenderUtil::kPixelScale;
  board_pos_x = (getWidth() - board_width) / 2;
  board_pos_y = (getHeight() - board_height) / 2;
  white_pieces.loadImage("pieces/WhitePieces-Sheet.png");
  white_pieces.setFiltering(2, 2);
  black_pieces.loadImage("pieces/BlackPieces-Sheet.png");
  black_pieces.setFiltering(2, 2);
  outliner.loadImage("outliner.png");
  outliner.setFiltering(2, 2);
  show_moves = false;
  hover_piece_x = -1;
  hover_piece_y = -1;
  font.loadFont("PixeloidMono.ttf", 12, false);
  animate = false;
  piece_offset = white_pieces.getHeight() - 11;
  input_lock = true;
  game_time = 0.0;
  fell_anim_dist = 1.0f;
  wait_turn = false;
  appmanager->setTargetFramerate(120);
  chess_board = new ChessBoard(*connection->GetBoardData());
  connection->SetOnMove([this](int x, int y, int to_x, int to_y, PieceType piece_type, bool is_capture) {
    Move(x, y, to_x, to_y);
  });
  connection->SetOnTurnChange([this](PieceColor color) {
    if (color == player_color) {
      wait_turn = false;
      SetInfoText("Your turn!");
    } else {
      wait_turn = true;
      SetInfoText("Opponent's turn!");
    }
  });
  connection->SetOnStartGame([this]() {
    input_lock = false;
  });
  connection->Ready();
}


void GameCanvas::update() {
  if (has_next_text) {
    info_text = next_info_text;
    info_pos_x = getWidth() / 2;
    info_pos_y = board_pos_y + board_height + 50;
    has_next_text = false;
  }
  if (!connection->IsOpen()) {
    // todo disconnect screen
    return;
  }
  game_time += appmanager->getElapsedTime();
  fell_anim_dist = Lerp(fell_anim_dist, 0.0f, 0.02f);
  if (fell_anim_dist < 0.005f) {
    input_lock = false;
  }
}

void GameCanvas::draw() {
  clearColor(0x6f, 0x75, 0x83);
  DrawBoard();
  if (!info_text.empty()) {
    RenderUtil::DrawFont(info_text, info_pos_x, info_pos_y, {1.0f, 1.0f, 1.0f}, true);
  }
  /*for (int x = 0; x < 8; ++x) {
    for (int y = 0; y < 8; ++y) {
      RenderUtil::DrawFont(gToStr(x) + " " + gToStr(y),
                    board_pos_x + (7 + x * 16) * RenderUtil::kPixelScale,
                    board_pos_y + (10 + y * 12) * RenderUtil::kPixelScale, {1.0f, 0, 0});
    }
  }*/
}

void GameCanvas::HighlightPosition(int board_x, int board_y, const gColor& color) {
  int render_offset_x = (7 + board_x * 16) * RenderUtil::kPixelScale;
  int render_offset_y = (6 + ConvertY(board_y) * 12) * RenderUtil::kPixelScale;
  const gColor& og_color = renderer->getColor();
  renderer->setColor(color);
  outliner.draw(board_pos_x + render_offset_x, board_pos_y + render_offset_y, 16 * RenderUtil::kPixelScale, 12 * RenderUtil::kPixelScale);
  renderer->setColor(og_color);
}

void GameCanvas::Move(int x, int y, int to_x, int to_y) {
  animate_move_piece_x = to_x;
  animate_move_piece_y = to_y;
  animate_from_pos_x = (7 + x * 16) * RenderUtil::kPixelScale;
  animate_from_pos_y = (6 + ConvertY(y) * 12 - piece_offset) * RenderUtil::kPixelScale;
  animate_to_pos_x = (7 + to_x * 16) * RenderUtil::kPixelScale;
  animate_to_pos_y = (6 + ConvertY(to_y) * 12 - piece_offset) * RenderUtil::kPixelScale;
  animate = true;

  Ref<Piece> piece = chess_board->GetPiece(x, y);
  chess_board->SetPiece(to_x, to_y, piece);
  chess_board->SetPiece(x, y, nullptr);
}

void GameCanvas::DrawBoard() {
  renderer->setColor(255, 255, 255);
  board_textures[flip_board].draw(board_pos_x, board_pos_y, board_width, board_height);
  if (flip_board) {
    for (int x = 0; x < 8; x++) {
      for (int y = 0; y < 8; y++) {
        DrawPiece(x, y);
      }
    }
  } else {
    for (int x = 0; x < 8; x++) {
      for (int y = 7; y >= 0; y--) {
        DrawPiece(x, y);
      }
    }
  }
}

void GameCanvas::DrawPiece(int x, int y) {
  Ref<Piece> piece = chess_board->GetPiece(x, y);
  if (hover_piece_x == x && hover_piece_y == y) {
    if (!show_moves) {
      HighlightPosition(x, y, {0.0f, 139 / 255.0f, 203 / 255.0f});
    }
  }
  if (show_moves) {
    if (move_piece_x == x && move_piece_y == y) {
      HighlightPosition(x, y, {250 / 255.0f, 100 / 255.0f, 20 / 255.0f});
    }
    Ref<Piece> hover_piece = chess_board->GetPiece(move_piece_x, move_piece_y);
    if (hover_piece) {
      if (hover_piece->GetColor() == connection->GetCurrentTurn()) {
        if (chess_board->IsValidMove(move_piece_x, move_piece_y, x, y)) {
          if (hover_piece_x == x && hover_piece_y == y) {
            HighlightPosition(x, y, {0.3f, 0 / 255.0f, 203 / 255.0f});
          } else {
            HighlightPosition(x, y, {0.0f, 139 / 255.0f, 203 / 255.0f});
          }
        }/* else if (hover_piece->IsValidMove(move_piece_x, move_piece_y, x, y)) {
              HighlightPosition(x, y, {1.0f, 0 / 255.0f, 0 / 255.0f});
            }*/
      }
    }
  }

  if (!piece) {
    return;
  }
  auto& image = piece->GetColor() == kPieceColorWhite ? white_pieces : black_pieces;
  int index = piece->GetType() - 1;
  if (index < 0) {
    return;
  }
  int render_offset_x = (7 + x * 16) * RenderUtil::kPixelScale;
  int render_offset_y = (6 + ConvertY(y) * 12 - piece_offset) * RenderUtil::kPixelScale;
  if (animate && animate_move_piece_x == x && animate_move_piece_y == y) {
    image.drawSub(board_pos_x + animate_from_pos_x, board_pos_y + animate_from_pos_y, // render pos
                  16 * RenderUtil::kPixelScale, 32 * RenderUtil::kPixelScale, // render size
                  index * 16, index * 32, // image pos
                  16, 32 // image size
    );
    animate_from_pos_x = Lerp(animate_from_pos_x, animate_to_pos_x, 0.15f);
    animate_from_pos_y = Lerp(animate_from_pos_y, animate_to_pos_y, 0.15f);
    if (std::abs(animate_to_pos_x - animate_from_pos_x) < 0.02f && std::abs(animate_to_pos_y - animate_from_pos_y) < 0.02f) {
      animate = false;
    }
  } else {
    int start_offset = fell_anim_dist * -400 * (x + 1);
    image.drawSub(board_pos_x + render_offset_x, board_pos_y + render_offset_y + start_offset, // render pos
                  16 * RenderUtil::kPixelScale, 32 * RenderUtil::kPixelScale, // render size
                  index * 16, index * 32, // image pos
                  16, 32 // image size,
    );
  }
}

int GameCanvas::ConvertY(int y) {
  return flip_board ? y : 7 - y;
}
void GameCanvas::keyPressed(int key) {
//	gLogi("GameCanvas") << "keyPressed:" << key;
}

void GameCanvas::keyReleased(int key) {
//	gLogi("GameCanvas") << "keyReleased:" << key;
}

void GameCanvas::charPressed(unsigned int codepoint) {
//	gLogi("GameCanvas") << "charPressed:" << gCodepointToStr(codepoint);
}

void GameCanvas::mouseMoved(int x, int y) {
  if (input_lock || wait_turn) {
    return;
  }
//	gLogi("GameCanvas") << "mouseMoved" << ", x:" << x << ", y:" << y;
  int board_x = (x - board_pos_x) / RenderUtil::kPixelScale - 7;
  int board_y = (y - board_pos_y) / RenderUtil::kPixelScale - 6;
  int piece_x = board_x / 16;
  int piece_y = ConvertY(board_y / 12);
  if (piece_x < 0 || piece_x > 8 || piece_y < 0 || piece_y > 8) {
    hover_piece_x = -1;
    hover_piece_y = -1;
    return;
  }
  hover_piece_x = piece_x;
  hover_piece_y = piece_y;
}

void GameCanvas::mouseDragged(int x, int y, int button) {
//	gLogi("GameCanvas") << "mouseDragged" << ", x:" << x << ", y:" << y << ", b:" << button;
}

void GameCanvas::mousePressed(int x, int y, int button) {
//	gLogi("GameCanvas") << "mousePressed" << ", x:" << x << ", y:" << y << ", b:" << button;
}

void GameCanvas::mouseReleased(int x, int y, int button) {
  if (animate || input_lock || wait_turn) {
    return;
  }
  int board_x = (x - board_pos_x) / RenderUtil::kPixelScale - 7;
  int board_y = (y - board_pos_y) / RenderUtil::kPixelScale - 6;
  int piece_x = board_x / 16;
  int piece_y = ConvertY(board_y / 12);
  if (piece_x < 0 || piece_x >= 8 || piece_y < 0 || piece_y >= 8) {
    hover_piece_x = -1;
    hover_piece_y = -1;
    return;
  }
  hover_piece_x = piece_x;
  hover_piece_y = piece_y;

  bool was_show_moves = show_moves;
  show_moves = false;
  Ref<Piece> piece = chess_board->GetPiece(hover_piece_x, hover_piece_y);
  if (!was_show_moves && piece && piece->GetColor() == player_color) {
    show_moves = true;
    move_piece_x = hover_piece_x;
    move_piece_y = hover_piece_y;
  } else if (was_show_moves && (piece == nullptr || piece->GetColor() != player_color)) {
    if (animate || input_lock || wait_turn || !chess_board->IsValidMove(move_piece_x, move_piece_y, hover_piece_x, hover_piece_y)) {
      show_moves = false;
      hover_piece_x = -1;
      hover_piece_y = -1;
      return;
    }
    connection->Move(move_piece_x, move_piece_y, hover_piece_x, hover_piece_y, piece != nullptr ? piece->GetType() : kPieceTypeNone, false);
    Move(move_piece_x, move_piece_y, hover_piece_x, hover_piece_y);
    wait_turn = true;

    show_moves = false;
    hover_piece_x = -1;
    hover_piece_y = -1;
  }
}

void GameCanvas::SetInfoText(const std::string& text) {
  next_info_text = text;
  has_next_text = true;
}

void GameCanvas::mouseScrolled(int x, int y) {

}

void GameCanvas::mouseEntered() {

}

void GameCanvas::mouseExited() {

}

void GameCanvas::windowResized(int w, int h) {

}

void GameCanvas::showNotify() {

}

void GameCanvas::hideNotify() {

}
