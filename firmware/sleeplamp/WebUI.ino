// SleepLamp · WebUI.ino — serves the dashboard page + the PWA manifest.
// Read-only JSON endpoints live in Api.ino; control/action endpoints in Control.ino.
#include "types.h"
#include "config.h"
#include "page_head.h"   // <!doctype> + <head> + opening <style>
#include "page_css.h"    // stylesheet
#include "page_body.h"   // </style></head><body> + markup
#include "page_js.h"     // <script> ... </script></body></html>

// The dashboard is one big page, so it's split across four PROGMEM blobs and
// streamed in chunks — keeps each source file readable and avoids one huge
// string. Concatenated in order they are byte-for-byte the original document.
void handleRoot() {
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);   // chunked transfer
  server.send(200, "text/html", "");
  server.sendContent_P(PAGE_HEAD);
  server.sendContent_P(PAGE_CSS);
  server.sendContent_P(PAGE_BODY);
  server.sendContent_P(PAGE_JS);
  server.sendContent("");                            // end of chunks
}

// PWA manifest -> installable on a phone home screen
void handleManifest() {
  server.send(200, "application/manifest+json",
    "{\"name\":\"SleepLamp\",\"short_name\":\"SleepLamp\",\"start_url\":\"/\",\"display\":\"standalone\","
    "\"background_color\":\"#020308\",\"theme_color\":\"#020308\","
    "\"icons\":[{\"src\":\"data:image/svg+xml,<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 64 64'><circle cx='32' cy='32' r='28' fill='%237d7aff'/></svg>\",\"sizes\":\"any\",\"type\":\"image/svg+xml\"}]}");
}
