/*
    src/example_icons.cpp -- C++ version of an example application that shows
    all available Entypo icons as they would appear in NanoGUI itself.  For a Python
    implementation, see '../python/example_icons.py'.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

/* Developer note: need to make a change to this file?
 * Please raise an Issue on GitHub describing what needs to change.  This file
 * was generated, so the scripts that generated it need to update as well.
 */

#include <nanogui/nanogui.h>
using namespace nanogui;

class EscapeScreen : public nanogui::Screen {
public:
    EscapeScreen(const Vector2i &size, const std::string &title, bool resizable)
        : nanogui::Screen(size, title, resizable) { }

    virtual ~EscapeScreen() { /* nothing to free */ }

    // allow <ESCAPE> to exit
    virtual bool keyboardEvent(int key, int scancode, int action, int modifiers) override {
        if (key == GLFW_KEY_ESCAPE && modifiers == 0) {
            setVisible(false);
            return true;
        }

        return Screen::keyboardEvent(key, scancode, action, modifiers);
    }
};

// Convenience macro for creating an IconBox. Make sure you put a semicolon after the call to this macro!
#define ADD_ICON(parent, icon, boxWidth) \
    new IconBox(parent, #icon, icon, boxWidth)

class IconBox : public Widget {
public:
    IconBox(Widget *parent, const std::string &name, int icon, int width)
        : Widget(parent) {

        this->setLayout(new BoxLayout(Orientation::Horizontal));

        auto *b = new Button(this, "", icon);
        b->setFixedWidth(40);

        auto *text = new TextBox(this, name);
        text->setDefaultValue(name);
        text->setEditable(true);
        /* Return false essentially makes it not possible to actually edit this text
         * box, but keeping it editable=true allows selection for copy-paste.  If the
         * text box is not editable, then the user cannot highlight it.
         */
        text->setCallback([](const std::string &) { return false; });
        text->setFont("mono-bold");
        text->setFixedWidth(width - 40);
    }
};

int main(int /* argc */, char ** /* argv */) {
    nanogui::init();

    /* scoped variables */ {
        static constexpr int width      = 1000;
        static constexpr int half_width = width / 2;
        static constexpr int height     = 800;

        // create a fixed size screen with one window
        EscapeScreen *screen = new EscapeScreen({width, height}, "NanoGUI Icons", false);
        Window *window = new Window(screen, "");
        window->setPosition({0, 0});
        window->setFixedSize({width, height});

        // attach a vertical scroll panel
        auto vscroll = new VScrollPanel(window);
        vscroll->setFixedSize({width, height});

        // vscroll should only have *ONE* child. this is what `wrapper` is for
        auto wrapper = new Widget(vscroll);
        wrapper->setFixedSize({width, height});
        wrapper->setLayout(new GridLayout());// defaults: 2 columns

        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        ADD_ICON(wrapper, ENTYPO_ICON_500PX, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_500PX_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ADD_TO_LIST, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ADD_USER, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ADDRESS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ADJUST, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_AIR, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_AIRCRAFT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_AIRCRAFT_LANDING, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_AIRCRAFT_TAKE_OFF, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ALIGN_BOTTOM, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ALIGN_HORIZONTAL_MIDDLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ALIGN_LEFT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ALIGN_RIGHT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ALIGN_TOP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ALIGN_VERTICAL_MIDDLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_APP_STORE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ARCHIVE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_AREA_GRAPH, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ARROW_BOLD_DOWN, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ARROW_BOLD_LEFT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ARROW_BOLD_RIGHT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ARROW_BOLD_UP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ARROW_DOWN, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ARROW_LEFT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ARROW_LONG_DOWN, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ARROW_LONG_LEFT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ARROW_LONG_RIGHT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ARROW_LONG_UP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ARROW_RIGHT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ARROW_UP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ARROW_WITH_CIRCLE_DOWN, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ARROW_WITH_CIRCLE_LEFT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ARROW_WITH_CIRCLE_RIGHT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ARROW_WITH_CIRCLE_UP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ATTACHMENT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_AWARENESS_RIBBON, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_BACK, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_BACK_IN_TIME, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_BAIDU, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_BAR_GRAPH, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_BASECAMP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_BATTERY, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_BEAMED_NOTE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_BEHANCE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_BELL, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_BLACKBOARD, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_BLOCK, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_BOOK, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_BOOKMARK, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_BOOKMARKS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_BOWL, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_BOX, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_BRIEFCASE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_BROWSER, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_BRUSH, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_BUCKET, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_BUG, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CAKE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CALCULATOR, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CALENDAR, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CAMERA, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CCW, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CHAT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CHECK, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CHEVRON_DOWN, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CHEVRON_LEFT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CHEVRON_RIGHT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CHEVRON_SMALL_DOWN, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CHEVRON_SMALL_LEFT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CHEVRON_SMALL_RIGHT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CHEVRON_SMALL_UP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CHEVRON_THIN_DOWN, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CHEVRON_THIN_LEFT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CHEVRON_THIN_RIGHT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CHEVRON_THIN_UP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CHEVRON_UP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CHEVRON_WITH_CIRCLE_DOWN, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CHEVRON_WITH_CIRCLE_LEFT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CHEVRON_WITH_CIRCLE_RIGHT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CHEVRON_WITH_CIRCLE_UP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CIRCLE_WITH_CROSS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CIRCLE_WITH_MINUS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CIRCLE_WITH_PLUS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CIRCULAR_GRAPH, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CLAPPERBOARD, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CLASSIC_COMPUTER, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CLIPBOARD, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CLOCK, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CLOUD, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CODE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_COG, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_COLOURS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_COMPASS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CONTROLLER_FAST_BACKWARD, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CONTROLLER_FAST_FORWARD, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CONTROLLER_JUMP_TO_START, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CONTROLLER_NEXT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CONTROLLER_PAUS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CONTROLLER_PLAY, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CONTROLLER_RECORD, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CONTROLLER_STOP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CONTROLLER_VOLUME, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_COPY, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CREATIVE_CLOUD, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CREATIVE_COMMONS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CREATIVE_COMMONS_ATTRIBUTION, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CREATIVE_COMMONS_NODERIVS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CREATIVE_COMMONS_NONCOMMERCIAL_EU, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CREATIVE_COMMONS_NONCOMMERCIAL_US, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CREATIVE_COMMONS_PUBLIC_DOMAIN, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CREATIVE_COMMONS_REMIX, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CREATIVE_COMMONS_SHARE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CREATIVE_COMMONS_SHAREALIKE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CREDIT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CREDIT_CARD, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CROP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CROSS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CUP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CW, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_CYCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_DATABASE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_DIAL_PAD, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_DIRECTION, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_DOCUMENT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_DOCUMENT_LANDSCAPE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_DOCUMENTS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_DOT_SINGLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_DOTS_THREE_HORIZONTAL, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_DOTS_THREE_VERTICAL, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_DOTS_TWO_HORIZONTAL, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_DOTS_TWO_VERTICAL, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_DOWNLOAD, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_DRIBBBLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_DRIBBBLE_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_DRINK, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_DRIVE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_DROP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_DROPBOX, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_EDIT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_EMAIL, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_EMOJI_FLIRT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_EMOJI_HAPPY, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_EMOJI_NEUTRAL, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_EMOJI_SAD, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ERASE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ERASER, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_EVERNOTE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_EXPORT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_EYE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_EYE_WITH_LINE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_FACEBOOK, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_FACEBOOK_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_FEATHER, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_FINGERPRINT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_FLAG, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_FLASH, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_FLASHLIGHT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_FLAT_BRUSH, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_FLATTR, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_FLICKR, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_FLICKR_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_FLOW_BRANCH, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_FLOW_CASCADE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_FLOW_LINE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_FLOW_PARALLEL, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_FLOW_TREE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_FLOWER, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_FOLDER, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_FOLDER_IMAGES, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_FOLDER_MUSIC, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_FOLDER_VIDEO, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_FORWARD, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_FOURSQUARE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_FUNNEL, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_GAME_CONTROLLER, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_GAUGE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_GITHUB, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_GITHUB_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_GLOBE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_GOOGLE_DRIVE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_GOOGLE_HANGOUTS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_GOOGLE_PLAY, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_GOOGLE_PLUS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_GOOGLE_PLUS_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_GRADUATION_CAP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_GRID, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_GROOVESHARK, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_HAIR_CROSS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_HAND, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_HEART, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_HEART_OUTLINED, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_HELP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_HELP_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_HOME, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_HOUR_GLASS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_HOUZZ, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ICLOUD, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_IMAGE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_IMAGE_INVERTED, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_IMAGES, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_INBOX, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_INFINITY, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_INFO, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_INFO_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_INSTAGRAM, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_INSTAGRAM_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_INSTALL, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_KEY, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_KEYBOARD, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LAB_FLASK, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LANDLINE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LANGUAGE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LAPTOP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LASTFM, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LASTFM_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LAYERS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LEAF, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LEVEL_DOWN, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LEVEL_UP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LIFEBUOY, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LIGHT_BULB, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LIGHT_DOWN, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LIGHT_UP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LINE_GRAPH, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LINK, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LINKEDIN, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LINKEDIN_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LIST, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LOCATION, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LOCATION_PIN, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LOCK, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LOCK_OPEN, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LOG_OUT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LOGIN, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_LOOP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_MAGNET, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_MAGNIFYING_GLASS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_MAIL, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_MAIL_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_MAN, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_MAP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_MASK, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_MEDAL, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_MEDIUM, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_MEDIUM_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_MEGAPHONE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_MENU, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_MERGE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_MESSAGE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_MIC, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_MINUS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_MIXI, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_MOBILE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_MODERN_MIC, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_MOON, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_MOUSE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_MOUSE_POINTER, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_MUSIC, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_NETWORK, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_NEW, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_NEW_MESSAGE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_NEWS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_NEWSLETTER, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_NOTE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_NOTIFICATION, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_NOTIFICATIONS_OFF, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_OLD_MOBILE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_OLD_PHONE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ONEDRIVE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_OPEN_BOOK, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_PALETTE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_PAPER_PLANE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_PAYPAL, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_PENCIL, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_PHONE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_PICASA, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_PIE_CHART, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_PIN, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_PINTEREST, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_PINTEREST_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_PLUS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_POPUP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_POWER_PLUG, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_PRICE_RIBBON, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_PRICE_TAG, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_PRINT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_PROGRESS_EMPTY, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_PROGRESS_FULL, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_PROGRESS_ONE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_PROGRESS_TWO, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_PUBLISH, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_QQ, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_QQ_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_QUOTE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_RADIO, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_RAFT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_RAFT_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_RAINBOW, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_RDIO, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_RDIO_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_REMOVE_USER, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_RENREN, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_REPLY, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_REPLY_ALL, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_RESIZE_100_PERCENT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_RESIZE_FULL_SCREEN, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_RETWEET, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ROCKET, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_ROUND_BRUSH, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_RSS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_RULER, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SAVE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SCISSORS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SCRIBD, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SELECT_ARROWS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SHARE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SHARE_ALTERNATIVE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SHAREABLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SHIELD, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SHOP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SHOPPING_BAG, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SHOPPING_BASKET, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SHOPPING_CART, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SHUFFLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SIGNAL, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SINA_WEIBO, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SKYPE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SKYPE_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SLIDESHARE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SMASHING, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SOUND, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SOUND_MIX, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SOUND_MUTE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SOUNDCLOUD, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SPORTS_CLUB, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SPOTIFY, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SPOTIFY_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SPREADSHEET, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SQUARED_CROSS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SQUARED_MINUS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SQUARED_PLUS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_STAR, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_STAR_OUTLINED, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_STOPWATCH, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_STUMBLEUPON, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_STUMBLEUPON_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SUITCASE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SWAP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SWARM, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SWEDEN, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_SWITCH, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_TABLET, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_TABLET_MOBILE_COMBO, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_TAG, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_TEXT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_TEXT_DOCUMENT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_TEXT_DOCUMENT_INVERTED, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_THERMOMETER, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_THUMBS_DOWN, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_THUMBS_UP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_THUNDER_CLOUD, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_TICKET, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_TIME_SLOT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_TOOLS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_TRAFFIC_CONE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_TRASH, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_TREE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_TRIANGLE_DOWN, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_TRIANGLE_LEFT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_TRIANGLE_RIGHT, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_TRIANGLE_UP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_TRIPADVISOR, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_TROPHY, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_TUMBLR, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_TUMBLR_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_TV, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_TWITTER, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_TWITTER_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_TYPING, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_UNINSTALL, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_UNREAD, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_UNTAG, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_UPLOAD, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_UPLOAD_TO_CLOUD, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_USER, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_USERS, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_V_CARD, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_VIDEO, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_VIDEO_CAMERA, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_VIMEO, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_VIMEO_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_VINE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_VINE_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_VINYL, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_VK, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_VK_ALTERNITIVE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_VK_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_VOICEMAIL, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_WALLET, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_WARNING, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_WATER, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_WINDOWS_STORE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_XING, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_XING_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_YELP, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_YOUKO, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_YOUKO_WITH_CIRCLE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_YOUTUBE, half_width);
        ADD_ICON(wrapper, ENTYPO_ICON_YOUTUBE_WITH_CIRCLE, half_width);
        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////

        screen->performLayout();
        screen->setVisible(true);

        nanogui::mainloop();
    }

    nanogui::shutdown();
    return 0;
}
