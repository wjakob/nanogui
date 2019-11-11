#include <nanogui/meter.h>
#include <nanogui/theme.h>
#include <math.h>
#include <nanovg.h>

NAMESPACE_BEGIN(nanogui)

Meter::Meter(Widget *parent)
    : Widget(parent)
{
   m_precision=0;
   m_steps=10;
   m_thresholdFlag=false;

   m_foreground=Color(0xff, 0xff, 0xff, 0xff);
   m_background=Color(0, 0, 0, 0xff);

   m_thresholdEnabled=true;
   m_numericIndicatorEnabled=true;
   setMinValue(0);
   setMaxValue(100);
   setValue(0);
   setStartAngle(320);
   setEndAngle(40);
   setMinSize(Vector2i(80,80));
   setLabel("Speed");
   setUnits("Km/h");
   setThreshold(80);
   setEnableValidWindow(false);
   setBeginValidValue(40.0);
   setEndValidValue(50.0);
   setEnableWarningWindow(false);
   setBeginWarningValue(30.0);
   setEndWarningValue(60.0);
}

void Meter::setValue(double value)
{
    if(value>m_maxValue)
    {
      m_value=m_maxValue;
      if (onError)
        onError(OutOfRange);
    }
    else
      if(value<m_minValue)
      {
        m_value=m_minValue;
        if (onError)
          onError(OutOfRange);
      }
      else
          m_value=value;

    if(m_thresholdEnabled)
     thresholdManager();
}

void Meter::setValue(int value)
{
   setValue((double)value);
}

void Meter::setMinValue(double value)
{
   m_minValue=value;
}

void Meter::setMinValue(int value)
{
  setMinValue((double)value);
}

void Meter::setMaxValue(double value)
{
  if(value > m_minValue)
  {
      m_maxValue=value;
   }
   else
  {
    if (onError)
      onError(MaxValueError);
  }
}

void Meter::setMaxValue(int value)
{
  setMaxValue((double)value);
}


void Meter::setThreshold(double value)
{
  if(value > m_minValue && value < m_maxValue)
  {
      m_threshold=value;
   }
   else
  {
    if (onError)
       onError(ThresholdError);
  }
}

void Meter::setThreshold(int value)
{
  setThreshold((double)value);
}

void Meter::setPrecision(int precision)
{
   m_precision=precision;
}

void Meter::setPrecisionNumeric(int precision)
{
   m_precisionNumeric=precision;
}

void Meter::setUnits(std::string units)
{
  m_units=units;
  mNeedUpdateUnitsText = true;
}

void Meter::setLabel(std::string label)
{
    m_label=label;
    mNeedUpdateLabelText = true;
}

void Meter::draw(NVGcontext *ctx)
{
    int side = std::min(width(), height());

    thresholdManager();

    nvgSave(ctx);

    drawBackground(ctx);
    drawCoverGlass(ctx);
    drawTicks(ctx);
    drawScale(ctx);
    drawUnits(ctx);
    drawThresholdLine(ctx);
    drawWarningWindow(ctx);
    drawValidWindow(ctx);
    drawNeedle(ctx);
    drawNumericValue(ctx);
    drawLabel(ctx);
    drawCrown(ctx);

    nvgRestore(ctx);
}

void Meter::setSteps(int nSteps)
{
  m_steps=nSteps;
}

void Meter::setStartAngle(double value)
{
  m_startAngle=value;
}

void Meter::setEndAngle(double value)
{
  m_endAngle=value;
}

void Meter::setForeground(const Color& newForeColor)
{
  m_foreground=newForeColor;
}

void Meter::setBackground(const Color& newBackColor)
{
  m_background=newBackColor;
}

void Meter::thresholdManager()
{
  if (fabs(m_value - mLastValue) < 0.01)
    return;

  mLastValue = m_value;
  mNeedUpdateLastValueText = true;

  if(m_value > m_threshold && !m_thresholdFlag)
  {
    m_thresholdFlag=true;
    if (onTreshold)
      onTreshold(m_thresholdFlag);
  }
  else if(m_value < m_threshold && m_thresholdFlag)
  {
    m_thresholdFlag=false;
    if (onTreshold)
      onTreshold(m_thresholdFlag);
  }
}

