#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "audioplayer.h"
#include "button.h"
#include "debug.h"
#include "flatbutton.h"
#include "flatdivider.h"
#include "flatinput.h"
#include "flatlabel.h"
#include "flatpanel.h"
#include "flatoption.h"
#include "flatselect.h"
#include "flatslider.h"
#include "flattext.h"
#include "flatvideo.h"
#include "fontbook.h"
#include "optiongroup.h"
#include "panel.h"
#include "window.h"

std::map<std::string, SGI::FlatButtonPtr> sidebarButtons;
SGI::FlatLabelPtr lblHeaderTitle;
SGI::ContainerPtr cntBody;

void showWelcome()
{
  SGI::FlatTextPtr txtWelcome = SGI::FlatText::create();
  txtWelcome->setValue(R"(
Welcome to Widgets

This is a side-project by Justin Zaun to re-learn C++ and SDL after many years in the JavaScript world. The goal is to create a set a Wigets that can be used to create simple user interfaces without have to deal with all the nitty-gritty inner workings.

This project also includes show helper classes not directly related to widgets that can be used to bootstrap an application. This Demo application is designed to show what the current widgets are and to show some examples of what is possible. 
  )");

  cntBody->removeAll();
  cntBody->addChild(txtWelcome);
}

void showButtons()
{
  cntBody->removeAll();
  cntBody->setSpaceContent(SGI::Container::SpaceContent::Between);
  cntBody->setJustifyContent(SGI::Container::JustifyContent::Start);
  cntBody->setConstraintFixed(SGI::Container::ConstraintType::Spacing, 10);

  SGI::FlatTextPtr txtWelcome = SGI::FlatText::create();
  txtWelcome->setValue(R"(
:[fp;36]:Buttons:[fp;16]:
Use SGI's flat button class for actions in forms, dialogs, and more with support for multiple sizes, states, and more.
  )");
  cntBody->addChild(txtWelcome);

  SGI::FlatDividerPtr divTitle = SGI::FlatDivider::create();
  cntBody->addChild(divTitle);

  SGI::FlatTextPtr txtNormal = SGI::FlatText::create();
  txtNormal->setValue(R"(
:[fp;26]:Default Buttons:[fp;16]:
This is the standard look for the button in its different styles.
  )");
  cntBody->addChild(txtNormal);

  // Normal Buttons

  SGI::FlatPanelPtr pnlButtons = SGI::FlatPanel::create();
  pnlButtons->setConstraintFixed(SGI::Container::ConstraintType::Height, 70);
  pnlButtons->setDirection (SGI::Container::Direction::Row);
  pnlButtons->setSpaceContent(SGI::Container::SpaceContent::Around);
  cntBody->addChild(pnlButtons);

  SGI::FlatButtonPtr btnPrimary = SGI::FlatButton::create();
  btnPrimary->setStyle(SGI::FlatButton::Style::Primary);
  btnPrimary->setLabel("Primany");
  pnlButtons->addChild(btnPrimary);

  SGI::FlatButtonPtr btnSecondary = SGI::FlatButton::create();
  btnSecondary->setStyle(SGI::FlatButton::Style::Secondary);
  btnSecondary->setLabel("Secondary");
  pnlButtons->addChild(btnSecondary);

  SGI::FlatButtonPtr btnInfo = SGI::FlatButton::create();
  btnInfo->setStyle(SGI::FlatButton::Style::Info);
  btnInfo->setLabel("Info");
  pnlButtons->addChild(btnInfo);

  SGI::FlatButtonPtr btnWarn = SGI::FlatButton::create();
  btnWarn->setStyle(SGI::FlatButton::Style::Warning);
  btnWarn->setLabel("Warning");
  pnlButtons->addChild(btnWarn);

  SGI::FlatButtonPtr btnDanger = SGI::FlatButton::create();
  btnDanger->setStyle(SGI::FlatButton::Style::Danger);
  btnDanger->setLabel("Danger");
  pnlButtons->addChild(btnDanger);

  SGI::FlatTextPtr txtOutline = SGI::FlatText::create();
  txtOutline->setValue(R"(
:[fp;26]:Outline Buttons:[fp;16]:
In need of a button, but not the hefty background colors they bring?
  )");
  cntBody->addChild(txtOutline);

  // Outline Buttons

  SGI::FlatPanelPtr pnlOutlineButtons = SGI::FlatPanel::create();
  pnlOutlineButtons->setConstraintFixed(SGI::Container::ConstraintType::Height, 70);
  pnlOutlineButtons->setDirection (SGI::Container::Direction::Row);
  pnlOutlineButtons->setSpaceContent(SGI::Container::SpaceContent::Around);
  cntBody->addChild(pnlOutlineButtons);

  SGI::FlatButtonPtr btnOutlinePrimary = SGI::FlatButton::create();
  btnOutlinePrimary->setStyle(SGI::FlatButton::Style::Primary);
  btnOutlinePrimary->setLabel("Primany");
  btnOutlinePrimary->setOutline(true);
  pnlOutlineButtons->addChild(btnOutlinePrimary);

  SGI::FlatButtonPtr btnOutlineSecondary = SGI::FlatButton::create();
  btnOutlineSecondary->setStyle(SGI::FlatButton::Style::Secondary);
  btnOutlineSecondary->setLabel("Secondary");
  btnOutlineSecondary->setOutline(true);
  pnlOutlineButtons->addChild(btnOutlineSecondary);

  SGI::FlatButtonPtr btnOutlineInfo = SGI::FlatButton::create();
  btnOutlineInfo->setStyle(SGI::FlatButton::Style::Info);
  btnOutlineInfo->setLabel("Info");
  btnOutlineInfo->setOutline(true);
  pnlOutlineButtons->addChild(btnOutlineInfo);

  SGI::FlatButtonPtr btnOutlineWarn = SGI::FlatButton::create();
  btnOutlineWarn->setStyle(SGI::FlatButton::Style::Warning);
  btnOutlineWarn->setLabel("Warning");
  btnOutlineWarn->setOutline(true);
  pnlOutlineButtons->addChild(btnOutlineWarn);

  SGI::FlatButtonPtr btnOutlineDanger = SGI::FlatButton::create();
  btnOutlineDanger->setStyle(SGI::FlatButton::Style::Danger);
  btnOutlineDanger->setLabel("Danger");
  btnOutlineDanger->setOutline(true);
  pnlOutlineButtons->addChild(btnOutlineDanger);

  SGI::FlatTextPtr txtDisabled = SGI::FlatText::create();
  txtDisabled->setValue(R"(
:[fp;26]:Disabled Buttons:[fp;16]:
Make buttons look inactive by calling setDisabled(true).
  )");
  cntBody->addChild(txtDisabled);

  // Disabled Buttons

  SGI::FlatPanelPtr pnlDisabledButtons = SGI::FlatPanel::create();
  pnlDisabledButtons->setConstraintFixed(SGI::Container::ConstraintType::Height, 70);
  pnlDisabledButtons->setDirection (SGI::Container::Direction::Row);
  pnlDisabledButtons->setSpaceContent(SGI::Container::SpaceContent::Around);
  cntBody->addChild(pnlDisabledButtons);

  SGI::FlatButtonPtr btnDisabledPrimary = SGI::FlatButton::create();
  btnDisabledPrimary->setStyle(SGI::FlatButton::Style::Primary);
  btnDisabledPrimary->setLabel("Primany");
  btnDisabledPrimary->setDisabled(true);
  pnlDisabledButtons->addChild(btnDisabledPrimary);

  SGI::FlatButtonPtr btnDisabledSecondary = SGI::FlatButton::create();
  btnDisabledSecondary->setStyle(SGI::FlatButton::Style::Secondary);
  btnDisabledSecondary->setLabel("Secondary");
  btnDisabledSecondary->setDisabled(true);
  pnlDisabledButtons->addChild(btnDisabledSecondary);

  SGI::FlatButtonPtr btnDisabledInfo = SGI::FlatButton::create();
  btnDisabledInfo->setStyle(SGI::FlatButton::Style::Info);
  btnDisabledInfo->setLabel("Info");
  btnDisabledInfo->setDisabled(true);
  pnlDisabledButtons->addChild(btnDisabledInfo);

  SGI::FlatButtonPtr btnDisabledWarn = SGI::FlatButton::create();
  btnDisabledWarn->setStyle(SGI::FlatButton::Style::Warning);
  btnDisabledWarn->setLabel("Warning");
  btnDisabledWarn->setDisabled(true);
  pnlDisabledButtons->addChild(btnDisabledWarn);

  SGI::FlatButtonPtr btnDisabledDanger = SGI::FlatButton::create();
  btnDisabledDanger->setStyle(SGI::FlatButton::Style::Danger);
  btnDisabledDanger->setLabel("Danger");
  btnDisabledDanger->setDisabled(true);
  pnlDisabledButtons->addChild(btnDisabledDanger);

}

