/*
nanogui/saveload.h -- Standard save/load converter

NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
The widget drawing code is based on the NanoVG demo application
by Mikko Mononen.

All rights reserved. Use of this source code is governed by a
BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/common.h>
#include <nanogui/serializer/json.h>
#include <fstream>

NAMESPACE_BEGIN(nanogui)

struct json {
  Json::object obj;
  using _v = Json::value;
  using _s = std::string;
  using _ss = std::vector<std::string>;
  using _a = Json::value::array;
  using _r = std::pair<float, float>;
  using _vi = Vector2i;
  using _vf = VectorXf;

  json() {};
  json(Json::object o) : obj(o) {};
  template<typename... Args>
  json& $(const _s& key, const Args&... args) { obj[key] = _v(args...); return *this; }
  template<typename T>
  json& set(const T& v) { assert(false && "Cant deduce type"); return $("value", v); }
  json& set(const _s& v) { $("value", v); return type("string"); }
  json& set(const int& v) { $("value", v); return type("integer"); }
  json& set(const bool& v) { $("value", v); return type("boolean"); }
  json& set(const BoolObservable& v) { $("value", (bool)v); return type("boolean"); }
  json& set(const float& v) { $("value", v); return type("float"); }
  json& set(const FloatObservable& v) { $("value", (float)v); return type("boolean"); }
  json& set(const Color& v) { $("value", v.toInt()); return type("color"); }
  json& set(const _a& v) { $("value", v); return type("array"); }
  json& set(const _vi& v) { $("x", v.x()); $("y", v.y()); return type("position"); }
  json& set(const _r& v) { $("min", v.first).$("max", v.second); return type("rangef"); }
  json& set(const _ss& v) { _a items; for (auto& e : v) items.push_back(_v(e)); return set(items); }
  json& set(const _vf& v) { _a items; for (auto& e : v) items.push_back(_v(e)); return set(items); }

  Json::value& operator[](const _s& v) { return obj[v]; }

  json& name(const _s& n) { obj["name"] = _v(n); return *this; }
  json& type(const _s& n) { obj["type"] = _v(n); return *this; }

  template<typename T> T get(const _s& n) const { auto it = obj.find(n); return it != obj.end() ? it->second.get("value") : T(); }

  inline operator Json::value() const { return Json::value(obj); }

  template<typename T> std::vector<T> get_array(const _s& n, std::function<T (const _v&)> conv) const {
    auto it = obj.find(n); 
    if (it == obj.end())
      return{};

    std::vector<T> ret;
    int i = 0;
    Json::value e = it->second.get(i);
    while (!e.is<Json::null>()) { 
      ret.push_back(conv(e));
      e = it->second.get(i++); 
    }
    return ret;
  }


  void load(const _s& filename)
  {
    Json::value x;
    std::ifstream t(filename, std::ifstream::binary);
    t >> x;
    if (x.is<Json::object>()) obj = x.get_obj();
    else obj = Json::object{};
    t.close();
  }

  void save(const _s& filename)
  {
    std::ofstream t(filename, std::ofstream::binary);
    Json::value x(obj);
    t << x;
    t.close();
  }
};

template<> inline json::_s json::get<json::_s>(const _s& n) const { auto it = obj.find(n); return it != obj.end() ? it->second.get_str("value") : ""; }
template<> inline int json::get<int>(const _s& n) const { auto it = obj.find(n); return it != obj.end() ? (int)it->second.get_int("value") : 0; }
template<> inline int64_t json::get<int64_t>(const _s& n) const { auto it = obj.find(n); return it != obj.end() ? it->second.get_int("value") : 0; }
template<> inline float json::get<float>(const _s& n) const { auto it = obj.find(n); return it != obj.end() ? it->second.get_float("value") : 0; }
template<> inline bool json::get<bool>(const _s& n) const { auto it = obj.find(n); return it != obj.end() ? it->second.get_bool("value") : false; }
template<> inline Color json::get<Color>(const _s& n) const { auto it = obj.find(n); return it != obj.end() ? Color((int)it->second.get_int("value")): Color(); }
template<> inline json::_vi json::get<json::_vi>(const _s& n) const { auto it = obj.find(n);  return it != obj.end() ? _vi((int)it->second.get_int("x"), (int)it->second.get_int("y")) : _vi{0, 0}; }
template<> inline json::_r json::get<json::_r>(const _s& n) const { auto it = obj.find(n);  return it != obj.end() ? _r(it->second.get_float("min"), it->second.get_float("max")) : _r{ 0.f, 0.f }; }


template<> inline json::_ss json::get<json::_ss>(const _s& n) const { return get_array<_s>(n, [](const _v& v) { return v.to_str(); }); }
template<> inline json::_vf json::get<json::_vf>(const _s& n) const { return get_array<float>(n, [](const _v& v) { return v.get_float(); }); }


NAMESPACE_END(nanogui)