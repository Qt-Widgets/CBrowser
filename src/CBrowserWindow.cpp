#include <CBrowserWindow.h>
#include <CBrowserDocument.h>
#include <CBrowserHistory.h>
#include <CBrowserIFace.h>
#include <CBrowserWindowWidget.h>
#include <CBrowserFont.h>
#include <CBrowserMain.h>
#include <CBrowserBreak.h>
#include <CBrowserCanvas.h>
#include <CBrowserImage.h>
#include <CBrowserNamedImage.h>
#include <CBrowserLabel.h>
#include <CBrowserLink.h>
#include <CBrowserRule.h>
#include <CBrowserSymbol.h>
#include <CBrowserTable.h>
#include <CBrowserText.h>
#include <CBrowserJS.h>
#include <CRGBName.h>
#include <CEnv.h>

class CHtmlLayoutPrintVisitor : public CHtmlLayoutVisitor {
 public:
  void enter(CHtmlLayoutArea *area) {
    std::cout << prefix_ << "<area ";
    area->printSize(std::cout); std::cout << ">" << std::endl;
    enterBlock();
  }
  void leave(CHtmlLayoutArea *) {
    leaveBlock();
    std::cout << prefix_ << "</area>" << std::endl;
  }

  void enter(CHtmlLayoutCell *cell) {
    std::cout << prefix_ << "<cell ";
    cell->printSize(std::cout); std::cout << ">" << std::endl;
    enterBlock();
  }
  void leave(CHtmlLayoutCell *) {
    leaveBlock();
    std::cout << prefix_ << "</cell>" << std::endl;
  }

  void enter(CHtmlLayoutSubCell *subCell) {
    std::cout << prefix_ << "<subCell ";
    subCell->printSize(std::cout); std::cout << ">" << std::endl;
    enterBlock();
  }
  void leave(CHtmlLayoutSubCell *) {
    leaveBlock();
    std::cout << prefix_ << "</subCell>" << std::endl;
  }

  void enter(CHtmlLayoutBox *box) {
    CBrowserObject *obj = dynamic_cast<CBrowserObject *>(box);
    if (! obj) return;
    std::cout << prefix_; obj->print(std::cout); std::cout << std::endl;
  }

  void enterBlock() {
    prefix_ += " ";
  }

  void leaveBlock() {
    prefix_ = prefix_.substr(0, prefix_.size() - 1);
  }

 private:
  std::string prefix_;
};

std::list<CBrowserWindow *>  CBrowserWindow::window_list_;
std::string                  CBrowserWindow::default_font_face_ = "";
std::string                  CBrowserWindow::window_target_     = "";
CBrowserLink                *CBrowserWindow::mouse_link_        = nullptr;

CBrowserWindow::
CBrowserWindow(const std::string &filename)
{
  init();

  if (filename != "")
    setDocument(filename);

  window_list_.push_back(this);
}

CBrowserWindow::
~CBrowserWindow()
{
  window_list_.remove(this);

  document_->freeLinks();

  freeFonts();

  delete area_data_;
  delete document_;
  delete history_;

  if (window_list_.size() == 0)
    exit(0);
}

void
CBrowserWindow::
init()
{
  default_font_face_ = "";

  window_target_ = "";

  mouse_link_ = nullptr;

  name_     = "";
  document_ = nullptr;

  iface_ = nullptr;
  w_     = nullptr;

  x_      = LEFT_MARGIN;
  y_      = TOP_MARGIN;
  width_  = x_;
  height_ = y_;

  left_margin_ = LEFT_MARGIN;
  top_margin_  = TOP_MARGIN;

  bg_image_ = CImagePtr();
  bg_fixed_ = false;

  layout_mgr_ = new CHtmlLayoutMgr();
  area_data_  = new CHtmlLayoutArea();

  tableMgr_ = new CBrowserTableMgr(this);

  linkMgr_ = new CBrowserLinkMgr(this);

  currentFontFace_  = nullptr;
  base_font_size_     = 3;
  currentFontStyle_ = CFONT_STYLE_NORMAL;
  currentFontSize_  = 3;

  history_ = new CBrowserHistory(this);

  //---

  loadResources();

  //---

  startFontFace(default_font_face_);
}

