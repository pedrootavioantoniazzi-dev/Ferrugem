// Shorts verticais (1080x1920) do Ferrugem 2D: relogio virtual + foto de cada quadro.
// Uso: node shorts.js <primeira|energia|evolucao>  (gera frames_<nome>/)
const {chromium}=require('/opt/node22/lib/node_modules/playwright');
const fs=require('fs'),path=require('path');
const GAME='file://'+path.resolve(__dirname,'../../index.html'),FPS=30,DT=1000/FPS,W=1080,H=1920;
const WHICH=process.argv[2];const OUT=path.join(process.env.SAIDA||__dirname,'frames_'+WHICH);fs.rmSync(OUT,{recursive:true,force:true});fs.mkdirSync(OUT,{recursive:true});
const CLOCK=()=>{let vt=0;const q=[];performance.now=()=>vt;window.requestAnimationFrame=cb=>{q.push(cb);return q.length};window.cancelAnimationFrame=()=>{};
 window.__step=ms=>{vt+=ms;const c=q.splice(0);for(const f of c)f(vt)};
 try{localStorage.setItem('ferrugem_temporada','2');localStorage.setItem('ferrugem_mode','pc');localStorage.setItem('ferrugem_som','0');localStorage.setItem('ferrugem_lite','0');localStorage.setItem('ferrugem_tut','-1')}catch(e){}};
const OVERLAY=()=>{document.getElementById('splash')?.remove();const st=document.createElement('style');st.textContent=`
 #top,#bar,#ctl,#ctb,#help,#fact,#panel,#toast,#md,#mm{display:none!important}
 #ov{position:fixed;inset:0;z-index:1000;pointer-events:none;font-family:'Inter Display',Inter,sans-serif}
 #ovDim{position:absolute;inset:0;background:#05070a;opacity:0}
 #ovG{position:absolute;left:0;right:0;top:0;height:34%;background:linear-gradient(180deg,#05070ae0,#05070a00)}
 #ovG2{position:absolute;left:0;right:0;bottom:0;height:36%;background:linear-gradient(0deg,#05070ae6,#05070a00)}
 #ovT{position:absolute;left:60px;right:60px;top:170px;text-align:center;font-weight:900;font-size:104px;line-height:.98;color:#fff;letter-spacing:-2px;text-shadow:0 6px 30px #000c,0 2px 4px #000;opacity:0}
 #ovT em{font-style:normal;color:#ffa733}
 #ovS{position:absolute;left:50px;right:50px;bottom:360px;text-align:center;opacity:0}
 #ovS div{display:inline-block;background:rgba(10,14,20,.88);border:3px solid #ff9f1c;border-radius:26px;padding:22px 34px;font-weight:800;font-size:58px;line-height:1.12;color:#fff;box-shadow:0 12px 40px #000a}
 #ovS small{display:block;font-family:Inter,sans-serif;font-weight:500;font-size:38px;color:#c9d4de;margin-top:10px;line-height:1.25}
 #ovN{position:absolute;right:50px;top:90px;font-family:Inter,sans-serif;font-weight:800;font-size:40px;color:#ffb84d;opacity:0;background:#0a0e14cc;padding:8px 22px;border-radius:40px;border:2px solid #ff9f1c88}
 #end{position:absolute;inset:0;display:flex;flex-direction:column;align-items:center;justify-content:center;opacity:0;text-align:center;padding:0 60px}
 #end .logo{font-weight:900;font-size:168px;letter-spacing:-5px;line-height:.9;background:linear-gradient(180deg,#ffe0a8,#ff9f1c 55%,#c2410c);-webkit-background-clip:text;color:transparent;filter:drop-shadow(0 10px 40px #ff9f1c55)}
 #end .sub{font-weight:700;font-size:48px;letter-spacing:22px;color:#e6edf3;margin:14px 0 30px;padding-left:22px}
 #end .by{font-family:Inter,sans-serif;font-size:36px;color:#c9d4de;margin-bottom:70px}#end .by b{color:#ffb84d}
 #end .cta{font-weight:800;font-size:58px;color:#fff;background:#ff9f1c22;border:3px solid #ff9f1c;border-radius:30px;padding:24px 40px;line-height:1.15}
 #end .cta small{display:block;font-family:Inter,sans-serif;font-weight:500;font-size:36px;color:#ffb84d;margin-top:10px}`;document.head.appendChild(st);
 document.body.insertAdjacentHTML('beforeend',`<div id=ov><div id=ovDim></div><div id=ovG></div><div id=ovG2></div><div id=ovN></div><div id=ovT></div><div id=ovS></div>
  <div id=end><div class=logo>FERRUGEM</div><div class=sub>FÁBRICA</div><div class=by>Um jogo <b>Nexvora</b></div><div class=cta>🎮 Jogue grátis<small>link na descrição · PC e celular</small></div></div></div>`);
 window.__frame=o=>{const $=i=>document.getElementById(i);if(o.cam)cam=o.cam;
  const T=$('ovT');if(T.__h!==o.title){T.innerHTML=o.title||'';T.__h=o.title}T.style.opacity=o.ta||0;T.style.transform=`scale(${1.05-.05*(o.ta||0)})`;
  const S=$('ovS');const sh=o.sub?'<div>'+o.sub+'</div>':'';if(S.__h!==sh){S.innerHTML=sh;S.__h=sh}S.style.opacity=o.sa||0;S.style.transform=`translateY(${(1-(o.sa||0))*30}px)`;
  const N=$('ovN');N.textContent=o.n||'';N.style.opacity=o.n?1:0;
  $('ovDim').style.opacity=o.dim||0;$('end').style.opacity=o.end||0;$('end').style.transform=`scale(${1.04-.04*(o.end||0)})`;
  $('ovG').style.opacity=o.end?1-o.end:1;$('ovG2').style.opacity=o.end?1-o.end:1;hideFact();__step(o.dt)}};
