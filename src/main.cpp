#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "audioplayer.h"
#include "button.h"
#include "debug.h"
#include "flatbutton.h"
#include "flatinput.h"
#include "flatlabel.h"
#include "flatpanel.h"
#include "flatoption.h"
#include "flatselect.h"
#include "flattext.h"
#include "flatvideo.h"
#include "fontbook.h"
#include "panel.h"
#include "window.h"

int main(int argc, char* args[]) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    ERROR(APP, "SDL_Init Error: %s", SDL_GetError());
    return 1;
  }

  if (atexit([](){
    SDL_Quit();
    LOG(APP, "SDL has been shutdown");
  }) != 0) {
    ERROR(APP, "atexit Error: Failed to register SDL_Quit");
    SDL_Quit();
    return 1;
  }

  SGI::FontBook::addFont("future", "fonts/kenvector_future.ttf");

  SGI::WindowPtr win = SGI::Window::create("Test", 1024, 768);
  win.get()->setName("window");
  win.get()->addTexture("glassPanel", "images/ui/glassPanel.png", SGI::Window::TextureSlice{30, 30, 30, 30});
  win.get()->addTexture("glassPanelProjection", "images/ui/glassPanel_projection.png", SGI::Window::TextureSlice{30, 30, 30, 30});
  win.get()->addTexture("metalPanel", "images/ui/metalPanel.png", SGI::Window::TextureSlice{10, 10, 10, 10});
  win.get()->addTexture("metalPanelBlue", "images/ui/metalPanel_blue.png", SGI::Window::TextureSlice{10, 10, 30, 10});
  win.get()->addTexture("metalPanelBlueCorner", "images/ui/metalPanel_blueCorner.png", SGI::Window::TextureSlice{10, 50, 30, 10});
  win.get()->addTexture("metalPanelGreen", "images/ui/metalPanel_green.png", SGI::Window::TextureSlice{10, 10, 30, 10});
  win.get()->addTexture("metalPanelYellow", "images/ui/metalPanel_yellow.png", SGI::Window::TextureSlice{10, 10, 30, 10});
  win.get()->addTexture("button", "images/ui/button_1.png", SGI::Window::TextureSlice{22, 22, 22, 22});
  win.get()->addTexture("buttonHover", "images/ui/button_1_hover.png", SGI::Window::TextureSlice{22, 22, 22, 22});
  win.get()->addTexture("buttonPressed", "images/ui/button_1_pressed.png", SGI::Window::TextureSlice{22, 22, 22, 22});
  win.get()->addCursor(SGI::Window::CursorType::DEFAULT, "images/ui/cursor_pointerFlat.png");

  // win.get()->setBackgroundTexture("metalPanel");
  win.get()->setSpacing(SGI::Container::ChildSpacing::Around);
  win.get()->setDirection(SGI::Container::ChildDirection::Column);
  win.get()->setCursor(SGI::Window::CursorType::DEFAULT);
  win.get()->setPadding(20, 20, 20, 20);

  SGI::FlatPanelPtr sectionA = SGI::FlatPanel::create();
  sectionA.get()->setName("sectionA");
  sectionA.get()->setConstraintFixed(SGI::Widget::ConstraintType::Width, 400);
  sectionA.get()->setConstraintMaximum(SGI::Widget::ConstraintType::Height, 280);
  sectionA.get()->setDirection(SGI::Container::ChildDirection::Column);
  sectionA.get()->setSpacing(SGI::Container::ChildSpacing::Around);
  win.get()->addChild(sectionA);

  SGI::FlatPanelPtr sectionA1 = SGI::FlatPanel::create();
  sectionA1.get()->setName("sectionA");
  sectionA1.get()->setConstraintFixed(SGI::Widget::ConstraintType::Width, 400);
  sectionA1.get()->setConstraintFixed(SGI::Widget::ConstraintType::Height, 200);
  sectionA1.get()->setSpacing(SGI::Container::ChildSpacing::Around);
  sectionA.get()->addChild(sectionA1);

  SGI::FlatPanelPtr sectionA2 = SGI::FlatPanel::create();
  sectionA2.get()->setName("sectionA");
  sectionA2.get()->setConstraintFixed(SGI::Widget::ConstraintType::Width, 400);
  sectionA2.get()->setConstraintFixed(SGI::Widget::ConstraintType::Height, 50);
  sectionA2.get()->setSpacing(SGI::Container::ChildSpacing::Around);
  sectionA.get()->addChild(sectionA2);

  SGI::FlatPanelPtr sectionB = SGI::FlatPanel::create();
  sectionB.get()->setName("sectionB");
  sectionB.get()->setConstraintMaximum(SGI::Widget::ConstraintType::Height, 300);
  sectionB.get()->setSpacing(SGI::Container::ChildSpacing::Around);
  sectionB.get()->setDirection(SGI::Container::ChildDirection::Column);
  win.get()->addChild(sectionB);

  SGI::FlatPanelPtr sectionB1 = SGI::FlatPanel::create();
  sectionB1.get()->setName("sectionB1");
  sectionB1.get()->setConstraintMaximum(SGI::Widget::ConstraintType::Height, 70);
  sectionB1.get()->setSpacing(SGI::Container::ChildSpacing::Around);
  sectionB.get()->addChild(sectionB1);

  SGI::FlatPanelPtr sectionB2 = SGI::FlatPanel::create();
  sectionB2.get()->setName("sectionB2");
  sectionB2.get()->setSpacing(SGI::Container::ChildSpacing::Around);
  sectionB.get()->addChild(sectionB2);

  SGI::FlatPanelPtr sectionB3 = SGI::FlatPanel::create();
  sectionB3.get()->setName("sectionB3");
  sectionB3.get()->setConstraintMaximum(SGI::Widget::ConstraintType::Height, 70);
  sectionB3.get()->setSpacing(SGI::Container::ChildSpacing::Around);
  sectionB.get()->addChild(sectionB3);

  SGI::FlatButtonPtr b1 = SGI::FlatButton::create("Play WAV");
  b1.get()->setName("b1");
  b1.get()->addMouseListener(SGI::Widget::MouseEventType::Click, [](SGI::WindowPtr window, SGI::WidgetPtr widget, uint8_t button)->bool {
    SGI::FlatButtonPtr btn = std::dynamic_pointer_cast<SGI::FlatButton>(widget);
    if (SGI::AudioPlayer::isPlaying("test1")) {
      SGI::AudioPlayer::stop("test1");
      btn.get()->setLabel("Play WAV");
    } else {
      SGI::AudioPlayer::play("test1");
      btn.get()->setLabel("Stop WAV");
    }
    return false;
  });
  sectionA2.get()->addChild(b1);

  SGI::FlatButtonPtr b2 = SGI::FlatButton::create("Play OGG");
  b2.get()->setName("b2");
  b2.get()->addMouseListener(SGI::Widget::MouseEventType::Click, [](SGI::WindowPtr window, SGI::WidgetPtr widget, uint8_t button)->bool {
    SGI::FlatButtonPtr btn = std::dynamic_pointer_cast<SGI::FlatButton>(widget);
    if (SGI::AudioPlayer::isPlaying("test2")) {
      SGI::AudioPlayer::stop("test2");
      btn.get()->setLabel("Play OGG");
    } else {
      SGI::AudioPlayer::play("test2");
      btn.get()->setLabel("Stop OGG");
    }
    return false;
  });
  sectionA2.get()->addChild(b2);

  SGI::FlatButtonPtr b3 = SGI::FlatButton::create("Play Video");
  b3.get()->setName("b3");
  b3.get()->addMouseListener(SGI::Widget::MouseEventType::Click, [](SGI::WindowPtr window, SGI::WidgetPtr widget, uint8_t button)->bool {
    SGI::FlatButtonPtr btn = std::dynamic_pointer_cast<SGI::FlatButton>(widget);
    return false;
  });
  sectionA2.get()->addChild(b2);

  SGI::FlatLabelPtr l1 = SGI::FlatLabel::create("Text Input:");
  l1.get()->setName("l1");
  l1.get()->setConstraintFixed(SGI::Widget::ConstraintType::Width, 120);
  l1.get()->setJustification(SGI::FlatLabel::TextJustification::Right);
  sectionB1.get()->addChild(l1);

  SGI::FlatInputPtr i1 = SGI::FlatInput::create();
  i1.get()->setName("b1");
  i1.get()->setConstraintMinimum(SGI::Widget::ConstraintType::Width, 200);
  i1.get()->addChangeListener([](SGI::WindowPtr window, SGI::WidgetPtr widget)->bool {
    SGI::FlatInputPtr input = std::dynamic_pointer_cast<SGI::FlatInput>(widget);
    LOG(APP, "Input: %s", input.get()->getValue().c_str());
    return false;
  });
  sectionB1.get()->addChild(i1);

  SGI::FlatLabelPtr l2 = SGI::FlatLabel::create("Select:");
  l2.get()->setName("l2");
  l2.get()->setConstraintFixed(SGI::Widget::ConstraintType::Width, 120);
  l2.get()->setJustification(SGI::FlatLabel::TextJustification::Right);
  sectionB1.get()->addChild(l2);

  SGI::FlatSelectPtr s1 = SGI::FlatSelect::create();
  s1.get()->setConstraintMinimum(SGI::Widget::ConstraintType::Width, 200);
  std::vector<std::string> options = {
    "Alien Green",
    "Light",
    "Dark",
    "Techie Blue",
    "Devilish Red",
    "Pastel",
  };
  s1.get()->setOptions(options);
  s1.get()->addChangeListener([](SGI::WindowPtr window, SGI::WidgetPtr widget)->bool {
    SGI::FlatSelectPtr select = std::dynamic_pointer_cast<SGI::FlatSelect>(widget);
    window.get()->setTheme(select.get()->getValue());
    return false;
  });
  sectionB1.get()->addChild(s1);

  SGI::FlatTextPtr t1 = SGI::FlatText::create("This is a test :[fg;255;0;0;]:Red :[fg;0;255;0;]:Green :[fg;0;0;255;]:Blue :[fg;0;]:. Ставка :[fs;B;]:Lorem :[fp;20;]:Ipsum:[fp;16;]: is simply:[fs;b;]:\n\n:[fs;IU;]:dummy text of the:[fs;u;]: printing and:[fs;i;]: typesetting industry. :[fs;O;]:Lorem Ipsum has:[fs;o;]: been the industry's standard dummy text ever since the 1500s, when an :[fs;S;]:unknown printer:[fs;s;]: took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.");
  sectionB2.get()->addChild(t1);

  SGI::FlatOptionPtr o1 = SGI::FlatOption::create("Option 1");
  o1.get()->setName("l1");
  o1.get()->setConstraintFixed(SGI::Widget::ConstraintType::Width, 120);
  sectionB3.get()->addChild(o1);

  SGI::FlatOptionPtr o2 = SGI::FlatOption::create("Option 2");
  o2.get()->setName("l1");
  o2.get()->setConstraintFixed(SGI::Widget::ConstraintType::Width, 120);
  sectionB3.get()->addChild(o2);

  SGI::FlatVideoPtr v1 = SGI::FlatVideo::create();
  v1.get()->setName("v1");
  v1.get()->setConstraintFixed(SGI::Widget::ConstraintType::Height, 200);
  v1.get()->loadFile("video/test.mp4");
  v1.get()->setRepeat(true);
  v1.get()->play();
  sectionA1.get()->addChild(v1);

  SGI::AudioPlayer::load("test1", "audio/test.wav");
  SGI::AudioPlayer::load("test2", "audio/test.ogg");
  SGI::AudioPlayer::load("test3", "audio/test.m4a");

  bool quit = false;
  SDL_Event e;
  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      win.get()->processEvent(&e);

      if (e.type == SDL_EVENT_QUIT) {
        quit = true;
      }
    }
  
    win.get()->render();
  }

  return 0;
}