void Meter::setThresholdEnabled(bool enable)
{
  m_thresholdEnabled=enable;
}

void Meter::setNumericIndicatorEnabled(bool enable)
{
  m_numericIndicatorEnabled=enable;
}

void Meter::setBeginValidValue(double beginValue)
{
    m_beginValidValue=beginValue;
}

void Meter::setEndValidValue(double endValue)
{
    m_endValidValue=endValue;
}

void Meter::setEnableValidWindow(bool enable)
{
    m_enableValidWindow=enable;
}

void Meter::setBeginWarningValue(double beginValue)
{
    m_beginWarningValue=beginValue;
}

void Meter::setEndWarningValue(double endValue)
{
    m_endWarningValue=endValue;
}

void Meter::setEnableWarningWindow(bool enable)
{
    m_enableWarningWindow=enable;
}

Vector2i Meter::preferredSize(NVGcontext *ctx) const
{
  return Widget::preferredSize(ctx);
}

float deg2rad(float angle)
{
  return angle / 180. * 3.1415;
}

void Meter::drawCrown(NVGcontext *ctx)
{
  nvgBeginPath(ctx);

  int startAngle = 0;
  int spanAngle = 360;
  auto g = nvgLinearGradient(ctx, mPos.x(), mPos.y(), mPos.x() + mSize.x(), mPos.y() + mSize.y(),
                                  Color(0xff, 0xff, 0xff, 0xff), Color(60,60,60,250));
  nvgStrokeWidth(ctx, 6);

  int radius = std::min(width(), height()) / 2;
  Vector2i center = mPos + mSize / 2;
  nvgBeginPath(ctx);
  float r0 = radius;
  float r1 = radius * 0.94f;
  nvgArc(ctx, center.x(), center.y(), r0, deg2rad(startAngle), deg2rad(spanAngle), NVG_CW);
  nvgArc(ctx, center.x(), center.y(), r1, deg2rad(spanAngle), deg2rad(startAngle), NVG_CCW);
  nvgClosePath(ctx);

  nvgFillPaint(ctx, g);
  nvgFill(ctx);
}

void Meter::drawBackground(NVGcontext *ctx)
{
  nvgBeginPath(ctx);
  nvgFillColor(ctx, m_background);
  Vector2i center = mPos + mSize / 2;
  float r = std::min(mSize.x(), mSize.y()) / 2;
  nvgEllipse(ctx, center.x(), center.y(), r, r);
  nvgFill(ctx);
}

void Meter::drawTicks(NVGcontext *ctx)
{
  nvgBeginPath(ctx);
  int nSteps=m_steps;
  Vector2i center = mPos + mSize / 2;
  double angleStep=(m_endAngle-m_startAngle)/m_steps;
  int r1 = std::min(mSize.x(), mSize.y()) / 2 * 0.6;
  int r2 = std::min(mSize.x(), mSize.y()) / 2 * 0.7;
  for (int i=0; i<=nSteps; i++)
  {
    float ax = sin(deg2rad(m_startAngle + angleStep * i)) * r1;
    float ay = cos(deg2rad(m_startAngle + angleStep * i)) * r1;
    float bx = sin(deg2rad(m_startAngle + angleStep * i)) * r2;
    float by = cos(deg2rad(m_startAngle + angleStep * i)) * r2;
    nvgMoveTo(ctx, ax + center.x(), ay + center.y());
    nvgLineTo(ctx, bx + center.x(), by + center.y());
  }

  nvgStrokeColor(ctx, m_foreground);
  nvgStrokeWidth(ctx, 1);
  nvgStroke(ctx);

  nvgBeginPath(ctx);
  nvgArc(ctx, center.x(), center.y(), r1, deg2rad(m_endAngle + 90), deg2rad(m_startAngle + 90), NVG_CW);
  nvgStroke(ctx);
}


