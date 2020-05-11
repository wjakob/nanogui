#include <nanogui/picflow.h>
#include <nanovg.h>
#include <nanogui/keyboard.h>
#include <nanogui/screen.h>

#define DEFAULT_SMOOTH 0.6f

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(Picflow, Widget)

void PickflowItem::setTextureId( int ptx )
{
    mTexture = ptx;
}

void PickflowItem::setTextureId( const std::string& pathTo )
{
    mPath = pathTo;
    //setTexture( Texture() );
}

void PickflowItem::updateTextures( NVGcontext* /*ctx*/ )
{
    mDownTexture = mTexture; 
}

Picflow::Picflow( Widget* parent)
	: Widget(parent)
{
	mActiveIndex = 0;
  mPictureRect = { 0.25f, 0.25f } ;
	mDrawBackground = true;
}

uint32_t Picflow::addItem( int texture, const std::string& text, void* object )
{
  mImages.push_back({});
  auto& descr = mImages.back();
	descr.setTextureId( texture ); 
	descr.setText( text );

	descr.mRectangle = Vector4i( 0, 0, 0, 0 );

  descr.object = object;

  mNeedUpdateImages = true;

	return mImages.size() - 1;
}

void Picflow::fillImages(std::function<void(Widget*)> f)
{
  if (f)
    f(this);
}

Vector4i Picflow::_correctRect( NVGcontext* ctx, int texture, const Vector4i& rectangle )
{
	if (texture > 0)
	{
    int32_t w, h;
    nvgImageSize( ctx, texture, &w, &h);
    Vector2i txsSize = { w, h };

    float koeff = (float)rectangle.width() / txsSize.x();
    Vector2i imSize = ( txsSize * koeff * 0.5f ).cast<int>();

    Vector2i rc = rectangle.center();
		Vector4i realRect( rc.x() - imSize.x(), rc.y() - imSize.y(),
					             rc.x() + imSize.x(), rc.y() + imSize.y() );

		return realRect - Vector2i( 0, int( height() * 0.1f ) );
	}
	else
		return rectangle - Vector2i( 0, int( height() * 0.1f ) );
}

Vector4i Picflow::_getDownRect( const Vector4f& rectangle )
{
  Vector4i ret( rectangle.x(), rectangle.lowerright().y(),
                rectangle.z(), rectangle.y() * DEFAULT_SMOOTH);
  ret += Vector2i(0, 10);
  return ret;
}

void Picflow::performLayout(NVGcontext* ctx)
{
  Widget::performLayout(ctx);

  _updaterects(ctx);
}

void Picflow::_updaterects( NVGcontext* ctx)
{
  if( mImages.size() == 0 )
    return;

  Vector2i center = rect().center();
  int wh = mPictureRect.x() * height();

  Vector4i tmpRect(center - Vector2i(wh), center + Vector2i(wh));

  if (mActiveIndex < (int)mImages.size())
  {
    mImages[mActiveIndex].mRectangle = _correctRect(ctx, mImages[mActiveIndex].textureId(), tmpRect);
  }

  if (mMode == Mode::Horizon)
  {
    int offsetx = 0;
    int offsetWidth = width() / 4;
    Vector4i lRect = tmpRect;
    if (mActiveIndex - 1 >= 0)
    {
      for (int k = std::max<int>(0, mActiveIndex - 1); k >= 0; k--)
      {
        offsetx += static_cast<int>(offsetWidth * (0.7f - (mActiveIndex - k)*0.1f));
        Vector2i sides = Vector2i(lRect.width(), lRect.height()) * 0.45f;
        lRect = Vector4i(center - sides, center + sides);

        mImages[k].mRectangle = _correctRect(ctx, mImages[k].textureId(), lRect) - Vector2i(offsetx, 0);
      }
    }

    offsetx = 0;
    Vector4i rRect = tmpRect;
    for (uint32_t k = std::min<int>(mActiveIndex + 1, mImages.size()); k < mImages.size(); k++)
    {
      offsetx += static_cast<int>(offsetWidth * (0.7f - (k - mActiveIndex)*0.1f));
      Vector2i sides = Vector2i(rRect.width(), rRect.height()) * 0.45f;
      rRect = Vector4i(center - sides, center + sides);

      mImages[k].mRectangle = _correctRect(ctx, mImages[k].textureId(), rRect) + Vector2i(offsetx, 0);
    }
  }
  else if (mMode == Mode::Projectile)
  {

  }
}

