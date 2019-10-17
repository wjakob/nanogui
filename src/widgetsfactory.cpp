#include <nanogui/widgetsfactory.h>
#include <nanogui/textbox.h>

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

void WidgetFactory::registerFactory(WidgetFactory* factoryToAdd)
{
  if (factoryToAdd)
    _factories_.push_back( factoryToAdd );
}

WidgetFactory::WidgetFactory()
{
  registerCreator("string_property", [](Widget* parent) { return new TextBox(parent); });
    //registerFactory( new WindowsFactory() );
    //registerFactory( new ButtonsFactory() );
    //registerFactory( new EditsFactory() );
    //registerFactory( new SwitchersFactory() );
    //registerFactory( new ViewersFactory() );
    //registerFactory( new CompoziteWidgetsFactory() );
    //registerFactory( new ListboxesFactory() );
    //registerFactory( new LayoutsFactory() );
    //registerFactory( new ScrollbarsFactory() );
    //registerFactory( new MenusFactory() );
    //registerFactory( new AnimatorsFactory() );
}

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

size_t WidgetFactory::factoriesCount() const
{
  return _factories_.size();
}

WidgetFactory* WidgetFactory::getFactory(size_t index ) const
{
  if( index < _factories_.size() )
    return _factories_[ index ];
  else
    return nullptr;
}

WidgetFactory& WidgetFactory::instance()
{
  if (!_mainf_)
    _mainf_ = new WidgetFactory();
  return *_mainf_;
}

NAMESPACE_END(nanogui)