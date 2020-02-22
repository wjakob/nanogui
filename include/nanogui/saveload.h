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

NAMESPACE_BEGIN(nanogui)

struct json {
  Json::object obj;
  using _v = Json::value;
  using _s = std::string;
  using _ss = std::vector<std::string>;
  using _a = Json::value::array;
  using _r = std::pair<float, float>;
  using _vi = Vector2i;

  json() {};
  json(Json::object o) : obj(o) {};
  template<typename... Args>
  json& $(const _s& key, const Args&... args) { obj[key] = _v(args...); return *this; }
  template<typename T>
  json& set(const T& v) { static_assert("Cant deduce type"); return $("value", v); }
  template<> json& set<_s>(const _s& v) { $("value", v); return type("string"); }
  template<> json& set<int>(const int& v) { $("value", v); return type("integer"); }
  template<> json& set<bool>(const bool& v) { $("value", v); return type("boolean"); }
  template<> json& set<BoolObservable>(const BoolObservable& v) { $("value", (bool)v); return type("boolean"); }
  template<> json& set<float>(const float& v) { $("value", v); return type("float"); }
  template<> json& set<FloatObservable>(const FloatObservable& v) { $("value", (float)v); return type("boolean"); }
  template<> json& set<Color>(const Color& v) { $("value", v.toInt()); return type("color"); }
  template<> json& set<_a>(const _a& v) { $("value", v); return type("array"); }
  template<> json& set<_vi>(const _vi& v) { $("x", v.x()); $("y", v.y()); return type("position"); }
  template<> json& set<_r>(const _r& v) { $("min", v.first).$("max", v.second); return type("rangef"); }

  template<> json& set<_ss>(const _ss& v) {
    _a items; for (auto& e : v) items.push_back(_v(e));
    return set(items);
  }

  json& name(const _s& n) { obj["name"] = _v(n); return *this; }
  json& type(const _s& n) { obj["type"] = _v(n); return *this; }

  template<typename T> T get(const _s& n) const { auto it = obj.find(n); return it != obj.end() ? it->second.get("value") : Json::null; }
  template<> _s get<_s>(const _s& n) const { auto it = obj.find(n); return it != obj.end() ? it->second.get_str("value") : ""; }
  template<> int get<int>(const _s& n) const { auto it = obj.find(n); return it != obj.end() ? it->second.get_int("value") : 0; }
  template<> float get<float>(const _s& n) const { auto it = obj.find(n); return it != obj.end() ? it->second.get_float("value") : 0; }
  template<> bool get<bool>(const _s& n) const { auto it = obj.find(n); return it != obj.end() ? it->second.get_bool("value") : false; }
  template<> Color get<Color>(const _s& n) const { auto it = obj.find(n); return it != obj.end() ? Color(it->second.get_int("value")): Color(); }
  template<> _vi get<_vi>(const _s& n) const { auto it = obj.find(n);  return it != obj.end() ? _vi(it->second.get_int("x"), it->second.get_int("y")) : _vi{0, 0}; }
  template<> _r get<_r>(const _s& n) const { auto it = obj.find(n);  return it != obj.end() ? _r(it->second.get_int("min"), it->second.get_int("max")) : _r{ 0, 0 }; }

  template<> _ss get<_ss>(const _s& n) const {
    auto items = obj.find(n);
    auto it = obj.find(n); 
    if (it == obj.end())
      return{};

    _ss ret;
    int i = 0;
    Json::value e = it->second.get(i);
    while (!e.is<Json::null>()) {
      ret.push_back(e.to_str());
      e = it->second.get(i++);
    }
    return ret;
  }


  inline operator Json::value() const { return Json::value(obj); }
};

NAMESPACE_END(nanogui)