#include <nanogui/nanogui.h>
#include <nanogui/opengl.h>
#include <iostream>

using namespace nanogui;
using namespace Eigen;
using namespace std;

FormScreen::FormScreen(float refreshTime)
: Screen()
{
  mCurrentWindow = nullptr;
  mLastRefresh = 0;
  mRefreshTime = refreshTime;
  mInputCellSize = Vector2i(80,20);
  mFontSize = 16;
  mMargin = 0;
  mSpacing = 1;
}

void FormScreen::draw()
{ 
  double time = glfwGetTime();

  if((time - mLastRefresh) > mRefreshTime)
  {
    // synch with mapped values
    for_each(syncJobs.begin(),syncJobs.end(),[](std::function<void()> &cur){
      cur();
    });

    mLastRefresh = time;
  }

  this->drawWidgets();
}

Window* FormScreen::addNewWindow(const Vector2i& position,const string &title)
{
  // save state of current window
  if(mCurrentWindow)
  {
    auto it = windowStates.find(mCurrentWindow);
    it->second = mCurrentWindowState;
  }

  Window* window = new nanogui::Window(this,title);
  window->setPosition(position);
  window->setLayout(new nanogui::GroupLayout(10,5,10,5));

  // add new
  WindowState state;
  state.layout = Layout::Vertical;
  state.mode = Mode::None;
  state.widget = window;
  windowStates[window] = state;

  mCurrentWindow = window;
  mCurrentWindowState = state;
  
  return window;
}

Button* FormScreen::addButton(const std::string &caption,std::function<void()> callback)
{
  if(mCurrentWindowState.mode != Mode::SingleCell)
  {
    Widget* panel = new Widget(mCurrentWindow);
    
    /*BoxLayout* layout;
    if(mCurrentWindowState.layout == LAYOUT_HORIZONTAL)
      layout = new BoxLayout(BoxLayout::Horizontal,BoxLayout::Middle,0,1);
    else
      layout = new BoxLayout(BoxLayout::Vertical,BoxLayout::Minimum,0,1);*/
    
    GridLayout* layout;
    if(mCurrentWindowState.layout == Layout::Horizontal)
    {
      layout = new GridLayout(GridLayout::Orientation::Vertical,1,mMargin,mSpacing);
      layout->setDefaultColAlignment(GridLayout::Alignment::Fill);
    }
    else
    {
      layout = new GridLayout(GridLayout::Orientation::Horizontal,1,mMargin,mSpacing);
      layout->setDefaultColAlignment(GridLayout::Alignment::Fill);
    }

    panel->setLayout(layout);
    mCurrentWindowState.widget = panel;
  }

  mCurrentWindowState.mode = Mode::SingleCell;

  Screen* sc = dynamic_cast<Screen*>(mCurrentWindow->parent());

  Button* b = new Button(mCurrentWindowState.widget,caption);
  b->setCallback(callback);
  //b->setFixedSize(Vector2i(b->preferredSize(sc->nvgContext()).x(),25));
  b->setFontSize(mFontSize+2);
  return b;
}

nanogui::Label* FormScreen::addNewGroup(const std::string &caption,Layout layout)
{
  mCurrentWindowState.layout = layout;
  mCurrentWindowState.mode = Mode::None;
  mCurrentWindowState.widget = mCurrentWindow;

  Label* label = new Label((Widget*)mCurrentWindow,caption,"sans-bold");
  label->setFontSize(mFontSize+4);

  return label;
}

void FormScreen::setCurrentWindow(nanogui::Window* window)
{
  auto it = windowStates.find(mCurrentWindow);
  assert(it != windowStates.end()); // The provided window is not valid!

  mCurrentWindow = window;
}

void FormScreen::setCurrentLayout(Layout layout)
{
  mCurrentWindowState.layout = layout;
  mCurrentWindowState.mode = Mode::None;
}

nanogui::CheckBox* FormScreen::addVariable(bool& value,const std::string &label,bool enabled)
{
  CheckBox* cb = initBoolVariableWidget(value,label,enabled);
  
  cb->setCallback([&value](bool checked){ value = checked; });

  CheckBox& cbr = *cb;
  syncJobs.push_back([&value,&cbr](){
    if(cbr.checked() != value) cbr.setChecked(value);
  });

  return cb;
}

