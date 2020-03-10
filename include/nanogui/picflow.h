/*
    nanogui/picflow.h -- Simple picture flow widget which display
    images in row

    NanoGUI was redeigned by dalerank <dalerankn8@gmail.com>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/


#pragma once

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

class NANOGUI_EXPORT PickflowItem
{
public:
    Vector4i mRectangle, mMirrorRect;
    Vector4f mCurrent;
    std::string name;
    Color fontColor;
    std::string font;
    int blend;
    void* object;

    PickflowItem();

    ~PickflowItem();

    void setTextureId( const std::string& pathTo );
    void setTextureId( const int ptx );

    int textureId() const;
    int mirrorTextureId();
    void updateTextures( NVGcontext* ctx );

    const std::string& text() const { return mText; }
    const std::string& tetrurePath() const { return mPath; }
    void setText( const std::string& text ) { mText = text; }

private:
    int mTexture;
    int mDownTexture;
    std::string mPath; 
    std::string mText;
};

class NANOGUI_EXPORT Picflow : public Widget
{
    Picflow();
public:
  enum Mode { Horizon=0, Projectile };
    ~Picflow();
    Picflow(Widget* parent, const Vector2f& picrect);

    uint32_t addItem(int texture, const std::string& text = "", void* object = nullptr);

    void setMode(Mode mode) { mMode = mode; mNeedUpdateImages = true; }
    void setItemTexture( uint32_t index, int texture );
    void setItemBlend( uint32_t index, int blend );
    void setItemFont( uint32_t index, const std::string& font );

    //! set all item colors at given index to color
    virtual void setItemOverrideColor(uint32_t index, const Color &color);

    void removeItem(uint32_t index);

    void draw( NVGcontext* ctx ) override;
    void next( int offset );
    void prev( int offset );

    void performLayout(NVGcontext* ctx) override;

    bool keyboardEvent(int key, int /* scancode */, int action, int modifiers) override;
    bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;

    void clear();
    int getSelected() const { return mActiveIndex; }
    void* getObject( uint32_t index );
    void setSelected(const char *item);
    void setSelected(int index);
    void setDrawBackground(bool draw) { mDrawBackground = draw; }
    void setPictureRect( const Vector2f& rectangle ) { mPictureRect = rectangle; }

    size_t itemCount() const { return mImages.size(); }
    const char* listItem(uint32_t id) const;

    //! return element type name for gui factory
    std::string wtypename() const override;

protected:

    virtual int getItemAt(int xpos, int ypos) const;
 
    void _drawpair( NVGcontext* ctx, 
                         const PickflowItem& pDesk, 
                         float transparent=1.f, 
                         bool drawMirror=true );

    void _updaterects(NVGcontext* ctx);
    void _updatepos();
    Vector4i _correctRect( NVGcontext* ctx, int texture, const Vector4i& rectangle );
    void _drawimg( NVGcontext* painter, int txs, 
                   const Vector4f& rectabgle, Color* colors );
    Vector4i _getDownRect( const Vector4f& rectangle );
    void _updatetxs( NVGcontext* painter );
    int mActiveIndex;
    int mLastTimeKey;
    std::vector<PickflowItem> mImages;

    bool mDrawBackground;
    Vector2f mPictureRect;
    Mode mMode = Mode::Horizon;
    bool mNeedUpdateImages = false;
};

NAMESPACE_END(nanogui)