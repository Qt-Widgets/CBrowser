#include <CBrowserIFace.h>
#include <CBrowserWindowWidget.h>
#include <CBrowserWindow.h>
#include <CBrowserScrolledWindow.h>
#include <CBrowserMain.h>
#include <CBrowserGraphics.h>
#include <CBrowserJS.h>
#include <CBrowserDomTree.h>
#include <CBrowserObject.h>
#include <CBrowserLayout.h>
#include <CBrowserBox.h>

#include <CQJDocument.h>
#include <CQJWindow.h>
#include <CQJEvent.h>
#include <CQJDialog.h>
#include <CQMenu.h>
#include <CQToolBar.h>
#include <CDir.h>
#include <CEnv.h>

#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QStatusBar>

#include <svg/dom_svg.h>
#include <svg/javascript_svg.h>

CBrowserIFace::
CBrowserIFace() :
 CQMainWindow("CBrowser")
{
  QWidget::resize(1000, 1200);
}

CBrowserIFace::
~CBrowserIFace()
{
}

void
CBrowserIFace::
init()
{
  CQMainWindow::init();
}

QWidget *
CBrowserIFace::
createCentralWidget()
{
  QWidget *widget = new QWidget;

  widget->setObjectName("iface");

  QVBoxLayout *layout = new QVBoxLayout(widget);

  input_ = new QLineEdit;

  input_->setObjectName("input");
  input_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  layout->addWidget(input_);

  connect(input_, SIGNAL(returnPressed()), this, SLOT(inputSlot()));

  //---

  tab_ = new QTabWidget;

  tab_->setObjectName("tab");
  tab_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  connect(tab_, SIGNAL(currentChanged(int)), this, SLOT(updateTitles()));

  layout->addWidget(tab_);

  //---

  addWindow();

  return widget;
}

CBrowserScrolledWindow *
CBrowserIFace::
addWindow()
{
  CBrowserScrolledWindow *window = new CBrowserScrolledWindow(this);

  tab_->addTab(window, "");

  tab_->setCurrentIndex(tab_->count() - 1);

  windows_.push_back(window);

  return window;
}

CBrowserScrolledWindow *
CBrowserIFace::
currentWindow() const
{
  int i = tab_->currentIndex();

  if (i >= 0 && i < int(windows_.size()))
    return windows_[i];

  return nullptr;
}

void
CBrowserIFace::
createMenus()
{
  CQMenu *fileMenu = new CQMenu(this, "&File");

  CQMenuItem *newMenuItem = new CQMenuItem(fileMenu, "&New");
  newMenuItem->setShortcut("Ctrl+N");
  newMenuItem->connect(this, SLOT(newProc()));

  CQMenuItem *readMenuItem = new CQMenuItem(fileMenu, "&Read");
  readMenuItem->setShortcut("Ctrl+R");
  readMenuItem->connect(this, SLOT(readProc()));

  CQMenuItem *printMenuItem = new CQMenuItem(fileMenu, "&Print");
  printMenuItem->connect(this, SLOT(printProc()));

  CQMenuItem *saveImageMenuItem = new CQMenuItem(fileMenu, "Save &Image");
  saveImageMenuItem->connect(this, SLOT(saveImageProc()));

  jsMenuItem_ = new CQMenuItem(fileMenu, "&JavaScript");
  jsMenuItem_->setIcon(CQPixmapCacheInst->getIcon("JAVASCRIPT"));
  jsMenuItem_->connect(this, SLOT(jsProc()));

  domMenuItem_ = new CQMenuItem(fileMenu, "&DOM");
  domMenuItem_->setIcon(CQPixmapCacheInst->getIcon("DOM"));
  domMenuItem_->connect(this, SLOT(domProc()));

  CQMenuItem *quitMenuItem = new CQMenuItem(fileMenu, "&Quit");
  quitMenuItem->setShortcut("Ctrl+Q");
  quitMenuItem->connect(this, SLOT(quitProc()));

  //---

  CQMenu *goMenu = new CQMenu(this, "&Go");

  CQMenuItem *backMenuItem = new CQMenuItem(goMenu, "&Back");

  backMenuItem->connect(this, SLOT(goBackProc()));

  CQMenuItem *forwardMenuItem = new CQMenuItem(goMenu, "&Forward");

  forwardMenuItem->connect(this, SLOT(goForwardProc()));

  //---

  CQMenu *viewMenu = new CQMenu(this, "&View");

  CQMenuItem *viewBoxes = new CQMenuItem(viewMenu, "&Boxes");
  viewBoxes->setCheckable(true);
  viewBoxes->connect(this, SLOT(viewBoxesProc()));

  CQMenuItem *mouseOver = new CQMenuItem(viewMenu, "&Mouse Over");
  mouseOver->setCheckable(true);
  mouseOver->connect(this, SLOT(mouseOverProc()));

  //---

  historyMenu_ = new CQMenu(this, "&History");

  //---

  CQMenu *helpMenu = new CQMenu(this, "&Help");

  new CQMenuItem(helpMenu, "&Help");
}

