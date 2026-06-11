// SleepLamp · page.h — full dashboard ("deep-space universe" glass UI)
#pragma once
#include <Arduino.h>

const char PAGE[] PROGMEM = R"PAGE(
<!DOCTYPE html><html lang="en"><head>
<meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
<meta name="theme-color" content="#020308">
<meta name="apple-mobile-web-app-capable" content="yes">
<meta name="apple-mobile-web-app-status-bar-style" content="black-translucent">
<meta name="apple-mobile-web-app-title" content="SleepLamp">
<link rel="manifest" href="/manifest.json">
<link rel="icon" href="data:image/svg+xml,<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 32 32'><circle cx='16' cy='16' r='13' fill='%237d7aff'/></svg>">
<title>SleepLamp</title>
<style>
:root{
 --bg:#020308; --tx:#f5f6fa; --mut:#8e93a8;
 --ind:#7d7aff; --vio:#bf5af2; --teal:#5ce0d8; --hr:#ff5e7a; --br:#5ac8fa;
 --amber:#ffd60a; --green:#32d74b; --red:#ff453a;
 --awake:#ff9f0a; --light:#0a84ff; --deep:#5e5ce6;
 --card:rgba(255,255,255,.05); --c2:rgba(255,255,255,.08);
 --line:rgba(255,255,255,.09); --track:rgba(255,255,255,.09);
}
*{box-sizing:border-box;margin:0;padding:0}
html{scroll-behavior:smooth}
body{background:var(--bg);color:var(--tx);
 font:15px/1.45 -apple-system,BlinkMacSystemFont,"SF Pro Display","SF Pro Text",system-ui,sans-serif;
 min-height:100vh;padding:max(14px,env(safe-area-inset-top)) 16px 48px;-webkit-font-smoothing:antialiased;
 overflow-x:hidden}
.wrap{max-width:780px;margin:0 auto}

