#include "shortduid.h"

namespace shortduid {

  using v8::Function;
  using v8::FunctionCallbackInfo;
  using v8::FunctionTemplate;
  using v8::HandleScope;
  using v8::Isolate;
  using v8::Local;
  using v8::Number;
  using v8::Object;
  using v8::Persistent;
  using v8::String;
  using v8::Value;

  Persistent<Function> ShortDUID::constructor;

  ShortDUID::ShortDUID(const uint32_t shard_id, const std::string salt, const uint64_t epoch_start) : salt_(salt), epoch_start_(epoch_start), shard_id_(shard_id), hash(salt, 0, DEFAULT_ALPHABET) {
    time_offset_ = 0; // Mainly used in tests, applied to the time before ID is generated
    sequence_ = 0ULL; // Sub-millisecond sequence

    //Setup time related variables
    auto mono_time = (uint64_t) std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    system_time_at_start_ = (uint64_t) std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    // In case steady clock do not show same time as system clock
    mono_epoch_diff_ = system_time_at_start_ - mono_time;

    shard_id_ &= ((1ULL << 10) - 1); //Ensure that shard_id is no larger than 10 bits integer
    std::fill(std::begin(ts_seq_), std::end(ts_seq_), 0ULL); //This is used to track overflow of sequence per unit of time
    //Check to see if custom epoch does not overflow current time and reset it to 0 if it does
    if(epoch_start_ > system_time_at_start_) {
      epoch_start_ = 0ULL;
    }
  }

  ShortDUID::~ShortDUID() {
  }