void Meter::drawScale(NVGcontext *ctx)
{
  nvgFillColor(ctx, m_foreground);
  double sina,cosa;
  int r = std::min(width(), height());
  Vector2i center = mPos + mSize / 2;

  double angleStep = (m_endAngle - m_startAngle) / m_steps;
  int r1 = std::min(mSize.x(), mSize.y()) / 2 * 0.82;
  nvgFontSize(ctx, fontSize() - 2);
  nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
  int h = fontSize() - 2;

  for (int i = 0; i <= m_steps; i++)
  {
    float ax = sin(deg2rad(m_startAngle + angleStep * i)) * r1;
    float ay = cos(deg2rad(m_startAngle + angleStep * i)) * r1;

    double tmpVal=i*((m_maxValue-m_minValue)/m_steps);

    tmpVal += m_minValue;
    std::string  str = std::to_string(tmpVal);
    str.resize(3);

    int w = nvgTextBounds(ctx, 0, 0, str.c_str(), nullptr, nullptr) + 2;

    nvgText(ctx, center.x() + ax - w/2, center.y() + ay - h/2, str.c_str(), nullptr);
  }
}

void Meter::drawUnits(NVGcontext *ctx)
{
  if (mNeedUpdateUnitsText)
  {
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    int tw = nvgTextBounds(ctx, 0, 0, m_units.c_str(), nullptr, nullptr) + 2;
    int th = fontSize();
    mTextRealSize = Vector2i(tw, th);
    mNeedUpdateUnitsText = false;
  }

  nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
  int xpos = (mSize.x() - mTextRealSize.x()) / 2;
  int ypos = (mSize.y() - mTextRealSize.y()) / 2;
  nvgFillColor(ctx, m_foreground);
  nvgText(ctx, mPos.x() + xpos, mPos.y() + ypos - 0.1 * height(), m_units.c_str(), nullptr);
}

void Meter::drawNumericValue(NVGcontext *ctx)
{
  if(!enableNumericIndicator())
        return;

  if (mNeedUpdateLastValueText)
  {
    m_value_text = std::to_string(m_value);
    m_value_text.resize(3);
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    int tw = nvgTextBounds(ctx, 0, 0, m_value_text.c_str(), nullptr, nullptr) + 2;
    int th = fontSize();
    mValueTextRealSize = Vector2i(tw, th);
    mNeedUpdateLastValueText = false;
  }

  Color color;
  if(!m_thresholdFlag)
    color=Color(0xff, 0xff, 0xff, 0xff);
  else
    color=Color(0xff, 0, 0, 0xff);

  int xpos = (mSize.x() - mValueTextRealSize.x()) / 2;
  int ypos = (mSize.y() - mValueTextRealSize.y()) / 2;

  nvgFontSize(ctx, fontSize()+1);
  nvgFontFace(ctx, "sans-bold");
  nvgFillColor(ctx, color);
  nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
  nvgText(ctx, mPos.x() + xpos, mPos.y() + ypos + 0.2 * height(), m_value_text.c_str(), nullptr);
}

float sindr(float angle, float r) { return sin(deg2rad(angle)) * r; }
float cosdr(float angle, float r) { return cos(deg2rad(angle)) * r; }