/* ===== deep-space universe background ===== */
::selection{background:#7d7aff66}
::-webkit-scrollbar{width:10px;height:10px}
::-webkit-scrollbar-track{background:transparent}
::-webkit-scrollbar-thumb{background:linear-gradient(#5e5ce6,#bf5af2);border-radius:6px;border:2px solid #05060f}
.fx{position:fixed;inset:0;z-index:-3;overflow:hidden;background:
 radial-gradient(1100px 520px at 50% -160px,#10142e 0,var(--bg) 60%),
 radial-gradient(900px 700px at 88% 112%,#0c1026 0,transparent 60%)}
.fx::after{content:'';position:absolute;inset:-25%;pointer-events:none;background:
 linear-gradient(115deg,transparent 36%,rgba(124,120,255,.05) 45%,rgba(188,160,255,.10) 50%,rgba(92,224,216,.05) 55%,transparent 64%)}
.blob{position:absolute;width:58vmax;height:58vmax;border-radius:50%;filter:blur(70px);opacity:.16;will-change:transform}
.b1{background:#5e5ce6;top:-24vmax;left:-16vmax;animation:dr1 38s ease-in-out infinite alternate}
.b2{background:#bf5af2;top:22%;right:-26vmax;animation:dr2 47s ease-in-out infinite alternate}
.b3{background:#0a84ff;bottom:-30vmax;left:12%;animation:dr3 55s ease-in-out infinite alternate}
@keyframes dr1{to{transform:translate(9vmax,7vmax) scale(1.18)}}
@keyframes dr2{to{transform:translate(-8vmax,-5vmax) scale(1.12)}}
@keyframes dr3{to{transform:translate(6vmax,-8vmax) scale(1.22)}}
.stars,.stars2,.stars3{position:fixed;inset:-60px;z-index:-2;pointer-events:none;background-repeat:repeat;background-size:520px 520px;will-change:transform}
.stars{background-image:
 radial-gradient(1.3px 1.3px at 40px 60px,#ffffffcc 50%,transparent 51%),
 radial-gradient(1px 1px at 180px 120px,#ffffff88 50%,transparent 51%),
 radial-gradient(1.6px 1.6px at 300px 40px,#cfd2ffaa 50%,transparent 51%),
 radial-gradient(1px 1px at 420px 220px,#ffffff77 50%,transparent 51%),
 radial-gradient(1.2px 1.2px at 90px 320px,#ffffff99 50%,transparent 51%),
 radial-gradient(1px 1px at 250px 420px,#cfd2ff88 50%,transparent 51%),
 radial-gradient(1.4px 1.4px at 470px 380px,#ffffffaa 50%,transparent 51%);
 animation:twinkle 7s ease-in-out infinite alternate}
.stars2{background-image:
 radial-gradient(1px 1px at 120px 200px,#ffffff66 50%,transparent 51%),
 radial-gradient(1.4px 1.4px at 360px 150px,#dcdcff99 50%,transparent 51%),
 radial-gradient(1px 1px at 60px 460px,#ffffff55 50%,transparent 51%),
 radial-gradient(1.2px 1.2px at 480px 80px,#ffffff88 50%,transparent 51%);
 animation:twinkle 9s ease-in-out infinite alternate-reverse}
@keyframes twinkle{from{opacity:.35}to{opacity:.95}}
.stars3{background-size:640px 640px;opacity:.7;background-image:
 radial-gradient(1px 1px at 90px 80px,#ffffff3e 50%,transparent 51%),
 radial-gradient(1px 1px at 300px 300px,#cfd2ff44 50%,transparent 51%),
 radial-gradient(1.1px 1.1px at 520px 160px,#ffffff36 50%,transparent 51%),
 radial-gradient(1px 1px at 200px 520px,#ffffff30 50%,transparent 51%);
 animation:drift 150s linear infinite}
@keyframes drift{to{background-position:0 640px}}
/* shooting stars */
.meteor{position:fixed;z-index:-1;width:150px;height:2px;pointer-events:none;opacity:0;border-radius:2px;
 background:linear-gradient(90deg,#fff,rgba(255,255,255,0));filter:drop-shadow(0 0 6px #fff);
 transform:rotate(215deg);animation:shoot 11s linear infinite}
.m1{top:9%;left:74%}
.m2{top:20%;left:30%;animation-delay:4.5s;animation-duration:14s}
.m3{top:5%;left:50%;animation-delay:8.5s;animation-duration:17s}
@keyframes shoot{0%{opacity:0;transform:rotate(215deg) translateX(0)}
 1%,4%{opacity:.9}9%,100%{opacity:0;transform:rotate(215deg) translateX(46vmin)}}
/* ringed planet + drifting moon */
.planet{position:fixed;top:13%;right:-46px;width:130px;height:130px;border-radius:50%;z-index:-2;opacity:.55;pointer-events:none;
 background:radial-gradient(circle at 34% 30%,#a7a5ff,#4a48b8 55%,#181740 88%);
 box-shadow:inset -18px -14px 36px #000b,0 0 60px #5e5ce62e;animation:bob 26s ease-in-out infinite alternate}
.planet::after{content:'';position:absolute;left:50%;top:50%;width:205%;height:46%;border-radius:50%;
 border:6px solid rgba(173,170,255,.28);border-top-color:transparent;border-left-color:rgba(173,170,255,.12);
 transform:translate(-50%,-50%) rotate(-16deg)}
.moon{position:fixed;bottom:9%;left:-26px;width:84px;height:84px;border-radius:50%;z-index:-2;opacity:.45;pointer-events:none;
 background:radial-gradient(circle at 60% 35%,#e8e9f5,#9da3c4 55%,#3c4060 90%);
 box-shadow:inset -12px -9px 24px #000a,0 0 40px #9da3c422;animation:bob 33s ease-in-out infinite alternate-reverse}
@keyframes bob{to{transform:translateY(-26px)}}
@media(max-width:700px){.planet{width:88px;height:88px;right:-30px}.moon{display:none}}

/* ===== glass header ===== */
header{position:sticky;top:10px;z-index:40;display:flex;align-items:center;gap:14px;flex-wrap:wrap;
 margin:4px 0 26px;padding:13px 18px;border-radius:22px;
 background:rgba(8,10,20,.55);backdrop-filter:blur(26px) saturate(1.7);-webkit-backdrop-filter:blur(26px) saturate(1.7);
 border:1px solid rgba(255,255,255,.09);box-shadow:0 16px 48px -18px #000c,inset 0 1px 0 rgba(255,255,255,.07)}
.brand{font-size:23px;font-weight:800;letter-spacing:-.03em;display:flex;align-items:center;gap:11px;
 background:linear-gradient(120deg,#fff 30%,#c9c8ff 70%,#9f9dff);-webkit-background-clip:text;background-clip:text;color:transparent}
.brand small{display:block;font-size:10.5px;font-weight:600;color:var(--mut);-webkit-text-fill-color:var(--mut);letter-spacing:.16em;margin-top:3px}
.orb{position:relative;width:30px;height:30px;border-radius:50%;flex:0 0 auto;
 background:radial-gradient(circle at 62% 32%,#fff,#cfcdff 35%,#7d7aff 75%,#4b48d6);
 box-shadow:0 0 22px #7d7affaa,0 0 60px #7d7aff44;animation:orb 5s ease-in-out infinite}
.orb::after{content:'';position:absolute;left:50%;top:50%;width:168%;height:48%;border-radius:50%;
 border:1.6px solid rgba(207,205,255,.6);border-top-color:transparent;transform:translate(-50%,-50%) rotate(-22deg)}
@keyframes orb{0%,100%{box-shadow:0 0 18px #7d7aff88,0 0 50px #7d7aff33}50%{box-shadow:0 0 30px #7d7affd0,0 0 80px #7d7aff55}}
.status{margin-left:auto;display:flex;gap:15px;flex-wrap:wrap;font-size:12.5px;color:var(--mut)}
.status b{color:var(--tx);font-weight:600}
.dot{display:inline-block;width:8px;height:8px;border-radius:50%;background:var(--red);margin-right:6px;vertical-align:middle;transition:.3s}
.dot.on{background:var(--green);animation:livePulse 2.4s ease-in-out infinite}
@keyframes livePulse{0%,100%{box-shadow:0 0 0 0 rgba(50,215,75,.6)}50%{box-shadow:0 0 0 7px rgba(50,215,75,0)}}

/* ===== layout / glass cards ===== */
.grid{display:grid;gap:14px}.g2{grid-template-columns:repeat(2,1fr)}.g3{grid-template-columns:repeat(3,1fr)}.g4{grid-template-columns:repeat(4,1fr)}
.card{position:relative;border-radius:24px;padding:21px;
 background:linear-gradient(165deg,rgba(255,255,255,.075),rgba(255,255,255,.028) 55%,rgba(255,255,255,.045));
 backdrop-filter:blur(20px) saturate(1.5);-webkit-backdrop-filter:blur(20px) saturate(1.5);
 border:1px solid rgba(255,255,255,.10);
 box-shadow:0 24px 60px -28px rgba(0,0,0,.9),inset 0 1px 0 rgba(255,255,255,.09);
 transition:transform .25s,box-shadow .25s}
.card::after{content:'';position:absolute;inset:0;border-radius:inherit;pointer-events:none;
 background:radial-gradient(560px 90px at 18% -8%,rgba(255,255,255,.085),transparent 62%)}
@media(hover:hover){.card.vis:hover{transform:translateY(-3px);
 box-shadow:0 30px 70px -26px rgba(0,0,0,.95),0 0 0 1px rgba(125,122,255,.18),inset 0 1px 0 rgba(255,255,255,.12)}}
.lbl{font-size:11.5px;color:var(--mut);font-weight:600;margin-bottom:10px;text-transform:uppercase;letter-spacing:.12em}
.val{font-size:34px;font-weight:700;letter-spacing:-.03em;line-height:1}
.unit{font-size:14px;color:var(--mut);font-weight:500;margin-left:4px;letter-spacing:-.01em}
.sub{color:var(--mut);font-size:13px;margin-top:7px}
.sec{display:flex;align-items:baseline;gap:11px;margin:34px 6px 14px;font-size:22px;font-weight:800;letter-spacing:-.025em}
.sec::before{content:'';width:9px;height:9px;border-radius:3px;flex:0 0 auto;align-self:center;
 background:linear-gradient(135deg,var(--ind),var(--vio));box-shadow:0 0 14px #7d7aff99}
.sec span{font-size:13px;font-weight:500;letter-spacing:0;color:var(--mut)}
.row{display:flex;align-items:center;gap:8px;justify-content:space-between;padding:12px 0;border-bottom:.5px solid var(--line);font-size:15.5px}
.row:last-child{border:0}.row span{color:var(--mut)}.row b{font-weight:500;color:var(--tx)}
.pill{display:inline-block;padding:6px 16px;border-radius:980px;font-weight:700;font-size:14px;
 background:var(--c2);border:1px solid rgba(255,255,255,.08)}
.note{color:var(--mut);font-size:13px;margin-top:12px;line-height:1.5}
.center{display:flex;flex-direction:column;align-items:center;text-align:center}
.flex{display:flex;align-items:center;gap:14px;flex-wrap:wrap}
@media(max-width:880px){.g4{grid-template-columns:repeat(2,1fr)}.g3{grid-template-columns:1fr}}
@media(max-width:560px){.g2,.g4{grid-template-columns:1fr}}

/* ===== hero score ring + halo ===== */
@property --p{syntax:'<number>';inherits:false;initial-value:0}
.ringWrap{position:relative;width:176px;height:176px;display:grid;place-items:center;margin:6px 0 2px}
.halo{position:absolute;inset:-16px;border-radius:50%;z-index:-1;
 background:conic-gradient(from 0deg,#5e5ce6,#bf5af2,#0a84ff,#5ce0d8,#5e5ce6);
 filter:blur(20px);opacity:.4;animation:spin 16s linear infinite}
@keyframes spin{to{transform:rotate(1turn)}}
.ring{--p:0;--c:var(--ind);width:176px;height:176px;border-radius:50%;transition:--p .8s cubic-bezier(.4,0,.2,1);
 background:conic-gradient(var(--c) calc(var(--p)*1%), rgba(255,255,255,.08) 0);
 display:grid;place-items:center;filter:drop-shadow(0 0 16px rgba(125,122,255,.35))}
.ring i{width:142px;height:142px;border-radius:50%;display:grid;place-items:center;font-style:normal;text-align:center;
 background:radial-gradient(circle at 50% 38%,#171a2e,#0b0d18 75%);box-shadow:inset 0 2px 14px #00000088}
.ring i b{font-size:42px;font-weight:800;letter-spacing:-.04em;display:block;line-height:1;
 background:linear-gradient(180deg,#fff,#c6c5ff);-webkit-background-clip:text;background-clip:text;color:transparent}
.ring i span{font-size:11px;color:var(--mut);letter-spacing:.14em;text-transform:uppercase;margin-top:5px;display:block}
.ring.small{width:118px;height:118px}.ring.small i{width:92px;height:92px}.ring.small i b{font-size:26px}

/* ===== vitals ===== */
.beat,.brth{display:inline-block;width:13px;height:13px;border-radius:50%;margin-right:10px;vertical-align:9px;opacity:.3;transform:scale(.75)}
.beat{background:var(--hr);box-shadow:0 0 14px #ff5e7a88}.brth{background:var(--br);box-shadow:0 0 14px #5ac8fa88}
.beat.on{animation:beat ease-in-out infinite}
.brth.on{animation:brth ease-in-out infinite}
@keyframes beat{0%,100%{transform:scale(.7);opacity:.5}20%{transform:scale(1.25);opacity:1}42%{transform:scale(.85)}60%{transform:scale(1.08);opacity:.9}}
@keyframes brth{0%,100%{transform:scale(.62);opacity:.4}50%{transform:scale(1.15);opacity:1}}
#hr{color:var(--hr);text-shadow:0 0 24px #ff5e7a55}
#br{color:var(--br);text-shadow:0 0 24px #5ac8fa55}

/* ===== charts / bars ===== */
canvas{width:100%;display:block}#hyp{height:186px}#vit{height:152px}#trend{height:164px}
.bar{height:7px;border-radius:4px;background:var(--track);overflow:hidden;margin-top:12px}
.bar>i{display:block;height:100%;width:0;border-radius:4px;transition:width .5s;
 background:linear-gradient(90deg,var(--teal),var(--ind));box-shadow:0 0 12px #7d7aff77}

/* ===== history list (one card per session, expandable, deletable) ===== */
.hitem{border-bottom:.5px solid var(--line)}.hitem:last-child{border:0}
.hrow{display:flex;align-items:center;gap:13px;padding:13px 2px;cursor:pointer;user-select:none}
.hrow:active{opacity:.55}
.hbadge{width:46px;height:46px;border-radius:50%;flex:0 0 auto;display:grid;place-items:center;
 font-weight:800;font-size:16px;font-variant-numeric:tabular-nums;background:var(--c2);border:2px solid}
.hmain{flex:1;min-width:0}
.hdate{display:block;font-weight:600;font-size:15px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis}
.hdate small{color:var(--teal);font-weight:700;font-size:10.5px;margin-left:7px;letter-spacing:.08em;text-transform:uppercase}
.hsub{display:block;color:var(--mut);font-size:12.5px;margin-top:3px}
.hbar{display:flex;height:5px;border-radius:3px;overflow:hidden;margin-top:7px;background:var(--track)}
.hbar i{height:100%}
.chev{color:var(--mut);transition:transform .25s;flex:0 0 auto}
.hitem.open .chev{transform:rotate(90deg)}
.hdet{max-height:0;overflow:hidden;transition:max-height .3s ease;display:grid;grid-template-columns:repeat(3,1fr);gap:9px}
.hitem.open .hdet{max-height:520px;padding:4px 0 16px}
.hdet div{background:var(--c2);border:1px solid rgba(255,255,255,.06);border-radius:13px;padding:11px 12px}
.hdet span{display:block;color:var(--mut);font-size:11px}
.hdet b{font-size:17px;font-weight:700}
.hdel{grid-column:1/-1;background:transparent;border:1px solid rgba(255,69,58,.45);color:var(--red);
 font-weight:700;font-size:13.5px;font-family:inherit;padding:11px;border-radius:13px;cursor:pointer;transition:.2s}
.hdel:hover{background:rgba(255,69,58,.12)}
.hdel:active{opacity:.6}
@media(max-width:560px){.hdet{grid-template-columns:repeat(2,1fr)}}

/* ===== controls ===== */
.seg{display:inline-flex;background:rgba(0,0,0,.35);border:1px solid rgba(255,255,255,.07);border-radius:12px;padding:3px;gap:3px}
.seg button{background:transparent;color:var(--tx);border:0;padding:8px 18px;font-weight:600;cursor:pointer;font-size:14px;border-radius:9px;transition:.2s}
.seg button.on{background:linear-gradient(135deg,#6e6cff,#9d5cf6);color:#fff;box-shadow:0 4px 16px -4px #7d7affcc}
.sw{display:flex;gap:12px;flex-wrap:wrap;margin-top:16px}
.sw div{width:38px;height:38px;border-radius:50%;cursor:pointer;border:2px solid #ffffff1c;transition:transform .15s,box-shadow .15s}
.sw div:hover{transform:scale(1.12);box-shadow:0 0 16px #ffffff33}
.sw div:active{transform:scale(.9)}
input[type=range]{-webkit-appearance:none;width:100%;height:5px;border-radius:3px;outline:none;margin-top:12px;
 background:linear-gradient(90deg,#6e6cff,#9d5cf6);box-shadow:0 0 10px #7d7aff44}
input[type=range]::-webkit-slider-thumb{-webkit-appearance:none;width:26px;height:26px;border-radius:50%;background:#fff;
 box-shadow:0 2px 10px #000c,0 0 0 5px #ffffff14;cursor:pointer}
input[type=time]{background:rgba(0,0,0,.35);border:1px solid rgba(255,255,255,.1);color:var(--tx);border-radius:11px;padding:9px 12px;font-size:16px;font-family:inherit}
.setupBtn{display:inline-block;text-decoration:none;text-align:center;border:0;border-radius:13px;padding:12px 19px;
 font-weight:700;font-size:15px;cursor:pointer;color:#fff;transition:.18s;
 background:linear-gradient(135deg,#6e6cff,#9d5cf6);box-shadow:0 10px 28px -10px #7d7affcc}
.setupBtn:hover{filter:brightness(1.12);box-shadow:0 14px 34px -10px #7d7affee}
.setupBtn:active{transform:scale(.97);filter:brightness(.95)}
.ghostBtn{background:var(--c2)!important;box-shadow:none!important;border:1px solid rgba(255,255,255,.1)}

/* ===== modal ===== */
.modal{position:fixed;inset:0;background:#000000c4;backdrop-filter:blur(26px) saturate(1.6);-webkit-backdrop-filter:blur(26px) saturate(1.6);
 display:none;align-items:center;justify-content:center;z-index:60;padding:18px}
.modal.show{display:flex}
.msheet{background:linear-gradient(170deg,#171a2cf2,#0c0e1af2);border:1px solid rgba(255,255,255,.1);
 border-radius:28px;padding:26px;max-width:390px;width:100%;text-align:center;box-shadow:0 40px 100px -30px #000}
.msheet::before{content:'';display:block;width:38px;height:5px;border-radius:3px;background:#ffffff2e;margin:-8px auto 16px}
.modal.show .msheet{animation:sheetUp .4s cubic-bezier(.32,.72,0,1)}
@keyframes sheetUp{from{transform:translateY(34px);opacity:.4}to{transform:none;opacity:1}}
.mhead{display:flex;justify-content:space-between;align-items:center;font-weight:800;font-size:20px;letter-spacing:-.02em;margin-bottom:6px}
.mx{cursor:pointer;color:var(--mut);font-size:23px;line-height:1}
.pstat{font-size:15px;font-weight:600;margin:16px 0;min-height:40px}
.checks{text-align:left;margin:6px 0 4px}
.chk{display:flex;align-items:center;gap:12px;padding:11px 0;border-bottom:.5px solid var(--line);font-size:15px;color:var(--mut);transition:color .3s}
.chk:last-child{border:0}
.chk i{width:23px;height:23px;border-radius:50%;border:2px solid var(--mut);flex:0 0 auto;display:grid;place-items:center;font-style:normal;font-size:13px;color:transparent;transition:.3s}
.chk.ok{color:var(--tx)}.chk.ok i{background:var(--green);border-color:var(--green);color:#000;box-shadow:0 0 14px #32d74b88}
.chk.ok i::after{content:'\2713'}
.pacer{width:64px;height:64px;border-radius:50%;margin:15px auto 4px;
 background:radial-gradient(circle at 50% 40%,#aaa6ff,#5e5ce6 72%);box-shadow:0 0 40px #7d7aff77;animation:pace 8s ease-in-out infinite}
@keyframes pace{0%,100%{transform:scale(.5);opacity:.5}50%{transform:scale(1);opacity:1}}

/* ===== stage chips under the hypnogram ===== */
.chips{display:flex;gap:8px;flex-wrap:wrap;margin-top:13px;min-height:30px}
.chip{display:inline-flex;align-items:center;gap:7px;padding:6px 13px;border-radius:999px;font-size:12.5px;font-weight:600;color:var(--mut);
 background:var(--c2);border:1px solid rgba(255,255,255,.08)}
.chip i{width:9px;height:9px;border-radius:3px;font-style:normal}
.chip b{color:var(--tx);font-variant-numeric:tabular-nums}

/* ===== cosmic extras: progress bar, to-top, toast ===== */
#prog{position:fixed;top:0;left:0;height:3px;width:0;z-index:90;
 background:linear-gradient(90deg,#5ce0d8,#7d7aff,#bf5af2);box-shadow:0 0 12px #7d7aff99;transition:width .15s linear}
#toTop{position:fixed;right:16px;bottom:16px;width:46px;height:46px;border-radius:50%;z-index:70;cursor:pointer;
 display:grid;place-items:center;font-size:19px;color:#fff;border:1px solid rgba(255,255,255,.14);
 background:rgba(12,14,26,.72);backdrop-filter:blur(14px);-webkit-backdrop-filter:blur(14px);
 opacity:0;pointer-events:none;transition:.3s;box-shadow:0 10px 30px -8px #000c}
#toTop.show{opacity:1;pointer-events:auto}
#toTop:hover{box-shadow:0 0 0 1px rgba(125,122,255,.4),0 10px 30px -8px #000c}
#toast{position:fixed;left:50%;bottom:24px;transform:translateX(-50%) translateY(16px);z-index:95;opacity:0;pointer-events:none;
 background:rgba(18,20,38,.94);border:1px solid rgba(255,255,255,.14);color:var(--tx);
 padding:11px 20px;border-radius:14px;font-size:14px;font-weight:600;transition:.35s;box-shadow:0 16px 44px -12px #000}
#toast.show{opacity:1;transform:translateX(-50%)}

/* ===== reveal on scroll ===== */
.sec,.card{opacity:0;transform:translateY(16px)}
.sec.vis,.card.vis{opacity:1;transform:none;transition:opacity .65s ease,transform .65s cubic-bezier(.2,.7,.2,1)}
.val,.ring i b,.hdet b,.status b{font-variant-numeric:tabular-nums}
.seg button,.setupBtn,.sw div{-webkit-tap-highlight-color:transparent}
@media(prefers-reduced-motion:reduce){*{animation:none!important;transition:none!important}.sec,.card{opacity:1;transform:none}}
</style></head><body>
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

<script>
const $=id=>document.getElementById(id),clamp=(v,a,b)=>Math.max(a,Math.min(b,v));
const STATE=[['Deep sleep','--deep'],['Light sleep','--light'],['Awake','--awake'],['None','--mut']];
const MOT=['None','Still','Active'];
function rateOf(s){return s>=75?['Good','--green']:s>=50?['Average','--amber']:s>0?['Poor','--red']:['-','--mut'];}
const PRESET=[['Warm',255,150,60],['Soft',255,210,170],['Reading',255,240,220],['Calm',255,70,30],['Cool',170,200,255],['Moon',120,150,235]];
const cssv=n=>getComputedStyle(document.documentElement).getPropertyValue(n).trim();
function ring(el,id,v,lo,hi,t){el.style.setProperty('--p',clamp((v-lo)/(hi-lo)*100,0,100));if(id)$(id).textContent=t!=null?t:v;}
function fmtUp(s){let h=s/3600|0,m=(s%3600)/60|0;return (h?h+'h ':'')+m+'m';}
// smooth number display (lerp toward target -> no jumpy vitals)
const disp={};
function smooth(id,v){
 if(!v){$(id).textContent='0';delete disp[id];return;}
 let c=disp[id];if(c==null||Math.abs(c-v)>40)c=v;else c+= (v-c)*0.35;
 disp[id]=c;$(id).textContent=Math.round(c);
}

// lamp
function setLight(p){fetch('/api/light?'+new URLSearchParams(p)).then(r=>r.json()).then(updLight).catch(()=>{});}
function updLight(L){document.querySelectorAll('.lmode').forEach(b=>b.classList.toggle('on',+b.dataset.m===L.mode));
 if(document.activeElement!==$('bri')){$('bri').value=L.bright;$('briV').textContent=L.bright;}
 const c='rgb('+L.r+','+L.g+','+L.b+')';
 $('prev').style.background=c;$('prev').style.boxShadow='0 0 20px '+c;}
(function(){const s=$('swatches');PRESET.forEach(([n,r,g,b])=>{const d=document.createElement('div');
 d.title=n;d.style.background='rgb('+r+','+g+','+b+')';d.onclick=()=>setLight({mode:1,r,g,b});s.appendChild(d);});
 document.querySelectorAll('.lmode').forEach(b=>b.onclick=()=>setLight({mode:+b.dataset.m}));
 $('bri').oninput=()=>$('briV').textContent=$('bri').value;
 $('bri').onchange=()=>setLight({mode:1,bright:+$('bri').value});
 $('pick').onchange=()=>{const h=$('pick').value;setLight({mode:1,r:parseInt(h.substr(1,2),16),g:parseInt(h.substr(3,2),16),b:parseInt(h.substr(5,2),16)});};})();

// alarm
let al={en:0};
function setAlarm(p){fetch('/api/alarm?'+new URLSearchParams(p)).then(r=>r.json()).then(a=>al=a).catch(()=>{});}
(function(){$('alBtn').onclick=()=>setAlarm({en:al.en?0:1});
 $('alTime').onchange=()=>{const p=$('alTime').value.split(':');setAlarm({en:1,h:+p[0],m:+p[1]});};
 $('alWin').oninput=()=>$('alWinV').textContent=$('alWin').value;
 $('alWin').onchange=()=>setAlarm({win:+$('alWin').value});})();
const p2=x=>String(x).padStart(2,'0');
function updAlarm(d){const b=$('alBtn');b.textContent=d.alOn?'On':'Off';b.classList.toggle('on',!!d.alOn);
 if(document.activeElement!==$('alTime'))$('alTime').value=p2(d.alH)+':'+p2(d.alM);
 if(document.activeElement!==$('alWin')){$('alWin').value=d.alWin;$('alWinV').textContent=d.alWin;}
 $('alStat').textContent=d.alFire?'Sunrise alarm firing — get out of bed to stop it.':
  (d.alOn?('Armed for '+p2(d.alH)+':'+p2(d.alM)+' · light-sleep window '+d.alWin+' min.'):
  'Turn On to arm. Sunrise lamp wakes you during light sleep.');}

// placement
let placeOpen=false;
function openPlace(){placeOpen=true;$('placeModal').classList.add('show');}
function closePlace(){placeOpen=false;$('placeModal').classList.remove('show');}
function recalib(){fetch('/api/sensor?reset=1').catch(()=>{});$('pStatus').textContent='Recalibrating radar…';toast('Recalibrating radar…');}
function updPlace(d){
 const c1=!!d.presence,c2=c1&&d.movingRange<=25,c3=d.breathRate>0||(d.breathState>=1&&d.breathState<=3),c4=d.heartRate>0;
 [['ck1',c1],['ck2',c2],['ck3',c3],['ck4',c4]].forEach(([id,v])=>$(id).classList.toggle('ok',v));
 const sc=(c1?20:0)+(c2?20:0)+(c3?30:0)+(c4?30:0);
 $('pRing').style.setProperty('--p',sc);$('pScore').textContent=sc;
 $('pRing').style.setProperty('--c',sc>=80?'var(--green)':sc>=40?'var(--amber)':'var(--ind)');
 let m;
 if(!c1)m='Step in front of the lamp (within 1.5 m).';
 else if(!c2)m='Detected — hold still and face the lamp.';
 else if(!c3)m='Almost — keep still, breathe slowly…';
 else if(!c4)m='Breathing found — locking heart rate…';
 else m='Perfect placement! Vitals locked.';
 $('pStatus').textContent=m;$('pStatus').style.color=sc>=80?cssv('--green'):cssv('--tx');}

// session control
let endingMsg=false;
async function endSession(){
 if(!confirm('End the sleep session now and save the report?'))return;
 try{await fetch('/api/report?end=1');}catch(e){}
 endingMsg=true;$('tStat').textContent='Generating report…';toast('Session ended — report saved below');
 setTimeout(()=>{endingMsg=false;loadTrend();loadSession();},2500);
}
async function clearHist(){
 if(!confirm('Delete ALL saved sessions?'))return;
 try{await fetch('/api/history?clear=1');}catch(e){}
 toast('History cleared');loadTrend();
}
// tiny toast notifications
let toastT;
function toast(m){const t=$('toast');t.textContent=m;t.classList.add('show');
 clearTimeout(toastT);toastT=setTimeout(()=>t.classList.remove('show'),2600);}

// rule-based sleep coach — one human sentence about the last report
function insightOf(d){
 if(!d.rOk)return 'Complete a sleep session to get your first report and personal insight.';
 const eff=d.rBed>0?Math.round(d.rSleep*100/d.rBed):0;
 if(d.rOnset>30)return 'It took '+d.rOnset+' min to fall asleep. Try winding down 30 min earlier — dim the lamp, no screens in bed.';
 if(eff>0&&eff<70)return 'Sleep efficiency was only '+eff+'% — you spent '+fmtMin(d.rAwake)+' awake in bed. Go to bed when actually sleepy, not just tired.';
 if(d.rWakes>=3)return 'You woke up '+d.rWakes+' times. A cooler, darker, quieter room usually cuts night awakenings.';
 if(d.rDeepP<15&&d.rSleep>=120)return 'Deep sleep was '+d.rDeepP+'% (target ~20-25%). Regular bed times and no late caffeine help deepen sleep.';
 if(d.rSleep<360&&d.rSleep>=60)return 'Only '+fmtMin(d.rSleep)+' of sleep — most adults need 7-9 h. Tonight, aim for an earlier bedtime.';
 return 'Solid session: '+fmtMin(d.rSleep)+' asleep, '+eff+'% efficiency, '+d.rDeepP+'% deep, '+d.rWakes+' awakening'+(d.rWakes==1?'':'s')+'. Keep this rhythm going.';
}

// ===== history: stats strip + organized session list + per-session delete =====
const MON=['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];
function fmtDate(s){if(!s||!/^\d{4}-/.test(s))return s||'-';const[d,t]=s.split(' ');const[y,mo,da]=d.split('-');
 return da+' '+(MON[(+mo)-1]||mo)+' · '+(t||'');}
function fmtMin(m){m=+m||0;return (m/60|0)+'h '+(m%60)+'m';}
let histCache=[];
async function loadTrend(){try{const H=await(await fetch('/api/history')).json();renderHistory(H);}catch(e){}}
function renderHistory(H){histCache=H;drawTrend(H);drawStats(H);drawHist(H);}
function drawStats(H){const n=H.length;$('hN').textContent=n;
 $('histSub').textContent=n?('· '+n+' session'+(n==1?'':'s')+' stored on the lamp'):'· stored on the lamp';
 if(!n){$('hAvgS').textContent='-';$('hAvgM').textContent='-';$('hBest').textContent='-';return;}
 const L7=H.slice(-7);
 $('hAvgS').textContent=Math.round(L7.reduce((a,r)=>a+(+r.score||0),0)/L7.length);
 $('hAvgM').textContent=fmtMin(Math.round(L7.reduce((a,r)=>a+(+r.sleep||0),0)/L7.length));
 $('hBest').textContent=H.reduce((m,r)=>Math.max(m,+r.score||0),0);}
// "Today"/"Yesterday" tag so the list is scannable at a glance
function dayTag(s){if(!/^\d{4}-/.test(s))return'';
 const d=new Date(s.replace(' ','T')),t=new Date();
 const dd=Math.round((new Date(t.getFullYear(),t.getMonth(),t.getDate())
                     -new Date(d.getFullYear(),d.getMonth(),d.getDate()))/864e5);
 return dd===0?'Today':dd===1?'Yesterday':'';}
// deep/light/awake composition strip for one session
function compBar(r){const sl=+r.sleep||0,dp=Math.round(sl*(+r.deep||0)/100),lt=Math.round(sl*(+r.shallow||0)/100),
  aw=+r.wake||0,tot=Math.max(1,dp+lt+aw);
 return '<span class="hbar"><i style="width:'+(dp*100/tot)+'%;background:'+cssv('--deep')+'"></i>'
  +'<i style="width:'+(lt*100/tot)+'%;background:'+cssv('--light')+'"></i>'
  +'<i style="width:'+(aw*100/tot)+'%;background:'+cssv('--awake')+'"></i></span>';}
function detail(r,i){const it=(k,v)=>'<div><span>'+k+'</span><b>'+v+'</b></div>';
 return it('Score',r.score)+it('Total sleep',fmtMin(r.sleep))+it('In bed',fmtMin(r.bed||0))
  +it('Deep',(r.deep||0)+'%')+it('Light',(r.shallow||0)+'%')+it('Awake',fmtMin(r.wake))
  +it('Fell asleep',(r.onset||0)+' min')+it('Awakenings',r.wakes||0)+it('Turnovers',r.turn||0)
  +it('Avg heart',(r.hr||0)+' bpm')+it('Avg resp',(r.resp||0)+' rpm')+it('Apnea',r.apnea||0)
  +'<button class="hdel" onclick="delSession(event,'+i+')">Delete this session</button>';}
function drawHist(H){const n=H.length;
 if(!n){$('hist').innerHTML='<div class="note" style="margin:0">No sessions yet — finish a sleep session and it appears here automatically.</div>';return;}
 let h='';
 for(let i=n-1;i>=0;i--){const r=H[i],sc=+r.score||0,col=cssv(sc>=75?'--green':sc>=50?'--amber':'--red');
  const tag=dayTag(r.t),hasT=/^\d{4}-/.test(r.t),wk=+r.wakes||0;
  h+='<div class="hitem"><div class="hrow" onclick="this.parentNode.classList.toggle(\'open\')">'
   +'<span class="hbadge" style="color:'+col+';border-color:'+col+'">'+sc+'</span>'
   +'<span class="hmain"><span class="hdate">'+(hasT?fmtDate(r.t):'Clock not set')
   +(tag?'<small>'+tag+'</small>':'')+'</span>'
   +'<span class="hsub">'+fmtMin(r.sleep)+' asleep &middot; '+(r.deep||0)+'% deep &middot; '+wk+' wake'+(wk==1?'':'s')+'</span>'
   +compBar(r)+'</span>'
   +'<span class="chev">&#9656;</span>'
   +'</div><div class="hdet">'+detail(r,i)+'</div></div>';}
 $('hist').innerHTML=h;}
// delete one session: index + timestamp guard; device answers with the
// updated list so the UI re-renders from truth, not assumption
async function delSession(ev,i){ev.stopPropagation();
 const r=histCache[i];if(!r)return;
 if(!confirm('Delete this session ('+(/^\d{4}-/.test(r.t)?fmtDate(r.t):'no clock')+', score '+r.score+')?'))return;
 try{const H=await(await fetch('/api/history?del='+i+'&t='+encodeURIComponent(r.t))).json();
  renderHistory(H);toast('Session deleted');}
 catch(e){toast('Delete failed — try again');loadTrend();}}

// history buffers — hypnogram (stg) comes from the DEVICE session recorder
const VMAX=300;let hrA=[],brA=[],stg=[];
function pushV(h,b){hrA.push(h>0?h:null);brA.push(b>0?b:null);if(hrA.length>VMAX){hrA.shift();brA.shift();}}
async function loadSession(){try{const s=await(await fetch('/api/session')).json();
 if(s&&s.stage){stg=s.stage;drawHyp();}}catch(e){}}

// canvases
function prep(c){const dpr=devicePixelRatio||1,w=c.clientWidth,h=c.clientHeight;c.width=w*dpr;c.height=h*dpr;
 const x=c.getContext('2d');x.setTransform(dpr,0,0,dpr,0,0);x.clearRect(0,0,w,h);return[x,w,h];}
function roundBar(x,rx,ry,rw,rh,r){r=Math.min(r,rw/2,rh/2);x.beginPath();
 x.moveTo(rx+r,ry);x.arcTo(rx+rw,ry,rx+rw,ry+rh,r);x.arcTo(rx+rw,ry+rh,rx,ry+rh,r);
 x.arcTo(rx,ry+rh,rx,ry,r);x.arcTo(rx,ry,rx+rw,ry,r);x.closePath();x.fill();}
function drawHyp(){const c=$('hyp');if(!c.clientWidth)return;const[x,w,h]=prep(c);
 const lanes=[{s:2,n:'Awake',c:'--awake'},{s:1,n:'Light',c:'--light'},{s:0,n:'Deep',c:'--deep'}];
 const x0=48,top=6,bot=h-6,lh=(bot-top)/lanes.length;
 x.font='11px system-ui';x.textAlign='left';
 lanes.forEach((L,k)=>{const yc=top+lh*k+lh/2;x.fillStyle=cssv(L.c);x.fillText(L.n,6,yc+4);
  x.strokeStyle=cssv('--line');x.beginPath();x.moveTo(x0,top+lh*k);x.lineTo(w-8,top+lh*k);x.stroke();});
 const N=stg.length;if(N<2){hypRuns=[];hypN=N;hypDefault();return;}const xw=w-8-x0;
 const laneY=s=>{const i=lanes.findIndex(L=>L.s===s);return top+lh*i+lh/2;};
 const th=Math.min(18,lh*0.52);
 let runs=[],i=0;while(i<N){const s=stg[i];if(s>2){i++;continue;}let j=i;while(j+1<N&&stg[j+1]===s)j++;runs.push([s,i,j]);i=j+1;}
 x.strokeStyle=cssv('--line');x.lineWidth=2;
 for(let k=1;k<runs.length;k++){const xb=x0+xw*runs[k][1]/(N-1),ya=laneY(runs[k-1][0]),yb=laneY(runs[k][0]);
  x.beginPath();x.moveTo(xb,Math.min(ya,yb));x.lineTo(xb,Math.max(ya,yb));x.stroke();}
 runs.forEach(([s,a,b])=>{const xa=x0+xw*a/(N-1),xb=x0+xw*b/(N-1),yc=laneY(s),col=cssv(lanes.find(L=>L.s===s).c);
  x.shadowColor=col;x.shadowBlur=12;x.fillStyle=col;
  roundBar(x,xa,yc-th/2,Math.max(th,xb-xa),th,th/2);});
 x.shadowBlur=0;
 // remember bar geometry so hover/tap can identify the stretch under the cursor
 hypRuns=runs.map(([s,a,b])=>{const xa=x0+xw*a/(N-1);
  return{s,a,b,xa,xb:Math.max(xa+th,x0+xw*b/(N-1))};});
 hypN=N;hypDefault();}

// hypnogram interactivity — hover/tap a bar -> exact start, end & duration;
// idle -> per-stage totals for this session
let hypRuns=[],hypN=0;
const SNAME=['Deep sleep','Light sleep','Awake'],SCOL=['--deep','--light','--awake'];
const fmtM=m=>m>=60?(m/60|0)+'h '+(m%60)+'m':m+'m';
const chipOf=(c,t)=>'<span class="chip"><i style="background:'+cssv(c)+';box-shadow:0 0 8px '+cssv(c)+'"></i>'+t+'</span>';
function hypDefault(){
 if(!stg.some(s=>s<=2)){$('hypInfo').innerHTML=chipOf('--mut','No stages yet — the chart fills in while you sleep');return;}
 let n=[0,0,0];stg.forEach(s=>{if(s<=2)n[s]++;});
 $('hypInfo').innerHTML=
  chipOf('--awake','Awake <b>'+fmtM(n[2])+'</b>')+
  chipOf('--light','Light <b>'+fmtM(n[1])+'</b>')+
  chipOf('--deep','Deep <b>'+fmtM(n[0])+'</b>')+
  '<span class="chip">hover / tap a bar for details</span>';
}
function hypHit(ev){
 if(!hypRuns.length)return;
 const c=$('hyp'),r=c.getBoundingClientRect();
 const cx=(ev.touches&&ev.touches.length?ev.touches[0].clientX:ev.clientX)-r.left;
 const hit=hypRuns.find(R=>cx>=R.xa-3&&cx<=R.xb+3);
 if(!hit){hypDefault();return;}
 // ring holds 1 sample/min and the newest sample is "now"
 const tAt=i=>new Date(Date.now()-(hypN-1-i)*60000);
 const f=t=>p2(t.getHours())+':'+p2(t.getMinutes());
 const t0=tAt(hit.a),t1=new Date(Math.min(Date.now(),tAt(hit.b).getTime()+60000));
 $('hypInfo').innerHTML=chipOf(SCOL[hit.s],
  SNAME[hit.s]+' <b>'+f(t0)+' &ndash; '+f(t1)+'</b> &middot; lasted <b>'+fmtM(hit.b-hit.a+1)+'</b>');
}
(function(){const c=$('hyp');
 c.addEventListener('mousemove',hypHit);c.addEventListener('click',hypHit);
 c.addEventListener('touchstart',hypHit,{passive:true});
 c.addEventListener('mouseleave',hypDefault);hypDefault();})();
function line(x,a,x0,xw,h,pad,lo,hi,col){x.strokeStyle=col;x.lineWidth=2;x.lineJoin='round';
 x.shadowColor=col;x.shadowBlur=9;x.beginPath();let st=false;
 for(let i=0;i<a.length;i++){const v=a[i];if(v==null){st=false;continue;}
  const px=x0+xw*i/(a.length-1),py=h-pad-((clamp(v,lo,hi)-lo)/(hi-lo))*(h-2*pad);
  if(!st){x.moveTo(px,py);st=true;}else x.lineTo(px,py);}x.stroke();x.shadowBlur=0;}
function drawVit(){const c=$('vit');if(!c.clientWidth)return;const[x,w,h]=prep(c);const pad=14,x0=8,xw=w-16;
 for(let i=0;i<=2;i++){x.strokeStyle=cssv('--line');const yy=pad+(h-2*pad)*i/2;x.beginPath();x.moveTo(x0,yy);x.lineTo(w-8,yy);x.stroke();}
 line(x,hrA,x0,xw,h,pad,40,120,cssv('--hr'));line(x,brA,x0,xw,h,pad,5,30,cssv('--br'));}
function drawTrend(H){H=H.slice(-14);const c=$('trend');if(!c.clientWidth)return;const[x,w,h]=prep(c);const n=H.length;
 const pL=30,pR=12,pT=16,pB=16;
 x.font='10px system-ui';x.textAlign='right';
 [0,25,50,75,100].forEach(v=>{const yy=pT+(h-pT-pB)*(1-v/100);
  x.strokeStyle=cssv('--line');x.beginPath();x.moveTo(pL,yy);x.lineTo(w-pR,yy);x.stroke();
  x.fillStyle=cssv('--mut');x.fillText(v,pL-6,yy+3);});
 if(!n){x.fillStyle=cssv('--mut');x.textAlign='left';x.fillText('No sessions saved yet.',pL,h/2);return;}
 const xw=w-pL-pR,gx=i=>n>1?pL+xw*i/(n-1):pL+xw/2,gy=v=>pT+(h-pT-pB)*(1-clamp(v,0,100)/100);
 const grad=x.createLinearGradient(0,pT,0,h-pB);
 grad.addColorStop(0,'rgba(125,122,255,.32)');grad.addColorStop(1,'rgba(125,122,255,0)');
 x.beginPath();x.moveTo(gx(0),h-pB);
 for(let i=0;i<n;i++)x.lineTo(gx(i),gy(+H[i].score||0));
 x.lineTo(gx(n-1),h-pB);x.closePath();x.fillStyle=grad;x.fill();
 x.beginPath();for(let i=0;i<n;i++){const px=gx(i),py=gy(+H[i].score||0);i?x.lineTo(px,py):x.moveTo(px,py);}
 x.strokeStyle=cssv('--ind');x.lineWidth=2;x.lineJoin='round';
 x.shadowColor=cssv('--ind');x.shadowBlur=10;x.stroke();x.shadowBlur=0;
 for(let i=0;i<n;i++){const s=+H[i].score||0,px=gx(i),py=gy(s);
  const col=s>=75?cssv('--green'):s>=50?cssv('--amber'):cssv('--red');
  x.shadowColor=col;x.shadowBlur=8;x.fillStyle=col;
  x.beginPath();x.arc(px,py,3.5,0,7);x.fill();x.shadowBlur=0;
  x.fillStyle=cssv('--tx');x.font='10px system-ui';x.textAlign='center';x.fillText(s,px,py-8);}}

async function tick(){
 try{
  const d=await(await fetch('/api/data',{cache:'no-store'})).json();
  $('dot').classList.add('on');
  $('conn').textContent=d.radarOk?'live':'radar starting…';
  $('conn').style.color=d.radarOk?'':'var(--amber)';
  $('rssi').textContent=d.rssi+'dBm';$('up').textContent=fmtUp(d.uptime);$('seq').textContent=d.seq;
  $('fw').textContent='v'+(d.fw||'?');$('heapKB').textContent=(d.heap/1024|0)+' KB';
  $('clock').textContent=d.timeOk?d.time:'--:--';
  if(placeOpen)updPlace(d);
  // score ring: live engine score during a session, else the last saved report
  const sc=d.sessOn?d.liveScore:(d.rOk?d.rScore:0);
  ring($('scoreRing'),'score',sc,0,100,sc>0?sc:'-');
  $('scoreRing').style.setProperty('--c',sc>=75?'var(--green)':sc>=50?'var(--amber)':sc>0?'var(--red)':'var(--ind)');
  $('scoreSub').textContent=d.sessOn?'live':(d.rOk?'last session':'tonight');
  const rt=rateOf(sc);$('rating').textContent=rt[0];$('rating').style.color=cssv(rt[1]);
  // current state from OUR engine (responds from minute 1, unlike the radar's)
  const pill=$('state');
  if(d.sessOn){const st=STATE[d.myStage]||STATE[3];pill.textContent=st[0];pill.style.color=cssv(st[1]);}
  else{pill.textContent=d.presence?'Up':'Away';pill.style.color=cssv('--mut');}
  $('sess').textContent=d.sessOn?('since '+d.since+' · '+fmtMin(d.bedMin)):'—';
  $('presence').textContent=d.presence?'Present':'None';
  $('bed').textContent=d.inBed?'In bed':'Out';
  $('motion').textContent=MOT[d.motion]||'-';
  // vitals: smooth numbers + dots pulsing at the MEASURED rate
  smooth('hr',d.heartRate);smooth('br',d.breathRate);
  const bd=$('beatDot'),bb=$('brthDot');
  bd.classList.toggle('on',d.heartRate>0);bb.classList.toggle('on',d.breathRate>0);
  bd.style.animationDuration=(d.heartRate>0?(60/d.heartRate).toFixed(2):'1.4')+'s';
  bb.style.animationDuration=(d.breathRate>0?(60/d.breathRate).toFixed(2):'4')+'s';
  const vn=$('vitNote');
  if(d.heartRate>0&&d.breathRate>0){vn.textContent='● Vitals locked';vn.style.color=cssv('--green');}
  else{vn.textContent='Needs stillness within 1.5 m';vn.style.color='';}
  $('temp').textContent=d.envValid&&d.tempC>0?d.tempC:'-';
  $('humid').textContent=d.envValid&&d.humidity>0?d.humidity:'-';
  $('cTurn').textContent=d.sessOn?d.turns:d.cTurn;
  $('mrv').textContent=d.movingRange;$('mrBar').style.width=clamp(d.movingRange,0,100)+'%';
  updLight({mode:d.lightMode,r:d.lr,g:d.lg,b:d.lb,bright:d.lbright});
  updAlarm(d);
  // tonight (live session)
  $('tBed').textContent=d.bedMin;$('tAsleep').textContent=d.asleepMin;
  $('tDeep').textContent=d.deepMin;$('tLight').textContent=d.lightMin;
  $('tOnset').textContent=(d.sessOn&&d.onset>=0)?d.onset+' min':'-';
  $('tWakes').textContent=d.wakes;$('tTurns').textContent=d.turns;
  $('tonightSub').textContent=d.sessOn?'· live session':'· waiting for bed';
  $('endBtn').style.display=d.sessOn?'':'none';
  $('ip').textContent=d.ip||'-';
  if(!endingMsg)$('tStat').textContent=d.sessOn
    ?('Session running since '+d.since+' — awake '+d.awakeMin+' min. Auto-ends 8 min after you get up, or end it now:')
    :'No session — lie in bed and stay still; tracking starts within a minute.';
  // night report (last completed session)
  $('rWhen').textContent=d.rOk?('· '+fmtDate(d.rWhen)):'· no session saved yet';
  $('rScore').textContent=d.rOk?d.rScore:'-';
  $('rSleep').textContent=d.rSleep;
  $('rEff').textContent=d.rBed>0?Math.round(d.rSleep*100/d.rBed):'-';
  $('rDeepP').textContent=d.rDeepP;$('rLightP').textContent=d.rLightP+' %';
  $('rBed').textContent=d.rOk?fmtMin(d.rBed):'-';
  $('rAwake').textContent=d.rOk?fmtMin(d.rAwake):'-';
  $('rOnset').textContent=d.rOk?d.rOnset+' min':'-';
  $('rWakes').textContent=d.rWakes;$('rTurn').textContent=d.rTurn;$('rApnea').textContent=d.rApnea;
  $('rHR').textContent=d.rHR?d.rHR+' bpm':'-';$('rBR').textContent=d.rBR?d.rBR+' rpm':'-';
  $('insight').textContent=insightOf(d);
  pushV(d.heartRate,d.breathRate);
  drawVit();
 }catch(e){$('dot').classList.remove('on');$('conn').textContent='offline';$('conn').style.color='';}
}
tick();setInterval(tick,500);
loadSession();setInterval(loadSession,30000);
loadTrend();setInterval(loadTrend,60000);
addEventListener('resize',()=>{drawHyp();drawVit();loadTrend();});

// reveal-on-scroll (with fallback so the page can never stay invisible)
if('IntersectionObserver' in window){
 const io=new IntersectionObserver(es=>es.forEach(e=>{
  if(e.isIntersecting){e.target.classList.add('vis');io.unobserve(e.target);}}),{threshold:.06});
 document.querySelectorAll('.card,.sec').forEach(el=>io.observe(el));
}else{
 document.querySelectorAll('.card,.sec').forEach(el=>el.classList.add('vis'));
}

// scroll progress bar + back-to-top
addEventListener('scroll',()=>{
 const m=document.documentElement.scrollHeight-innerHeight;
 $('prog').style.width=(m>0?scrollY/m*100:0)+'%';
 $('toTop').classList.toggle('show',scrollY>600);
},{passive:true});

// subtle star parallax (desktop pointers only — phones keep it calm)
if(matchMedia('(pointer:fine)').matches){
 let pend=false;
 addEventListener('mousemove',e=>{
  if(pend)return;pend=true;
  requestAnimationFrame(()=>{pend=false;
   const dx=e.clientX/innerWidth-.5,dy=e.clientY/innerHeight-.5;
   document.querySelector('.stars').style.transform='translate('+(-dx*12)+'px,'+(-dy*8)+'px)';
   document.querySelector('.stars2').style.transform='translate('+(-dx*24)+'px,'+(-dy*16)+'px)';
  });
 });
}
</script></body></html>
)PAGE";