  void ShortDUID::Init(Local<Object> exports) {
    auto isolate = Isolate::GetCurrent();

    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New);
    tpl->SetClassName(String::NewFromUtf8(isolate, "ShortDUID"));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    NODE_SET_PROTOTYPE_METHOD(tpl, "getDUID", GetDUID);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getDUIDInt", GetDUIDInt);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getShardID", GetShardID);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getEpochStart", GetEpochStart);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getSalt", GetSalt);
    NODE_SET_PROTOTYPE_METHOD(tpl, "hashidEncode", HashidEncode);
    NODE_SET_PROTOTYPE_METHOD(tpl, "hashidDecode", HashidDecode);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getRandomAPIKey", GetRandomAPIKey);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getRandomPassword", GetRandomPassword);
    NODE_SET_PROTOTYPE_METHOD(tpl, "driftTime", DriftTime);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getCurrentTimeMs", GetCurrentTimeMs);

    constructor.Reset(isolate, tpl->GetFunction());
    exports->Set(String::NewFromUtf8(isolate, "ShortDUID"), tpl->GetFunction());
  }

  void ShortDUID::New(const FunctionCallbackInfo<Value>& args) {
    auto isolate = args.GetIsolate();

    if (args.IsConstructCall()) {
      std::string salt("");
      // Invoked as constructor: `new ShortDUID(...)`
      uint32_t shard_id    = std::abs(args[0]->IsUndefined() ? 0 : args[0]->IntegerValue()) & ((1UL << 10) - 1);
      uint64_t epoch_start = 0;

      if(!args[2]->IsUndefined()) {
        String::Utf8Value _s_int64(args[2]->ToString());
        epoch_start = std::strtoll(*_s_int64, NULL, 10);
      }

      if(!args[1]->IsUndefined()) {
        String::Utf8Value _salt(args[1]->ToString());
        salt = *_salt;
      }

      ShortDUID* obj = new ShortDUID(shard_id, salt, epoch_start);
      obj->Wrap(args.This());
      args.GetReturnValue().Set(args.This());
    } else {
      // Invoked as plain function `ShortDUID(...)`, turn into construct call.
      const int argc = 3;
      Local<Value> argv[] = { args[0], args[1], args[2] };
      Local<Function> cons = Local<Function>::New(isolate, constructor);
      args.GetReturnValue().Set(cons->NewInstance(argc, argv));
    }
  }

  void ShortDUID::DriftTime(const FunctionCallbackInfo<Value>& args) {
    auto isolate = args.GetIsolate();
    auto obj = ObjectWrap::Unwrap<ShortDUID>(args.Holder());

    if(!args[0]->IsUndefined()) {
      obj->time_offset_ = args[0]->IntegerValue();
    }

    std::string offset_str(std::to_string(obj->time_offset_));

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, offset_str.c_str()));
  }

  void ShortDUID::GetCurrentTimeMs(const FunctionCallbackInfo<Value>& args) {
    auto isolate = args.GetIsolate();
    auto obj = ObjectWrap::Unwrap<ShortDUID>(args.Holder());

    uint64_t milliseconds_since_epoch = obj->mono_epoch_diff_ + (uint64_t) std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

    std::string milliseconds_since_epoch_str(std::to_string(milliseconds_since_epoch));

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, milliseconds_since_epoch_str.c_str()));
  }

  void ShortDUID::GetDUIDInt(const FunctionCallbackInfo<Value>& args) {
    // Method to return unique uint64 integers in a string form, wrapped in JS array
    auto isolate = args.GetIsolate();
    auto obj = ObjectWrap::Unwrap<ShortDUID>(args.Holder());

    unsigned short cnt   = std::abs(args[0]->IsUndefined() ? 1 : args[0]->IntegerValue());
    cnt = (cnt > 8192) ? 1 : cnt; // Check boundaries
    v8::Handle<v8::Array> numArr = v8::Array::New( isolate, cnt );

    for(auto i = 0; i < cnt; ++i) {
      numArr->Set( v8::Number::New(isolate, i), String::NewFromUtf8(isolate, std::to_string(obj->GetUniqueID(args)).c_str()) );
    }

    args.GetReturnValue().Set(numArr);
  }

  void ShortDUID::GetDUID(const FunctionCallbackInfo<Value>& args) {
    // Method to return unique hashed IDs in a string form, wrapped in JS array
    auto isolate = args.GetIsolate();
    auto obj = ObjectWrap::Unwrap<ShortDUID>(args.Holder());

    unsigned short cnt   = std::abs(args[0]->IsUndefined() ? 1 : args[0]->IntegerValue());
    cnt = (cnt > 8192) ? 1 : cnt; // Check boundaries
    v8::Handle<v8::Array> strArr = v8::Array::New( isolate, cnt );

    for(auto i = 0; i < cnt; ++i) {
      strArr->Set( v8::Number::New(isolate, i), String::NewFromUtf8(isolate, obj->hash.encode({obj->GetUniqueID(args)}).c_str()) );
    }

    args.GetReturnValue().Set(strArr);
  }

  void ShortDUID::HashidEncode(const FunctionCallbackInfo<Value>& args) {
    auto isolate = args.GetIsolate();
    auto obj = ObjectWrap::Unwrap<ShortDUID>(args.Holder());

    v8::Handle<v8::Array> numArr = v8::Handle<v8::Array>::Cast(args[0]);
    std::vector<uint64_t> v;
    v.reserve(numArr->Length());
    for (unsigned int i = 0; i < numArr->Length(); ++i) {
      String::Utf8Value u_uint64(numArr->Get(i)->ToString());
      auto IntVal(std::strtoull(*u_uint64, NULL, 10));
      v.push_back(IntVal);
    }

    std::string _hash(obj->hash.encode(v.begin(), v.end()));
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, _hash.c_str()));
  }

  void ShortDUID::HashidDecode(const FunctionCallbackInfo<Value>& args) {
    auto isolate = args.GetIsolate();
    auto obj = ObjectWrap::Unwrap<ShortDUID>(args.Holder());

    std::vector<uint64_t> uInt64_;
    if(args[0]->IsString()) {
      String::Utf8Value hash_(args[0]->ToString());
      auto hash(*hash_);
      uInt64_ = obj->hash.decode(hash);
    }

    v8::Handle<v8::Array> numArr = v8::Array::New( isolate, uInt64_.size() );
    for(unsigned int i = 0; i < uInt64_.size(); ++i) {
      numArr->Set( v8::Number::New(isolate, i), String::NewFromUtf8(isolate, std::to_string(uInt64_[i]).c_str()) );
    }

    args.GetReturnValue().Set(numArr);
  }

  void ShortDUID::GetShardID(const FunctionCallbackInfo<Value>& args) {
    auto isolate = args.GetIsolate();
    auto obj = ObjectWrap::Unwrap<ShortDUID>(args.Holder());

    args.GetReturnValue().Set(Number::New(isolate, obj->shard_id_));
  }

  void ShortDUID::GetEpochStart(const FunctionCallbackInfo<Value>& args) {
    auto isolate = args.GetIsolate();
    auto obj = ObjectWrap::Unwrap<ShortDUID>(args.Holder());

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, std::to_string(obj->epoch_start_).c_str()));
  }

  void ShortDUID::GetSalt(const FunctionCallbackInfo<Value>& args) {
    auto isolate = args.GetIsolate();
    auto obj = ObjectWrap::Unwrap<ShortDUID>(args.Holder());

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, obj->salt_.c_str()));
  }

  void ShortDUID::GetRandomAPIKey(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    const std::string urlsafe_alphabet = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    auto len   = args[0]->IsUndefined() ? 64 : args[0]->Uint32Value();
    len = (len > 4096) ? 64 : len; //Check boundaries

    auto ret(ShortDUID::GetRandomString(len, urlsafe_alphabet));
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, ret.c_str()));
  }

  void ShortDUID::GetRandomPassword(const v8::FunctionCallbackInfo<v8::Value>& args) {
    auto isolate = args.GetIsolate();
    const std::string password_alphabet = "!#$%&()=-~^[{]};+:*_?/><0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    auto len   = args[0]->IsUndefined() ? 16 : args[0]->Uint32Value();
    len = (len > 1024) ? 16 : len; //Check boundaries

    auto ret(ShortDUID::GetRandomString(len, password_alphabet));
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, ret.c_str()));
  }

  std::string ShortDUID::GetRandomString(unsigned short len, const std::string &alphabet) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, alphabet.length() - 1);

    std::string output;
    output.reserve(len);
    for(auto i = 0; i < len; i++) {
      output.push_back(alphabet[dis(gen)]);
    }

    return output;
  }

  uint64_t ShortDUID::GetUniqueID(const v8::FunctionCallbackInfo<v8::Value>& args) {
    // Generate distributed-safe unique ID based on milliseconds timestanp, sequence, and shard id
    // 42 bits (not bytes) are for milliseconds, should fit 139 years of milliseconds
    // 10 bits for shard ID, 2^10 shards (1024)
    // 12 bits for atomic sequence, 2^12 unique numbers per millisecond (4096)

    auto obj = ObjectWrap::Unwrap<ShortDUID>(args.Holder());
 
    // Get fresh milli time since epoch from monotonic clock
    uint64_t milliseconds_since_epoch = obj->mono_epoch_diff_ + (uint64_t) std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();

    // Create milliseconds since custom epoch, we want those numbers short
    uint64_t milliseconds_since_this_epoch = milliseconds_since_epoch - (obj->epoch_start_ + obj->time_offset_);

    // Create submillisecond sequence number
    uint64_t submilli_sequence = obj->sequence_.fetch_add(1, std::memory_order_relaxed); //Order is not important, only atomicity
    submilli_sequence &= ((1ULL << 12) - 1); // Bitmask for the sequence, allows to go up to 4095

    {
      // Deal with sequence overflow within same time unit, XXX still experimental, need more testing
      // Also, since iojs/nodejs are single-threaded, this "atomic" code is perhapse redundant. Maybe in the future this can be made
      // multithreaded, if it will give speed advantages, doubt it will. It was fun to write though ...
      bool overflow = false;
      uint64_t milliseconds_since_this_epoch_copy(milliseconds_since_this_epoch);

      std::atomic_thread_fence(std::memory_order_seq_cst); // Fence against anything that might access obj->ts_seq_[submilli_sequence] while in this block
      overflow = std::atomic_compare_exchange_strong(&obj->ts_seq_[submilli_sequence], &milliseconds_since_this_epoch_copy, milliseconds_since_this_epoch + 1);

      if (overflow || milliseconds_since_this_epoch_copy > milliseconds_since_this_epoch) { // Second condition is not needed with steady clock will keep for tests
        milliseconds_since_this_epoch = milliseconds_since_this_epoch_copy + 1; // Continue drifting time
      }

      milliseconds_since_this_epoch &= ((1ULL << 42) - 1); // We have only 42bit of space, overflow if not fitting
      if(false == overflow) obj->ts_seq_[submilli_sequence].store(milliseconds_since_this_epoch); // Store timestamp of last used sequence number if we did not do it earlier
    }

    // Pack ID and return
    return ((milliseconds_since_this_epoch) << 22) | ((obj->shard_id_) << 12) | submilli_sequence;
  }

}  // namespace shortduid
// vim: syntax=cpp11:ts=2:sw=2
