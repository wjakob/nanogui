#pragma once

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

class NANOGUI_EXPORT Meter : public Widget
{
public:
  RTTI_CLASS_UID(Meter)
  RTTI_DECLARE_INFO(Meter)

  enum ErrorCode {MaxValueError=1,MinValueError,ThresholdError,TargetError,PrecisionError,ColorError,UnitsEmpty,OutOfRange};
  Meter(Widget *parent);
  double value() const { return m_value; }
  double minValue() const { return m_minValue; }
  double maxValue() const { return m_maxValue; }
  double threshold() const { return m_threshold; }
  double beginValidValue() const { return m_beginValidValue; }
  double endValidValue() const { return m_endValidValue; }
  double beginWarningValue() const { return m_beginWarningValue; }
  double endWarningValue() const { return m_endWarningValue; }
  int precision() const { return m_precision; }
  int precisionNumeric() const { return m_precisionNumeric; }
  std::string units()const { return m_units; }
  std::string label()const { return m_label; }
  int steps() const { return m_steps; }
  double startAngle() const { return m_startAngle; }
  double endAngle() const { return m_endAngle; }
  Color foreground() const { return m_foreground; }
  Color background() const { return m_background; }
  bool enableThreshold() const { return m_thresholdEnabled; }
  bool enableValidWindow() const { return m_enableValidWindow; }
  bool enableWarningWindow() const { return m_enableWarningWindow; }
  bool enableNumericIndicator() const { return m_numericIndicatorEnabled; }

public:

  void setValue(double);
  void setValue(int);
  void setMinValue(double);
  void setMinValue(int);
  void setMaxValue(double);
  void setMaxValue(int);
  void setThreshold(double);
  void setThreshold(int);
  void setPrecision(int);
  void setPrecisionNumeric(int);
  void setUnits(std::string);
  void setLabel(std::string);
  void setSteps(int);
  void setStartAngle(double);
  void setEndAngle(double);
  void setForeground(const Color&);
  void setBackground(const Color&);
  void setThresholdEnabled(bool);
  void setNumericIndicatorEnabled(bool);
  void setBeginValidValue(double);
  void setEndValidValue(double);
  void setEnableValidWindow(bool);
  void setBeginWarningValue(double);
  void setEndWarningValue(double);
  void setEnableWarningWindow(bool);

  Vector2i preferredSize(NVGcontext*) const override;

  std::function<void(int)> onError;
  std::function<void(int)> onTreshold;

protected:

    double m_value, mLastValue;
    bool mNeedUpdateLastValueText;
    bool mNeedUpdateLabelText;
    bool mNeedUpdateUnitsText;
    double m_maxValue, m_minValue;
    int m_precision;
    int m_precisionNumeric;
    Vector2i mTextRealSize;
    Vector2i mValueTextRealSize;
    Vector2i mLabelTextRealSize;
    void thresholdManager();
    void draw(NVGcontext *event) override;
    void drawCrown(NVGcontext *painter);
    void drawBackground(NVGcontext *painter);
    void drawTicks(NVGcontext *painter);
    void drawScale(NVGcontext *painter);
    void drawUnits(NVGcontext *painter);
    void drawNeedle(NVGcontext *painter);
    void drawThresholdLine(NVGcontext *painter);
    void drawNumericValue(NVGcontext *painter);
    void drawCoverGlass(NVGcontext *painter);
    void drawLabel(NVGcontext *painter);
    void drawValidWindow(NVGcontext *painter);
    void drawWarningWindow(NVGcontext *painter);

    std::string m_units;
    std::string m_label;
    std::string m_value_text;
    int m_steps;
    double m_startAngle,m_endAngle;
    double m_threshold;
    bool m_thresholdFlag;
    bool m_enableValidWindow;
    double m_beginValidValue, m_endValidValue;
    bool m_enableWarningWindow;
    double m_beginWarningValue, m_endWarningValue;
    bool m_thresholdEnabled;
    bool m_numericIndicatorEnabled;
    Color m_foreground;
    Color m_background;
};

NAMESPACE_END(nanogui)