void
CBrowserWindow::
setIFace(CBrowserIFace *iface)
{
  iface_ = iface;
  w_     = iface_->widget();
}

void
CBrowserWindow::
loadResources()
{
  default_font_face_ = "helvetica";
}

void
CBrowserWindow::
startArea(CHtmlLayoutArea *area_data)
{
  layout_mgr_->startArea(area_data);
}

void
CBrowserWindow::
endArea()
{
  layout_mgr_->endArea();
}

void
CBrowserWindow::
startFontFace(const std::string &face)
{
  if (currentFontFace_)
    font_face_stack_.push_back(currentFontFace_);

  currentFontFace_ = lookupFontFace(face);

  if (! currentFontFace_)
    return;

  base_font_size_     = 3;
  currentFontSize_  = 3;
  currentFontStyle_ = CFONT_STYLE_NORMAL;

  setFontStyle();
}

void
CBrowserWindow::
endFontFace()
{
  if (font_face_stack_.empty())
    return;

  currentFontFace_ = font_face_stack_[font_face_stack_.size() - 1];

  font_face_stack_.pop_back();

  if (! currentFontFace_)
    return;

  setFontStyle();
}

std::string
CBrowserWindow::
getCurrentFontFace() const
{
  if (! currentFontFace_)
    return "helvetica";

  return currentFontFace_->getFace();
}

void
CBrowserWindow::
setCurrentFontFace(const std::string &face)
{
  currentFontFace_ = lookupFontFace(face);
}

CBrowserFontFace *
CBrowserWindow::
lookupFontFace(const std::string &face)
{
  CBrowserFontFace *font_face = nullptr;

  std::vector<CBrowserFontFace *>::iterator pfont1 = font_face_list_.begin();
  std::vector<CBrowserFontFace *>::iterator pfont2 = font_face_list_.end  ();

  for ( ; pfont1 != pfont2; ++pfont1)
    if ((*pfont1)->getFace() == face)
      return *pfont1;

  font_face = loadFontFace(face);

  return font_face;
}

CBrowserFontFace *
CBrowserWindow::
loadFontFace(const std::string &face)
{
  CBrowserFontFace *font_face = new CBrowserFontFace(this, face);

  font_face_list_.push_back(font_face);

  return font_face;
}

void
CBrowserWindow::
setBaseFontSize(int size)
{
  base_font_size_ = size - 1;

  setFontStyle();
}

int
CBrowserWindow::
getBaseFontSize()
{
  return (base_font_size_ + 1);
}

void
CBrowserWindow::
resetBaseFontSize()
{
  base_font_size_ = 3;

  setFontStyle();
}

void
CBrowserWindow::
increaseBaseFontSize()
{
  base_font_size_++;

  setFontStyle();
}

void
CBrowserWindow::
decreaseBaseFontSize()
{
  base_font_size_--;

  setFontStyle();
}

void
CBrowserWindow::
setFontSize(int size)
{
  currentFontSize_ = size - 1;

  setFontStyle();
}

int
CBrowserWindow::
getFontSize()
{
  return (currentFontSize_ + 1);
}

void
CBrowserWindow::
resetFontSize()
{
  currentFontSize_ = 3;

  setFontStyle();
}

void
CBrowserWindow::
increaseFontSize()
{
  currentFontSize_++;

  setFontStyle();
}

void
CBrowserWindow::
decreaseFontSize()
{
  currentFontSize_--;

  setFontStyle();
}

void
CBrowserWindow::
setFontColor(const std::string &colorName)
{
  currentFontColor_ = CRGBName::toRGBA(colorName);
}

std::string
CBrowserWindow::
getFontColor()
{
  return currentFontColor_.stringEncode();
}

void
CBrowserWindow::
startBold()
{
  currentFontStyle_ |= CFONT_STYLE_BOLD;

  setFontStyle();
}

void
CBrowserWindow::
endBold()
{
  currentFontStyle_ &= ~CFONT_STYLE_BOLD;

  setFontStyle();
}

void
CBrowserWindow::
startItalic()
{
  currentFontStyle_ |= CFONT_STYLE_ITALIC;

  setFontStyle();
}

void
CBrowserWindow::
endItalic()
{
  currentFontStyle_ &= ~CFONT_STYLE_ITALIC;

  setFontStyle();
}