void selectDisplay(const std::string& section = "", const std::string& item = "")
{
  std::string key = section + "-" + item;

  for (const auto& btn : sidebarButtons) {
    if (key == btn.first) {
      btn.second->setOutline(false);
    } else {
      btn.second->setOutline(true);
    }
  }

  if (lblHeaderTitle != nullptr) {
    if (item == "" && section == "") {
      lblHeaderTitle->setLabel("SGI Widgets Demo");
    } else if (item == "") {
      lblHeaderTitle->setLabel("SGI Widgets Demo: " + section);
    } else if (section == "") {
      lblHeaderTitle->setLabel("SGI Widgets Demo: " + item);
    } else {
      lblHeaderTitle->setLabel("SGI Widgets Demo: " + section + ", " + item);
    }
  }

  if (key == "Widgets-Buttons") {
    showButtons();
  } else {
    showWelcome();
  }
}

SGI::WidgetPtr createSidebar()
{
  SGI::FlatPanelPtr sidebar = SGI::FlatPanel::create();
  sidebar->setName("sidebar");
  sidebar->setDirection(SGI::Container::Direction::Column);
  sidebar->setSpaceContent(SGI::Container::SpaceContent::Between);
  sidebar->setJustifyContent(SGI::Container::JustifyContent::Start);
  sidebar->setConstraintFixed(SGI::Container::ConstraintType::Spacing, 10);

  std::map<std::string, std::vector<std::string>> sections;
  sections["Widgets"] = {
    "Buttons",
    "Containers",
    "Lables",
    "Inputs",
    "Options",
    "Panels",
    "Selects",
    "Slider",
    "Text",
    "Videos",
    "Windows",
  };
  sections["Other"] ={
    "Themes",
  };
  sections["Demos"] ={
    "Chat",
  };

  for (const auto& section : sections) {
    SGI::FlatLabelPtr lblSidebarTitle = SGI::FlatLabel::create(section.first);
    lblSidebarTitle->setFontSize(26);
    lblSidebarTitle->setJustification(SGI::FlatLabel::TextJustification::Center);
    sidebar->addChild(lblSidebarTitle);
    
    SGI::FlatDividerPtr div = SGI::FlatDivider::create();
    sidebar->addChild(div);

    for (const auto& item : section.second) {
      SGI::FlatButtonPtr btn = SGI::FlatButton::create(item);
      btn->setConstraintMinimum(SGI::Widget::ConstraintType::Width, 200);
      btn->addMouseListener(SGI::Widget::MouseEventType::Click, [section, item](SGI::WindowPtr window, SGI::WidgetPtr widget, uint8_t button) -> bool {
        window->closeSidebar();
        selectDisplay(section.first, item);
        return true;
      });
      sidebar->addChild(btn);

      sidebarButtons[section.first + "-" + item] = btn;
    }
  }

  return sidebar;
}

