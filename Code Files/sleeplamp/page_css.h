// SleepLamp - dashboard stylesheet (PROGMEM, streamed by handleRoot in WebUI.ino)
#pragma once
#include <Arduino.h>

const char PAGE_CSS[] PROGMEM = R"SLCSS(
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
)SLCSS";