void
CBrowserWindow::
setFontStyle()
{
  if (! currentFontFace_)
    return;

  int size = currentFontSize_ + base_font_size_ - 3;

  if      (size > 6)
    size = 6;
  else if (size < 0)
    size = 0;

  if      (currentFontStyle_ == CFONT_STYLE_NORMAL)
    font_ = currentFontFace_->getNormalFont(size);
  else if (currentFontStyle_ == CFONT_STYLE_BOLD)
    font_ = currentFontFace_->getBoldFont(size);
  else if (currentFontStyle_ == CFONT_STYLE_ITALIC)
    font_ = currentFontFace_->getItalicFont(size);
  else if (currentFontStyle_ == CFONT_STYLE_BOLD_ITALIC)
    font_ = currentFontFace_->getBoldItalicFont(size);
}

void
CBrowserWindow::
freeFonts()
{
  for (auto &f : font_face_list_)
    delete f;
}

//------

CBrowserObject *
CBrowserWindow::
addCanvas(const CBrowserCanvasData &canvasData)
{
  CBrowserCanvas *canvas = new CBrowserCanvas(this, canvasData);

  canvas->setId(canvasData.id);

  addCellRedrawData(canvas);

  addObject(canvas);

  return canvas;
}

CBrowserObject *
CBrowserWindow::
addLabel(const std::string &text, int width, CHAlignType align, const CRGBA &color)
{
  CBrowserLabel *label = new CBrowserLabel(this, text, width, align, color);

  addCellRedrawData(label);

  //addObject(label);

  return label;
}

CBrowserObject *
CBrowserWindow::
addText(const std::string &str, const CBrowserTextData &data)
{
  CBrowserText *text = new CBrowserText(this, str, data);

  addCellRedrawData(text);

  return text;
}

CBrowserObject *
CBrowserWindow::
addSymbol(CBrowserSymbolType type)
{
  int width, ascent, descent;

  getTextSize("X", &width, &ascent, &descent);

  CBrowserSymbol *symbol = new CBrowserSymbol(this, type, width, ascent);

  addCellRedrawData(symbol);

  return symbol;
}

CBrowserObject *
CBrowserWindow::
addImage(const CBrowserImageData &imageData)
{
  if (imageData.src.substr(0, 6) == "_html_")
    return addNamedImage(imageData.src.substr(6));

  //---

  CImageFileSrc file(imageData.src);

  CImagePtr image = CImageMgrInst->createImage(file);

  if (image.isValid() && imageData.width != -1 && imageData.height != -1 &&
      ((int) image->getWidth () != imageData.width ||
       (int) image->getHeight() != imageData.height)) {
    CImagePtr image1 = image->resize(imageData.width, imageData.height);

    image = image1;
  }

  if (! image.isValid()) {
    if (imageData.alt != "" && CBrowserMainInst->getUseAlt()) {
      CBrowserTextData textData;

      textData.color   = currentFontColor();
      textData.breakup = false;

      return addText(imageData.alt, textData);
    }

    image = CBrowserNamedImage::genNoImage();
  }

  if (! image)
    return 0;

  CBrowserImage *imageObj =
    new CBrowserImage(this, image, imageData.align, imageData.border,
                      imageData.width, imageData.height, imageData.hspace, imageData.vspace);

  addCellRedrawData(imageObj);

  return imageObj;
}

CBrowserObject *
CBrowserWindow::
addNamedImage(const std::string &name)
{
  CImagePtr image = CBrowserNamedImage::lookup(name);

  if (! image)
    return 0;

  CBrowserImage *imageObj =
    new CBrowserImage(this, image, CBrowserImageAlign::ABSBOTTOM, 0, -1, -1, 0, 0);

  addCellRedrawData(imageObj);

  return imageObj;
}

//------

CBrowserText *
CBrowserWindow::
formatText(CBrowserText *draw_text, const std::string &text)
{
  int ascent, descent;

  getTextHeight(draw_text->font(), &ascent, &descent);

  updateSubCellHeight(ascent, descent);

  int width;

  getTextWidth(draw_text->font(), text, &width);

  updateSubCellWidth(width);

  //---

  CBrowserText *draw_text1 = new CBrowserText(this, *draw_text, text);

  addSubCellRedrawData(draw_text1);

  return draw_text1;
}