nanogui::CheckBox* FormScreen::addVariable(std::function<void(bool)> setCallback,std::function<bool()> getCallback,const std::string &label,bool initValue,bool enabled)
{
  CheckBox* cb = initBoolVariableWidget(initValue,label,enabled);

  cb->setCallback(setCallback);

  if(getCallback)
  {
    CheckBox& cbr = *cb;
    boolVariableGetCallbacks.push_back(getCallback);
    std::function<bool()>& lf = boolVariableGetCallbacks.back();
    syncJobs.push_back([&cbr,&lf](){
      bool val = lf();
      if(val != cbr.checked())
        cbr.setChecked(val);
    });
  }

  return cb;
}


nanogui::TextBox* FormScreen::addVariable(char& value,const std::string &label,bool editable)
{
  TextBox* tb = initCharVariableWidget(value,label,editable);

  tb->setCallback([&value](const std::string& str){
    if(str.size() > 0)
      value = (char)str[0];
    else
      value = '\0';
    return true;
  });

  TextBox& tbr = *tb;
  syncJobs.push_back([&value,&tbr](){
    string str = std::string(1,value);
    if(str != tbr.value())
      tbr.setValue(str);
  });

  return tb;
}

nanogui::TextBox* FormScreen::addVariable(std::function<void(char)> setCallback,std::function<char()> getCallback,const std::string &label,char initValue,bool editable)
{
  TextBox* tb = initCharVariableWidget(initValue,label,editable);

  if(setCallback)
  {
    charVariableSetCallbacks.push_back(setCallback);
    std::function<void(char)>& lf = charVariableSetCallbacks.back();
    tb->setCallback([&lf](const std::string& str){
      if(str.size() > 0)
        lf((char)str[0]);
      else
        lf('\0');
      return true;
    });
  }

  if(getCallback)
  {
    TextBox& tbr = *tb;
    charVariableGetCallbacks.push_back(getCallback);
    std::function<char()>& lf = charVariableGetCallbacks.back();
    syncJobs.push_back([&tbr,&lf](){
      string str = std::string(1,lf());
      if(str != tbr.value())
        tbr.setValue(str);
    });
  }

  return tb;
}

nanogui::TextBox* FormScreen::addVariable(int& value,const std::string &label,bool editable)
{
  TextBox* tb = initIntVariableWidget(value,label,editable);

  tb->setCallback([&value](const std::string& str){
    try {
      value = std::stoi(str);
      return true;
    }
    catch(...) {
      return false;
    }
  });
  
  TextBox& tbr = *tb;
  syncJobs.push_back([&value,&tbr](){
    std::string str = std::to_string(value);
    if(str != tbr.value())
      tbr.setValue(str);
  });

  return tb;
}

nanogui::TextBox* FormScreen::addVariable(std::function<void(int)> setCallback,std::function<int()> getCallback,const std::string &label,int initValue,bool editable)
{
  TextBox* tb = initIntVariableWidget(initValue,label,editable);

  if(setCallback)
  {
    intVariableSetCallbacks.push_back(setCallback);
    std::function<void(int)>& lf = intVariableSetCallbacks.back();
    tb->setCallback([&lf](const std::string& str){
      int value;

      try {
        value = std::stoi(str);
      }
      catch(...) {
        return false;
      }

      lf(value);
      return true;
    });
  }

  if(getCallback)
  {
    TextBox& tbr = *tb;
    intVariableGetCallbacks.push_back(getCallback);
    std::function<int()>& lf = intVariableGetCallbacks.back();
    syncJobs.push_back([&tbr,&lf](){
      std::string str = std::to_string(lf());
      if(str != tbr.value())
        tbr.setValue(str);
    });
  }

  return tb;
}

nanogui::TextBox* FormScreen::addVariable(unsigned int& value,const std::string &label,bool editable)
{
  TextBox* tb = initUintVariableWidget(value,label,editable);

  tb->setCallback([&value](const std::string& str){
    try {
      value = std::stoi(str);
      return true;
    }
    catch(...) {
      return false;
    }
  });

  TextBox& tbr = *tb;
  syncJobs.push_back([&value,&tbr](){
    std::string str = std::to_string(value);
    if(str != tbr.value())
      tbr.setValue(str);
  });

  return tb;
}

