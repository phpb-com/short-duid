#ifndef SHORTDUID_H
#define SHORTDUID_H

#include <node.h>
#include <string>
#include <cstdint>
#include <atomic>
#include <vector>
#include <chrono>
#include <random>
#include <iostream>
#include <sstream>
#include <limits>
#include <limits.h>
#include <sys/time.h>
#include <stdlib.h>
#include "../hashids/hashids.h"
#include <node_object_wrap.h>


namespace shortduid {

  class ShortDUID : public node::ObjectWrap {
  public:
    static void Init(v8::Local<v8::Object> exports);

  private:
    explicit ShortDUID(uint32_t shard_id = 0, std::string salt = "", uint64_t epoch_start = 0);
    ~ShortDUID();

    static void New(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetDUIDInt(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetDUID(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetShardID(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetEpochStart(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetSalt(const v8::FunctionCallbackInfo<v8::Value>& args);
    //HashID stuff
    static void HashidEncode(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void HashidDecode(const v8::FunctionCallbackInfo<v8::Value>& args);
    //Only should be used for unit testing
    static void DriftTime(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetCurrentTimeMs(const v8::FunctionCallbackInfo<v8::Value>& args);
    //Password and key generators
    static void GetRandomAPIKey(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetRandomPassword(const v8::FunctionCallbackInfo<v8::Value>& args);
    //JS stuff
    static v8::Persistent<v8::Function> constructor;
    //Non JS methods
    static std::string GetRandomString(unsigned short len, const std::string &alphabet);
    uint64_t GetUniqueID(const v8::FunctionCallbackInfo<v8::Value>& args);
    std::atomic_ullong sequence_;
    std::atomic<uint64_t> ts_seq_[4096];
    std::string salt_;
    uint64_t epoch_start_;
    uint32_t shard_id_;
    int64_t time_offset_;         //For testing only
    hashidsxx::Hashids hash;
  };

}  // namespace shortduid

#endif
// vim: syntax=cpp11:ts=2:sw=2
