#ifndef CBrowserBreak_H
#define CBrowserBreak_H

#include <CBrowserObject.h>
#include <CBrowserData.h>

class CBrowserBreak : public CBrowserObject {
 public:
  explicit CBrowserBreak(CBrowserWindow *window);

 ~CBrowserBreak();

  CBrowserClear::Type getClear() const { return data_.clear; }

  void init();

  void setNameValue(const std::string &name, const std::string &value) override;

  void getInlineWords(Words &words) const;

  CBrowserRegion calcRegion() const override;

  bool isBreak() const override { return true; }

 private:
  CBrowserBreakData data_;
};

//---

class CBrowserWbr : public CBrowserObject {
 public:
  explicit CBrowserWbr(CBrowserWindow *window);
 ~CBrowserWbr();
};

#endif