//------

void
CBrowserWindow::
startObject(CBrowserObject *obj)
{
  assert(obj);

  CBrowserObject *currentObj = this->currentObj();

  if (currentObj)
    currentObj->addChild(obj);

  addObject(obj);

  objStack_.push_back(obj);
}

void
CBrowserWindow::
endObject()
{
  if (! objStack_.empty())
    objStack_.pop_back();
}

CBrowserObject *
CBrowserWindow::
currentObj() const
{
  if (! objStack_.empty())
    return objStack_.back();
  else
    return 0;
}

void
CBrowserWindow::
addObject(CBrowserObject *obj)
{
  objects_[obj->id()] = obj;

  CBrowserJSInst->addHtmlObject(obj);
}

CBrowserObject *
CBrowserWindow::
getObject(const std::string &id) const
{
  auto p = objects_.find(id);

  if (p == objects_.end())
    return nullptr;

  return (*p).second;
}

//------

void
CBrowserWindow::
addScript(const std::string &text)
{
  scripts_.push_back(text);
}

void
CBrowserWindow::
addScriptFile(const std::string &filename)
{
  scriptFiles_.push_back(filename);
}

void
CBrowserWindow::
runScripts()
{
  for (const auto &s : scriptFiles_)
    CBrowserJSInst->runScriptFile(this, s);

  for (const auto &s : scripts_)
    CBrowserJSInst->runScript(this, s);

  CBrowserJSInst->onLoad();
}

//------

CHtmlLayoutArea *
CBrowserWindow::
getCurrentArea()
{
  return layout_mgr_->getCurrentArea();
}

CHtmlLayoutCell *
CBrowserWindow::
getCurrentCell()
{
  return getCurrentArea()->getCurrentCell();
}

CHtmlLayoutSubCell *
CBrowserWindow::
getCurrentSubCell()
{
  return getCurrentCell()->getCurrentSubCell();
}

void
CBrowserWindow::
updateSubCellWidth(int width)
{
  CHtmlLayoutCell *redraw_cell = getCurrentCell();

  redraw_cell->updateSubCellWidth(width);
}

void
CBrowserWindow::
updateSubCellHeight(int ascent, int descent)
{
  CHtmlLayoutCell *redraw_cell = getCurrentCell();

  redraw_cell->updateSubCellHeight(ascent, descent);
}

void
CBrowserWindow::
addCellRedrawData(CHtmlLayoutBox *box)
{
  CHtmlLayoutCell *cell = getCurrentCell();

  cell->addBox(box);
}

void
CBrowserWindow::
addSubCellRedrawData(CHtmlLayoutBox *box)
{
  CHtmlLayoutSubCell *sub_cell = getCurrentSubCell();

  if (sub_cell)
    sub_cell->addBox(box);
}

void
CBrowserWindow::
close()
{
}

void
CBrowserWindow::
setName(const std::string &name)
{
  name_ = name;
}

void
CBrowserWindow::
setDocument(const std::string &filename)
{
  delete document_;

  document_ = new CBrowserDocument(this);

  document_->read(filename);

  outputDocument();

  if (! history_->goTo(document_->getUrl()))
    history_->addUrl(document_->getUrl());
}

void
CBrowserWindow::
outputDocument()
{
  if (! document_)
    return;

  document_->freeLinks();

  //---

  layout_mgr_->init();

  area_data_->init();

  //---

  startArea(area_data_);

  //---

  newLine();

  //---

  mouse_link_ = nullptr;

  //---

  startFontFace(default_font_face_);

  setFontColor("#000000");

  //---

  document_->output();

  //---

  endArea();

  //---

  runScripts();

  //---

  resize();

  //---

  //CHtmlLayoutPrintVisitor printVistor;
  //area_data_->accept(printVistor);
}

void
CBrowserWindow::
recalc()
{
  iface_->setBusy();

  //---

  x_      = left_margin_;
  y_      = top_margin_;
  width_  = w_->width () - 2*left_margin_;
  height_ = w_->height() - 2*top_margin_;

  linkMgr()->deleteLinkRects();

  //---

  area_data_->setX     (left_margin_);
  area_data_->setY     (top_margin_ );
  area_data_->setWidth (w_->width () - 2*left_margin_);
  area_data_->setHeight(w_->height() - 2*top_margin_ );

  area_data_->format(layout_mgr_);

  //---

  iface_->setSize(area_data_->getWidth () + 2*left_margin_,
                  area_data_->getHeight() + 2*top_margin_);

  //---

  redraw();

  //---

  iface_->setReady();
}