void Picflow::_drawimg( NVGcontext* ctx, int txs, const Vector4f& r, Color* colors )
{
  if (txs)
  {
    NVGpaint imgPaint = nvgImagePattern(ctx, r.x(), r.y(), r.width(), r.height(), 0, txs, 1.0);
    nvgSave(ctx);
    //nvgTransform(ctx, 0, 0, 0, sin(nvgDegToRad(-90)), 0, cos(nvgDegToRad(50)));
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, r.x(), r.y(), r.width(), r.height(), 5);
    nvgFillPaint(ctx, imgPaint);
    nvgFill(ctx);
    nvgRestore(ctx);
  }
  else
  {
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x() + r.x(), mPos.y() + r.y(), 
                        r.width(), r.height(), 
                        mTheme->mButtonCornerRadius - 1);

    NVGpaint bg = nvgLinearGradient(ctx, mPos.x() + r.x(), mPos.y() + r.y(),
                                         mPos.x() + r.width(), mPos.y() + r.height(), 
                                         Color(0xff0000ff), Color(0x00ff00ff));

    nvgFillPaint(ctx, bg);
    nvgFill(ctx);
  }
}

void Picflow::_drawpair( NVGcontext* ctx, const PickflowItem& pDesk, float transparent, bool drawMirror )
{
  Color clr(0xffffffff);
  Color colorsA[] = { clr, clr, clr, clr};
	_drawimg( ctx, pDesk.textureId(), pDesk.mCurrent, colorsA );

  /*if( drawMirror )
  {
	  colorsA[ 1 ] = colorsA[ 2 ] = 0;
      blend = ( getOpacity() + 0xC0 ) / 2;
      clr = getResultColor();
      clr.setAlpha( uint32_t( transparent * blend * getResultOpacity() / 255.f ) );
      colorsA[ 3 ] = colorsA[ 0 ] = clr;
	  _drawimg( painter, pDesk->GetMirrorTexture(), pDesk->mDownRect, colorsA );
  }*/

  /*Font font = pDesk->font.available() ? pDesk->font : getFont();
	if( font.available() )
	{
		RectI clipRect = getAbsoluteRect();
        Color tmpColor = pDesk->fontColor;
        tmpColor.setAlpha( uint32_t( tmpColor.getAlpha() * getOpacity() / 0xff ) );
		font.Draw( pDesk->GetText(), rectangle, getResultColor( tmpColor ), true, true, &clipRect );
	}*/
}

void Picflow::draw( NVGcontext* ctx )
{
	if( !visible() )
		return;

  _updatetxs( ctx );
	_updatepos();

  if (mDrawBackground)
  {
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y(),
                        mTheme->mButtonCornerRadius - 1);

    NVGpaint bg = nvgLinearGradient(ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y(),
                                    Color(0x00000ff), Color(0x00000ff));

    nvgFillPaint(ctx, bg);
    nvgFill(ctx);
  }

	if( mImages.size() > 0 )
	{
    for( int pos=std::max<int>( 0, mActiveIndex-6 ); pos < mActiveIndex; pos++ )
    {
			_drawpair( ctx, mImages[ pos ], ( 1 - 0.1f * pos ) );
    }

    for( int pos=std::min<int>( mActiveIndex + 6, mImages.size()-1); pos > mActiveIndex; pos-- )
    {
      _drawpair( ctx, mImages[ pos ], ( 1 - 0.1f * pos ) );
    }

		if( mActiveIndex < static_cast< int >( mImages.size() ) )
			_drawpair( ctx, mImages[ mActiveIndex ] );
	}

	Widget::draw(ctx);
}

void Picflow::_updatetxs( NVGcontext* ctx )
{
  if (mNeedUpdateImages)
  {
    _updaterects(ctx);
    mNeedUpdateImages = false;
  }
}

void Picflow::_updatepos()
{
	for(auto& img: mImages)
	{
    float fps = 20;
		if(!math::isEqual<float>(img.mRectangle.z(), img.mCurrent.z(), 0.1f) )
		{
			float offset = (img.mRectangle.z() - img.mCurrent.z()) / fps;
      img.mCurrent.z() += offset;
		}

		if(!math::isEqual<float>(img.mRectangle.w(), img.mCurrent.w(), 0.1f) )
		{
			float offset = (img.mRectangle.w() - img.mCurrent.w()) / fps;
      img.mCurrent.w() += offset;
		}

		if(!math::isEqual<float>(img.mRectangle.x(), img.mCurrent.x(), 0.1f) )
		{
			float offset = (img.mRectangle.x() - img.mCurrent.x()) / fps;
      img.mCurrent.x() += offset;
		}

		if(!math::isEqual<float>(img.mRectangle.y(), img.mCurrent.y(), 0.1f) )
		{
			float offset = (img.mRectangle.y() - img.mCurrent.y()) / fps;

      img.mCurrent.y() += offset;
		}

    img.mMirrorRect = _getDownRect(img.mCurrent );
	}
}

