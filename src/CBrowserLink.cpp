#include <CBrowserLink.h>
#include <CBrowserWindow.h>
#include <CBrowserDocument.h>
#include <CBrowserProperty.h>
#include <CUrl.h>
#include <CDir.h>

CBrowserLinkMgr::
CBrowserLinkMgr(CBrowserWindow *window) :
 window_(window)
{
}

void
CBrowserLinkMgr::
startSourceLink(const CBrowserLinkData &data)
{
  if (current_link_) {
    fprintf(stderr, "Invalid Link within a Link\n");
    return;
  }

  std::string href1 = expandDestLink(data.href);

  if (href1 == "")
    href1 = "????";

  std::string title1 = data.title;

  if (title1 == "")
    title1 = href1;

  CBrowserAnchorLink *link =
    new CBrowserAnchorLink(CBrowserAnchorLink::Type::SOURCE, "", href1, title1);

  window_->getDocument()->addLink(link);

  current_link_ = link;
}

void
CBrowserLinkMgr::
startDestLink(const CBrowserLinkData &data)
{
  if (current_link_) {
    fprintf(stderr, "Invalid Link within a Link\n");
    return;
  }

  std::string title1 = data.title;

  if (title1 == "")
    title1 = data.id;

  CBrowserAnchorLink *link =
    new CBrowserAnchorLink(CBrowserAnchorLink::Type::DEST, data.id, "", title1);

  window_->getDocument()->addAnchor(link);

  current_link_ = link;
}

void
CBrowserLinkMgr::
endLink()
{
  if (! current_link_)
    return;

  current_link_ = nullptr;
}

CBrowserAnchorLink *
CBrowserLinkMgr::
getCurrentLink()
{
  return current_link_;
}

void
CBrowserLinkMgr::
deleteLinkRects()
{
  if (! window_->getDocument())
    return;

  int num_links = window_->getDocument()->getNumLinks();

  for (int i = 0; i < num_links; i++) {
    CBrowserAnchorLink *link = window_->getDocument()->getLink(i);

    link->deleteRects();
  }

  int num_anchors = window_->getDocument()->getNumAnchors();

  for (int i = 0; i < num_anchors; i++) {
    CBrowserAnchorLink *anchor = window_->getDocument()->getAnchor(i);

    anchor->deleteRects();
  }
}

CBrowserAnchorLink *
CBrowserLinkMgr::
getSourceLink(int x, int y)
{
  if (! window_->getDocument())
    return nullptr;

  int num_links = window_->getDocument()->getNumLinks();

  for (int i = 0; i < num_links; i++) {
    CBrowserAnchorLink *link = window_->getDocument()->getLink(i);

    if (link->getType() != CBrowserAnchorLink::Type::SOURCE)
      continue;

    int num_rects = link->getNumRects();

    for (int j = 0; j < num_rects; j++) {
      CBrowserLinkRect *rect = link->getRect(j);

      if (x >= rect->x1 && x <= rect->x2 && y >= rect->y1 && y <= rect->y2)
        return link;
    }
  }

  return nullptr;
}

int
CBrowserLinkMgr::
getDestLinkPos(const std::string &name, int *x, int *y)
{
  if (! window_->getDocument())
    return false;

  int num_links = window_->getDocument()->getNumAnchors();

  for (int i = 0; i < num_links; i++) {
    CBrowserAnchorLink *anchor = window_->getDocument()->getAnchor(i);

    if (anchor->getType() != CBrowserAnchorLink::Type::DEST)
      continue;

    if (anchor->getName() == name)
      continue;

    int num_rects = anchor->getNumRects();

    if (num_rects > 0) {
      CBrowserLinkRect *rect = anchor->getRect(0);

      *x = rect->x1;
      *y = rect->y1;

      return true;
    }
  }

  return false;
}

std::string
CBrowserLinkMgr::
expandDestLink(const std::string &dest) const
{
  int len = dest.size();

  int i = 0;

  CStrUtil::skipSpace(dest, &i);

  int j = i;

  while (i < len && dest[i] != ':')
    i++;

  std::string prefix;

  if (dest[i] == ':') {
    prefix = dest.substr(j, i - j);

    prefix = CStrUtil::stripSpaces(prefix);

    i++;
  }
  else {
    prefix = "file";

    i = 0;
  }

  CStrUtil::skipSpace(dest, &i);

  std::string dest1;

  if     (prefix.substr(0, 4) == "file") {
    if (dest[i] != '/') {
      std::string current_dir = CDir::getCurrent();

      dest1 = current_dir + "/" + dest.substr(i);
    }
    else
      dest1 = dest.substr(i);
  }
  else
    dest1 = dest[i];

  dest1 = CStrUtil::stripSpaces(dest1);

  std::string dest2 = prefix + "://" + dest1;

  return dest2;
}

//------

