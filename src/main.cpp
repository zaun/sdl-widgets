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
#include "flatslider.h"
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
  win->setName("window");
  win->addTexture("glassPanel", "images/ui/glassPanel.png", SGI::Window::TextureSlice{30, 30, 30, 30});
  win->addTexture("glassPanelProjection", "images/ui/glassPanel_projection.png", SGI::Window::TextureSlice{30, 30, 30, 30});
  win->addTexture("metalPanel", "images/ui/metalPanel.png", SGI::Window::TextureSlice{10, 10, 10, 10});
  win->addTexture("metalPanelBlue", "images/ui/metalPanel_blue.png", SGI::Window::TextureSlice{10, 10, 30, 10});
  win->addTexture("metalPanelBlueCorner", "images/ui/metalPanel_blueCorner.png", SGI::Window::TextureSlice{10, 50, 30, 10});
  win->addTexture("metalPanelGreen", "images/ui/metalPanel_green.png", SGI::Window::TextureSlice{10, 10, 30, 10});
  win->addTexture("metalPanelYellow", "images/ui/metalPanel_yellow.png", SGI::Window::TextureSlice{10, 10, 30, 10});
  win->addTexture("button", "images/ui/button_1.png", SGI::Window::TextureSlice{22, 22, 22, 22});
  win->addTexture("buttonHover", "images/ui/button_1_hover.png", SGI::Window::TextureSlice{22, 22, 22, 22});
  win->addTexture("buttonPressed", "images/ui/button_1_pressed.png", SGI::Window::TextureSlice{22, 22, 22, 22});
  win->addCursor(SGI::Window::CursorType::DEFAULT, "images/ui/cursor_pointerFlat.png");

  // win->setBackgroundTexture("metalPanel");
  win->setSpacing(SGI::Container::ChildSpacing::Around);
  win->setDirection(SGI::Container::ChildDirection::Column);
  win->setCursor(SGI::Window::CursorType::DEFAULT);
  win->setPadding(20, 20, 20, 20);

  SGI::FlatPanelPtr sidebar = SGI::FlatPanel::create();
  sidebar->setName("sidebar");
  sidebar->setDirection(SGI::Container::ChildDirection::Column);
  sidebar->setSpacing(SGI::Container::ChildSpacing::Around);
  win->getSidebar()->addChild(sidebar);

  SGI::FlatPanelPtr sectionA = SGI::FlatPanel::create();
  sectionA->setName("sectionA");
  sectionA->setConstraintFixed(SGI::Widget::ConstraintType::Width, 450);
  sectionA->setConstraintMaximum(SGI::Widget::ConstraintType::Height, 300);
  sectionA->setDirection(SGI::Container::ChildDirection::Column);
  sectionA->setSpacing(SGI::Container::ChildSpacing::Around);
  win->addChild(sectionA);

  SGI::FlatVideoPtr v1 = SGI::FlatVideo::create();
  v1->setName("v1");
  v1->setConstraintFixed(SGI::Widget::ConstraintType::Height, 200);
  v1->loadFile("video/test.mp4");
  v1->setRepeat(true);
  v1->play();
  v1->setLocationCallback([](SGI::WindowPtr window, SGI::WidgetPtr widget, float currentTime, float maxTime) {
    SGI::WidgetPtr sliderWidget = window->findFirst("hs1");
    if (!sliderWidget) {
      return;
    }
    SGI::FlatSliderPtr slider = std::dynamic_pointer_cast<SGI::FlatSlider>(sliderWidget);
    if (slider) {
      int value = currentTime * 100;
      int max = maxTime * 100;
      slider->setValue(value);
      slider->setMaxValue(max);
    }
  });
  sectionA->addChild(v1);

  SGI::FlatSliderPtr hs1 = SGI::FlatSlider::create();
  hs1->setName("hs1");
  hs1->setConstraintFixed(SGI::Widget::ConstraintType::Height, 20);
  hs1->addChangeListener([](SGI::WindowPtr window, SGI::WidgetPtr widget)->bool {
    SGI::WidgetPtr videoWidget = window->findFirst("v1");
    if (!videoWidget) {
      return false;
    }

    SGI::FlatVideoPtr video = std::dynamic_pointer_cast<SGI::FlatVideo>(videoWidget);
    SGI::FlatSliderPtr slider = std::dynamic_pointer_cast<SGI::FlatSlider>(widget);

    int time = slider->getValue();
    video->seek(time / 100);
    return false;
  });
  sectionA->addChild(hs1);

  // SGI::FlatPanelPtr sectionA1 = SGI::FlatPanel::create();
  // sectionA1->setName("sectionA");
  // sectionA1->setConstraintFixed(SGI::Widget::ConstraintType::Width, 400);
  // sectionA1->setConstraintFixed(SGI::Widget::ConstraintType::Height, 200);
  // sectionA1->setSpacing(SGI::Container::ChildSpacing::Around);
  // sectionA->addChild(sectionA1);

  SGI::FlatPanelPtr sectionA2 = SGI::FlatPanel::create();
  sectionA2->setName("sectionA");
  sectionA2->setConstraintFixed(SGI::Widget::ConstraintType::Height, 50);
  sectionA2->setSpacing(SGI::Container::ChildSpacing::Around);
  sectionA->addChild(sectionA2);

  SGI::FlatPanelPtr sectionB = SGI::FlatPanel::create();
  sectionB->setName("sectionB");
  sectionB->setConstraintMaximum(SGI::Widget::ConstraintType::Height, 300);
  sectionB->setSpacing(SGI::Container::ChildSpacing::Around);
  sectionB->setDirection(SGI::Container::ChildDirection::Column);
  win->addChild(sectionB);

  SGI::FlatPanelPtr sectionB1 = SGI::FlatPanel::create();
  sectionB1->setName("sectionB1");
  sectionB1->setConstraintMaximum(SGI::Widget::ConstraintType::Height, 70);
  sectionB1->setSpacing(SGI::Container::ChildSpacing::Around);
  sectionB->addChild(sectionB1);

  SGI::FlatPanelPtr sectionB2 = SGI::FlatPanel::create();
  sectionB2->setName("sectionB2");
  sectionB2->setSpacing(SGI::Container::ChildSpacing::Around);
  sectionB->addChild(sectionB2);

  SGI::FlatPanelPtr sectionB3 = SGI::FlatPanel::create();
  sectionB3->setName("sectionB3");
  sectionB3->setConstraintMaximum(SGI::Widget::ConstraintType::Height, 70);
  sectionB3->setSpacing(SGI::Container::ChildSpacing::Around);
  sectionB->addChild(sectionB3);

  SGI::FlatButtonPtr b1 = SGI::FlatButton::create("Play WAV");
  b1->setName("b1");
  b1->addMouseListener(SGI::Widget::MouseEventType::Click, [](SGI::WindowPtr window, SGI::WidgetPtr widget, uint8_t button)->bool {
    SGI::FlatButtonPtr btn = std::dynamic_pointer_cast<SGI::FlatButton>(widget);
    if (SGI::AudioPlayer::isPlaying("test1")) {
      SGI::AudioPlayer::stop("test1");
      btn->setLabel("Play WAV");
    } else {
      SGI::AudioPlayer::play("test1");
      btn->setLabel("Stop WAV");
    }
    return false;
  });
  sectionA2->addChild(b1);

  SGI::FlatButtonPtr b2 = SGI::FlatButton::create("Play OGG");
  b2->setName("b2");
  b2->addMouseListener(SGI::Widget::MouseEventType::Click, [](SGI::WindowPtr window, SGI::WidgetPtr widget, uint8_t button)->bool {
    SGI::FlatButtonPtr btn = std::dynamic_pointer_cast<SGI::FlatButton>(widget);
    if (SGI::AudioPlayer::isPlaying("test2")) {
      SGI::AudioPlayer::stop("test2");
      btn->setLabel("Play OGG");
    } else {
      SGI::AudioPlayer::play("test2");
      btn->setLabel("Stop OGG");
    }
    return false;
  });
  sectionA2->addChild(b2);

  SGI::FlatButtonPtr b3 = SGI::FlatButton::create("Open Sidebar");
  b3->setName("b3");
  b3->setConstraintFixed(SGI::Widget::ConstraintType::Width, 130);
  b3->addMouseListener(SGI::Widget::MouseEventType::Click, [](SGI::WindowPtr window, SGI::WidgetPtr widget, uint8_t button)->bool {
    SGI::FlatButtonPtr btn = std::dynamic_pointer_cast<SGI::FlatButton>(widget);
    if (window->isSidebarOpen()) {
      window->closeSidebar();
      btn->setLabel("Open Sidebar");
    } else {
      window->openSidebar();
      btn->setLabel("Close Sidebar");
    }
    return false;
  });
  sectionA2->addChild(b3);

  SGI::FlatLabelPtr l1 = SGI::FlatLabel::create("Text Input:");
  l1->setName("l1");
  l1->setConstraintFixed(SGI::Widget::ConstraintType::Width, 120);
  l1->setJustification(SGI::FlatLabel::TextJustification::Right);
  sectionB1->addChild(l1);

  SGI::FlatInputPtr i1 = SGI::FlatInput::create();
  i1->setName("b1");
  i1->setConstraintMinimum(SGI::Widget::ConstraintType::Width, 200);
  i1->addChangeListener([](SGI::WindowPtr window, SGI::WidgetPtr widget)->bool {
    SGI::FlatInputPtr input = std::dynamic_pointer_cast<SGI::FlatInput>(widget);
    LOG(APP, "Input: %s", input->getValue().c_str());
    return false;
  });
  sectionB1->addChild(i1);

  SGI::FlatLabelPtr l2 = SGI::FlatLabel::create("Select:");
  l2->setName("l2");
  l2->setConstraintFixed(SGI::Widget::ConstraintType::Width, 120);
  l2->setJustification(SGI::FlatLabel::TextJustification::Right);
  sectionB1->addChild(l2);

  SGI::FlatSelectPtr s1 = SGI::FlatSelect::create();
  s1->setConstraintMinimum(SGI::Widget::ConstraintType::Width, 200);
  std::vector<std::string> options = {
    "Alien Green",
    "Techie Blue",
    "Devil Red",
    "Light",
    "Dark",
    "Midnight",
  };
  s1->setOptions(options);
  s1->addChangeListener([](SGI::WindowPtr window, SGI::WidgetPtr widget)->bool {
    SGI::FlatSelectPtr select = std::dynamic_pointer_cast<SGI::FlatSelect>(widget);
    window->setTheme(select->getValue());
    return false;
  });
  sectionB1->addChild(s1);

  SGI::FlatTextPtr t1 = SGI::FlatText::create("This is a test :[fg;255;0;0;]:Red :[fg;0;255;0;]:Green :[fg;0;0;255;]:Blue :[fg;0;]:. Ставка :[fs;B;]:Lorem :[fp;20;]:Ipsum:[fp;16;]: is simply:[fs;b;]:\n\n:[fs;IU;]:dummy text of the:[fs;u;]: printing and:[fs;i;]: typesetting industry. :[fs;O;]:Lorem Ipsum has:[fs;o;]: been the industry's standard dummy text ever since the 1500s, when an :[fs;S;]:unknown printer:[fs;s;]: took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.");
  sectionB2->addChild(t1);

  SGI::FlatOptionPtr o1 = SGI::FlatOption::create("Option 1");
  o1->setName("l1");
  o1->setConstraintFixed(SGI::Widget::ConstraintType::Width, 120);
  sectionB3->addChild(o1);

  SGI::FlatOptionPtr o2 = SGI::FlatOption::create("Option 2");
  o2->setName("l1");
  o2->setConstraintFixed(SGI::Widget::ConstraintType::Width, 120);
  sectionB3->addChild(o2);

  // SGI::FlatVideoPtr v1 = SGI::FlatVideo::create();
  // v1->setName("v1");
  // v1->setConstraintFixed(SGI::Widget::ConstraintType::Height, 200);
  // v1->loadFile("video/test.mp4");
  // v1->setRepeat(true);
  // v1->play();
  // sectionA1->addChild(v1);

  SGI::AudioPlayer::load("test1", "audio/test.wav");
  SGI::AudioPlayer::load("test2", "audio/test.ogg");
  SGI::AudioPlayer::load("test3", "audio/test.m4a");

  bool quit = false;
  SDL_Event e;
  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      win->processEvent(&e);

      if (e.type == SDL_EVENT_QUIT) {
        quit = true;
      }
    }
  
    win->render();
  }

  return 0;
}
