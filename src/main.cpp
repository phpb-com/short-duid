#include <node.h>
#include "shortduid.h"

namespace shortduid {

  using v8::Local;
  using v8::Object;

  void InitAll(Local<Object> exports) {
    ShortDUID::Init(exports);
  }

  NODE_MODULE(addon, InitAll)

}  // namespace shortduid
// vim: syntax=cpp11:ts=2:sw=2
