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
function base(title,section,source){const s=pptx.addSlide();count++;s.background={color:C.bg};box(s,.48,.45,.08,.5,C.accent);text(s,section.toUpperCase(),.72,.34,11,.25,11,C.accent,true);text(s,title,.72,.79,12, .65,30,C.ink,true);box(s,.72,7.03,11.9,.012,C.line);text(s,'3D TARGET SHOOTER  /  CSE 4102',.72,7.13,5,.18,9,C.muted);text(s,String(count).padStart(2,'0')+' / 18',11.7,7.1,.9,.22,10,C.muted,false,{align:'right'});if(source) s.addNotes('Project sources: '+source);return s;}
function body(s,items,x=.75,y=1.73,w=5.2,size=21){let yy=y;for(const t of items){text(s,'•',x,yy,.2,.3,size,C.accent,true);text(s,t,x+.3,yy,w-.3,.83,size);yy+=1.02;}}
function pic(s,p,x,y,w,h,caption=''){if(!fs.existsSync(p))throw Error(p);const b=fs.readFileSync(p);const iw=b.readUInt32BE(16),ih=b.readUInt32BE(20),k=Math.min(w/iw,h/ih);s.addImage({path:p,x:x+(w-iw*k)/2,y:y+(h-ih*k)/2,w:iw*k,h:ih*k});if(caption)text(s,caption,x,y+h+.10,w,.38,12,C.muted);}
function eq(s,t,x,y,w,h=.65,size=23){box(s,x,y,w,h);text(s,t,x+.16,y+.09,w-.32,h-.16,size,C.accent,false,{fontFace:'Cambria Math',valign:'mid'});}
function table(s,rows,x,y,w,widths){s.addTable(rows,{x,y,w,colW:widths,fontFace:'Aptos',fontSize:17,color:C.ink,border:{type:'solid',color:C.line,pt:1},fill:'FFFFFF',margin:10,rowH:.46,bold:false,autoPage:false});}

let s=base('3D Target Shooter','Project report presentation','README.md; report/report.tex');
text(s,'An interactive graphics project',.76,1.66,5.4,.7,29,C.accent,true);
text(s,'Cube-based modelling • animation\nlighting • collision • user interaction',.76,2.53,5.25,1,22);
text(s,'Kazi Rifat Al Muin\nRoll: 2107042',.76,4.26,5, .85,23,C.ink,true);
text(s,'CSE 4102\nComputer Graphics and Image Processing Laboratory\nKhulna University of Engineering & Technology',.76,5.31,5.2,1,16,C.muted);
pic(s,I('release-arena'),6.27,1.65,6.3,4.8,'Actual project capture: the completed arena.');

s=base('Report overview','Abstract & structure','README.md; report/report-sections.tex');
text(s,'A playable arena that makes the graphics pipeline visible.',.76,1.65,11.6,.7,27,C.accent,true);
body(s,['Reusable cube geometry becomes an arena, weapons, targets and characters.','C++17 and OpenGL 3.3 connect graphics mathematics with interactive gameplay.','Seven Challenge levels, five modes and three weapons demonstrate the system.'],.76,2.6,6,21);
box(s,7.3,2.55,5.25,3.6);text(s,'REPORT SEQUENCE',7.6,2.82,4.6,.35,15,C.accent,true);text(s,'01  Introduction & objectives\n02  Design & methodology\n03  Graphics implementation\n04  Results & verification\n05  Limitations & conclusion',7.6,3.45,4.6,2.25,22);

s=base('Introduction and objectives','01 / Introduction','report/report-sections.tex, Section 1');
body(s,['Build a complete, interactive 3D scene from a shared primitive.','Demonstrate scale, shear, rotation, translation and perspective projection.','Combine textures, lighting, animation and exact geometric collision.','Keep the implementation modular and calculations reproducible.'],.76,1.75,5.6,20);
pic(s,I('release-pistol'),6.65,1.78,5.95,4.55,'Player view combines the world, aiming and HUD.');

