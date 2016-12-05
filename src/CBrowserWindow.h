#ifndef CBrowserWindow_H
#define CBrowserWindow_H

#include <CBrowserTypes.h>
#include <CBrowserData.h>
#include <CFont.h>
#include <QImage>

#define LEFT_MARGIN   8
#define RIGHT_MARGIN  8
#define BOTTOM_MARGIN 8
#define TOP_MARGIN    8

class CBrowserWindow {
 public:
  CBrowserWindow(const std::string &filename="");
 ~CBrowserWindow();

  void setIFace(CBrowserIFace *iface);

  CBrowserWindowWidget *widget() const { return w_; }

  CBrowserDocument *getDocument() const { return document_; }

  int getX() const { return x_; }
  int getY() const { return y_; }

  int getWidth () const { return width_ ; }
  int getHeight() const { return height_; }

  int getLeftMargin() const { return left_margin_; }
  int getTopMargin () const { return top_margin_ ; }

  CHtmlLayoutMgr *getLayoutMgr() const { return layout_mgr_; }

  CHtmlLayoutArea *getAreaData() const { return area_data_; }

  CBrowserTableMgr *tableMgr() const { return tableMgr_; }

  CBrowserLinkMgr *linkMgr() const { return linkMgr_; }

  void setHAlign(CHAlignType halign);
  void setVAlign(CVAlignType valign);
  void setAlign(CHAlignType halign, CVAlignType valign);

  void startArea(CHtmlLayoutArea *area_data);
  void endArea();

  //---

  void startFontFace(const std::string &face);
  void endFontFace();

  CBrowserFontFace *currentFontFace() const { return currentFontFace_; }

  std::string getCurrentFontFace() const;

  void setCurrentFontFace(const std::string &face);

  CBrowserFontFace *lookupFontFace(const std::string &face);
  CBrowserFontFace *loadFontFace(const std::string &face);

  //---

  void setBaseFontSize(int size);
  int  getBaseFontSize();
  void resetBaseFontSize();
  void increaseBaseFontSize();
  void decreaseBaseFontSize();

  void setFontSize(int size);
  int  getFontSize();
  void resetFontSize();
  void increaseFontSize();
  void decreaseFontSize();

  void setFontColor(const std::string &color);
  std::string getFontColor();

  void startBold();
  void endBold();

  void startItalic();
  void endItalic();

  void setFontStyle();

  const CRGBA &currentFontColor() const { return currentFontColor_; }

  //---

  void freeFonts();

  CFontPtr getFont() const { return font_; }

  //---

  CBrowserObject *addCanvas(const CBrowserCanvasData &canvasData);

  CBrowserObject *addLabel(const std::string &text, int width, CHAlignType align,
                           const CRGBA &color);

  CBrowserObject *addText(const std::string &text, const CBrowserTextData &data);

  CBrowserObject *addSymbol(CBrowserSymbolType type);

  CBrowserObject *addImage(const CBrowserImageData &imageData);

  CBrowserObject *addNamedImage(const std::string &name);

  //---

  CBrowserText *formatText(CBrowserText *draw_text, const std::string &text);

  //---

  const CBrowserObject *rootObject() const { return rootObject_; }
  void setRootObject(CBrowserObject *p) { rootObject_ = p; }

  void startObject(CBrowserObject *obj);
  void endObject();

  CBrowserObject *currentObj() const;

  void addObject(CBrowserObject *obj);

  //---

  CBrowserObject *getObject(const std::string &id) const;

  //---

  void addScript(const std::string &text);
  void addScriptFile(const std::string &filename);

  void runScripts();

  //---

  CHtmlLayoutArea    *getCurrentArea();
  CHtmlLayoutCell    *getCurrentCell();
  CHtmlLayoutSubCell *getCurrentSubCell();

  void updateSubCellWidth(int width);
  void updateSubCellHeight(int ascent, int descent);

  void addCellRedrawData   (CHtmlLayoutBox *box);
  void addSubCellRedrawData(CHtmlLayoutBox *box);

  void close();

  void setName(const std::string &name);

  void setDocument(const std::string &filename);

  void outputDocument();

  void resize();
  void redraw();

