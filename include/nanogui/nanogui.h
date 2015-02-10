#if !defined(__NANOGUI_H)
#define __NANOGUI_H

#include <Eigen/Core>
#include <stdint.h>
#include <array>
#include <vector>

#if defined(WIN32)
#pragma warning(disable : 4244)
#endif

/* Convenience definitions */
#define NANOGUI_NAMESPACE_BEGIN namespace nanogui {
#define NANOGUI_NAMESPACE_END }

/* Set to 1 to draw boxes around widgets */
//#define NANOGUI_SHOW_WIDGET_BOUNDS 1

struct NVGcontext;
struct NVGcolor;
struct GLFWwindow;

NANOGUI_NAMESPACE_BEGIN

/* Import some common Eigen types */
using Eigen::Vector2f;
using Eigen::Vector3f;
using Eigen::Vector4f;
using Eigen::Vector2i;
using Eigen::Vector3i;
using Eigen::Vector4i;
using Eigen::Matrix3f;
using Eigen::Matrix4f;
using Eigen::MatrixXf;
typedef Eigen::Matrix<uint32_t, Eigen::Dynamic, Eigen::Dynamic> MatrixXu;

/// Stores an RGBA color value
class Color : public Eigen::Vector4f {
public:
	Color() { }
	Color(int intensity, int alpha) {
		const float scale = 1.f / 255.f;
		x() = y() = z() = intensity * scale; w() = alpha * scale;
	}
	Color(int r, int g, int b, int a) {
		const float scale = 1.f / 255.f;
		x() = r * scale; y() = g * scale;
		z() = b * scale; w() = a * scale;
	}
	inline operator const NVGcolor &() const;
};

/* Forward declarations */
class BoxLayout;
class Button;
class CheckBox;
class ComboBox;
class GLFramebuffer;
class GLShader;
class GroupLayout;
class ImagePanel;
class Label;
class Layout;
class MessageDialog;
class Popup;
class PopupButton;
class ProgressBar;
class Screen;
class Slider;
class TextBox;
struct Theme;
class ToolButton;
class VScrollPanel;
class Widget;
class Window;

/// Static initialization; should be called once before invoking any NanoGUI functions
extern void init();

/// Static shutdown; should be called before the application terminates
extern void shutdown();

/// Enter the application main loop
extern void mainloop();

/// Request the application main loop to terminate
extern void leave();

/**
 * \brief Open a native file open/save dialog.
 *
 * \param filetypes
 *     Pairs of permissible formats with descriptions like
 *     <tt>("png", "Portable Network Graphics")</tt>
 */
extern std::string file_dialog(const std::vector<std::pair<std::string, std::string>> &filetypes, bool save);

#if defined(__APPLE__)
/**
 * \brief Move to the application bundle's parent directory
 *
 * This is function is convenient when deploying .app bundles on OSX. It
 * adjusts the file path to the parent directory containing the bundle.
 */
void chdir_to_bundle_parent();
#endif

/**
 * \brief Convert a single UTF32 character code to UTF8
 *
 * NanoGUI uses this to convert the icon character codes
 * defined in entypo.h
 */
extern std::array<char, 8> utf8(int c);

/// Load a directory of PNG images and upload them to the GPU (suitable for use with ImagePanel)
extern std::vector<std::pair<int, std::string>>
	loadImageDirectory(NVGcontext *ctx, const std::string &path);

/// Convenience function for instanting a PNG icon from the application's data segment (via bin2c)
#define nvgImageIcon(ctx, name) __nanogui_get_image(ctx, #name, name##_png, name##_png_size)
/// Helper function used by nvgImageIcon
extern int __nanogui_get_image(NVGcontext *ctx, const std::string &name, uint8_t *data, uint32_t size);

NANOGUI_NAMESPACE_END

#endif /* __NANOGUI_H */