const clamp=(x,a,b)=>Math.max(a,Math.min(b,x)),ease=t=>t<.5?2*t*t:1-Math.pow(-2*t+2,2)/2;
const fade=(t,a,b,f=.35)=>t<a||t>b?0:Math.min(1,(t-a)/f,(b-t)/f);
// camera por pontos-chave: em cada tempo, desliza (1 s) ate a nova posicao
const keyCam=(t,K)=>{let i=0;while(i+1<K.length&&t>=K[i+1][0])i++;const a=K[Math.max(0,i-1)][1],b=K[i][1],k=i?ease(clamp((t-K[i][0])/1.0,0,1)):1;return{x:lerp(a[0],b[0],k)*32,y:lerp(a[1],b[1],k)*32,z:lerp(a[2],b[2],k)}};
const lerp=(a,b,k)=>a+(b-a)*k,camAt=(p0,p1,t,d)=>{const k=ease(clamp(t/d,0,1));return{x:lerp(p0[0],p1[0],k)*32,y:lerp(p0[1],p1[1],k)*32,z:lerp(p0[2],p1[2],k)}};
// fabrica media (nivel 11)
const FAB=()=>{resetGame();tutSkip();xp=60000;money=1e7;const M=(t,x,y)=>place(t,x,y),B=(pts,d)=>{for(const[x,y]of pts){const o=place('conv',x,y);if(o)o.d=d}};
  M('miner',32,25);M('furn',34,25);M('press',36,25);M('sell',38,25);M('miner',34,27);M('miner',34,23);M('miner',33,34);M('gen',35,34);M('gen',31,34);M('gen',33,36);M('gen',33,32);
  B([[42,25],[43,25],[44,25],[45,25],[46,25],[47,25]],2);M('furn',40,25);M('miner',46,26);M('miner',44,23);M('miner',44,26);M('miner',46,23);M('press',40,27);B([[39,27]],3);M('wire',40,23);M('sell',40,21);M('asm',36,23);B([[39,24],[38,24]],2);M('sell',36,21);
  for(let i=0;i<3;i++)M('solar',44+i*2,30);for(const b of blds.values())if(b.type!=='conv')b.lv=4;for(let i=0;i<20*90;i++)tick(.05);hideFact();setSpeed(3)};
