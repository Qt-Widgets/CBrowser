#ifndef CBrowserHead_H
#define CBrowserHead_H

#include <CBrowserObject.h>

class CBrowserHead : public CBrowserObject {
 public:
  CBrowserHead(CBrowserWindow *window);

  void init();

  void setNameValue(const std::string &name, const std::string &value) override;
};

#endif