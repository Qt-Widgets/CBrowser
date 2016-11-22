#include <CQJObject.h>
#include <CBrowserJS.h>
#include <CJFunction.h>
#include <CJavaScript.h>

CQJObject::
CQJObject(CJavaScript *js, const CJObjTypeP &type) :
 CJObj(js, type)
{
  objType_->addObjFunction(js, "addEventListener", objType_);

  CBrowserJSInst->addJSObject(this);
}

CQJObject::
~CQJObject()
{
  CBrowserJSInst->removeJSObject(this);
}

CJValueP
CQJObject::
execNameFn(CJavaScript *, const std::string &name, const Values &values)
{
  if      (name == "addEventListener") {
    if (values.size() >= 2) {
      std::string id = values[1]->toString();
      CJValueP    fn = values[2];

      eventListeners_[id] = fn;
    }

    return CJValueP();
  }
  else
    return CJValueP();
}

bool
CQJObject::
callEventListener(const std::string &name, const std::string &prop,
                  const EventArgs &args, const NameValues &nameValues)
{
  CJValueP value = getProperty(js_, prop);

  if (callEventListener(value, args, nameValues))
    return true;

  auto p = eventListeners_.find(name);

  if (p != eventListeners_.end()) {
    if (callEventListener((*p).second, args, nameValues))
      return true;
  }

  return false;
}

bool
CQJObject::
callEventListener(CJValueP value, const EventArgs &args, const NameValues &nameValues)
{
  if (! value || value->type() != CJToken::Type::Function)
    return false;

  CJFunctionBaseP fn = CJValue::cast<CJFunctionBase>(value);

  if (fn->type() == CJFunctionBase::Type::User) {
    CJFunctionP userFn = CJValue::cast<CJFunction>(value);

    for (auto &nv : nameValues)
      userFn->setProperty(js_, nv.first, nv.second);
  }

  CJObjType::Values fnValues;

  fnValues.push_back(shared_from_this());

  for (auto &arg : args)
    fnValues.push_back(arg);

  fn->exec(js_, fnValues);

  return true;
}