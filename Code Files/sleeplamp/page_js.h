// SleepLamp - dashboard logic (JavaScript) (PROGMEM, streamed by handleRoot in WebUI.ino)
#pragma once
#include <Arduino.h>

const char PAGE_JS[] PROGMEM = R"SLJS(<script>
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
)SLJS";