// fabrica gigante de veiculos (nivel 40)
const VEIC=()=>{resetGame();tutSkip();xp=LV[39]+1;money=1e10;const feed={bikef:{steel:100,tire:200,gear:200},scootf:{pAl:200,battery:100,tire:200},motof:{motor:100,tire:200,steel:200,battery:100},truckf:{motor:100,tire:300,steel:300},dronef:{plastic:200,battery:100,circ:200,motor:100},busf:{motor:100,tire:300,pAl:300,screen:100},trainf:{motor:200,rail:300,pAl:300,circ:300},shipy:{steel:600,motor:200,screen:100},planef:{pAl:600,motor:200,circ:300,screen:200},rocketf:{pAl:900,circ:600,battery:300,motor:200}};
  let i=0;for(const[t,inv]of Object.entries(feed)){const x=60+(i%5)*7,y=40+Math.floor(i/5)*5;const st=place('store',x,y);st.lv=5;Object.assign(st.in,inv);const f=place(t,x+2,y);f.lv=9;place('sell',x+4,y);i++}
  for(let k=0;k<12;k++)place('fusion',58+k*3,52);for(let k=0;k<6;k++)place('fusion',58+k*3,55);for(let k=0;k<20*20;k++)tick(.05);hideFact();setSpeed(3)};
// usinas de energia
const ENER=()=>{resetGame();tutSkip();xp=LV[39]+1;money=1e10;
  for(const[x,y]of[[60,60],[62,60],[60,62],[62,62]]){const g=place('gen',x,y);g.in.coal=20;g.lv=3}
  for(let i=0;i<6;i++)place('solar',70+i*2,60);for(let i=0;i<5;i++)place('wind',70+i*2,64);for(let i=0;i<4;i++)place('fusion',84+i*2,62);
  for(let k=0;k<20*5;k++)tick(.05);hideFact();setSpeed(1)};

