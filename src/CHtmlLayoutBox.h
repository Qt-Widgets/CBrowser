#ifndef CHtmlLayoutBox_H
#define CHtmlLayoutBox_H

#include <CHtmlLayoutTypes.h>
#include <iostream>

class CHtmlLayoutMgr;
class CHtmlLayoutVisitor;

class CHtmlLayoutBox {
 public:
  CHtmlLayoutBox() { }

  virtual ~CHtmlLayoutBox() { }

  virtual void format(CHtmlLayoutMgr *layout) = 0;

  virtual void draw(CHtmlLayoutMgr *layout, const CHtmlLayoutRegion &region) = 0;

  void accept(CHtmlLayoutVisitor &visitor);

  virtual void print(std::ostream &) { }
};

#endif