nanogui::TextBox* FormScreen::addVariable(std::function<void(unsigned int)> setCallback,std::function<unsigned int()> getCallback,const std::string &label,unsigned int initValue,bool editable)
{
  TextBox* tb = initUintVariableWidget(initValue,label,editable);

  if(setCallback)
  {
    uintVariableSetCallbacks.push_back(setCallback);
    std::function<void(unsigned int)>& lf = uintVariableSetCallbacks.back();
    tb->setCallback([&lf](const std::string& str){
      unsigned int value;

      try {
        value = std::stoi(str);
      }
      catch(...) {
        return false;
      }

      lf(value);
      return true;
    });
  }

  if(getCallback)
  {
    TextBox& tbr = *tb;
    uintVariableGetCallbacks.push_back(getCallback);
    std::function<unsigned int()>& lf = uintVariableGetCallbacks.back();
    syncJobs.push_back([&tbr,&lf](){
      std::string str = std::to_string(lf());
      if(str != tbr.value())
        tbr.setValue(str);
    });
  }

  return tb;
}

nanogui::TextBox* FormScreen::addVariable(float& value,const std::string &label,bool editable)
{
  TextBox* tb = initFloatVariableWidget(value,label,editable);
  TextBox& tbr = *tb;
  
  tb->setCallback([this,&value,&tbr](const std::string& str){
    try {
      value = std::stof(str);
      string formated = floatToString(value);
      if(formated.size() <= str.size())
        tbr.setValue(formated);
      return true;
    }
    catch(...) {
      return false;
    }
  });

  syncJobs.push_back([this,&value,&tbr](){
    if(value != std::stof(tbr.value()))
        tbr.setValue(floatToString(value));
  });

  return tb;
}

nanogui::TextBox* FormScreen::addVariable(std::function<void(float)> setCallback,std::function<float()> getCallback,const std::string &label,float initValue,bool editable)
{
  TextBox* tb = initFloatVariableWidget(initValue,label,editable);
  TextBox& tbr = *tb;

  if(setCallback)
  {
    floatVariableSetCallbacks.push_back(setCallback);
    std::function<void(float)>& lf = floatVariableSetCallbacks.back();
    tb->setCallback([&](const std::string& str){
      float value;
      try {
        value = std::stof(str);
        string formated = floatToString(value);
        if(formated.size() <= str.size())
          tbr.setValue(formated);
      }
      catch(...) {
        return false;
      }

      lf(value);
      return true;
    });
  }

  if(getCallback)
  {
    floatVariableGetCallbacks.push_back(getCallback);
    std::function<float()>& lf = floatVariableGetCallbacks.back();
    syncJobs.push_back([&](){
      float value = lf();
      if(value != std::stof(tbr.value()))
        tbr.setValue(floatToString(value));
    });
  }

  return tb;
}

nanogui::TextBox* FormScreen::addVariable(double& value,const std::string &label,bool editable)
{
  TextBox* tb = initDoubleVariableWidget(value,label,editable);
  TextBox& tbr = *tb;

  tb->setCallback([this,&value,&tbr](const std::string& str){
    try {
      value = std::stof(str);
      string formated = doubleToString(value);
      if(formated.size() < str.size())
        tbr.setValue(formated);
      return true;
    }
    catch(...) {
      return false;
    }
  });

  syncJobs.push_back([this,&value,&tbr](){
      tbr.setValue(doubleToString(value));
  });

  return tb;
}

nanogui::TextBox* FormScreen::addVariable(std::function<void(double)> setCallback,std::function<double()> getCallback,const std::string &label,double initValue,bool editable)
{
  TextBox* tb = initDoubleVariableWidget(initValue,label,editable);
  TextBox& tbr = *tb;

  if(setCallback)
  {
    doubleVariableSetCallbacks.push_back(setCallback);
    std::function<void(double)>& lf = doubleVariableSetCallbacks.back();
    tb->setCallback([&](const std::string& str){
      double value;
      try {
        value = std::stof(str);
        string formated = doubleToString(value);
        if(formated.size() < str.size())
          tbr.setValue(formated);
      }
      catch(...) {
        return false;
      }

      lf(value);
      return true;
    });
  }

  if(getCallback)
  {
    doubleVariableGetCallbacks.push_back(getCallback);
    std::function<double()>& lf = doubleVariableGetCallbacks.back();
    syncJobs.push_back([&](){
      double value = lf();
      if(value != std::stof(tbr.value()))
          tbr.setValue(doubleToString(value));
    });
  }

  return tb;
}

