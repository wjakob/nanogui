#include <nanogui/widgetsfactory.h>
#include <nanogui/button.h>
#include <nanogui/toolbutton.h>
#include <nanogui/textbox.h>
#include <nanogui/window.h>
#include <nanogui/switchbox.h>
#include <nanogui/entypo.h>

NAMESPACE_BEGIN(nanogui)

namespace {
  static WidgetFactory* _mainf_ = nullptr;
  std::vector<WidgetFactory*> _factories_;
  struct CreatorInfo {
    std::string name;
    WidgetFactory::creatorFunc creator;
  };
  std::vector<CreatorInfo> _creators_;
}

struct DummyFactory : public WidgetFactory
{
  std::string _name;
  std::vector<CreatorInfo> _creators;

  DummyFactory(std::string name) : _name(name) {}

  DummyFactory& func(std::string wtype, creatorFunc f) 
  {
    auto it = std::find_if(_creators.begin(), _creators.end(), [&wtype](const CreatorInfo& i) { return i.name == wtype; });
    if (it == _creators.end())
      _creators.push_back({ wtype, f });
    return *this;
  }

  std::vector<WidgetFactory*> factories() const { return{}; }

  std::vector<std::string> types() const override
  {
    std::vector<std::string> result;
    for (auto& c: _creators)
      result.emplace_back(c.name);
    return result;
  }
  WidgetFactory* factory(size_t index) const override { return nullptr; }
  std::string name() const override { return _name; }
  Widget* createWidget(const std::string& wtype, Widget* parent) override
  {
    auto it = std::find_if(_creators.begin(), _creators.end(), [&wtype](const CreatorInfo& i) { return i.name == wtype; });
    if (it != _creators.end())
      return it->creator(parent);
  }
};

void WidgetFactory::registerFactory(WidgetFactory* factoryToAdd)
{
  if (factoryToAdd)
    _factories_.push_back( factoryToAdd );
}

WidgetFactory::WidgetFactory() {}

void WidgetFactory::registerCreator(std::string wtype, creatorFunc creator)
{
  auto it = std::find_if(_creators_.begin(), _creators_.end(), [wtype](CreatorInfo& c) { return c.name == wtype; });
  if (it == _creators_.end())
    _creators_.push_back({ wtype, creator});
}

WidgetFactory::~WidgetFactory() {}

Widget* WidgetFactory::createWidget( const std::string& wtypename, Widget* parent )
{
    Widget* w = nullptr;

    for (auto& f: _factories_)
    {
      w = f->createWidget(wtypename, parent);
      if (w)
        return w;
    }

    return nullptr;
}

WidgetFactory* WidgetFactory::factory(size_t index ) const
{
  if ( index < _factories_.size() )
    return _factories_[ index ];
  else
    return nullptr;
}

std::vector<WidgetFactory*> WidgetFactory::factories() const
{
  return _factories_;
}

WidgetFactory& WidgetFactory::instance()
{
  if (!_mainf_)
  {
    _mainf_ = new WidgetFactory();
    _mainf_->registerCreator("string_property", [](Widget* parent) { return new TextBox(parent); });
    auto buttons = new DummyFactory("buttons");
    buttons->func("button", [](Widget* w) { return w->add<Button>(); })
      .func("toolbutton", [](Widget* w) { return w->add<ToolButton>(ENTYPO_ICON_ROCKET); })
      .func("togglebutton", [](Widget* w) { return w->add<ToggleButton>(ENTYPO_ICON_ROCKET); });
    _mainf_->registerFactory(buttons);

    auto windows = new DummyFactory("windows");
    windows->func("window", [](Widget* w) { return w->add<Window>(Caption{ "Untitled" }); });
    _mainf_->registerFactory(windows);

    auto edits = new DummyFactory("edits");
    edits->func("textbox", [](Widget* w) { return w->add<TextBox>(); })
      .func("intbox", [](Widget* w) { return w->add<IntBox<int>>(); })
      .func("floatbox", [](Widget* w) { return w->add<FloatBox<float>>(); });
    _mainf_->registerFactory(edits);

    auto switchers = new DummyFactory("switchers");
    switchers->func("checkbox", [](Widget* w) { return w->add<CheckBox>(); })
      .func("switchbox", [](Widget* w) { return w->add<SwitchBox>(); });
    _mainf_->registerFactory(switchers);

    auto layers = new DummyFactory("layers");
    layers->func("hlayer", [](Widget* w) { return &w->hlayer(); })
      .func("vlayer", [](Widget* w) { return &w->vlayer(); });
    _mainf_->registerFactory(layers);
    //registerFactory( new ListboxesFactory() );
    //registerFactory( new LayoutsFactory() );
    //registerFactory( new ScrollbarsFactory() );
    //registerFactory( new MenusFactory() );
    //registerFactory( new AnimatorsFactory() );

  }
  return *_mainf_;
}

NAMESPACE_END(nanogui)