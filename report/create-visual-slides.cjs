const pptxgen = require('../.report-build/slides-tooling/node_modules/pptxgenjs');
const path = require('path');
const fs = require('fs');
const pptx = new pptxgen();
pptx.layout='LAYOUT_WIDE';
pptx.author='Kazi Rifat Al Muin';
pptx.subject='Computer Graphics project report presentation';
pptx.title='3D Target Shooter';
pptx.company='KUET';
pptx.lang='en-US';
pptx.theme={headFontFace:'Aptos Display',bodyFontFace:'Aptos',lang:'en-US'};
const C={bg:'F7F9FC',ink:'203147',muted:'617085',accent:'147D92',line:'DCE5EF',pale:'E8F2F5'};
const root=path.resolve(__dirname,'..');
const I=f=>path.join(root,'docs/images',f+'.png');
const O=f=>path.join(root,'objects',f,'complete.png');
let count=0;
function text(s,t,x,y,w,h,size=20,color=C.ink,bold=false,extra={}){s.addText(t,{x,y,w,h,fontFace:'Aptos',fontSize:size,color,bold,margin:0,breakLine:false,paraSpaceAfterPt:10,...extra});}
function box(s,x,y,w,h,color=C.pale){s.addShape(pptx.ShapeType.rect,{x,y,w,h,line:{color,transparency:100},fill:{color},radius:0});}
function base(title,section,source){const s=pptx.addSlide();count++;s.background={color:C.bg};box(s,.48,.45,.08,.5,C.accent);text(s,section.toUpperCase(),.72,.34,11,.25,11,C.accent,true);text(s,title,.72,.79,12, .65,30,C.ink,true);box(s,.72,7.03,11.9,.012,C.line);text(s,'3D TARGET SHOOTER  /  CSE 4102',.72,7.13,5,.18,9,C.muted);text(s,String(count).padStart(2,'0')+' / 16',11.7,7.1,.9,.22,10,C.muted,false,{align:'right'});if(source) s.addNotes('Project sources: '+source);return s;}
function body(s,items,x=.75,y=1.73,w=5.2,size=21){let yy=y;for(const t of items){text(s,'•',x,yy,.2,.3,size,C.accent,true);text(s,t,x+.3,yy,w-.3,.83,size);yy+=1.02;}}
function pic(s,p,x,y,w,h,caption=''){if(!fs.existsSync(p))throw Error(p);const b=fs.readFileSync(p);const iw=b.readUInt32BE(16),ih=b.readUInt32BE(20),k=Math.min(w/iw,h/ih);s.addImage({path:p,x:x+(w-iw*k)/2,y:y+(h-ih*k)/2,w:iw*k,h:ih*k});if(caption)text(s,caption,x,y+h+.10,w,.38,12,C.muted);}
function eq(s,t,x,y,w,h=.65,size=23){box(s,x,y,w,h);text(s,t,x+.16,y+.09,w-.32,h-.16,size,C.accent,false,{fontFace:'Cambria Math',valign:'mid'});}
function table(s,rows,x,y,w,widths){s.addTable(rows,{x,y,w,colW:widths,fontFace:'Aptos',fontSize:17,color:C.ink,border:{type:'solid',color:C.line,pt:1},fill:'FFFFFF',margin:10,rowH:.46,bold:false,autoPage:false});}