nanogui::TextBox* FormScreen::addVariable(std::string& value,const std::string &label,bool editable)
{
  TextBox* tb = initStringVariableWidget(value,label,editable);
  
  tb->setCallback([&value](const std::string& str){
    value = str;
    return true;
  });

  TextBox& tbr = *tb;
  syncJobs.push_back([&value,&tbr](){
    if(value != tbr.value())
      tbr.setValue(value);
  });

  return tb;
}

nanogui::TextBox* FormScreen::addVariable(std::function<void(const std::string&)> setCallback,std::function<const std::string&()> getCallback,const std::string &label,const std::string& initValue,bool editable)
{
  TextBox* tb = initStringVariableWidget(initValue,label,editable);
  TextBox& tbr = *tb;

  if(setCallback)
  {
    stringVariableSetCallbacks.push_back(setCallback);
    std::function<void(const string&)>& lf = stringVariableSetCallbacks.back();
    tb->setCallback([&tbr,&lf](const string& str){
      lf(str);
      return true;
    });
  }

  if(getCallback)
  {
    
    stringVariableGetCallbacks.push_back(getCallback);
    std::function<const string&()>& lf = stringVariableGetCallbacks.back();
    syncJobs.push_back([&tbr,&lf](){
      string value = lf();
      if(value != tbr.value())
        tbr.setValue(value);
    });
  }

  return tb;
}

nanogui::PopupButton* FormScreen::addColorPicker(Eigen::Vector3f& color,const std::string &label,bool enabled)
{
  PopupButton* pb = initColorPickerWidget(color,label,enabled);

  PopupButton& pbr = *pb;
  pb->setCallback([&color,&pbr](){
    color = pbr.backgroundColor().block<3,1>(0,0);
  });

  syncJobs.push_back([&color,&pbr](){
    if(color != pbr.backgroundColor().block<3,1>(0,0))
      pbr.setBackgroundColor(Color(color));
  });

  return pb;
}

nanogui::PopupButton* FormScreen::addColorPicker(std::function<void(const Eigen::Vector3f&)> setCallback,std::function<const Eigen::Vector3f&()> getCallback,const std::string &label,const Eigen::Vector3f& initValue,bool enabled)
{
  PopupButton* pb = initColorPickerWidget(initValue,label,enabled);
  PopupButton& pbr = *pb;

  if(setCallback)
  {
    colorPickerSetCallbacks.push_back(setCallback);
    std::function<void(const Eigen::Vector3f&)>& lf = colorPickerSetCallbacks.back();
    pbr.setCallback([&pbr,&lf](){
      lf(pbr.backgroundColor().block<3,1>(0,0));
    });
  }

  if(getCallback)
  {
    colorPickerGetCallbacks.push_back(getCallback);
    std::function<const Eigen::Vector3f&()>& lf = colorPickerGetCallbacks.back();
    syncJobs.push_back([&pbr,&lf](){
      const Vector3f& value = lf();
      if(Color(value) != pbr.backgroundColor())
        pbr.setBackgroundColor(Color(value));
    });
  }

  return pb;
}

nanogui::ComboBox* FormScreen::addEnumerationHelper(int& value,const std::string &label,const std::vector<int>& enumIds,const std::vector<std::string>& names,bool enabled)
{
  ComboBox* cb = initEnumWidget(value,label,names,enabled);
  ComboBox& cbr = *cb;

  cb->setCallback([&value,&cbr](int id)
  {
    value = id;
  });

  enumIdsList.push_back(enumIds);
  std::vector<int>& enumIdsRef = enumIdsList.back();
  enumNamesList.push_back(names);
  std::vector<std::string>& namesRef = enumNamesList.back();
  syncJobs.push_back([&value,&cbr,&enumIdsRef,&namesRef]()
  {
    auto it = std::find(enumIdsRef.begin(),enumIdsRef.end(),value);
    if(namesRef[*it] != cbr.caption())
      cbr.setSelectedIndex(value);
  });

  return cb;
}