s=base('System design and rendering pipeline','02 / Design & methodology','README.md; src/; shaders/');
const stages=['Input & mode','Simulation','Scene objects','GPU shaders','Frame + HUD'];
stages.forEach((t,i)=>{box(s,.76+i*2.5,1.95,2.25,.85);text(s,t,.88+i*2.5,2.17,2.01,.4,18,C.accent,true,{align:'center'});if(i<4)text(s,'→',3.02+i*2.5,2.15,.25,.4,20,C.muted);});
table(s,[['Layer','Responsibility'],['GLFW + GLAD','Window/input and OpenGL function loading'],['C++ modules','Levels, transforms, camera, gameplay, world, UI and persistence'],['Object shaders','Model/view/projection, textures and lighting'],['Sky + UI shaders','Sky gradient and screen-space interface']],.76,3.3,11.8,[3,8.8]);

s=base('Gameplay modes and level progression','02 / Design & methodology','report/report-sections.tex, Section 2');
table(s,[['Mode','Purpose'],['Challenge','Seven levels; 46 targets in total'],['Free','180 seconds; targets respawn after 30 s'],['Developer','Inspect and restart individual levels'],["Bird’s-Eye",'Overview and click-to-observe camera'],['Practice','Seven targets; 1.8 s respawn']],.76,1.77,6.05,[1.7,4.35]);
text(s,'Increasing difficulty',7.2,1.83,5,.4,23,C.accent,true);text(s,'L1–3   Stationary → separate motion axes\nL4       Cover, rotation and walking\nL5–6   Compound motion and birds\nL7       Faster targets and human NPCs',7.2,2.53,5,2.3,21);
text(s,'Player movement is locked in Levels 1–3 and enabled in Levels 4–7.',7.2,5.21,4.9,.9,18,C.muted);

s=base('Object construction from a unit cube','03 / Graphics implementation','report/report-sections.tex, Section 3; objects/');
text(s,'One mesh: 8 corners → 12 triangles → 36 face-normal vertices',.76,1.63,11.9,.55,23,C.accent,true);
[['cargo-crate','Cargo crate'],['weapon-pistol','Pistol assembly'],['target','Target and stand']].forEach(([p,c],i)=>pic(s,O(p),.76+4.2*i,2.45,3.95,2.6,c));
text(s,'Parts are scaled, sheared, rotated and placed independently. The circular-looking target uses 32 cube slices; its six rings are shader-generated.',.76,5.72,11.7,.95,22);

s=base('Main formula: model-to-screen transformation','03 / Transformations','src/core/Transform.h; shaders/object.vert; report/report-sections.tex');
eq(s,'M = T Rz Ry Rx H S',.76,1.8,6.1,.8,30);
eq(s,'pworld = M plocal     •     pclip = P V M plocal',.76,2.85,11.8,.8,28);
eq(s,'pNDC = pclip.xyz / pclip.w',.76,3.91,6.1,.75,27);
text(s,'S  scale       H  shear\nRx, Ry, Rz  rotations\nT  translation\nV  view       P  projection',7.45,4.1,4.8,1.9,22);
text(s,'Column vectors: scale is applied first, translation last.\nThe same model transforms support geometry and collision.',.76,5.18,6.1,1.1,21);

s=base('Camera, interaction and screen-space UI','03 / Camera & interface','src/camera/; shaders/ui.vert; report/report-sections.tex, Section 5');
pic(s,I('release-overhead'),.76,1.83,6,3.8,'Overhead view reveals arena layout and observation positions.');
text(s,'Multiple views',7.16,1.82,5,.4,23,C.accent,true);text(s,'Player, arena, side and movable views expose the same world from different positions.',7.16,2.4,5,1.3,22);
eq(s,"x′ = 2x / 1280 − 1",7.16,4.02,5.1,.68,24);eq(s,"y′ = 1 − 2y / 800",7.16,4.9,5.1,.68,24);
text(s,'UI vertices map from the 1280 × 800 logical canvas to normalized coordinates; Y is flipped.',.76,6.08,11.5,.5,18,C.muted);

s=base('Target animation and scoring','03 / Animation','src/world/; report/report-sections.tex, Section 2; shaders/object.frag');
eq(s,'pj(t) = bj + aj sin(ωj t + φj)',.76,1.85,7,.74,27);
eq(s,'θy(t) = (s t) mod 360°',.76,2.82,7,.74,27);
eq(s,'D(r) = 60 / (r + 1),    r = 0, …, 5',.76,3.8,7,.74,24);
text(s,'b: base position   a: amplitude   ω: angular frequency\nφ: phase   s: spin speed   r: ring index',.76,4.87,7, .85,18,C.muted);
text(s,'60 health; ring damage = 60, 30, 20, 15, 12, 10.\nOnly front-face hits score. One shotgun trigger applies its best ring damage once per target.',.76,5.97,11.8,.7,19);
pic(s,I('release-target-front'),8.13,1.86,4.45,3.8,'Six concentric scoring regions.');

