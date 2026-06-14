// SleepLamp - dashboard markup (body) (PROGMEM, streamed by handleRoot in WebUI.ino)
#pragma once
#include <Arduino.h>

const char PAGE_BODY[] PROGMEM = R"SLBODY(</style></head><body>
<div class="fx"><div class="blob b1"></div><div class="blob b2"></div><div class="blob b3"></div></div>
<div class="stars"></div><div class="stars2"></div><div class="stars3"></div>
<div class="planet"></div><div class="moon"></div>
<div class="meteor m1"></div><div class="meteor m2"></div><div class="meteor m3"></div>
<div id="prog"></div>
<div class="wrap">

<header>
  <div class="brand"><span class="orb"></span><span>SleepLamp<small>CONTACTLESS SLEEP MONITOR</small></span></div>
  <div class="status">
    <span><span class="dot" id="dot"></span><b id="conn">connecting</b></span>
    <span id="clock">--:--</span>
    <span>signal <b id="rssi">-</b></span>
    <span>up <b id="up">-</b></span>
  </div>
</header>

<!-- HERO -->
<div class="grid g3">
  <div class="card center">
    <div class="lbl" style="align-self:flex-start">Sleep Score</div>
    <div class="ringWrap"><div class="halo"></div>
      <div class="ring" id="scoreRing"><i><b id="score">-</b><span id="scoreSub">tonight</span></i></div>
    </div>
    <div class="pill" id="rating" style="margin-top:14px">-</div>
  </div>
  <div class="card">
    <div class="lbl">Current State</div>
    <div class="pill" id="state" style="font-size:16px;padding:8px 18px">-</div>
    <div class="row" style="margin-top:12px"><span>Presence</span><b id="presence">-</b></div>
    <div class="row"><span>Bed</span><b id="bed">-</b></div>
    <div class="row"><span>Motion</span><b id="motion">-</b></div>
    <div class="row"><span>Session</span><b id="sess">&mdash;</b></div>
  </div>
  <div class="card">
    <div class="lbl">Vital Signs</div>
    <div class="flex" style="justify-content:space-around;margin-top:10px">
      <div class="center"><div><span class="beat" id="beatDot"></span><span class="val" id="hr">0</span></div><div class="sub">bpm &middot; heart</div></div>
      <div class="center"><div><span class="brth" id="brthDot"></span><span class="val" id="br">0</span></div><div class="sub">rpm &middot; breath</div></div>
    </div>
    <div class="sub" style="text-align:center;margin-top:16px" id="vitNote">Needs stillness within 1.5 m</div>
  </div>
</div>

<!-- HYPNOGRAM -->
<div class="sec">Sleep Stages</div>
<div class="card"><canvas id="hyp" style="cursor:crosshair"></canvas>
  <div class="chips" id="hypInfo"></div>
  <div class="note">One bar per stage stretch &mdash; hover or tap a bar to see when it started, when it ended and how long it lasted.</div></div>

<!-- TONIGHT — live session from the on-device staging engine -->
<div class="sec">Tonight <span id="tonightSub">&middot; waiting for bed</span></div>
<div class="grid g4">
  <div class="card"><div class="lbl">In Bed</div><div class="val"><span id="tBed">0</span><span class="unit">min</span></div></div>
  <div class="card"><div class="lbl">Asleep</div><div class="val"><span id="tAsleep">0</span><span class="unit">min</span></div></div>
  <div class="card"><div class="lbl">Deep</div><div class="val" style="color:var(--deep)"><span id="tDeep">0</span><span class="unit">min</span></div></div>
  <div class="card"><div class="lbl">Light</div><div class="val" style="color:var(--light)"><span id="tLight">0</span><span class="unit">min</span></div></div>
</div>
<div class="grid g2" style="margin-top:13px">
  <div class="card">
    <div class="row"><span>Fell asleep after</span><b id="tOnset">-</b></div>
    <div class="row"><span>Awakenings</span><b id="tWakes">0</b></div>
    <div class="row"><span>Turnovers</span><b id="tTurns">0</b></div>
  </div>
  <div class="card">
    <div class="note" id="tStat" style="margin:0 0 14px">No session &mdash; lie in bed and stay still; tracking starts within a minute.</div>
    <button class="setupBtn" style="width:100%" id="endBtn" onclick="endSession()">End session &amp; save report</button>
  </div>
</div>

