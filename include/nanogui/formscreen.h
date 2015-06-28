#if !defined(__SCREENFORM_H)
#define __SCREENFORM_H

#include <map>
#include <list>

#include <nanogui/screen.h>

NANOGUI_NAMESPACE_BEGIN

/**
* \brief A form layouter class which takes care of exposing variables as widgets based on their types.
*\

/**
* Example:
*
* FormScreen* form = new FormScreen();
*
* // add a new windows widget
* bar->addNewWindow(Eigen::Vector2i(10,10),"Menu");
* 
* // start a new group
* bar->addNewGroup("Group 1");
* 
* // expose an integer variable by reference
* bar->addVariable(aInt,"integer variable");
* 
* // expose a float variable by get/set functions
* bar->addVariable([&](float value){ aFloat = value; },[&](){ return *aFloat; },"float variable",1.0f);
* 
* // add a new button
* bar->addButton("Button",[&](){ std::cout << "Button pressed\n"; });
*/

class FormScreen: public Screen {

public:

  enum class Layout { Horizontal, Vertical };

  // Default constructor
  FormScreen(float refreshTime = 0.05);

  /// Release all resources
  virtual ~FormScreen() {}

  void init(GLFWwindow *window) { this->initScreen(window); }
  void draw();
  void layout() { this->performLayout(mNVGContext); }

  /// Add widgets to the gui
  Window* addNewWindow(const Eigen::Vector2i& position, const std::string &title = "Untitled");
  Label* addNewGroup(const std::string &caption,Layout layout=Layout::Vertical);
  void setCurrentWindow(Window* window);
  void setCurrentLayout(Layout layout);

  Button* addButton(const std::string &caption,std::function<void()> callback);
  
  CheckBox* addVariable(bool& value,const std::string &label,bool enabled=true);
  TextBox* addVariable(char& value,const std::string &label,bool editable=true);
  TextBox* addVariable(int& value,const std::string &label,bool editable=true);
  TextBox* addVariable(unsigned int& value,const std::string &label,bool editable=true);
  TextBox* addVariable(float& value,const std::string &label,bool editable=true);
  TextBox* addVariable(double& value,const std::string &label,bool editable=true);
  TextBox* addVariable(std::string& value,const std::string &label,bool editable=true);
  PopupButton* addColorPicker(Eigen::Vector3f& color,const std::string &label,bool enabled=true);

  CheckBox* addVariable(std::function<void(bool)> setCallback,std::function<bool()> getCallback,const std::string &label,bool initValue,bool enabled=true);
  TextBox* addVariable(std::function<void(char)> setCallback,std::function<char()> getCallback,const std::string &label,char initValue,bool editable=true);
  TextBox* addVariable(std::function<void(int)> setCallback,std::function<int()> getCallback,const std::string &label,int initValue,bool editable=true);
  TextBox* addVariable(std::function<void(unsigned int)> setCallback,std::function<unsigned int()> getCallback,const std::string &label,unsigned int initValue,bool editable=true);
  TextBox* addVariable(std::function<void(float)> setCallback,std::function<float()> getCallback,const std::string &label,float initValue,bool editable=true);
  TextBox* addVariable(std::function<void(double)> setCallback,std::function<double()> getCallback,const std::string &label,double initValue,bool editable=true);
  TextBox* addVariable(std::function<void(const std::string&)> setCallback,std::function<const std::string&()> getCallback,const std::string &label,const std::string& initValue,bool editable=true);
  PopupButton* addColorPicker(std::function<void(const Eigen::Vector3f&)> setCallback,std::function<const Eigen::Vector3f&()> getCallback,const std::string &label,const Eigen::Vector3f& initValue,bool enabled=true);

  template <typename T>
  inline typename std::enable_if<std::is_enum<T>::value,ComboBox*>::type addEnumeration(T& value,const std::string &label,std::vector<std::pair<T,std::string>> list,bool enabled=true)
  { 
    std::vector<int> ids;
    std::vector<std::string> names;
    for(auto v: list)
    {
      ids.push_back(static_cast<int>(v.first));
      names.push_back(v.second);
    }
    return this->addEnumerationHelper((int&)value,label,ids,names,enabled);
  }

  template <typename T>
  inline typename std::enable_if<std::is_enum<T>::value,ComboBox*>::type addEnumeration(std::function<void(int)> setCallback,std::function<int()> getCallback,const std::string &label,std::vector<std::pair<T,std::string>> list,T initValue,bool enabled=true)
  {
    std::vector<int> ids;
    std::vector<std::string> names;
    for(auto v: list)
    {
      ids.push_back(static_cast<int>(v.first));
      names.push_back(v.second);
    }
    return this->addEnumerationHelper(setCallback,getCallback,label,ids,names,static_cast<int>(initValue),enabled);
  }

  /// set/get refresh time of nano gui (polling of state of mapped variables)
  void setRefreshTime(float time) { mRefreshTime = time; }
  float refreshTime() const { return mRefreshTime; }