s=base('Shooting and collision detection','03 / Simulation','report/report-sections.tex, Section 4; src/gameplay/');
eq(s,'ℓ = min(v Δt, range − travelled)',.76,1.82,6.2,.72,25);
eq(s,'pnew = pold + ℓ d',.76,2.74,6.2,.72,26);
eq(s,'ol = A⁻¹(o − t),     dl = A⁻¹d',.76,3.66,6.2,.72,25);
text(s,'v: projectile speed   d: unit firing direction\nA, t: object linear transform and translation',.76,4.68,6.25,.85,18,C.muted);
text(s,'Ray–box slab test',7.4,1.85,5,.4,24,C.accent,true);text(s,'Transform the ray into each part’s local space. Keep the transformed direction unnormalized.\n\nIntersect the three [−0.5, 0.5] slabs; accept if entry ≤ exit.\n\nUse the nearest contact to stop travel, including cover and NPCs.',7.4,2.55,5,3.5,21);
text(s,'Camera-to-muzzle parallax is handled; a clear camera view does not imply a clear muzzle.',.76,6.19,11.6,.5,18,C.muted);

s=base('Texture techniques and materials','03 / Surface appearance','tools/generate_textures.cpp; shaders/object.vert; shaders/object.frag');
pic(s,I('release-textured-environment'),.76,1.82,6.2,4.35,'Procedural materials distinguish surfaces throughout the arena.');
body(s,['Nine 256 × 256 material maps are stored in a texture array.','Face-local planar UVs keep textures aligned through rotation and shear.','One texture sample combines with the object colour before lighting.'],7.35,1.84,5.1,20);
eq(s,'Cbase = Cobject × texture(UV, layer)',7.35,5.28,5.05,.83,20);

s=base('Main formula: ambient, diffuse and specular light','03 / Lighting','shaders/lighting.glsl; shaders/object.frag');
eq(s,'Clinear = Cbase ⊙ (A + Σ Ii max(N · Li, 0))',.76,1.82,11.8,.78,26);
eq(s,'                 + Σ Ii ks max(Ri · V, 0)ⁿ',.76,2.61,11.8,.75,26);
text(s,'A  ambient colour        Ii  effective light colour / intensity\nN  unit normal             Li  direction toward the light\nV  direction to viewer   Ri  reflected incoming light\nks  specular strength    n  shininess',.76,3.74,7,2,21);
box(s,8.25,3.78,4.3,2.2);text(s,'Phong reflection',8.51,4.04,3.8,.4,23,C.accent,true);text(s,'Specular is added only when N · Li > 0. Normals use the inverse-transpose transform.',8.51,4.65,3.65,1.05,19);
eq(s,'Nworld ∝ (M3×3⁻¹)ᵀ Nlocal',.76,6.12,7,.57,22);

s=base('Day/night lighting and shading methods','03 / Lighting & shading','shaders/lighting.glsl; docs/images/release-shading-*; docs/images/release-night-arena.png');
pic(s,I('release-arena'),.76,1.78,4.02,2.65,'Day: directional fill and material detail.');
pic(s,I('release-night-arena'),4.92,1.78,4.02,2.65,'Night: lamps and spotlight cones.');
text(s,'Light rig',9.38,1.82,3.2,.4,22,C.accent,true);text(s,'Ambient + directional\n8 point lamps\n6 soft-edged spots',9.38,2.5,3.2,1.65,20);
eq(s,'Point attenuation = 1 / (1 + 0.09d + 0.032d²)',.76,4.95,11.8,.68,24);
text(s,'Flat: one value per triangle     •     Gouraud: interpolated vertex lighting\nPhong shading: lighting per fragment     •     Display gamma: Cdisplay = clamp(Clinear)^(1/2.2)',.76,5.95,11.75,.8,20);