<!-- VITAL TRENDS -->
<div class="sec">Vital Trends</div>
<div class="card"><canvas id="vit"></canvas>
  <div class="note"><span style="color:var(--hr)">&#9679;</span> heart rate &nbsp; <span style="color:var(--br)">&#9679;</span> respiration</div></div>

<!-- ENVIRONMENT + ACTIVITY -->
<div class="sec">Environment &amp; Activity</div>
<div class="grid g4">
  <div class="card"><div class="lbl">Temperature</div><div class="val"><span id="temp">-</span><span class="unit">&deg;C</span></div></div>
  <div class="card"><div class="lbl">Humidity</div><div class="val"><span id="humid">-</span><span class="unit">%</span></div></div>
  <div class="card"><div class="lbl">Turnovers</div><div class="val" id="cTurn">0</div></div>
  <div class="card"><div class="lbl">Movement</div><div class="val" id="mrv">0</div><div class="bar"><i id="mrBar"></i></div></div>
</div>

<!-- SLEEP TREND + HISTORY -->
<div class="sec">Sleep Trend <span>&middot; score per night</span></div>
<div class="card"><canvas id="trend"></canvas>
  <div class="note">Last 14 sessions, newest on the right.</div></div>

<div class="sec">History <span id="histSub">&middot; stored on the lamp</span></div>
<div class="grid g4">
  <div class="card"><div class="lbl">Sessions</div><div class="val" id="hN">0</div></div>
  <div class="card"><div class="lbl">Avg Score <span style="text-transform:none">&middot; last 7</span></div><div class="val" id="hAvgS">-</div></div>
  <div class="card"><div class="lbl">Avg Sleep <span style="text-transform:none">&middot; last 7</span></div><div class="val" style="font-size:27px" id="hAvgM">-</div></div>
  <div class="card"><div class="lbl">Best Night</div><div class="val" style="color:var(--green)" id="hBest">-</div></div>
</div>
<div class="card" style="margin-top:13px">
  <div id="hist"></div>
  <div class="note">Tap a session for full details and per-session delete &middot; the lamp keeps the last 60 sessions.
    <a href="/api/export" style="color:var(--ind);text-decoration:none;font-weight:600">&nbsp;Download CSV</a> &nbsp;&middot;
    <a href="#" onclick="clearHist();return false" style="color:var(--red);text-decoration:none;font-weight:600">&nbsp;Clear all</a></div></div>

<!-- NIGHT REPORT — last completed session (our engine, validated) -->
<div class="sec">Night Report <span id="rWhen">&middot; no session saved yet</span></div>
<div class="grid g4">
  <div class="card"><div class="lbl">Score</div><div class="val" id="rScore">-</div></div>
  <div class="card"><div class="lbl">Total Sleep</div><div class="val"><span id="rSleep">0</span><span class="unit">min</span></div></div>
  <div class="card"><div class="lbl">Efficiency</div><div class="val"><span id="rEff">-</span><span class="unit">%</span></div></div>
  <div class="card"><div class="lbl">Deep</div><div class="val" style="color:var(--deep)"><span id="rDeepP">0</span><span class="unit">%</span></div></div>
</div>
<div class="grid g3" style="margin-top:13px">
  <div class="card">
    <div class="row"><span>In bed</span><b id="rBed">-</b></div>
    <div class="row"><span>Awake</span><b id="rAwake">-</b></div>
    <div class="row"><span>Fell asleep after</span><b id="rOnset">-</b></div>
  </div>
  <div class="card">
    <div class="row"><span>Awakenings</span><b id="rWakes">0</b></div>
    <div class="row"><span>Turnovers</span><b id="rTurn">0</b></div>
    <div class="row"><span>Apnea events</span><b id="rApnea">0</b></div>
  </div>
  <div class="card">
    <div class="row"><span>Avg heart rate</span><b id="rHR">-</b></div>
    <div class="row"><span>Avg respiration</span><b id="rBR">-</b></div>
    <div class="row"><span>Light sleep</span><b id="rLightP">0</b></div>
  </div>
</div>
<div class="card" style="margin-top:13px">
  <div class="lbl">Insight</div>
  <div id="insight" style="font-size:15px;line-height:1.55">Complete a sleep session to get your first personal insight.</div>
</div>