void
CBrowserWindow::
resize()
{
  recalc();

  //---

  if (window_target_ != "") {
    int x, y;

    if (linkMgr()->getDestLinkPos(window_target_, &x, &y))
      iface_->scrollTo(x, y);
  }

  //---

  redraw();
}

void
CBrowserWindow::
redraw()
{
  w_->update();

  const QObjectList &objs = w_->children();

  for (int i = 0; i < objs.length(); ++i) {
    QWidget *w = qobject_cast<QWidget *>(objs[i]);

    if (w)
      w->update();
  }
}

void
CBrowserWindow::
drawDocument()
{
  area_data_->setX(left_margin_ - iface_->getCanvasXOffset());
  area_data_->setY(top_margin_  - iface_->getCanvasYOffset());

  area_data_->redraw(layout_mgr_);
}

void
CBrowserWindow::
gotoDocument(const std::string &url)
{
  setDocument(url);
}

void
CBrowserWindow::
setTitle(const std::string &title)
{
  iface_->setTitle(title);
}

void
CBrowserWindow::
setMargins(int left, int top)
{
  left_margin_ = left;
  top_margin_  = top;
}

void
CBrowserWindow::
setBackgroundImage(const std::string &filename, bool fixed)
{
  if (filename == "")
    return;

  CImageFileSrc file(filename);

  CImagePtr image = CImageMgrInst->createImage(file);

  if (! image)
    fprintf(stderr, "Illegal Backgound Image Type %s\n", filename.c_str());

  bg_image_ = image;
  bg_fixed_ = fixed;
}

void
CBrowserWindow::
setTarget(const std::string &target)
{
  window_target_ = target;
}

void
CBrowserWindow::
indentLeft(int offset)
{
  CHtmlLayoutArea *area = getCurrentArea();

  area->setIndentLeft(area->getIndentLeft () + offset*8);
}

void
CBrowserWindow::
indentRight(int offset)
{
  CHtmlLayoutArea *area = getCurrentArea();

  area->setIndentRight(area->getIndentRight() + offset*8);
}

void
CBrowserWindow::
newLine()
{
  layout_mgr_->newCellBelow();
}

void
CBrowserWindow::
newColumn()
{
  layout_mgr_->newCellRight();
}

CHtmlLayoutSubCell *
CBrowserWindow::
newSubCellBelow(bool breakup)
{
  return layout_mgr_->newSubCellBelow(breakup);
}

CHtmlLayoutSubCell *
CBrowserWindow::
newSubCellRight(bool breakup)
{
  return layout_mgr_->newSubCellRight(breakup);
}

void
CBrowserWindow::
skipLine()
{
  newLine();

  addLabel(" ", 1, CHALIGN_TYPE_LEFT, currentFontColor_);

  newLine();
}

void
CBrowserWindow::
setAlign(CHAlignType halign, CVAlignType valign)
{
  layout_mgr_->setAlign(halign, valign);
}

void
CBrowserWindow::
setStatus(const std::string &status)
{
  iface_->setStatus(status);
}

void
CBrowserWindow::
displayError(const char *format, ...)
{
  va_list args;

  if (CBrowserMainInst->getQuiet())
    return;

  va_start(args, format);

  vfprintf(stderr, format, args);

  va_end(args);
}

bool
CBrowserWindow::
isVisible(int x1, int y1, int x2, int y2)
{
  if (x1 > x_ + width_  || x2 < x_ ||
      y1 > y_ + height_ || y2 < y_)
    return false;
  else
    return true;
}

void
CBrowserWindow::
print()
{
  double xmin = 0;
  double ymin = 0;
  double xmax = getAreaData()->getWidth () + 2*getLeftMargin();
  double ymax = getAreaData()->getHeight() + 2*getTopMargin();

  iface_->print(xmin, ymin, xmax, ymax);
}

void
CBrowserWindow::
goBack()
{
  std::string url = history_->goBack();

  if (url == "")
    return;

  setDocument(url);
}

