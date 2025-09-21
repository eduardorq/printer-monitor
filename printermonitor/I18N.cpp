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

#include "I18N.h"

String I18N::s_lang = "en";
String I18N::s_json = "";

static void replaceAllInPlace(String& s, const String& from, const String& to) {
  if (from.length() == 0) return;
  int idx = 0;
  while ((idx = s.indexOf(from, idx)) >= 0) {
    s = s.substring(0, idx) + to + s.substring(idx + from.length());
    idx += to.length();
  }
}

static inline bool isSpace(char c) {
  return c==' ' || c=='\t' || c=='\r' || c=='\n';
}

// Devuelve el path /i18n/<lang>.json
String I18N::pathFor(const String& lang) {
  String l = lang; l.trim();
  if (l.length() == 0) l = "en";
  return String("/i18n/") + l + ".json";
}

bool I18N::findKV(const String& key, String& outVal) {
  if (s_json.length() == 0) return false;

  String needle = "\"" + key + "\"";
  int pos = s_json.indexOf(needle);
  if (pos < 0) return false;

  pos += needle.length();
  while (pos < (int)s_json.length() && isSpace(s_json[pos])) pos++;
  if (pos >= (int)s_json.length() || s_json[pos] != ':') return false;
  pos++;
  while (pos < (int)s_json.length() && isSpace(s_json[pos])) pos++;
  if (pos >= (int)s_json.length() || s_json[pos] != '\"') return false;
  pos++; // dentro del valor

  String val;
  bool escaped = false;
  for (; pos < (int)s_json.length(); ++pos) {
    char c = s_json[pos];
    if (escaped) {
      val += c;
      escaped = false;
    } else {
      if (c == '\\') {
        escaped = true;
      } else if (c == '\"') {
        outVal = val;
        return true;
      } else {
        val += c;
      }
    }
  }
  return false;
}

// Carga /i18n/<lang>.json desde SPIFFS
bool I18N::load(const String& langCode) {
  String path = pathFor(langCode);
  File f = SPIFFS.open(path, "r");
  if (!f) {
    Serial.println(I18N::t("i18n.file.cant_open") + path);
    return false;
  }

  s_json = "";
  const size_t CHUNK = 512;
  std::unique_ptr<char[]> buf(new char[CHUNK + 1]);
  while (true) {
    size_t n = f.readBytes(buf.get(), CHUNK);
    if (n == 0) break;
    buf[n] = '\0';
    s_json += buf.get();
    if (s_json.length() > 32768) { // límite de seguridad
      break;
    }
  }
  f.close();

  s_lang = langCode;
  Serial.println(I18N::t("i18n.file.loaded") + path + " (" + String(s_json.length()) + " bytes)");
  return (s_json.length() > 0);
}

String I18N::t(const String& key) {
  String out;
  if (findKV(key, out)) return out;
  return key; // fallback
}

bool I18N::has(const String& key) {
  String tmp;
  return findKV(key, tmp);
}

String I18N::tr(const String& key, const String pairs[][2], size_t count) {
  String base = t(key);
  for (size_t i = 0; i < count; ++i) {
    String ph = "{" + String(pairs[i][0]) + "}";
    replaceAllInPlace(base, ph, pairs[i][1]);
  }
  return base;
}

// Sustituye tokens {{ clave }} dentro de una cadena.
// Si no existe la clave, deja el token.
void I18N::apply(String& s) {
  int start = 0;
  while (true) {
    int open = s.indexOf("{{", start);
    if (open < 0) break;
    int close = s.indexOf("}}", open + 2);
    if (close < 0) break;

    String key = s.substring(open + 2, close);
    key.trim();

    String val = I18N::t(key);
    if (val.length() == 0 || val == key) {
      start = close + 2; // sin traducción, saltar
      continue;
    }

    s = s.substring(0, open) + val + s.substring(close + 2);
    start = open + val.length();
  }
}

String I18N::current() {
  return s_lang;
}