<!-- DEVICE -->
<div class="sec">Device &amp; Setup</div>
<div class="card">
  <button class="setupBtn" style="width:100%;padding:13px;font-size:16px" onclick="openPlace()">Check Placement</button>
  <div class="row" style="margin-top:18px"><span>Firmware</span><b id="fw">-</b></div>
  <div class="row"><span>Free memory</span><b id="heapKB">-</b></div>
  <div class="row"><span>Address</span><b>sleeplamp.local</b></div>
  <div class="row"><span>IP address</span><b id="ip">-</b></div>
  <div class="flex" style="margin-top:18px;gap:10px">
    <a class="setupBtn ghostBtn" href="/wifi">Change WiFi</a>
    <a class="setupBtn ghostBtn" href="/update">Update firmware</a>
    <a class="setupBtn" href="/api/reset?confirm=1" style="background:linear-gradient(135deg,#ff5e57,#ff2d55);box-shadow:0 10px 28px -10px #ff453acc" onclick="return confirm('Erase all settings and history?')">Factory reset</a>
  </div>
</div>

<!-- LAMP -->
<div class="sec">Lamp</div>
<div class="card">
  <div class="flex" style="justify-content:space-between">
    <div class="seg" id="modeSeg">
      <button class="lmode" data-m="0">Auto</button>
      <button class="lmode" data-m="1">Manual</button>
      <button class="lmode" data-m="2">Off</button>
    </div>
    <div class="flex"><span class="note" style="margin:0">Preview</span>
      <div id="prev" style="width:32px;height:32px;border-radius:9px;border:1px solid #ffffff22;background:#000;box-shadow:0 0 18px #00000000;transition:box-shadow .4s"></div>
      <input type="color" id="pick" value="#ff9636" style="width:38px;height:32px;background:none;border:0;cursor:pointer">
    </div>
  </div>
  <div class="sw" id="swatches"></div>
  <div style="margin-top:16px"><span class="note" style="margin:0">Brightness &middot; <b id="briV">60</b>%</span>
    <input type="range" min="0" max="100" value="60" id="bri"></div>
  <div class="note">Auto follows your sleep &mdash; warm when up, dim in bed, off when asleep.</div>
</div>

<!-- SMART WAKE -->
<div class="sec">Smart Wake</div>
<div class="card">
  <div class="flex" style="justify-content:space-between">
    <div class="flex">
      <div class="seg"><button id="alBtn">Off</button></div>
      <span class="note" style="margin:0">Wake by</span>
      <input type="time" id="alTime" value="07:00">
    </div>
    <div class="flex"><span class="note" style="margin:0">Window <b id="alWinV">30</b>m</span>
      <input type="range" min="0" max="60" value="30" id="alWin" style="width:130px"></div>
  </div>
  <div class="note" id="alStat">Sunrise lamp wakes you during light sleep, up to your set time. Get out of bed to stop it.</div>
</div>

<p class="note" style="text-align:center;margin:28px 0 4px">SleepLamp &middot; C1001 mmWave + DHT11 &middot; seq <span id="seq">0</span></p>
</div>
<button id="toTop" onclick="scrollTo({top:0,behavior:'smooth'})" title="Back to top">&uarr;</button>
<div id="toast"></div>

<!-- PLACEMENT CHECK MODAL -->
<div class="modal" id="placeModal">
  <div class="msheet">
    <div class="mhead"><span>Placement Check</span><span class="mx" onclick="closePlace()">&times;</span></div>
    <div class="ringWrap" style="width:118px;height:118px;margin:8px auto"><div class="halo" style="inset:-10px"></div>
      <div class="ring small" id="pRing"><i><b id="pScore">0</b><span>signal</span></i></div>
    </div>
    <div class="pstat" id="pStatus">Sit or lie in front of the lamp&hellip;</div>
    <div class="checks">
      <div class="chk" id="ck1"><i></i><span>Presence detected</span></div>
      <div class="chk" id="ck2"><i></i><span>Holding still</span></div>
      <div class="chk" id="ck3"><i></i><span>Breathing detected</span></div>
      <div class="chk" id="ck4"><i></i><span>Heart rate locked</span></div>
    </div>
    <div class="pacer"></div>
    <div class="note" style="margin-top:2px">Breathe slowly with the circle &mdash; it helps the radar lock faster.</div>
    <button class="setupBtn" style="margin-top:13px;width:100%;padding:11px" onclick="recalib()">Recalibrate sensor</button>
    <div class="note">Place the lamp ~0.5&ndash;0.8 m away, chest facing it. All four green = perfect spot.</div>
  </div>
</div>

)SLBODY";
