#ifndef CBrowserTitle_H
#define CBrowserTitle_H

#include <CBrowserObject.h>
#include <CBrowserData.h>

class CBrowserTitle : public CBrowserObject {
 public:
  CBrowserTitle(CBrowserWindow *window);

  void initLayout() override;
  void termLayout() override;
};

#endif