s=base('Implementation and advanced features','04 / Implementation','src/; README.md; docs/release-verification.md');
body(s,['Separate modules manage rendering, gameplay, world construction, camera, UI and persistence.','Shared scene builders keep rendered parts and collision geometry consistent.','Seeded layouts and transform logs support repeatable inspection.','Cached lighting rigs and redundant-uniform suppression reduce renderer overhead.'],.76,1.75,6.2,20);
pic(s,I('release-menu'),7.33,1.9,5.2,3.4,'Five modes share one graphics and gameplay system.');
text(s,'Persistent CSV leaderboards\nGenerated font and synthesized audio\nMenus, pause, instructions and results',7.33,5.58,5.1,.95,18,C.muted);

s=base('Visual results: progressive arena complexity','04 / Results','docs/images/release-level-1.png; release-level-4.png; release-level-7.png; release-level-7-results.png');
[['release-level-1','Level 1 — stationary targets'],['release-level-4','Level 4 — cover and movement'],['release-level-7','Level 7 — combined motion and NPCs'],['release-level-7-results','Final-level results and session feedback']].forEach(([p,c],i)=>pic(s,I(p),.76+(i%2)*6.1,1.68+Math.floor(i/2)*2.53,5.83,2.12,c));

s=base('Verification and measured performance','04 / Evaluation','docs/release-verification.md (2026-10-03). Historical project results, not rerun for this presentation.');
text(s,'Recorded project verification • 3 October 2026',.76,1.68,11.7,.4,20,C.accent,true);
body(s,['Core, Challenge, mode and release test suites were reported as passed.','Render checks covered shaders, OpenGL errors and day/night/shading differences.','Independent calculation checks verified 56,712 unique observed cube rows.'],.76,2.4,5.6,20);
table(s,[['Median render time','Before','After'],['Day','6.40 ms','5.32 ms'],['Night','7.34 ms','5.60 ms']],6.8,2.48,5.8,[2.8,1.5,1.5]);
text(s,'Level 7 • 5,349 cube instances\n1280 × 800 • AMD Radeon Graphics\n200 measured frames per condition',6.8,4.65,5.8,1.15,19);
text(s,'Render-pass measurements only. Long-tail stalls remain; these figures do not establish total gameplay FPS or performance on other hardware.',.76,6.05,11.8,.63,17,C.muted);

s=base('Discussion, limitations and conclusion','05 / Conclusion','README.md; report/report-sections.tex, Discussion and Conclusion; docs/release-verification.md');
text(s,'What the project demonstrates',.76,1.8,5.7,.5,23,C.accent,true);body(s,['A full graphics pipeline built around reusable cube geometry.','Visible links between mathematical transforms, lighting and interaction.','Progressive gameplay supported by repeatable validation.'],.76,2.58,5.7,21);
text(s,'Limitations and future work',7,1.8,5.5,.5,23,C.accent,true);body(s,['Curves are approximated; target disks use cube slices.','Visibility validation samples time and space rather than proving every instant.','Future work: smoother curved meshes, batching and investigation of render stalls.'],7,2.58,5.55,21);
box(s,.76,6.05,11.8,.6);text(s,'The result is a playable application and a concrete demonstration of core 3D graphics concepts.',.96,6.2,11.35,.32,18,C.accent,true);

s=base('References and project evidence','References','Local repository sources used throughout the presentation.');
const refs=[['01','Project report','report/report.tex and report/report-sections.tex'],['02','Implementation overview','README.md — scope, architecture, levels and controls'],['03','Graphics formulas','shaders/object.vert, object.frag, lighting.glsl and ui.vert'],['04','Evaluation evidence','docs/release-verification.md — dated tests and benchmark limits'],['05','Pictures and model assemblies','docs/images/, objects/ and report/images/']];
refs.forEach(([n,t,d],i)=>{text(s,n,.8,1.76+i*.87,.55,.35,20,C.accent,true);text(s,t,1.55,1.74+i*.87,10,.32,20,C.ink,true);text(s,d,1.55,2.1+i*.87,10.6,.29,16,C.muted);});
text(s,'All pictures are existing captures from this project. Equations summarize the implemented code.\nDetailed source notes are included in the PowerPoint speaker notes.',.8,6.32,11.8,.5,15,C.muted);

if(count!==18)throw Error('Expected 18 slides');
pptx.writeFile({fileName:path.join(__dirname,'3D_Target_Shooter_Presentation.pptx')});

