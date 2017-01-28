#include <CBrowserCanvas.h>
#include <CQJCanvasWidget.h>
#include <CBrowserWindowWidget.h>
#include <CBrowserMain.h>
#include <CBrowserText.h>
#include <CBrowserLink.h>
#include <CBrowserWindow.h>
#include <CBrowserDocument.h>
#include <cstring>

CBrowserCanvas::
CBrowserCanvas(CBrowserWindow *window) :
 CBrowserObject(window, CHtmlTagId::CANVAS)
{
}

CBrowserCanvas::
~CBrowserCanvas()
{
}

void
CBrowserCanvas::
init()
{
}

void
CBrowserCanvas::
setNameValue(const std::string &name, const std::string &value)
{
  //std::string lname  = CStrUtil::toLower(name);

  CBrowserObject::setNameValue(name, value);
}

void
CBrowserCanvas::
setWidth(int w)
{
  CBrowserObject::setWidth(CBrowserUnitValue(w));

  window_->recalc();
}

void
CBrowserCanvas::
setHeight(int h)
{
  CBrowserObject::setHeight(CBrowserUnitValue(h));

  window_->recalc();
}

CBrowserRegion
CBrowserCanvas::
calcRegion() const
{
  int hspace = 0, vspace = 0;

  int width   = this->width ().value() + 2*hspace;
  int ascent  = this->height().value() + vspace;
  int descent = vspace;

  return CBrowserRegion(width, ascent, descent);
}

void
CBrowserCanvas::
draw(const CTextBox &region)
{
  fillBackground(region);

  //---

  region_ = region;

  if (canvas_) {
    canvas_->move  (region_.x(), region_.y());
    canvas_->resize(width().value(), height().value());
  }

  //---

  if (isSelected())
    window_->drawSelected(region.x(), region.y(), region.width(), region.height());
}

void
CBrowserCanvas::
createWidget()
{
  if (! canvas_) {
    canvas_ = new CQJCanvasWidget(this, window_->widget());

    canvas_->setObjectName("canvas");

    canvas_->resize(width().value(), height().value());

    canvas_->updateSize(width().value(), height().value());
  }
}

void
CBrowserCanvas::
update()
{
  if (canvas_)
    canvas_->update();
}