void Meter::drawNeedle(NVGcontext *ctx)
{
  nvgSave(ctx);

  int degRotate=0;
  degRotate = (int)( m_startAngle+(m_endAngle-m_startAngle)/(m_maxValue-m_minValue)*(m_value-m_minValue) );

  Vector2i c = mPos + mSize / 2;
  int w = std::min<int>(width(), height());

  //nvgRotate(ctx, nvgDegToRad(-degRotate));
  auto haloGradient = nvgRadialGradient(ctx, c.x() + 0, c.y() + 0, 0, 0.3 * w,
                                        Color(255,120,120, 0xff), Color(200,20,20, 0xff));
  nvgBeginPath(ctx);
  nvgMoveTo(ctx, c.x() + sindr(degRotate + 90, 0.02*w), c.y() + cosdr(degRotate + 90, 0.02*w));
  nvgLineTo(ctx, c.x() + sindr(degRotate + 279, 0.02*w), c.y() + cosdr(degRotate + 270, 0.02*w));
  nvgLineTo(ctx, c.x() + sindr(degRotate, 0.4 *w), c.y() + cosdr(degRotate, 0.4 * w));
  nvgClosePath(ctx);
  nvgFillPaint(ctx, haloGradient);
  nvgFill(ctx);

  nvgBeginPath(ctx);
  nvgStrokeWidth(ctx, 1);
  nvgStrokeColor(ctx, Color(255, 120, 120, 0xff));
  nvgMoveTo(ctx, c.x() + sindr(degRotate + 90, 0.01 * w), c.y() + cosdr(degRotate + 90, 0.01*w));
  nvgLineTo(ctx, c.x() + sindr(degRotate + 270, 0.01 * w), c.y() + cosdr(degRotate + 270, 0.01*w));
  nvgLineTo(ctx, c.x() + sindr(degRotate, 0.39 * w), c.y() + cosdr(degRotate, 0.39 * w));
  nvgClosePath(ctx);
  nvgStroke(ctx);

  nvgRestore(ctx);

  //draw needle hat
  nvgBeginPath(ctx);
  auto linearGrad = nvgLinearGradient(ctx, c.x() - 0.07 * w, c.y() - 0.07 * w,
                                           c.x() + 0.14 * w, c.y() + 0.14 * w,
                                           Color(0xff, 0xff, 0xff, 0xff), Color(0, 0, 0, 0xff));
  nvgFillPaint(ctx, linearGrad);
  nvgEllipse(ctx, c.x() + 0, c.y() + 0, 0.07 * w, 0.07 * w);
  nvgFill(ctx);
}

void Meter::drawThresholdLine(NVGcontext *ctx)
{
    if(!enableThreshold())
        return;

    nvgBeginPath(ctx);
    double thresholdAngle = (m_startAngle - m_endAngle)/(m_maxValue-m_minValue)*(m_threshold-m_minValue);
    nvgStrokeWidth(ctx, 5);
    nvgStrokeColor(ctx, Color(0, 0xff, 0, 0xff));

    Vector2i c = mPos + mSize / 2;
    float r1 = std::min(mSize.x(), mSize.y()) / 2 * 0.48;

    nvgArc(ctx, c.x(),c.y(), r1, deg2rad(m_endAngle + 90), deg2rad(m_endAngle + thresholdAngle + 90), NVG_CW);
    nvgStroke(ctx);

    nvgBeginPath(ctx);
    nvgStrokeColor(ctx, Color(0xff, 0, 0, 0xff));
    nvgArc(ctx, c.x(), c.y(), r1, deg2rad(m_endAngle + thresholdAngle + 90), deg2rad(m_startAngle + 90), NVG_CW);
    nvgStroke(ctx);
}

void Meter::drawCoverGlass(NVGcontext *ctx)
{
  int w = width();
  int h = height();
  Vector2i c = mPos + mSize / 2;
  auto linearGrad = nvgLinearGradient(ctx, c.x()-0.15*w, c.y()-0.45*h, c.x() + 0.9 * w, c.y() + 0.75 * h,
                                        Color(255,255,255,60), Color(120,120,120,40));
  nvgBeginPath(ctx);
  nvgEllipse(ctx, c.x(), c.y(), 0.48 * w, 0.48 * h);
  nvgFillPaint(ctx, linearGrad);
  nvgFill(ctx);
}

void Meter::drawLabel(NVGcontext *ctx)
{
  if (mNeedUpdateLabelText)
  {
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    int tw = nvgTextBounds(ctx, 0, 0, m_label.c_str(), nullptr, nullptr) + 2;
    int th = fontSize();
    mLabelTextRealSize = Vector2i(tw, th);
    mNeedUpdateLabelText = false;
  }

  nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
  int xpos = (mSize.x() - mLabelTextRealSize.x()) / 2;
  int ypos = (mSize.y() - mLabelTextRealSize.y()) / 2;
  nvgFillColor(ctx, m_foreground);
  nvgText(ctx, mPos.x() + xpos, mPos.y() + ypos + 0.3 * height(), m_label.c_str(), nullptr);
}

void Meter::drawWarningWindow(NVGcontext *ctx)
{
   if(enableWarningWindow())
   {
   }
}

void Meter::drawValidWindow(NVGcontext *ctx)
{
   if(enableValidWindow())
   {
   }
}

NAMESPACE_END(nanogui)
