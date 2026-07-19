// SleepLamp · Store.ino — session history on flash (LittleFS)
// One CSV line per finished session, written ONLY by the staging engine via
// reportSave() — never from raw radar frames, so corrupt-frame junk
// (HR 2 bpm, apnea 56, endless duplicates) can no longer reach the history.
//
// API (all under /api/history):
//   GET /api/history            -> JSON array, oldest first
//   GET /api/history?del=N&t=S  -> delete one session, returns updated array
//   GET /api/history?clear=1    -> wipe everything
//   GET /api/export             -> download sleeplamp_history.csv
// History is capped at HIST_MAX rows — oldest row is dropped on overflow.
#include "types.h"
#include "config.h"
#include <LittleFS.h>
#include <time.h>

// full "YYYY-MM-DD HH:MM" stamp from NTP time (no comma -> CSV-safe)
static String nowStamp() {
  struct tm t;
  if (getLocalTime(&t, 50)) { char b[20]; strftime(b, sizeof(b), "%Y-%m-%d %H:%M", &t); return String(b); }
  return "no-time";
}

static String field(const String& line, int idx) {
  int start = 0;
  for (int i = 0; i < idx; i++) { start = line.indexOf(',', start); if (start < 0) return ""; start++; }
  int end = line.indexOf(',', start);
  return (end < 0) ? line.substring(start) : line.substring(start, end);
}
static String num(const String& line, int idx) { String v = field(line, idx); return v.length() ? v : "0"; }

static int histCount() {
  File f = LittleFS.open("/history.csv", FILE_READ);
  if (!f) return 0;
  int n = 0;
  while (f.available()) { String l = f.readStringUntil('\n'); l.trim(); if (l.length()) n++; }
  f.close();
  return n;
}

// rewrite history without row dropIdx (streamed via temp file — RAM-safe even
// at HIST_MAX rows). Returns rows kept, -1 if the file couldn't be opened.
static int histDropRow(int dropIdx) {
  File in = LittleFS.open("/history.csv", FILE_READ);
  if (!in) return -1;
  File out = LittleFS.open("/history.tmp", FILE_WRITE);
  if (!out) { in.close(); return -1; }
  int idx = 0, kept = 0;
  while (in.available()) {
    String line = in.readStringUntil('\n'); line.trim();
    if (!line.length()) continue;
    if (idx != dropIdx) { out.println(line); kept++; }
    idx++;
  }
  in.close(); out.close();
  LittleFS.remove("/history.csv");
  LittleFS.rename("/history.tmp", "/history.csv");
  return kept;
}

void storeBegin() {
  if (!LittleFS.begin(true)) Serial.println(F("[FS] LittleFS mount FAILED"));
  else                       Serial.println(F("[FS] LittleFS ready"));
  reportLoadLast();   // dashboard shows the last saved night right after boot
}

// CSV columns: datetime, score, sleepMin, deep%, light%, awakeMin,
//              avgHR, avgBR, turnovers, apnea, onsetMin, awakenings, bedMin
void reportSave(NightReport& r) {
  String st = nowStamp();
  strlcpy(r.when, st.c_str(), sizeof(r.when));
  int deepPct  = r.sleepMin ? r.deepMin  * 100 / r.sleepMin : 0;
  int lightPct = r.sleepMin ? r.lightMin * 100 / r.sleepMin : 0;
  File f = LittleFS.open("/history.csv", FILE_APPEND);
  if (!f) { Serial.println(F("[FS] history open failed")); return; }
  f.printf("%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
           st.c_str(), r.score, r.sleepMin, deepPct, lightPct, r.awakeMin,
           r.avgHR, r.avgBR, r.turns, r.apnea, r.onsetMin, r.wakes, r.bedMin);
  f.close();
  if (histCount() > HIST_MAX) histDropRow(0);   // cap: drop the oldest
  Serial.printf("[FS] session saved: score=%d sleep=%d min (in bed %d) -- %d/%d sessions stored\n",
                r.score, r.sleepMin, r.bedMin, histCount(), HIST_MAX);
}

