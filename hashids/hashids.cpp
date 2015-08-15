/**
 *  Hashidsxx (c) 2014 Toon Schoenmakers
 *
 *  https://github.com/schoentoon/hashidsxx
 *  hashidsxx may be freely distributed under the MIT license.
 *
 * Modifications and optimizations: (c) 2015 Ian Matyssik <ian@phpb.com>
 */

#include "hashids.h"

#include <algorithm>

#define RATIO_SEPARATORS 3.5
#define RATIO_GUARDS 12

#include <iterator>
#include <iostream>
#include <sstream>
#include <cstdlib>

namespace hashidsxx {

  const static std::string separators("cfhistuCFHISTU");

  Hashids::Hashids(const std::string &salt, unsigned int min_length,
                   const std::string &alphabet)
  : _salt(salt), _alphabet(alphabet), _min_length(min_length), _separators(),
  _guards() {
  std::for_each(std::begin(separators), std::end(separators), [this](char c) {
    if (_alphabet.find(c) != std::string::npos)
    _separators.push_back(c);
  });
  _alphabet.erase(
    std::remove_if(std::begin(_alphabet), std::end(_alphabet), [this](char c) {
    return _separators.find(c) != std::string::npos;
  }), _alphabet.end());
  if (_alphabet.size() + _separators.size() < 16)
    _alphabet = DEFAULT_ALPHABET;

  _separators = _reorder(_separators, _salt);

  std::size_t min_separators =
    (std::size_t)std::ceil((float)_alphabet.length() / RATIO_SEPARATORS);

  if (_separators.empty() || _separators.length() < min_separators) {
    if (min_separators == 1)
      min_separators = 2;
    if (min_separators > _separators.length()) {
      auto split_at = min_separators - _separators.length();
      _separators.append(_alphabet.substr(0, split_at));
      _alphabet = _alphabet.substr(split_at);
      };
    };

  _alphabet = _reorder(_alphabet, _salt);
  std::size_t num_guards = (std::size_t)std::ceil((float)_alphabet.length() / RATIO_GUARDS);

  if (_alphabet.length() < 3) {
    _guards = _separators.substr(0, num_guards);
    _separators = _separators.substr(num_guards);
    } else {
    _guards = _alphabet.substr(0, num_guards);
    _alphabet = _alphabet.substr(num_guards);
    };
  }

  Hashids::Hashids(const Hashids &that)
  : _salt(that._salt), _alphabet(that._alphabet),
  _min_length(that._min_length), _separators(that._separators),
  _guards(that._guards) {
  }

  Hashids::Hashids(Hashids &&that)
  : _salt(std::move(that._salt)), _alphabet(std::move(that._alphabet)),
  _min_length(that._min_length), _separators(std::move(that._separators)),
  _guards(std::move(that._guards)) {
  }

  Hashids::~Hashids() {
  }

  std::string &Hashids::_reorder(std::string &input,
                                 const std::string &salt) const {
  uint_fast16_t i, j, v, p;

  if (salt.empty() || input.size() > UINT_FAST16_MAX || salt.size() > UINT_FAST16_MAX)
    return input;

  for (i = input.length() - 1, v = 0, p = 0; i > 0; --i, ++v) {
    v %= salt.length();
    p += salt[v];
    j = (uint16_t)(salt[v] + v + p) % i;

    std::swap(input[i], input[j]);
    }

  return input;
  }

  std::string Hashids::_reorder_norewrite(const std::string &input,
                                          const std::string &salt) const {
  auto output(input);
  return _reorder(output, salt);
  }

  std::string Hashids::_hash(uint64_t number, const std::string &alphabet) const {
  std::string output;
  uint_fast16_t rem;
  do {
    rem = number % (uint32_t)alphabet.size();
    number /= (uint32_t)alphabet.size();
    output.insert(std::begin(output), std::move(alphabet[rem]));
  } while (number);
  return output;
  }

  uint64_t Hashids::_unhash(const std::string &input,
                            const std::string &alphabet) const {
  uint64_t output = 0;
  for (std::string::size_type i = 0; i < input.size(); ++i) {
    char c = input[i];
    std::string::size_type pos = alphabet.find(c);
    output += pos * std::pow(alphabet.size(), input.size() - i - 1);
    };

  return output;
  }

  void Hashids::_ensure_length(std::string &output, std::string &alphabet,
                               uint32_t values_hash) const {
  uint32_t guard_index = (values_hash + output[0]) % _guards.size();
  output.insert(std::begin(output), _guards[guard_index]);

  if (output.size() < _min_length) {
    guard_index = (values_hash + output[2]) % _guards.size();
    output.push_back(_guards[guard_index]);
    };

  uint32_t split_at = alphabet.size() / 2;
  while (output.size() < _min_length) {
    alphabet = _reorder_norewrite(alphabet, alphabet);

    output = alphabet.substr(split_at) + output + alphabet.substr(0, split_at);

    uint32_t excess = output.size() - _min_length;
    if (excess > 0) {
      auto from_index = excess / 2;
      output = output.substr(from_index, _min_length);
      };
    };
  }

  std::vector<std::string> Hashids::_split(const std::string &input,
                                           const std::string &splitters) const {
  std::vector<std::string> parts;
  std::string tmp;

  for (auto c : input) {
    if (splitters.find(c) != std::string::npos) {
        parts.push_back(tmp);
        tmp.clear();
      } else {
        tmp.push_back(c);
      }
    };
  if (!tmp.empty())
    parts.push_back(tmp);

  return parts;
  }

  std::vector<uint64_t> Hashids::decode(const std::string &input) const {
  std::vector<uint64_t> output;

  auto parts = _split(input, _guards);

  auto hashid = parts[0];
  if (parts.size() >= 2)
    hashid = parts[1];

  if (hashid.empty())
    return output;

  output.reserve(parts.size());

  char lottery = hashid[0];
  auto alphabet(_alphabet);

  hashid.erase(std::begin(hashid));

  auto hash_parts = _split(hashid, _separators);
  for (const std::string &part : hash_parts) {
    auto alphabet_salt = (lottery + _salt + alphabet);
    alphabet_salt = alphabet_salt.substr(0, alphabet.size());

    alphabet = _reorder(alphabet, alphabet_salt);

    output.push_back(_unhash(part, alphabet));
    };

  return output;
  }
};
// vim: syntax=cpp11:ts=2:sw=2