  void drawDocument();

  void setTitle(const std::string &title);
  void setMargins(int, int);
  void setBackgroundImage(const std::string &name, bool fixed);
  void setTarget(const std::string &target);

  void indentLeft (int indent);
  void indentRight(int indent);

  CHtmlLayoutSubCell *newSubCellBelow(bool breakup=false);
  CHtmlLayoutSubCell *newSubCellRight(bool breakup=false);

  void newLine();
  void newColumn();

  void skipLine();

  void setStatus(const std::string &status);

  void displayError(const char *format, ...);

  bool isVisible(int x1, int y1, int x2, int y2);

  void print();

  void goBack();
  void goForward();

  bool hoverLink(int x, int y, std::string &link_name);

  bool activateLink(int x, int y);

  void addHistoryItem(const std::string &item);

  CRGBA getBg();
  CRGBA getFg();

  void drawImage(int x, int y, const CImagePtr &image);
  void drawImage(int x, int y, const QImage &image);

  void drawRectangle(int x1, int y1, int x2, int y2);
  void fillRectangle(int x1, int y1, int x2, int y2);

  void drawCircle(int x, int y, int r);
  void fillCircle(int x, int y, int r);

  void drawLine(int x1, int y1, int x2, int y2);

  void drawString(int x, int y, const std::string &str);

  void drawOutline(int x, int y, int width, int height, const std::string &color_name);
  void drawOutline(int x, int y, int width, int height, const CRGBA &c);

  void drawSelected(int x, int y, int width, int height);

  void drawBorder(int x, int y, int width, int height, CBrowserBorderType type);

  void drawHRule(int x1, int x2, int y, int height);

  void setForeground(const CRGBA &fg);
  void setFont(CFontPtr font);

  void getTextSize(const std::string &text, int *width, int *ascent, int *descent) const;
  void getTextWidth (CFontPtr font, const std::string &text, int *width) const;
  void getTextHeight(CFontPtr font, int *ascent, int *descent) const;

  void errorDialog(const std::string &msg);

  void recalc();

 private:
  void init();

  void loadResources();

  void setPixmapSize(int width, int height);

  void redrawArea();

  void gotoDocument(const std::string &filename);

 private:
  typedef std::list<CBrowserWindow *>             WindowList;
  typedef std::vector<CBrowserFontFace *>         FontFaces;
  typedef std::map<std::string, CBrowserObject *> Objects;
  typedef std::vector<CBrowserObject*>            ObjStack;
  typedef std::vector<std::string>                Scripts;
  typedef std::vector<std::string>                ScriptFiles;

  static WindowList     window_list_;
  static std::string    default_font_face_;
  static bool           ignore_redraw_;
  static std::string    window_target_;
  static CBrowserLink*  mouse_link_;

  std::string           name_;
  CBrowserDocument*     document_ { nullptr };

  CBrowserIFace*        iface_ { nullptr };
  CBrowserWindowWidget* w_ { nullptr };

  int                   x_ { 0 };
  int                   y_ { 0 };
  int                   width_ { 0 };
  int                   height_ { 0 };
  int                   left_margin_ { 0 };
  int                   top_margin_ { 0 };

  CImagePtr             bg_image_;
  bool                  bg_fixed_ { false };

  CHtmlLayoutMgr*       layout_mgr_ { nullptr };
  CHtmlLayoutArea*      area_data_ { nullptr };

  CBrowserTableMgr*     tableMgr_ { nullptr };
  CBrowserLinkMgr*      linkMgr_ { nullptr };

  Objects               objects_;
  CBrowserObject*       rootObject_ { nullptr };
  ObjStack              objStack_;
  Scripts               scripts_;
  ScriptFiles           scriptFiles_;

  CFontPtr              font_;
  FontFaces             font_face_stack_;
  FontFaces             font_face_list_;
  CBrowserFontFace*     currentFontFace_ { nullptr };
  int                   base_font_size_ { 0 };
  CRGBA                 currentFontColor_ { 0, 0, 0 };
  int                   currentFontStyle_ { 0 };
  int                   currentFontSize_ { 0 };

  CBrowserHistory*      history_ { nullptr };
};

#endif
