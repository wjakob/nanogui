#include <nanogui/messagedialog.h>
#include <nanogui/layout.h>
#include <nanogui/button.h>
#include <nanogui/entypo.h>
#include <nanogui/label.h>

NANOGUI_NAMESPACE_BEGIN

// Bug: MessageDialog can not be disposed withing here

MessageDialog::MessageDialog(Widget *parent, Type type, const std::string &title,
              const std::string &message,
              const std::string &buttonText,
              const std::string &altButtonText, bool altButton) : Window(parent, title) {
    setLayout(new BoxLayout(BoxLayout::Orientation::Vertical, BoxLayout::Alignment::Middle, 10, 10));
    setModal(true);

    Widget *panel1 = new Widget(this);
    panel1->setLayout(new BoxLayout(BoxLayout::Orientation::Horizontal,BoxLayout::Alignment::Middle,10,15));
    int icon = 0;
    switch (type) {
        case Information: icon = ENTYPO_ICON_CIRCLED_INFO; break;
        case Question: icon = ENTYPO_ICON_CIRCLED_HELP; break;
        case Warning: icon = ENTYPO_ICON_WARNING; break;
    }
    Label *iconLabel = new Label(panel1, std::string(utf8(icon).data()), "icons");
    iconLabel->setFontSize(50);
    Label *msgLabel = new Label(panel1, message);
    msgLabel->setFixedWidth(200);
    Widget *panel2 = new Widget(this);
    panel2->setLayout(new BoxLayout(BoxLayout::Orientation::Horizontal,BoxLayout::Alignment::Middle,0,15));

    if (altButton) {
        Button *button = new Button(panel2, altButtonText, ENTYPO_ICON_CIRCLED_CROSS);
        button->setCallback([&] { if (mCallback) mCallback(1); dispose(); });
    }
    Button *button = new Button(panel2, buttonText, ENTYPO_ICON_CHECK);
    button->setCallback([&] { if (mCallback) mCallback(0); dispose(); }); // Is causing a memory leak!
    center();
    requestFocus();
}

NANOGUI_NAMESPACE_END