SGI::WidgetPtr createHeader()
{
  SGI::FlatPanelPtr header = SGI::FlatPanel::create();
  header->setName("header");
  header->setConstraintFixed(SGI::Widget::ConstraintType::Height, 60);
  header->setSpaceContent(SGI::Container::SpaceContent::Between);
  header->setJustifyContent(SGI::Container::JustifyContent::Start);
  header->setDirection(SGI::Container::Direction::Row);
  header->setConstraintFixed(SGI::Container::ConstraintType::Spacing, 10);

  SGI::FlatButtonPtr btnSidebar = SGI::FlatButton::create();
  btnSidebar->setIcon("menuIcon");
  btnSidebar->setConstraintFixed(SGI::Widget::ConstraintType::Width, 50);
  btnSidebar->addMouseListener(SGI::Widget::MouseEventType::Click, [](SGI::WindowPtr window, SGI::WidgetPtr widget, uint8_t button) -> bool {
    window->openSidebar();
    return true;
  });
  header->addChild(btnSidebar);

  lblHeaderTitle = SGI::FlatLabel::create("SGI Widgets");
  lblHeaderTitle->setFontSize(18);
  header->addChild(lblHeaderTitle);

  return header;
}

int main(int argc, char* args[]) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
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
  win->addTexture("menuIcon", "images/icons/menu.png");
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
  win->setSpaceContent(SGI::Container::SpaceContent::Between);
  win->setJustifyContent(SGI::Container::JustifyContent::Start);
  win->setDirection(SGI::Container::Direction::Column);
  win->setConstraintFixed(SGI::Container::ConstraintType::Spacing, 0);
  win->setCursor(SGI::Window::CursorType::DEFAULT);
  win->setPadding(0, 0, 0, 0);

  SGI::WidgetPtr header = createHeader();
  win->addChild(header);

  cntBody = SGI::Container::create();
  cntBody->setName("content");
  cntBody->setSpaceContent(SGI::Container::SpaceContent::Between);
  cntBody->setJustifyContent(SGI::Container::JustifyContent::Start);
  cntBody->setDirection(SGI::Container::Direction::Column);
  cntBody->setConstraintFixed(SGI::Container::ConstraintType::Spacing, 0);
  cntBody->setPadding(20, 20, 10, 10);
  win->addChild(cntBody);

  SGI::WidgetPtr sidebar = createSidebar();
  win->getSidebar()->addChild(sidebar);

  selectDisplay();

  // SGI::FlatPanelPtr sectionA = SGI::FlatPanel::create();
  // sectionA->setName("sectionA");
  // sectionA->setConstraintFixed(SGI::Widget::ConstraintType::Width, 450);
  // sectionA->setConstraintMaximum(SGI::Widget::ConstraintType::Height, 300);
  // sectionA->setDirection(SGI::Container::Direction::Column);
  // sectionA->setSpaceContent(SGI::Container::SpaceContent::Around);
  // win->addChild(sectionA);

  // SGI::FlatVideoPtr v1 = SGI::FlatVideo::create();
  // v1->setName("v1");
  // v1->setConstraintFixed(SGI::Widget::ConstraintType::Height, 200);
  // v1->loadFile("video/test.mp4");
  // v1->setRepeat(true);
  // v1->play();
  // v1->setLocationCallback([](SGI::WindowPtr window, SGI::WidgetPtr widget, float currentTime, float maxTime) {
  //   SGI::WidgetPtr sliderWidget = window->findFirst("hs1");
  //   if (!sliderWidget) {
  //     return;
  //   }
  //   SGI::FlatSliderPtr slider = std::dynamic_pointer_cast<SGI::FlatSlider>(sliderWidget);
  //   if (slider) {
  //     int value = currentTime * 100;
  //     int max = maxTime * 100;
  //     slider->setValue(value);
  //     slider->setMaxValue(max);
  //   }
  // });
  // sectionA->addChild(v1);

  // SGI::FlatSliderPtr hs1 = SGI::FlatSlider::create();
  // hs1->setName("hs1");
  // hs1->setConstraintFixed(SGI::Widget::ConstraintType::Height, 20);
  // hs1->addChangeListener([](SGI::WindowPtr window, SGI::WidgetPtr widget)->bool {
  //   SGI::WidgetPtr videoWidget = window->findFirst("v1");
  //   if (!videoWidget) {
  //     return false;
  //   }

  //   SGI::FlatVideoPtr video = std::dynamic_pointer_cast<SGI::FlatVideo>(videoWidget);
  //   SGI::FlatSliderPtr slider = std::dynamic_pointer_cast<SGI::FlatSlider>(widget);

  //   int time = slider->getValue();
  //   video->seek(time / 100);
  //   return false;
  // });
  // sectionA->addChild(hs1);

  // SGI::Platform::createNotification("title", "message");

  // // SGI::FlatPanelPtr sectionA1 = SGI::FlatPanel::create();
  // // sectionA1->setName("sectionA");
  // // sectionA1->setConstraintFixed(SGI::Widget::ConstraintType::Width, 400);
  // // sectionA1->setConstraintFixed(SGI::Widget::ConstraintType::Height, 200);
  // // sectionA1->setSpaceContent(SGI::Container::SpaceContent::Around);
  // // sectionA->addChild(sectionA1);

  // SGI::FlatPanelPtr sectionA2 = SGI::FlatPanel::create();
  // sectionA2->setName("sectionA");
  // sectionA2->setConstraintFixed(SGI::Widget::ConstraintType::Height, 50);
  // sectionA2->setSpaceContent(SGI::Container::SpaceContent::Around);
  // sectionA2->setConstraint(SGI::Container::ConstraintType::Spacing, 20, 20);
  // sectionA->addChild(sectionA2);

  // SGI::FlatPanelPtr sectionB = SGI::FlatPanel::create();
  // sectionB->setName("sectionB");
  // sectionB->setConstraintMaximum(SGI::Widget::ConstraintType::Height, 300);
  // sectionB->setSpaceContent(SGI::Container::SpaceContent::Around);
  // sectionB->setDirection(SGI::Container::Direction::Column);
  // win->addChild(sectionB);

  // SGI::FlatPanelPtr sectionB1 = SGI::FlatPanel::create();
  // sectionB1->setName("sectionB1");
  // sectionB1->setConstraintMaximum(SGI::Widget::ConstraintType::Height, 70);
  // sectionB1->setSpaceContent(SGI::Container::SpaceContent::Around);
  // sectionB->addChild(sectionB1);

  // SGI::FlatPanelPtr sectionB2 = SGI::FlatPanel::create();
  // sectionB2->setName("sectionB2");
  // sectionB2->setSpaceContent(SGI::Container::SpaceContent::Around);
  // sectionB->addChild(sectionB2);

  // SGI::FlatPanelPtr sectionB3 = SGI::FlatPanel::create();
  // sectionB3->setName("sectionB3");
  // sectionB3->setConstraintMaximum(SGI::Widget::ConstraintType::Height, 70);
  // sectionB3->setSpaceContent(SGI::Container::SpaceContent::Around);
  // sectionB->addChild(sectionB3);

  // SGI::FlatButtonPtr b1 = SGI::FlatButton::create("Play WAV");
  // b1->setName("b1");
  // b1->setConstraintMinimum(SGI::Widget::ConstraintType::Width, 110);
  // b1->addMouseListener(SGI::Widget::MouseEventType::Click, [](SGI::WindowPtr window, SGI::WidgetPtr widget, uint8_t button)->bool {
  //   SGI::FlatButtonPtr btn = std::dynamic_pointer_cast<SGI::FlatButton>(widget);
  //   if (SGI::AudioPlayer::isPlaying("test1")) {
  //     SGI::AudioPlayer::stop("test1");
  //     btn->setLabel("Play WAV");
  //   } else {
  //     SGI::AudioPlayer::play("test1");
  //     btn->setLabel("Stop WAV");
  //   }
  //   return false;
  // });
  // sectionA2->addChild(b1);

  // SGI::FlatButtonPtr b2 = SGI::FlatButton::create("Play OGG");
  // b2->setName("b2");
  // b2->setConstraintMinimum(SGI::Widget::ConstraintType::Width, 110);
  // b2->addMouseListener(SGI::Widget::MouseEventType::Click, [](SGI::WindowPtr window, SGI::WidgetPtr widget, uint8_t button)->bool {
  //   SGI::FlatButtonPtr btn = std::dynamic_pointer_cast<SGI::FlatButton>(widget);
  //   if (SGI::AudioPlayer::isPlaying("test2")) {
  //     SGI::AudioPlayer::stop("test2");
  //     btn->setLabel("Play OGG");
  //   } else {
  //     SGI::AudioPlayer::play("test2");
  //     btn->setLabel("Stop OGG");
  //   }
  //   return false;
  // });
  // sectionA2->addChild(b2);

  // SGI::FlatButtonPtr b3 = SGI::FlatButton::create("Open Sidebar");
  // b3->setName("b3");
  // b3->setConstraintMinimum(SGI::Widget::ConstraintType::Width, 130);
  // b3->addMouseListener(SGI::Widget::MouseEventType::Click, [](SGI::WindowPtr window, SGI::WidgetPtr widget, uint8_t button)->bool {
  //   SGI::FlatButtonPtr btn = std::dynamic_pointer_cast<SGI::FlatButton>(widget);
  //   if (window->isSidebarOpen()) {
  //     window->closeSidebar();
  //     btn->setLabel("Open Sidebar");
  //   } else {
  //     window->openSidebar();
  //     btn->setLabel("Close Sidebar");
  //   }
  //   return false;
  // });
  // sectionA2->addChild(b3);

  // SGI::FlatLabelPtr l1 = SGI::FlatLabel::create("Text Input:");
  // l1->setName("l1");
  // l1->setConstraintFixed(SGI::Widget::ConstraintType::Width, 120);
  // l1->setJustification(SGI::FlatLabel::TextJustification::Right);
  // sectionB1->addChild(l1);

  // SGI::FlatInputPtr i1 = SGI::FlatInput::create();
  // i1->setName("b1");
  // i1->setConstraintMinimum(SGI::Widget::ConstraintType::Width, 200);
  // i1->addChangeListener([](SGI::WindowPtr window, SGI::WidgetPtr widget)->bool {
  //   SGI::FlatInputPtr input = std::dynamic_pointer_cast<SGI::FlatInput>(widget);
  //   LOG(APP, "Input: %s", input->getValue().c_str());
  //   return false;
  // });
  // sectionB1->addChild(i1);

  // SGI::FlatLabelPtr l2 = SGI::FlatLabel::create("Select:");
  // l2->setName("l2");
  // l2->setConstraintFixed(SGI::Widget::ConstraintType::Width, 120);
  // l2->setJustification(SGI::FlatLabel::TextJustification::Right);
  // sectionB1->addChild(l2);

  // SGI::FlatSelectPtr s1 = SGI::FlatSelect::create();
  // s1->setConstraintMinimum(SGI::Widget::ConstraintType::Width, 200);
  // std::vector<std::string> options = {
  //   "Alien Green",
  //   "Techie Blue",
  //   "Devil Red",
  //   "Light",
  //   "Dark",
  //   "Midnight",
  // };
  // s1->setOptions(options);
  // s1->addChangeListener([](SGI::WindowPtr window, SGI::WidgetPtr widget)->bool {
  //   SGI::FlatSelectPtr select = std::dynamic_pointer_cast<SGI::FlatSelect>(widget);
  //   window->setTheme(select->getValue());
  //   return false;
  // });
  // sectionB1->addChild(s1);

  // SGI::FlatTextPtr t1 = SGI::FlatText::create("This is a test :[fg;255;0;0;]:Red :[fg;0;255;0;]:Green :[fg;0;0;255;]:Blue :[fg;0;]:. Ставка :[fs;B;]:Lorem :[fp;20;]:Ipsum:[fp;16;]: is simply:[fs;b;]:\n\n:[fs;IU;]:dummy text of the:[fs;u;]: printing and:[fs;i;]: typesetting industry. :[fs;O;]:Lorem Ipsum has:[fs;o;]: been the industry's standard dummy text ever since the 1500s, when an :[fs;S;]:unknown printer:[fs;s;]: took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.");
  // sectionB2->addChild(t1);

  // SGI::OptionGroupPtr og = SGI::OptionGroup::create();
  // og->setSpaceContent(SGI::Container::SpaceContent::Around);
  // og->setMultiselect(true);
  // og->addChangeListener([](SGI::WindowPtr window, std::vector<SGI::FlatOptionPtr> widgets)->bool {
  //   std::string selected = "";
  //   for (auto w : widgets) {
  //     selected += w->getName() + " ";
  //   }

  //   LOG(APP, "OptionGroup selected: %s", selected.c_str());
  //   return false;
  // });
  // sectionB3->addChild(og);

  // SGI::FlatOptionPtr o1 = SGI::FlatOption::create("Option 1");
  // o1->setName("l1");
  // o1->setConstraintFixed(SGI::Widget::ConstraintType::Width, 120);
  // og->addChild(o1);

  // SGI::FlatOptionPtr o2 = SGI::FlatOption::create("Option 2");
  // o2->setName("l2");
  // o2->setConstraintFixed(SGI::Widget::ConstraintType::Width, 120);
  // og->addChild(o2);

  SGI::AudioPlayer::load("test1", "audio/test.wav");
  SGI::AudioPlayer::load("test2", "audio/test.ogg");
  // SGI::AudioPlayer::load("test3", "audio/test.m4a");

  // SGI::AudioPlayer::newBuffer("test1");
  // SGI::AudioPlayer::bufferAddMusic("test1", "O3T120L2CO4CO3L6BGABO4L2CO3L2CL2AL1GL2CFL4EL6CDEL4FL3DL6O2BO3CDL4EL2C");

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