nanogui::ComboBox* FormScreen::addEnumerationHelper(std::function<void(int)> setCallback,std::function<int()> getCallback,const std::string &label,const std::vector<int>& enumIds,const std::vector<std::string>& names,int initValue,bool enabled)
{
  ComboBox* cb = initEnumWidget(initValue,label,names,enabled);
  ComboBox& cbr = *cb;

  enumIdsList.push_back(enumIds);
  enumNamesList.push_back(names);

  std::vector<int>& enumIdsRef = enumIdsList.back();
  std::vector<std::string>& namesRef = enumNamesList.back();

  if(setCallback)
  {
    enumSetCallbacks.push_back(setCallback);
    std::function<void(int)>& lf = enumSetCallbacks.back();
    cb->setCallback([&lf,&enumIdsRef,&namesRef](int id)
    {
      int enumId = enumIdsRef[id];
      lf(enumId);
    });
  }

  if(getCallback)
  {
    enumGetCallbacks.push_back(getCallback);
    std::function<int()>& lf = enumGetCallbacks.back();
    syncJobs.push_back([&lf,&cbr,&enumIdsRef,&namesRef]()
    {
      int id = lf();
      auto it = std::find(enumIdsRef.begin(),enumIdsRef.end(),id);
      if(namesRef[it-enumIdsRef.begin()] != cbr.caption())
        cbr.setSelectedIndex(id);
    });
  }

  return cb;
}

CheckBox* FormScreen::initBoolVariableWidget(bool value,const std::string &label,bool enabled)
{
  addVariablePreWork(label);

  Widget* panel = new Widget(mCurrentWindowState.widget);
  GridLayout* layout = new GridLayout(GridLayout::Orientation::Vertical,1,0,0,GridLayout::Alignment::Middle,GridLayout::Alignment::Middle);
  panel->setLayout(layout);
  panel->setFixedSize(mInputCellSize);

  CheckBox* cb = new CheckBox(panel,"");
  cb->setEnabled(enabled);
  cb->setFixedSize(Vector2i(mFontSize+2,mFontSize+2));
  cb->setFontSize(mFontSize+2);
  cb->setChecked(value);

  return cb;
}

TextBox* FormScreen::initCharVariableWidget(char value,const std::string &label,bool editable)
{
  addVariablePreWork(label);

  TextBox* tb = new TextBox(mCurrentWindowState.widget);
  tb->setEditable(editable);
  tb->setFixedSize(mInputCellSize);
  tb->setValue(std::string(1,value));
  tb->setAlignment(TextBox::ALIGNMENT_LEFT);
  tb->setDefaultValue("");
  tb->setFontSize(mFontSize);
  tb->setFormat(".?(\\0)?");

  return tb;
}

TextBox* FormScreen::initIntVariableWidget(int value,const std::string &label,bool editable)
{
  addVariablePreWork(label);

  TextBox* tb = new TextBox(mCurrentWindowState.widget);
  tb->setEditable(editable);
  tb->setFixedSize(mInputCellSize);
  tb->setValue(std::to_string(value));
  tb->setAlignment(TextBox::ALIGNMENT_LEFT);
  tb->setDefaultValue("0");
  tb->setFontSize(mFontSize);
  tb->setFormat("[-]?[0-9]*");

  return tb;
}

TextBox* FormScreen::initUintVariableWidget(unsigned int value,const std::string &label,bool editable)
{
  addVariablePreWork(label);

  TextBox* tb = new TextBox(mCurrentWindowState.widget);
  tb->setEditable(editable);
  tb->setFixedSize(mInputCellSize);
  tb->setValue(std::to_string(value));
  tb->setAlignment(TextBox::ALIGNMENT_LEFT);
  tb->setDefaultValue("0");
  tb->setFontSize(mFontSize);
  tb->setFormat("[0-9]*");

  return tb;
}

TextBox* FormScreen::initFloatVariableWidget(float value,const std::string &label,bool editable)
{
  addVariablePreWork(label);

  TextBox* tb = new TextBox(mCurrentWindowState.widget);
  tb->setEditable(editable);
  tb->setFixedSize(mInputCellSize);
  tb->setValue(floatToString(value));
  tb->setAlignment(TextBox::ALIGNMENT_LEFT);
  tb->setDefaultValue("0");
  tb->setFontSize(mFontSize);
  //tb->setFormat("[-]?[0-9]*\\.?[0-9]+");
  tb->setFormat("[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?");

  return tb;
}

TextBox* FormScreen::initDoubleVariableWidget(double value,const std::string &label,bool editable)
{
  addVariablePreWork(label);

  TextBox* tb = new TextBox(mCurrentWindowState.widget);
  tb->setEditable(editable);
  tb->setFixedSize(mInputCellSize);
  tb->setValue(doubleToString(value));
  tb->setAlignment(TextBox::ALIGNMENT_LEFT);
  tb->setDefaultValue("0");
  tb->setFontSize(mFontSize);
  //tb->setFormat("[-]?[0-9]*\\.?[0-9]+");
  tb->setFormat("[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?");

  return tb;
}

