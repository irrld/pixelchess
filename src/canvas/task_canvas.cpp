#include "task_canvas.h"
#include "menu_canvas.h"

TaskCanvas::TaskCanvas(gApp* root, Ref<Task> task) : gBaseCanvas(root), task(task) {
  this->root = root;
}

TaskCanvas::~TaskCanvas() {

}

void TaskCanvas::setup() {
  loading_anim = 0;
  if (task) {
    title = task->title();
  } else {
    title = "";
  }
  white_pieces.loadImage("pieces/WhitePieces-Sheet.png");
  white_pieces.setFiltering(2, 2);
  button_ = new Button("BACK", getWidth() / 2, 638, 180, 60);
}

void TaskCanvas::update() {
  if (task) {
    title = task->title();
  } else {
    title = "";
  }
  loading_anim += appmanager->getElapsedTime();
  if (task && task->Update()) {
    task = task->Next();
  }
  button_->Update();
  if (button_->GetState() == ButtonState::Pressed) {
    if (task->CanStop()) {
      button_->Lock();
      if (task) {
        task->Stop();
      }
      root->setCurrentCanvas(new MenuCanvas(root));
    }
  }
}

void TaskCanvas::draw() {
  clearColor(CreateRGB(0x4c5465));
  const gColor& og_color = renderer->getColor();
  int size = 80;
  int scale = 3;
  int offset_x = (getWidth() / 2) - (size / 2);
  int offset_y = (getHeight() / 2) - (size / 2);
  for (int i = 0; i < 6; ++i) {
    double mp = std::min(1.0, loading_anim / 4.0);
    double delta = (loading_anim * 2.0) + (i * 1.1f * mp);
    int real_size = size * mp;
    int x = std::sin(delta) * real_size + offset_x;
    int y = std::cos(delta) * real_size + offset_y;
    white_pieces.drawSub(x + 8, y + 16, 16 * scale, 32 * scale, 16 * i, 0, 16, 32, 8, 16, (int) (delta * 50.0f) % 360);
  }
  RenderUtil::DrawFont(title, getWidth() / 2, 100, {1.0f, 1.0f, 1.0f}, true);
  setColor(og_color);
  if (!task || task->CanStop()) {
    button_->Draw();
  }
  root->DrawCursor();
}

void TaskCanvas::mouseMoved(int x, int y) {
  root->SetCursorPos(x, y);
  button_->OnMouseMoved(x, y);
}

void TaskCanvas::mousePressed(int x, int y, int button) {
  root->SetCursorPos(x, y);
  root->SetCursorType(CursorType::kHandClosed);
  button_->OnMousePressed(x, y);
}

void TaskCanvas::mouseReleased(int x, int y, int button) {
  root->SetCursorPos(x, y);
  root->SetCursorType(CursorType::kArrow);
  button_->OnMouseReleased(x, y);
}