(async()=>{const br=await chromium.launch();const ctx=await br.newContext({viewport:{width:W,height:H}});await ctx.addInitScript(CLOCK);
 const p=await ctx.newPage();const errs=[];p.on('pageerror',e=>errs.push(e.message));await p.goto(GAME);await p.evaluate(OVERLAY);await p.evaluate(()=>{for(let i=0;i<5;i++)__step(16)});
 let fn=0;const shot=async o=>{await p.evaluate(o=>__frame(o),o);await p.screenshot({path:path.join(OUT,String(fn++).padStart(5,'0')+'.jpg'),type:'jpeg',quality:90})};
 const scene=async(secs,setup,f,ev=[])=>{if(setup)await p.evaluate(setup);const done=new Set();const n=Math.round(secs*FPS);
  for(let i=0;i<n;i++){const t=i/FPS;for(let k=0;k<ev.length;k++)if(!done.has(k)&&t>=ev[k][0]){done.add(k);await p.evaluate(ev[k][1])}const o=await f(t);o.dt=o.dt||DT;await shot(o)}};
 const endCard=()=>scene(4,null,async t=>({dim:Math.min(.9,.4+t*.6),end:fade(t,.25,9,.5)}));

 if(WHICH==='primeira'){
  await scene(3,()=>{resetGame();tutSkip();setSpeed(1)},async t=>({cam:camAt([34,29,2.2],[34,30,2.0],t,3),title:'Sua <em>primeira<br>fábrica</em> em 30 s',ta:fade(t,.15,9,.4),dim:Math.max(0,.6-t*.6)}));
  const passos=[[0,'⚡ 1. Energia','gerador + mineradora no carvão (cinza)'],[4.5,'⛏️ 2. Mineradora','em cima do minério de ferro (laranja)'],[9,'➡️ 3. Esteiras','levam o minério sozinhas'],[12.5,'🔥 4. Forno','derrete e faz lingotes'],[16,'💰 5. Mercado','vende tudo automaticamente']];
  const ev=[[.2,"place('gen',35,34)"],[1.2,"place('miner',33,34)"],[4.7,"place('miner',30,25)"],[9.1,"for(let x=32;x<=34;x++)place('conv',x,25).d=0"],[12.6,"place('furn',35,25)"],[14.0,"for(let x=37;x<=38;x++)place('conv',x,25).d=0"],[16.1,"place('sell',39,25)"],[16.3,'setSpeed(3)']];
  await scene(21,()=>{resetGame();tutSkip();xp=0;money=2000;hideFact();setSpeed(1)},async t=>{let c=passos[0];for(const q of passos)if(t>=q[0])c=q;
   return{cam:keyCam(t,[[0,[34.5,33,2.8]],[4.5,[32.5,26,2.8]],[12.4,[35.5,26,2.6]],[16,[35,27,2.1]]]),sub:c[1]+'<small>'+c[2]+'</small>',sa:fade(t,.1,20.9,.25),n:(passos.indexOf(c)+1)+'/5'}},ev);
  await scene(4,null,async t=>({cam:camAt([35,27,2.1],[35,28.5,1.6],t,4),sub:'Pronto: funciona sozinho! 🤖<small>isso é automação</small>',sa:fade(t,.2,3.9,.3)}));
  await endCard()}

 if(WHICH==='energia'){
  await scene(3,ENER,async t=>({cam:camAt([74,62,1.0],[74,62,1.2],t,3),title:'4 jeitos de<br>gerar <em>energia</em> ⚡',ta:fade(t,.15,9,.4),dim:.35}));
  const fontes=[[[61.5,61.5,3.2],'🔥 Gerador a carvão','80 kW · precisa de carvão'],[[75.5,60.5,3.0],'☀️ Painel solar','35 kW · não gasta nada'],[[75,64.5,2.8],'🌬️ Turbina eólica','60 kW · energia do vento'],[[87.5,62.5,3.0],'⚛️ Reator de fusão','500 kW · o mais forte']];
  for(let i=0;i<4;i++){const[a,h,s]=fontes[i];const b=i?fontes[i-1][0]:a;const k=i;
   await scene(5,null,async t=>{const m=ease(clamp(t/.6,0,1));return{cam:{x:lerp(b[0],a[0],m)*32,y:lerp(b[1],a[1],m)*32,z:lerp(b[2],a[2],m)+.15*t/5},sub:h+'<small>'+s+'</small>',sa:fade(t,.3,4.85,.25),n:(k+1)+'/4'}})}
  await scene(4,null,async t=>({cam:camAt([87.5,62.5,3.0],[75,62,1.15],t,4),sub:'Sem energia, nada funciona ⚡<small>igualzinho a uma fábrica de verdade</small>',sa:fade(t,.3,3.9,.3)}));
  await endCard()}

 if(WHICH==='evolucao'){
  await scene(3,()=>{resetGame();tutSkip();setSpeed(1)},async t=>({cam:camAt([34,26,3.2],[34,26.2,2.8],t,3),title:'1 minuto<br>vs <em>10 horas</em>',ta:fade(t,.15,9,.4),dim:Math.max(.2,.6-t*.6)}));
  const ev=[[.1,"place('miner',30,25)"],[.6,"for(let x=32;x<=34;x++)place('conv',x,25).d=0"],[1.1,"place('furn',35,25)"],[1.6,"for(let x=37;x<=38;x++)place('conv',x,25).d=0"],[2.1,"place('sell',39,25)"],[2.2,'setSpeed(3)']];
  await scene(7,()=>{resetGame();tutSkip();xp=0;money=99999;place('miner',16,38);place('gen',18,38);hideFact();setSpeed(1)},async t=>({cam:camAt([34.5,25.5,3.0],[34.5,25.5,2.6],t,7),sub:'⏱️ 1 minuto<small>uma mineradora, um forno, um mercado</small>',sa:fade(t,.2,6.9,.25),n:'nível 1'}),ev);
  await scene(8,FAB,async t=>({cam:camAt([37,26,2.1],[39.5,27.5,1.65],t,8),sub:'⏱️ 1 hora<small>fábrica completa, com energia e várias linhas</small>',sa:fade(t,.2,7.8,.3),n:'nível 11'}));
  await scene(8,VEIC,async t=>({cam:camAt([68,43,1.6],[77,46,1.0],t,8),sub:'⏱️ 10 horas<small>veículos, aviões e foguetes 🚀</small>',sa:fade(t,.2,7.8,.3),n:'nível 40'}));
  await endCard()}

 console.log('quadros',fn,'erros',errs.length?errs:'nenhum');await br.close()})();