const obj=(n,f)=>path.join(root,'objects',n,f+'.png');
const light=n=>path.join(root,'lighting',n+'.png');
function caption(s,t,y=6.5){text(s,t,.76,y,11.8,.4,18,C.accent,false,{align:'center'});}
function grid(s,items,cols=3,y=1.65,h=4.8){const rows=Math.ceil(items.length/cols),gap=.22,cw=(11.8-gap*(cols-1))/cols,ch=h/rows;items.forEach(([p,c],i)=>pic(s,p,.76+(i%cols)*(cw+gap),y+Math.floor(i/cols)*ch,cw,ch-.48,c));}
function stages(n,ns){return ns.map(k=>[obj(n,'assembly-'+k),'Assembly after '+k+' parts']);}
let s=base('3D Target Shooter','Computer graphics project','report/report.tex; README.md');
pic(s,I('release-arena'),.76,1.55,11.8,4.7);caption(s,'Kazi Rifat Al Muin  •  2107042  •  CSE 4102  •  KUET');
s=base('The completed project','Introduction','report/report-sections.tex, Sections 1–2');
grid(s,[[I('release-pistol'),'Player view — aiming, targets and HUD'],[I('release-arena'),'Arena view — walls, cover and firing lanes']],2,1.7,4.75);caption(s,'Cube-based modelling • transformations • animation • lighting • interaction');
s=base('All seven Challenge levels','Gameplay and level design','report/report-sections.tex, Section 2.2');
const rows=[['Level','Targets','Walking','Motion / cover','NPCs'],['1','3','Locked','Stationary targets','None'],['2','3','Locked','Centre target moves horizontally','None'],['3','4','Locked','Separate X, Y and Z motion','None'],['4','6','Enabled','Cargo cover, longer range and rotation','None'],['5','8','Enabled','Compound target trajectories','8 birds'],['6','10','Enabled','Faster three-axis trajectories','16 birds'],['7','12','Enabled','Final pair: faster motion and spin','48 birds; 36 humans']];
s.addTable(rows.map((r,i)=>r.map(t=>({text:t,options:{bold:i===0,color:i===0?'FFFFFF':C.ink,fill:i===0?C.accent:(i%2?'FFFFFF':'EDF3F7')}}))),{x:.76,y:1.75,w:11.8,colW:[.9,1.2,1.3,5.45,2.95],fontFace:'Aptos',fontSize:16,border:{color:C.line,pt:.7},margin:8,rowH:.52,autoPage:false});caption(s,'46 targets across seven levels; movement unlocks from Level 4.');
s=base('Seven levels in pictures','Gameplay and level design','docs/images/release-level-1.png through release-level-7.png');
grid(s,Array.from({length:7},(_,i)=>[I('release-level-'+(i+1)),['1  Stationary','2  Horizontal motion','3  Separate motion axes','4  Cover + rotation','5  Compound motion + birds','6  Faster motion','7  Birds + humans'][i]]).concat([[I('release-level-7-results'),'Final-level results']]),4,1.67,5.1);
s=base('Construction: cube to finished object','Object construction and transformations','objects/weapon-pistol/; report/report-sections.tex, Section 3');
grid(s,['part-0-0','part-0-1','part-0-6','complete'].map((f,i)=>[obj('weapon-pistol',f),['Unit cube','Scaled component','Placed component','Complete pistol'][i]]),4,1.85,4.05);eq(s,'M = T Rz Ry Rx H S',3.9,6.08,5.5,.61,25);
s=base('Cargo crate: complete assembly','Object construction','objects/cargo-crate/; report/report-sections.tex, Section 3.4');
grid(s,stages('cargo-crate',[1,2,3,4]),4,1.82,4.5);caption(s,'Four transformed cubes form the full crate.');
s=base('Pistol: from first part to complete model','Object construction','objects/weapon-pistol/; report/report-sections.tex, Section 3.5');
grid(s,stages('weapon-pistol',[1,3,5,7,9,11]).map(([p,c],i)=>[p,i===5?'11 parts — complete pistol':c]),3,1.65,5.15);
s=base('Shotgun and rifle: complete assemblies','Object construction','objects/weapon-shotgun/; objects/weapon-rifle/; report/report-sections.tex, Section 3.6');
grid(s,[['weapon-shotgun',1,'Shotgun • first part'],['weapon-shotgun',7,'Shotgun • 7 parts'],['weapon-shotgun',14,'Shotgun • complete, 14 parts'],['weapon-rifle',1,'Rifle • first part'],['weapon-rifle',8,'Rifle • 8 parts'],['weapon-rifle',15,'Rifle • complete, 15 parts']].map(([n,k,c])=>[obj(n,'assembly-'+k),c]),3,1.65,5.15);
s=base('Target: stand, slices and scoring rings','Object construction','objects/target/; report/report-sections.tex, Section 3.7');
grid(s,[[obj('target','assembly-1'),'Base'],[obj('target','assembly-6'),'Complete stand'],[obj('target','assembly-14'),'First 8 plate slices'],[obj('target','assembly-22'),'First 16 plate slices'],[obj('target','assembly-38'),'38 parts — complete target'],[I('release-target-front'),'Six shader-generated rings']],3,1.63,4.8);caption(s,'32 plate slices:  width = 2√(r² − y²)  •  r = 0.8 m',6.55);
s=base('Human and bird: multipart construction','Object construction','objects/human/; objects/bird/; report/report-sections.tex, Section 4.2');
grid(s,[['human',1,'Human • first part'],['human',10,'Human • 10 parts'],['human',21,'Human • complete, 21 cuboids'],['bird',1,'Bird • first part'],['bird',5,'Bird • 5 parts'],['bird',10,'Bird • complete, 10 cuboids']].map(([n,k,c])=>[obj(n,'assembly-'+k),c]),3,1.65,5.1);
s=base('Lighting: day and night','Lighting and shading','report/report-sections.tex, Section 7; lighting/day-2-mask-15.png; lighting/night-2-mask-15.png');
grid(s,[[light('day-2-mask-15'),'Day — ambient fill and directional sunlight'],[light('night-2-mask-15'),'Night — ambient fill, 8 point lamps and 6 spotlights']],2,1.7,4.8);caption(s,'Same scene and camera; different source intensities.');
s=base('Lighting: source-by-source comparison','Lighting and shading','report/report-sections.tex, Sections 7.1–7.2; lighting/');
grid(s,[[light('day-2-mask-1'),'Ambient — constant background fill'],[light('day-2-mask-2'),'Directional — orientation-dependent sunlight'],[light('night-2-mask-4'),'Point lamps — local light with distance falloff'],[light('night-2-mask-8'),'Spotlights — focused beams with soft cone edges']],2,1.62,5.15);
s=base('Shading: flat, Gouraud and Phong','Lighting and shading','report/report-sections.tex, Section 7.3; lighting/night-*-mask-15.png');
grid(s,[0,1,2].map(n=>[light('night-'+n+'-mask-15'),['Flat — one lighting value per triangle','Gouraud — interpolated vertex lighting','Phong — lighting evaluated per fragment'][n]]),3,1.9,3.8);
s.addTable([['Flat','Gouraud','Phong'],['Constant triangle regions','Smooth vertex-to-vertex variation','Detailed local light and highlights']],{x:.76,y:5.7,w:11.8,colW:[3.93,3.93,3.94],fontFace:'Aptos',fontSize:17,color:C.ink,border:{color:C.line,pt:.7},fill:'FFFFFF',margin:10,rowH:.4,autoPage:false});
s=base('Shading: close-up comparison','Lighting and shading','report/report-sections.tex, Section 7.3; lighting/night-*-mask-15-close.png; shaders/lighting.glsl');
grid(s,[0,1,2].map(n=>[light('night-'+n+'-mask-15-close'),['Flat — triangle boundaries are visible','Gouraud — light blends between vertices','Phong — light responds at each fragment'][n]]),3,1.9,4.05);caption(s,'Same camera, geometry and lights; only the shading method changes.',6.05);eq(s,'Diffuse ∝ max(N · L, 0)     |     Specular ∝ max(R · V, 0)ⁿ',.95,6.55,11.4,.38,18);
s=base('Conclusion','Conclusion','report/report-sections.tex, Conclusion');
pic(s,I('release-arena'),.76,1.65,8.3,4.95);text(s,'One reusable cube.\nComplete 3D assemblies.\nSeven progressive levels.\nThree shading methods.',9.48,2.3,3.05,2.8,23,C.accent,true);text(s,'Modelling, animation and lighting combined in one interactive project.',9.48,5.4,3.05,1.1,19,C.muted);
s=base('Thank You','3D Target Shooter','');pic(s,I('release-night-arena'),.76,1.58,11.8,4.8);caption(s,'Kazi Rifat Al Muin  •  2107042',6.53);
if(count!==16)throw Error('Expected 16 slides');
pptx.writeFile({fileName:path.join(__dirname,'3D_Target_Shooter_Visual_Presentation.pptx')});