  /// set/get the cell size of the input widgets
  void setInputCellSize(const Eigen::Vector2i& size) { mInputCellSize = size; }
  const Eigen::Vector2i& inputCellSize() const { return mInputCellSize; }

  /// set/get the standard font size of the labels
  void setFontSize(int fontSize) { mFontSize = fontSize; }
  int fontSize() const { return mFontSize; }

  /// set/get the margin
  void setMargin(int margin) { mMargin = margin; }
  int margin() const { return mMargin; }

  /// set/get the spacing
  void setSpacing(int spacing) { mSpacing = spacing; }
  int spacing() const { return mSpacing; }

  /// gui events
  bool cursorPosEvent(GLFWwindow *w,double x,double y) { return this->cursorPosCallbackEvent(w,x,y); }
  bool mouseButtonEvent(GLFWwindow *w,int button,int action,int modifiers) { return this->mouseButtonCallbackEvent(w,button,action,modifiers); }
  bool keyEvent(GLFWwindow *w,int key,int scancode,int action,int mods) { return this->keyCallbackEvent(w,key,scancode,action,mods); }
  bool charEvent(GLFWwindow* w,unsigned int codepoint) { return this->charCallbackEvent(w,codepoint); }
  bool dropEvent(GLFWwindow *w,int count,const char **filenames) { return this->dropCallbackEvent(w,count,filenames); }
  bool scrollEvent(GLFWwindow *w,double x,double y) { return this->scrollCallbackEvent(w,x,y); }

protected:

  enum class Mode { None,SingleCell,DoubleCell };

  struct WindowState
  {
    Layout layout;
    Mode mode;
    Widget* widget;
  };

  std::map<Window*,WindowState> windowStates;
  std::list<std::function<void()>> syncJobs;

  std::list<std::function<bool()>> boolVariableGetCallbacks;
  std::list<std::function<void(char)>> charVariableSetCallbacks;
  std::list<std::function<char()>> charVariableGetCallbacks;
  std::list<std::function<void(int)>> intVariableSetCallbacks;
  std::list<std::function<int()>> intVariableGetCallbacks;
  std::list<std::function<void(unsigned int)>> uintVariableSetCallbacks;
  std::list<std::function<unsigned int()>> uintVariableGetCallbacks;
  std::list<std::function<void(float)>> floatVariableSetCallbacks;
  std::list<std::function<float()>> floatVariableGetCallbacks;
  std::list<std::function<void(double)>> doubleVariableSetCallbacks;
  std::list<std::function<double()>> doubleVariableGetCallbacks;
  std::list<std::function<void(const std::string&)>> stringVariableSetCallbacks;
  std::list<std::function<const std::string&()>> stringVariableGetCallbacks;
  std::list<std::function<void(int)>> enumSetCallbacks;
  std::list<std::function<int()>> enumGetCallbacks;
  std::list<std::function<void(const Eigen::Vector3f&)>> colorPickerSetCallbacks;
  std::list<std::function<const Eigen::Vector3f&()>> colorPickerGetCallbacks;

  std::list<std::vector<int>> enumIdsList;
  std::list<std::vector<std::string>> enumNamesList;

  Window* mCurrentWindow;
  WindowState mCurrentWindowState;
  float mRefreshTime;
  float mLastRefresh;
  Eigen::Vector2i mInputCellSize;
  int mFontSize;
  int mMargin;
  int mSpacing;

  CheckBox* initBoolVariableWidget(bool value,const std::string &label,bool enabled);
  TextBox* initCharVariableWidget(char value,const std::string &label,bool editable);
  TextBox* initIntVariableWidget(int value,const std::string &label,bool editable);
  TextBox* initUintVariableWidget(unsigned int value,const std::string &label,bool editable);
  TextBox* initFloatVariableWidget(float value,const std::string &label,bool editable);
  TextBox* initDoubleVariableWidget(double value,const std::string &label,bool editable);
  TextBox* initStringVariableWidget(const std::string& value,const std::string &label,bool editable);
  PopupButton* initColorPickerWidget(const Eigen::Vector3f& value,const std::string &label,bool enabled);
  ComboBox* initEnumWidget(int value,const std::string &label,const std::vector<std::string>& list,bool enabled);

  void addVariablePreWork(const std::string& label);

  ComboBox* addEnumerationHelper(int& value,const std::string &label,const std::vector<int>& enumIds,const std::vector<std::string>& names,bool enabled);
  ComboBox* addEnumerationHelper(std::function<void(int)> setCallback,std::function<int()> getCallback,const std::string &label,const std::vector<int>& enumIds,const std::vector<std::string>& names,int initValue,bool enabled);

  std::string floatToString(float value);
  std::string doubleToString(double value);
};

NANOGUI_NAMESPACE_END

#endif /* __SCREENFORM_H */