void
CBrowserIFace::
createToolBars()
{
  toolbar_ = new CQToolBar(this, "Tools");

  toolbar_->addItem(jsMenuItem_);
  toolbar_->addItem(domMenuItem_);
}

void
CBrowserIFace::
createStatusBar()
{
  message_ = new QLabel(" ");

  statusBar()->addWidget(message_);

  objLabel_ = new QLabel;

  objLabel_->setObjectName("obj");

  statusBar()->addPermanentWidget(objLabel_ );

  posLabel_ = new QLabel;

  posLabel_->setObjectName("pos");

  statusBar()->addPermanentWidget(posLabel_ );
}

void
CBrowserIFace::
inputSlot()
{
  QString url = input_->text();

  if (url != "")
    setDocument(url.toStdString());
}

void
CBrowserIFace::
addHistoryItem(const std::string &item)
{
  CQMenuItem *menuItem = new CQMenuItem(historyMenu_, "button");

  menuItem->setName(item);
}

void
CBrowserIFace::
saveImage(const std::string &filename)
{
  CBrowserScrolledWindow *w = currentWindow();
  if (! w) return;

  w->saveImage(filename);
}

void
CBrowserIFace::
setTitle(const std::string &title)
{
  setWindowTitle(title.c_str());
}

void
CBrowserIFace::
updateTitles()
{
  setWindowTitle("CBrowser");

  int ind = tab_->currentIndex();

  for (int i = 0; i < tab_->count(); ++i) {
    CBrowserScrolledWindow *w = qobject_cast<CBrowserScrolledWindow *>(tab_->widget(i));
    if (! w) continue;

    CBrowserWindow *window = w->getWindow();

    tab_->setTabText(i, window->filename().c_str());

    if (i == ind)
      setWindowTitle(w->title().c_str());
  }
}

void
CBrowserIFace::
setStatus(const std::string &status)
{
  if (status != "")
    message_->setText(status.c_str());
  else
    message_->setText(" ");
}

void
CBrowserIFace::
errorDialog(const std::string &msg)
{
  QMessageBox::warning(this, "Error", msg.c_str());
}

void
CBrowserIFace::
setBusy()
{
}

void
CBrowserIFace::
setReady()
{
}

void
CBrowserIFace::
newProc()
{
  std::string home = CDir::getHome();

  std::string directory = home + "/data/html";

  //---

  QString file =
    QFileDialog::getOpenFileName(this, "Select HTML File", directory.c_str(), "*.html");

  if (file == "")
    return;

  //---

  addDocument(file.toStdString());
}

void
CBrowserIFace::
readProc()
{
  std::string home = CDir::getHome();

  std::string directory = home + "/data/html";

  //---

  QString file =
    QFileDialog::getOpenFileName(this, "Select HTML File", directory.c_str(), "*.html");

  if (file == "")
    return;

  //---

  setDocument(file.toStdString());
}

void
CBrowserIFace::
addDocument(const std::string &filename)
{
  CBrowserScrolledWindow *window = addWindow();

  window->setDocument(filename);
}

void
CBrowserIFace::
setDocument(const std::string &filename)
{
  CBrowserScrolledWindow *window = currentWindow();
  if (! window) return;

  window->setDocument(filename);
}

void
CBrowserIFace::
printProc()
{
  CBrowserScrolledWindow *window = currentWindow();
  if (! window) return;

  window->print();
}

void
CBrowserIFace::
saveImageProc()
{
  QString fileName =
    QFileDialog::getSaveFileName(this, "Save Image", "image.png",
                                 "Image Files (*.png *.xpm *.jpg)");

  if (fileName.length())
    saveImage(fileName.toStdString());
}

void
CBrowserIFace::
jsProc()
{
  if (! jsDlg_)
    jsDlg_ = new CQJDialog(CBrowserJSInst->js());

  jsDlg_->show();
}

void
CBrowserIFace::
domProc()
{
  CBrowserScrolledWindow *swindow = currentWindow();
  if (! swindow) return;

  CBrowserWindow *window = swindow->getWindow();

  if (domDlg_ && domDlg_->window() != window) {
    delete domDlg_;

    domDlg_ = nullptr;
  }

  if (! domDlg_)
    domDlg_ = new CBrowserDomTreeDlg(window);

  domDlg_->show();
}

void
CBrowserIFace::
goBackProc()
{
  CBrowserScrolledWindow *window = currentWindow();
  if (! window) return;

  window->goBack();
}

void
CBrowserIFace::
goForwardProc()
{
  CBrowserScrolledWindow *window = currentWindow();
  if (! window) return;

  window->goForward();
}

void
CBrowserIFace::
viewBoxesProc()
{
  CBrowserMainInst->setShowBoxes(! CBrowserMainInst->getShowBoxes());
}

void
CBrowserIFace::
mouseOverProc()
{
  CBrowserMainInst->setMouseOver(! CBrowserMainInst->getMouseOver());
}

void
CBrowserIFace::
quitProc()
{
  close();
}