CBrowserAnchorLink::
CBrowserAnchorLink(Type type, const std::string &name, const std::string &dest,
                   const std::string &title) :
 type_(type), name_(name), dest_(dest), title_(title)
{
}

CBrowserAnchorLink::
~CBrowserAnchorLink()
{
  deleteRects();
}

int
CBrowserAnchorLink::
getNumRects()
{
  return rects_.size();
}

CBrowserLinkRect *
CBrowserAnchorLink::
getRect(int i)
{
  return rects_[i];
}

void
CBrowserAnchorLink::
addRect(int x1, int y1, int x2, int y2)
{
  CBrowserLinkRect *rect = new CBrowserLinkRect;

  rect->x1 = x1;
  rect->y1 = y1;
  rect->x2 = x2;
  rect->y2 = y2;

  rects_.push_back(rect);
}

void
CBrowserAnchorLink::
deleteRects()
{
  int num_rects = rects_.size();

  for (int i = 0; i < num_rects; i++)
    delete rects_[i];

  rects_.clear();
}

//------

CBrowserAnchor::
CBrowserAnchor(CBrowserWindow *window, const CBrowserLinkData &data) :
 CBrowserObject(window, CHtmlTagId::A, data), data_(data)
{
  setDisplay(CBrowserObject::Display::INLINE);

  setForeground(CBrowserColor(window_->getDocument()->getLinkColor()));

  textProp_.setDecoration(CBrowserTextDecoration("underline"));
}

CBrowserAnchor::
~CBrowserAnchor()
{
}

void
CBrowserAnchor::
setNameValue(const std::string &name, const std::string &value)
{
  std::string lname = CStrUtil::toLower(name);

  if      (lname == "download") {
    data_.download = value;
  }
  else if (lname == "href") {
    data_.href = value;
  }
  else if (lname == "methods") {
    data_.methods = value;
  }
  else if (lname == "rel") {
    data_.rel = value;
  }
  else if (lname == "rev") {
    data_.rev = value;
  }
  else if (lname == "target") {
    data_.target = value;
  }
  else if (lname == "type") {
    data_.type = value;
  }
  else if (lname == "title") {
    data_.title = value;
  }
  else if (lname == "url") {
    data_.url = value;
  }
  else if (lname == "itemprop") {
  }
  else {
    CBrowserObject::setNameValue(name, value);
  }
}

void
CBrowserAnchor::
init()
{
  std::vector<std::string> strs = {{
    "download", "href", "methods", "rel", "rev", "target", "title", "url" }};

  addProperties(strs);

  //---

  if (data_.href == "" && data_.id == "")
    window_->displayError("No 'href' or 'name' specified for 'a' Tag'\n");
}

void
CBrowserAnchor::
initProcess()
{
  if (data_.href != "") {
    window_->linkMgr()->startSourceLink(data_);
  }
  else
    window_->linkMgr()->startDestLink(data_);
}

void
CBrowserAnchor::
termProcess()
{
  window_->linkMgr()->endLink();
}

std::string
CBrowserAnchor::
propertyValue(int i) const
{
  const std::string &name = propertyName(i);

  if      (name == "download") return CBrowserProperty::toString(data_.download);
  else if (name == "href"    ) return CBrowserProperty::toString(data_.href);
  else if (name == "methods" ) return CBrowserProperty::toString(data_.methods);
  else if (name == "rel"     ) return CBrowserProperty::toString(data_.rel);
  else if (name == "rev"     ) return CBrowserProperty::toString(data_.rev);
  else if (name == "target"  ) return CBrowserProperty::toString(data_.target);
  else if (name == "title"   ) return CBrowserProperty::toString(data_.title);
  else if (name == "url"     ) return CBrowserProperty::toString(data_.url);

  return CBrowserObject::propertyValue(i);
}

//------

CBrowserLink::
CBrowserLink(CBrowserWindow *window, const CBrowserLinkData &data) :
 CBrowserObject(window, CHtmlTagId::LINK), data_(data)
{
}

CBrowserLink::
~CBrowserLink()
{
}

void
CBrowserLink::
init()
{
}

void
CBrowserLink::
setNameValue(const std::string &name, const std::string &value)
{
  std::string lname  = CStrUtil::toLower(name);
  std::string lvalue = CStrUtil::toLower(value);

  if      (lname == "rel") {
    data_.rel = value;
  }
  else if (lname == "href") {
    data_.href = value;
  }
  else {
    CBrowserObject::setNameValue(name, value);
  }
}

void
CBrowserLink::
initProcess()
{
  if (data_.rel == "stylesheet") {
    std::string href = window_->linkMgr()->expandDestLink(data_.href);

    if (href == "")
      return;

    CUrl url(href);

  //std::string prefix = url.getPrefix();
    std::string file   = url.getFile();
  //std::string target = url.getTarget();

    if (! window_->loadCSSFile(file))
      return;
  }
}

void
CBrowserLink::
termProcess()
{
}
