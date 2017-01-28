#ifndef CBrowserPosition_H
#define CBrowserPosition_H

#include <CStrUtil.h>

class CBrowserPosition {
 public:
  enum class Type {
    INVALID,
    STATIC,
    ABSOLUTE,
    FIXED,
    RELATIVE,
    INITIAL,
    INHERIT
  };

 public:
  CBrowserPosition() { }

  explicit CBrowserPosition(const std::string &str) :
   type_(stringToType(str)) {
  }

  const Type &type() const { return type_; }
  void setType(const Type &t) { type_ = t; }

  const CBrowserUnitValue &top() const { return top_; }
  void setTop(const CBrowserUnitValue &v) { top_ = v; }

  const CBrowserUnitValue &left() const { return left_; }
  void setLeft(const CBrowserUnitValue &v) { left_ = v; }

  void setType(const std::string &str) { type_ = stringToType(str); }

 public:
  static Type stringToType(const std::string &str) {
    std::string lstr = CStrUtil::toLower(str);

    if (lstr == "static"  ) return Type::STATIC;
    if (lstr == "absolute") return Type::ABSOLUTE;
    if (lstr == "fixed"   ) return Type::FIXED;
    if (lstr == "relative") return Type::RELATIVE;
    if (lstr == "initial" ) return Type::INITIAL;
    if (lstr == "inherit" ) return Type::INHERIT;

    return Type::INVALID;
  }

 private:
  Type              type_ { Type::INVALID };
  CBrowserUnitValue top_;
  CBrowserUnitValue left_;
};

#endif
