#ifndef CBrowserCanvas_H
#define CBrowserCanvas_H

#include <CBrowserObject.h>
#include <CBrowserData.h>
#include <CJavaScript.h>
#include <QWidget>
#include <QGradient>
#include <QImage>
#include <QFont>
#include <QColor>
#include <QPen>

class CBrowserCanvas : public CBrowserObject {
 public:
  CBrowserCanvas(CBrowserWindow *window, const CBrowserCanvasData &data);
 ~CBrowserCanvas();

  CBrowserWindow *window() const { return window_; }

  CQJCanvasWidget *canvas() const { return canvas_; }

  int width () const { return data_.width ; }
  void setWidth(int w);

  int height() const { return data_.height; }
  void setHeight(int h);

  void format(CHtmlLayoutMgr *) override;

  void draw(CHtmlLayoutMgr *, const CHtmlLayoutRegion &) override;

  void update();

  void createWidget();

 private:
  CBrowserWindow*    window_ { nullptr };
  CBrowserCanvasData data_;
  CQJCanvasWidget*   canvas_ { nullptr };
  CHtmlLayoutRegion  region_;
};

#endif