// parse the newest history line back into lastReport (boot + after deletes).
// At boot this runs before the tasks start; at runtime callers hold the mutex.
void reportLoadLast() {
  File f = LittleFS.open("/history.csv", FILE_READ);
  if (!f) return;
  String last;
  while (f.available()) {
    String line = f.readStringUntil('\n'); line.trim();
    if (line.length()) last = line;
  }
  f.close();
  if (!last.length()) return;
  NightReport r;
  r.valid = true;
  strlcpy(r.when, field(last, 0).c_str(), sizeof(r.when));
  r.score    = num(last, 1).toInt();
  r.sleepMin = num(last, 2).toInt();
  int deepPct  = num(last, 3).toInt();
  int lightPct = num(last, 4).toInt();
  r.deepMin  = r.sleepMin * deepPct  / 100;
  r.lightMin = r.sleepMin * lightPct / 100;
  r.awakeMin = num(last, 5).toInt();
  r.avgHR    = num(last, 6).toInt();
  r.avgBR    = num(last, 7).toInt();
  r.turns    = num(last, 8).toInt();
  r.apnea    = num(last, 9).toInt();
  r.onsetMin = num(last,10).toInt();
  r.wakes    = num(last,11).toInt();
  r.bedMin   = num(last,12).toInt();
  if (!r.bedMin) r.bedMin = r.sleepMin + r.awakeMin;   // rows from older firmware
  lastReport = r;
  Serial.printf("[FS] last report restored: %s score=%d\n", r.when, r.score);
}

// /api/export -> download the raw history as a CSV file
void handleExport() {
  File f = LittleFS.open("/history.csv", FILE_READ);
  if (!f) { server.send(404, "text/plain", "no history yet"); return; }
  server.sendHeader("Content-Disposition", "attachment; filename=sleeplamp_history.csv");
  server.streamFile(f, "text/csv");
  f.close();
}

// /api/history — list / delete-one / clear (see header at top of file)
void handleHistory() {
  if (server.hasArg("clear")) {
    LittleFS.remove("/history.csv");
    xSemaphoreTake(mux, portMAX_DELAY);
    lastReport = NightReport();
    xSemaphoreGive(mux);
    server.sendHeader("Cache-Control","no-store");
    server.send(200, "application/json", "[]"); return;
  }

  // delete one session: row index + its timestamp. The stamp guards against
  // the list shifting under the client (a session saved between page load
  // and the tap) — if the row at `del` no longer matches, fall back to the
  // first row carrying that exact stamp.
  if (server.hasArg("del")) {
    int    idx  = server.arg("del").toInt();
    String want = server.hasArg("t") ? server.arg("t") : "";
    int target = -1, i = 0;
    File f = LittleFS.open("/history.csv", FILE_READ);
    if (f) {
      while (f.available()) {
        String line = f.readStringUntil('\n'); line.trim();
        if (!line.length()) continue;
        bool stampOk = !want.length() || field(line, 0) == want;
        if (i == idx && stampOk) { target = i; break; }
        if (target < 0 && stampOk && want.length()) target = i;   // fallback
        i++;
      }
      f.close();
    }
    if (target >= 0) {
      histDropRow(target);
      xSemaphoreTake(mux, portMAX_DELAY);
      lastReport = NightReport();     // newest row may be the one we removed
      reportLoadLast();
      xSemaphoreGive(mux);
      Serial.printf("[FS] session %d deleted from history\n", target);
    }
    // falls through -> respond with the updated list
  }

  String out; out.reserve(4096);
  out = "[";
  int n = 0;
  File f = LittleFS.open("/history.csv", FILE_READ);
  if (f) {
    while (f.available()) {
      String line = f.readStringUntil('\n'); line.trim();
      if (!line.length()) continue;
      if (n) out += ",";
      out += "{\"t\":\"" + field(line,0) + "\",\"score\":" + num(line,1) + ",\"sleep\":" + num(line,2)
           + ",\"deep\":" + num(line,3) + ",\"shallow\":" + num(line,4) + ",\"wake\":" + num(line,5)
           + ",\"hr\":" + num(line,6) + ",\"resp\":" + num(line,7) + ",\"turn\":" + num(line,8)
           + ",\"apnea\":" + num(line,9) + ",\"onset\":" + num(line,10) + ",\"wakes\":" + num(line,11)
           + ",\"bed\":" + num(line,12) + "}";
      n++;
    }
    f.close();
  }
  out += "]";
  server.sendHeader("Cache-Control","no-store");
  server.send(200, "application/json", out);
}