void Picflow::prev( int offset )
{
	mActiveIndex -= offset;
  mActiveIndex = std::max<int>( mActiveIndex, 0 );

  mNeedUpdateImages = true;
}

void Picflow::next( int offset )
{
	mActiveIndex += offset;
  mActiveIndex = std::min<size_t>( mActiveIndex, mImages.size()-1 );

  mNeedUpdateImages = true;
}

bool Picflow::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers)
{
  if (down && isMouseButtonLeft(button))
  {
    int side = width() / 5;
    Vector2i center = rect().center();
    int wh = mPictureRect.x() * height();

    if (p.x() > center.x() + wh )
    {
      int offset = (p.x() - center.x()) / side;
      next(offset);
      return true;
    }

    if (p.x() < center.x() - wh )
    {
      int offset = (center.x() - p.x()) / side;
      prev(offset);
      return true;
    }

    Vector4i centralRect;

    centralRect.x() = center.x() - wh;
    centralRect.y() = center.y() - wh;
    centralRect.z() = center.x() + wh;
    centralRect.w() = center.y() + wh;

    if (centralRect.isPointInside(p))
    {
      //_SendEventSelected(event);
      return true;
    }

    return true;
  }
  return false;
}

bool Picflow::keyboardEvent(int key, int scancode, int action, int modifiers)
{
  if (!focused())
    return false;

  int curTime = getTimeFromStart();
	//uncheck for double event of keyInput
	if (isKeyboardKey(key, kbkey::right)&& isKeyboardActionPress(action) && (curTime - mLastTimeKey > 20))
	{
		next(1);
		mLastTimeKey = getTimeFromStart();

		return true;
	}

	if (isKeyboardKey(key, kbkey::left) && isKeyboardActionPress(action) && (curTime - mLastTimeKey > 20))
	{
		prev(1);
		mLastTimeKey = getTimeFromStart();
		return true;
	}
  
	if (isKeyboardKey(key, kbkey::space) && isKeyboardActionPress(action) && (curTime - mLastTimeKey > 20))
	{
		mLastTimeKey = getTimeFromStart();
		return true;
	}

	return Widget::keyboardEvent(key, scancode, action, modifiers);
}

Picflow::~Picflow() {}

const char* Picflow::listItem( uint32_t id ) const { return mImages[ id ].name.c_str(); }
void Picflow::setSelected( const char * /*item*/ ) {}

void Picflow::setSelected( int index )
{
	if( index < (int)mImages.size() && index >= 0 )
		mActiveIndex = index;
}

void Picflow::removeItem( uint32_t index )
{
	if( index < mImages.size() )
    mImages.erase(mImages.begin() + index);
}

void Picflow::clear()
{
	mImages.clear();
	mActiveIndex = 0;
}

void* Picflow::getObject( uint32_t index )
{
	return index < mImages.size() ? mImages[ index ].object : nullptr;
}

void Picflow::setItemTexture( uint32_t index, int texture )
{
	assert( index < mImages.size() );
	if( index < mImages.size() )
		mImages[ index ].setTextureId( texture );
}

void Picflow::setItemBlend( uint32_t index, int blend )
{
	assert( index < mImages.size() );
	if( index < mImages.size() )
		mImages[ index ].blend = blend;
}

int Picflow::getItemAt( int /*xpos*/, int /*ypos*/ ) const
{
  assert( false && "func not supported" );
  return -1;
}

void Picflow::setItemFont( uint32_t index, const std::string& font )
{
  assert( index < mImages.size() );
  if( index < mImages.size() )
    mImages[ index ].font = font;
}

void Picflow::setItemOverrideColor( uint32_t index, const Color& color )
{
  assert( index < mImages.size() );
  if( index < mImages.size() )
    mImages[ index ].fontColor = color;
}

std::string Picflow::wtypename() const { return "picflow"; }

PickflowItem::~PickflowItem() {}

PickflowItem::PickflowItem() : mPath( "" )
{
  mTexture = 0;
  mDownTexture = 0;
  blend = 0xff;
  font = "sans";
  fontColor = Color( 0xff, 0, 0, 0 );
}

int PickflowItem::textureId() const { return mTexture; }
int PickflowItem::mirrorTextureId() { return mDownTexture; }

NAMESPACE_END(nanogui)
