#include "Demonstration.h"
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace shooter::demo {
// Format numeric values with nine significant digits for reproducible demonstration tables.
std::string number(float v) { std::ostringstream s; s<<std::setprecision(9)<<v; return s.str(); }
// Format three coordinates as an XYZ tuple.
std::string vector(Vec3 v) { return "("+number(v.x)+", "+number(v.y)+", "+number(v.z)+")"; }
// Convert labels into lowercase filename components, replacing punctuation with hyphens.
std::string slug(std::string v) {
    for(char& c:v) c=std::isalnum(static_cast<unsigned char>(c))?char(std::tolower(static_cast<unsigned char>(c))):'-';
    return v;
}
// Create the document's directory and make output failures raise an exception.
std::ofstream document(const fs::path& path) {
    fs::create_directories(path.parent_path());
    std::ofstream out(path); out.exceptions(std::ios::badbit|std::ios::failbit); return out;
}
// Embed the current source file verbatim so generated guides match the implementation.
void source(std::ostream& out,const fs::path& root,const std::string& file) {
    std::ifstream in(root/file); if(!in) throw std::runtime_error("Missing source: "+file);
    out<<"\n### Exact source: `"<<file<<"`\n\n```"<<(file.find("shaders/")==0?"glsl":"cpp")<<"\n"<<in.rdbuf()<<"\n```\n";
}
// Print matrix entries by row for human reading, regardless of their column-major storage.
void matrix(std::ostream& out,const Mat4& m) {
    out<<"```text\n";
    for(int r=0;r<4;++r) { out<<"[ "; for(int c=0;c<4;++c) out<<number(m.at(r,c))<<" "; out<<"]\n"; }
    out<<"```\n";
}
// Transform all cube corners, find the bounding box, and position a camera far enough to frame it.
View fit(const std::vector<SceneObject>& objects,Vec3 direction) {
    Vec3 lo{1e9f,1e9f,1e9f},hi{-1e9f,-1e9f,-1e9f};
    for(const auto& o:objects) for(float x:{-.5f,.5f}) for(float y:{-.5f,.5f}) for(float z:{-.5f,.5f}) {
        const auto p=asVec3(transformPoint(composeModelMatrix(o.transform),{x,y,z,1}));
        lo={std::min(lo.x,p.x),std::min(lo.y,p.y),std::min(lo.z,p.z)};
        hi={std::max(hi.x,p.x),std::max(hi.y,p.y),std::max(hi.z,p.z)};
    }
    if(objects.empty()) throw std::runtime_error("Cannot frame an empty assembly");
    const auto center=(lo+hi)*.5f; const float radius=std::max(.02f,length(hi-lo)*.5f);
    return {center+normalize(direction)*(radius*3.3f),center,std::max(.001f,radius*.005f),radius*12+10};
}
namespace {
using Bytes=std::vector<unsigned char>;
// Write a 32-bit value most-significant byte first, as required by PNG chunk fields.
void u32(Bytes& b,std::uint32_t v) { for(int s=24;s>=0;s-=8) b.push_back(static_cast<unsigned char>(v>>s)); }
// Write PNG chunk length/type/data followed by a CRC-32 checksum that detects corrupted bytes.
void chunk(Bytes& out,const char* kind,const Bytes& data) {
    u32(out,static_cast<std::uint32_t>(data.size())); const auto start=out.size();
    out.insert(out.end(),kind,kind+4); out.insert(out.end(),data.begin(),data.end());
    std::uint32_t crc=0xffffffffu;
    for(std::size_t i=start;i<out.size();++i) {
        crc^=out[i]; for(int k=0;k<8;++k) crc=(crc>>1)^(0xedb88320u& (0u-(crc&1u)));
    }
    u32(out,crc^0xffffffffu);
}
// Dependency-free PNG encoder: fixed Huffman DEFLATE + a 32 KiB LZ77 window.
// Captured pixels are lossless; no image synthesis or approximation is used.
// Compress repeated byte sequences into length/distance references using fixed Huffman codes.
Bytes deflate(const Bytes& input) {
    Bytes out{0x78,0x01}; unsigned pending=0; int count=0;
    // Pack variable-length codes into bytes, least-significant bit first as DEFLATE requires.
    auto bits=[&](unsigned v,int n) { pending|=v<<count; count+=n; while(count>=8) { out.push_back(pending&255); pending>>=8; count-=8; } };
    auto symbol=[&](int s) {
        unsigned code; int n;
        if(s<144) {code=0x30+s;n=8;} else if(s<256) {code=0x190+s-144;n=9;}
        else if(s<280) {code=s-256;n=7;} else {code=0xc0+s-280;n=8;}
        unsigned reverse=0; for(int i=0;i<n;++i) {reverse=(reverse<<1)|(code&1);code>>=1;} bits(reverse,n);
    };
    // Tables map match lengths and backward distances to a base value plus extra encoded bits.
    const int lb[]={3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258};
    const int le[]={0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0};
    const int db[]={1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577};
    const int de[]={0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};
    std::vector<int> last(65536,-1);
    auto hash=[&](std::size_t p) { return ((unsigned(input[p])*251+input[p+1])*251+input[p+2])&65535; };
    bits(3,3); // final block, fixed Huffman
    for(std::size_t p=0;p<input.size();) {
        int length=0,distance=0;
        if(p+2<input.size()) {
            int previous=last[hash(p)]; last[hash(p)]=int(p);
            if(previous>=0 && p-previous<=32768) {
                distance=int(p)-previous;
                while(length<258 && p+length<input.size() && input[previous+length]==input[p+length]) ++length;
            }
        }
        if(length<3) {symbol(input[p++]);continue;}
        int l=28; while(lb[l]>length) --l; symbol(257+l); bits(length-lb[l],le[l]);
        int d=29; while(db[d]>distance) --d;
        unsigned rev=0; for(int k=0;k<5;++k) rev=(rev<<1)|((d>>k)&1); bits(rev,5); bits(distance-db[d],de[d]);
        for(int k=1;k<length && p+k+2<input.size();++k) last[hash(p+k)]=int(p+k);
        p+=length;
    }
    symbol(256); if(count) out.push_back(pending&255);
    // Adler-32 checksum: a=(a+byte)%65521, b=(b+a)%65521, result=(b<<16)|a.
    std::uint32_t a=1,b=0; for(auto v:input) {a=(a+v)%65521;b=(b+a)%65521;} u32(out,(b<<16)|a); return out;
}
}
// Initialize the actual game renderer for documentation screenshots.
Capture::Capture(const fs::path& root) { renderer.initialize(root/"shaders"); }
// Render a chosen view and save its exact framebuffer pixels as a lossless PNG.
void Capture::save(const fs::path& path,const std::vector<SceneObject>& objects,const View& view,bool night,int mask,int shading,int w,int h,float fieldOfView) {
    glViewport(0,0,w,h); glEnable(GL_DEPTH_TEST); glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    renderer.shadingMode=shading;
    renderer.drawArena(objects,makeLookAt(view.eye,view.center,{0,1,0}),makePerspective(fieldOfView,float(w)/h,view.nearPlane,view.farPlane),view.eye,night,-1,mask);
    Bytes pixels(w*h*3); glPixelStorei(GL_PACK_ALIGNMENT,1); glReadBuffer(GL_BACK);
    glReadPixels(0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());
    if(glGetError()!=GL_NO_ERROR) throw std::runtime_error("OpenGL capture failed: "+path.string());
    Bytes raw; raw.reserve((w*3+1)*h);
    // Flip OpenGL's bottom-up rows and prefix each PNG scanline with filter byte 0 (no filtering).
    for(int y=h-1;y>=0;--y) { raw.push_back(0); raw.insert(raw.end(),pixels.begin()+y*w*3,pixels.begin()+(y+1)*w*3); }
    Bytes png{137,80,78,71,13,10,26,10},header; u32(header,w);u32(header,h);header.insert(header.end(),{8,2,0,0,0});
    chunk(png,"IHDR",header);chunk(png,"IDAT",deflate(raw));chunk(png,"IEND",{});
    fs::create_directories(path.parent_path()); std::ofstream out(path,std::ios::binary); out.exceptions(std::ios::badbit|std::ios::failbit);
    out.write(reinterpret_cast<const char*>(png.data()),png.size()); ++images;
}
}
