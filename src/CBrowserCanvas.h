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

  CQJCanvasWidget *canvas() const { return canvas_; }

  void setWidth(int w);
  void setHeight(int h);

  void init() override;

  void setNameValue(const std::string &name, const std::string &value) override;

  CBrowserRegion calcRegion() const override;

  void draw(const CTextBox &) override;

  void update();

  void createWidget();

 private:
  CBrowserCanvasData data_;
  CQJCanvasWidget*   canvas_ { nullptr };
  CTextBox           region_;
};

#endif
