#include <CBrowserText.h>
#include <CBrowserWindow.h>
#include <CBrowserLink.h>
#include <CFont.h>

CBrowserText::
CBrowserText(CBrowserWindow *window, const std::string &text) :
 CBrowserObject(window, CHtmlTagId::TEXT), text_(text)
{
  setDisplay(Display::INLINE);

  link_ = window->linkMgr()->getCurrentLink();
}

CBrowserText::
CBrowserText(CBrowserWindow *window, const CBrowserText &draw_text, const std::string &text) :
 CBrowserObject(window, CHtmlTagId::TEXT), text_(text), link_(draw_text.link_)
{
  setDisplay(Display::INLINE);
}

CBrowserText::
~CBrowserText()
{
}

void
CBrowserText::
getInlineWords(Words &words) const
{
  bool selected = isHierSelected();

  CFontPtr                   font = hierFont();
  CPen                       pen  = hierFgColor();
  CBrowserObject::WhiteSpace ws   = hierWhiteSpace();

  // whitespace:
  //  normal  : collapse whitespace, suppress line break (wrapping) and break lines to fix in box
  //  nowrap  : collapse whitespace, break lines at \n or <br>
  //  pre     : keep whitespace, break lines at \n or <br>
  //  pre-wrap: keep whitespace, break lines at \n or <br>, break lines to fix in box
  //  pre-line: collapse whitespace, break lines at \n or <br>, break lines to fix in box
  bool keep_newline (ws == CBrowserObject::WhiteSpace::PRE ||
                     ws == CBrowserObject::WhiteSpace::PRE_LINE ||
                     ws == CBrowserObject::WhiteSpace::PRE_WRAP);

  bool keep_space = (ws == CBrowserObject::WhiteSpace::PRE ||
                     ws == CBrowserObject::WhiteSpace::PRE_WRAP);

  bool text_wrap = (ws != CBrowserObject::WhiteSpace::NOWRAP &&
                    ws != CBrowserObject::WhiteSpace::PRE);

  // split text into words
  if      (! keep_newline && ! keep_space && text_wrap) {
    int i = 0;

    // skip space at start
    if (text_[i] != '\0' && isspace(text_[i])) {
      while (text_[i] != '\0' && isspace(text_[i]))
        i++;

      words.push_back(CBrowserWord(" ", pen, font, /*break*/false, selected));
    }

    while (text_[i] != '\0') {
      // get word
      int j = i;

      while (text_[i] != '\0' && ! isspace(text_[i]))
        i++;

      if (i - j == 0)
        break;

      std::string word = text_.substr(j, i - j);

      words.push_back(CBrowserWord(word, pen, font, /*break*/false, selected));

      //--

      // skip space after word
      if (text_[i] != '\0' && isspace(text_[i])) {
        while (text_[i] != '\0' && isspace(text_[i]))
          i++;

        words.push_back(CBrowserWord(" ", pen, font, /*break*/false, selected));
      }
    }
  }
  // no split
  else if (keep_newline && ! text_wrap) {
    words.push_back(CBrowserWord(text_, pen, font, /*break*/false, selected));
  }
  // split text into lines
  else {
    int i = 0;

    while (text_[i] != '\0') {
      while (text_[i] == '\n') {
        words.push_back(CBrowserWord("", pen, font, /*break*/true, selected));

        i++;
      }

      // get line
      int j = i;

      while (text_[i] != '\0' && text_[i] != '\n')
        i++;

      if (i > j) {
        std::string line = text_.substr(j, i - j);

        if (text_[i] == '\n') {
          words.push_back(CBrowserWord(line, pen, font, /*break*/true, selected));

          ++i;
        }
        else
          words.push_back(CBrowserWord(line, pen, font, /*break*/false, selected));
      }
    }
  }
}

CBrowserRegion
CBrowserText::
calcRegion() const
{
  CFontPtr font = hierFont();

  int width, ascent, descent;

  window_->getTextWidth (font, text_, &width);
  window_->getTextHeight(font, &ascent, &descent);

  return CBrowserRegion(width, ascent, descent);
}

void
CBrowserText::
draw(const CTextBox &region)
{
  fillBackground(region);

  //---

  CFontPtr font = hierFont();

  int width, ascent, descent;

  getTextBounds(font, text_, &width, &ascent, &descent);

  int y_offset = 0;

  int x1 = region.x();
  int y1 = region.y() + y_offset + region.ascent();

  CPen pen(hierFgColor());

  window_->drawText(x1, y1, text_, pen, font);

  if (link_)
    link_->addRect(x1, y1 - region.ascent(), x1 + width, y1 + region.descent());

  //---

  if (isHierSelected())
    window_->drawSelected(region.x(), region.y(), region.width(), region.height());

  //region.setX(region.x() + width);
}

bool
CBrowserText::
isHierSelected() const
{
  if (isSelected())
    return true;

  const CBrowserObject *parent = this->parent();

  if (parent)
    return parent->isHierSelected();

  return false;
}

void
CBrowserText::
getTextBounds(CFontPtr font, const std::string &text, int *width, int *ascent, int *descent)
{
  *width   = font->getStringWidth(text);
  *ascent  = font->getCharAscent();
  *descent = font->getCharDescent();
}

CFontPtr
CBrowserText::
hierFont() const
{
  CBrowserObject *parentObj = this->parent();

  if (parentObj)
    return parentObj->hierFont();

  return window_->getFont();
}

CRGBA
CBrowserText::
hierFgColor() const
{
  CBrowserObject *parentObj = this->parent();

  if (parentObj)
    return parentObj->hierFgColor();

  return window_->getFgColor();
}

CBrowserObject::WhiteSpace
CBrowserText::
hierWhiteSpace() const
{
  CBrowserObject *parentObj = this->parent();

  if (parentObj)
    return parentObj->hierWhiteSpace();

  return CBrowserObject::WhiteSpace::NORMAL;
}