void
CBrowserWindow::
goForward()
{
  std::string url = history_->goForward();

  if (url == "")
    return;

  setDocument(url);
}

bool
CBrowserWindow::
hoverLink(int x, int y, std::string &link_name)
{
  CBrowserLink *link = linkMgr()->getSourceLink(x, y);

  if (mouse_link_ == link)
    return false;

  if (link) {
    mouse_link_ = link;

    link_name = link->getTitle();
  }
  else {
    mouse_link_ = nullptr;

    link_name = "";
  }

  setStatus(link_name);

  return true;
}

bool
CBrowserWindow::
activateLink(int x, int y)
{
  CBrowserLink *link = linkMgr()->getSourceLink(x, y);

  if (link)
    gotoDocument(link->getDest());

  return true;
}

void
CBrowserWindow::
addHistoryItem(const std::string &item)
{
  iface_->addHistoryItem(item);
}

CRGBA
CBrowserWindow::
getBg()
{
  if (document_)
    return document_->getBgColor();
  else
    return CRGBA();
}

CRGBA
CBrowserWindow::
getFg()
{
  if (document_)
    return document_->getFgColor();
  else
    return CRGBA();
}

void
CBrowserWindow::
drawImage(int x, int y, const CImagePtr &image)
{
  w_->drawImage(x, y, image);
}

void
CBrowserWindow::
drawImage(int x, int y, const QImage &image)
{
  w_->drawImage(x, y, image);
}

void
CBrowserWindow::
drawRectangle(int x1, int y1, int x2, int y2)
{
  w_->drawRectangle(x1, y1, x2, y2);
}

void
CBrowserWindow::
fillRectangle(int x1, int y1, int x2, int y2)
{
  w_->fillRectangle(x1, y1, x2, y2);
}

void
CBrowserWindow::
drawCircle(int x, int y, int r)
{
  w_->drawCircle(x, y, r);
}

void
CBrowserWindow::
fillCircle(int x, int y, int r)
{
  w_->fillCircle(x, y, r);
}

void
CBrowserWindow::
drawLine(int x1, int y1, int x2, int y2)
{
  w_->drawLine(x1, y1, x2, y2);
}

void
CBrowserWindow::
drawString(int x, int y, const std::string &str)
{
  w_->drawString(x, y, str);
}

void
CBrowserWindow::
drawOutline(int x, int y, int width, int height, const std::string &color_name)
{
  if (CEnvInst.exists("HTML_OUTLINE"))
    w_->drawOutline(x, y, width, height, color_name);
}

void
CBrowserWindow::
drawOutline(int x, int y, int width, int height, const CRGBA &c)
{
  if (CEnvInst.exists("HTML_OUTLINE"))
    w_->drawOutline(x, y, width, height, c);
}

void
CBrowserWindow::
drawSelected(int x, int y, int width, int height)
{
  w_->drawOutline(x, y, width, height, CRGBA(1,0,0));
}

void
CBrowserWindow::
drawBorder(int x, int y, int width, int height, CBrowserBorderType type)
{
  w_->drawBorder(x, y, width, height, type);
}

void
CBrowserWindow::
drawHRule(int x1, int x2, int y, int height)
{
  w_->drawHRule(x1, x2, y, height);
}

void
CBrowserWindow::
setForeground(const CRGBA &fg)
{
  w_->setForeground(fg);
}

void
CBrowserWindow::
setFont(CFontPtr font)
{
  w_->setFont(font);
}

void
CBrowserWindow::
getTextSize(const std::string &text, int *width, int *ascent, int *descent) const
{
  CFontPtr font = getFont();

  *width   = font->getStringWidth(text);
  *ascent  = font->getCharAscent();
  *descent = font->getCharDescent();
}

void
CBrowserWindow::
getTextWidth(CFontPtr font, const std::string &text, int *width) const
{
  *width = font->getStringWidth(text);
}

void
CBrowserWindow::
getTextHeight(CFontPtr font, int *ascent, int *descent) const
{
  *ascent  = font->getCharAscent();
  *descent = font->getCharDescent();
}

void
CBrowserWindow::
errorDialog(const std::string &msg)
{
  iface_->errorDialog(msg);
}
