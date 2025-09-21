/** The MIT License (MIT)

  Copyright (c) 2018 David Payne

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
  */

// Additional Contributions:
/* 17 Sep 2025 : Eduardo Romero @eduardorq : Add translations */

#pragma once
#include <Arduino.h>
#include <FS.h>

class I18N {
public:
  // Load /i18n/<lang>.json (p.ej. "es" => "/i18n/es.json")
  static bool load(const String& langCode);

  static String t(const String& key);

  // With placeholders: pairs = {{"printer","Ender3"},{"value","42"}}
  static String tr(const String& key, const String pairs[][2], size_t count);

  static bool has(const String& key);

  static void apply(String& s);

  static String current();

private:
  // Estado interno
  static String s_lang;
  static String s_json;

  // Utilidades internas
  static String pathFor(const String& lang);
  static bool findKV(const String& key, String& outVal);
};