TextBox* FormScreen::initStringVariableWidget(const std::string& value,const std::string &label,bool editable)
{
  addVariablePreWork(label);

  TextBox* tb = new TextBox(mCurrentWindowState.widget);
  tb->setEditable(editable);
  tb->setFixedSize(mInputCellSize);
  tb->setValue(value);
  tb->setAlignment(TextBox::ALIGNMENT_LEFT);
  tb->setDefaultValue("");
  tb->setFontSize(mFontSize);
  tb->setFormat(".*");

  return tb;
}

nanogui::PopupButton* FormScreen::initColorPickerWidget(const Eigen::Vector3f& value,const std::string &label,bool enabled)
{
  addVariablePreWork(label);

  PopupButton* pb = new PopupButton(mCurrentWindowState.widget,"",0);

  pb->setBackgroundColor(Color(value));
  pb->setFontSize(mFontSize);
  pb->setFixedSize(mInputCellSize);
  pb->setEnabled(enabled);
  
  Popup* popup = pb->popup();
  popup->setLayout(new GroupLayout());

  ColorWheel* colorwheel = new ColorWheel(popup);
  colorwheel->setColor(pb->backgroundColor().block<3,1>(0,0));

  Button* cbReset = new Button(popup,"Reset");
  cbReset->setFixedSize(Vector2i(100,25));
  cbReset->setBackgroundColor(Color(colorwheel->color()));

  Button* cbPick = new Button(popup,"Pick");
  cbPick->setFixedSize(Vector2i(100,25));
  cbPick->setBackgroundColor(Color(colorwheel->color()));
  
  ColorWheel& cwr = *colorwheel;
  Button& cbPickRef = *cbPick;
  Button& cbResetRef = *cbReset;
  PopupButton& pbr = *pb;
  
  // live update
  colorwheel->setCallback([&pbr,&cwr,&cbPickRef](const Vector3f &value) {
    cbPickRef.setBackgroundColor(cwr.color());
    pbr.setBackgroundColor(cwr.color());
    pbr.callback()();
  });

  // pick
  cbPick->setChangeCallback([&pbr,&cbPickRef](bool pushed) {
    if(pushed) {
      pbr.setBackgroundColor(cbPickRef.backgroundColor());
      pbr.callback()();
      pbr.setPushed(false);
    }
  });

  // reset
  cbReset->setChangeCallback([&pbr,&cbResetRef](bool pushed) {
    if(pushed) {
      pbr.setBackgroundColor(cbResetRef.backgroundColor());
      pbr.callback()();
      pbr.setPushed(false);
    }
  });

  return pb;
}

nanogui::ComboBox* FormScreen::initEnumWidget(int value,const std::string &label,const std::vector<std::string>& list,bool enabled)
{
  addVariablePreWork(label);

  ComboBox* cb = new ComboBox(mCurrentWindowState.widget,list);

  cb->setFontSize(mFontSize);
  cb->setFixedSize(mInputCellSize);
  cb->setHideOverlay(true);

  return cb;
}

void FormScreen::addVariablePreWork(const std::string& label)
{
  if(mCurrentWindowState.mode != Mode::DoubleCell)
  {
    Widget* gridPanel = new Widget(mCurrentWindow);

    GridLayout* layout;
    if(mCurrentWindowState.layout == Layout::Vertical)
    {
      layout = new GridLayout(GridLayout::Orientation::Horizontal,2,mMargin,mSpacing);
      layout->setColAlignment({GridLayout::Alignment::Minimum,GridLayout::Alignment::Maximum});
    }
    else
      layout = new GridLayout(GridLayout::Orientation::Vertical,2,mMargin,mSpacing);
    
    gridPanel->setLayout(layout);
    mCurrentWindowState.widget = gridPanel;
  }

  mCurrentWindowState.mode = Mode::DoubleCell;

  new Label(mCurrentWindowState.widget,label,"sans-bold");
}

std::string FormScreen::floatToString(float value)
{
  char buffer[20];
  sprintf(buffer,"%.4g",value);
  return string(buffer);
}

std::string FormScreen::doubleToString(double value)
{
  char buffer[30];
  sprintf(buffer,"%.7g",value);
  return string(buffer);
}