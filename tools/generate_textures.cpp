// Offline, deterministic, original seamless surface assets. No runtime generation.
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <string>
#include <cstdint>
// Hash wrapped XY coordinates and a seed into [0,1], producing repeatable 256-pixel tiling noise.
float noise(int x,int y,int seed=0) {
    uint32_t n=uint32_t(x&255)*374761393u+uint32_t(y&255)*668265263u+uint32_t(seed)*2246822519u;
    n=(n^(n>>13))*1274126177u; return float((n^(n>>16))&65535)/65535;
}
// Blend four neighboring noise samples with smooth weights u'=u*u*(3-2*u), v'=v*v*(3-2*v).
float cloud(int x,int y,int step,int seed) {
    int a=x/step,b=y/step; float u=float(x%step)/step,v=float(y%step)/step;
    u=u*u*(3-2*u); v=v*v*(3-2*v);
    auto n=[&](int i,int j){return noise((i%(256/step))*step,(j%(256/step))*step,seed);};
    return (n(a,b)*(1-u)+n(a+1,b)*u)*(1-v)+(n(a,b+1)*(1-u)+n(a+1,b+1)*u)*v;
}
// Generate nine 256x256 RGB material textures as binary PPM files for later GPU loading.
int main(int argc,char** argv) {
    const std::filesystem::path root=argc>1?argv[1]:"assets/textures";
    std::filesystem::create_directories(root);
    const char* names[]={"plain","concrete","masonry","gravel","wood","metal","fabric","paper","rubber"};
    for(int material=0;material<9;++material) {
        std::ofstream out(root/(std::string(names[material])+".ppm"),std::ios::binary);
        out<<"P6\n256 256\n255\n";
        for(int y=0;y<256;++y) for(int x=0;x<256;++x) {
            float n=noise(x,y,material), c=cloud(x,y,32,material), fine=cloud(x,y,8,material), value=1;
            if(material==1) value=.76f+.15f*c+.09f*n-(n<.035f?.14f:0);
            if(material==2) {
                // Alternate brick-row offsets by half a brick; modulo repeats the masonry pattern.
                int row=y/64, bx=(x+(row%2)*64)%128, by=y%64;
                float edge=std::min(std::min(bx,127-bx),std::min(by,63-by));
                value=edge<3?.36f+.09f*n:.67f+.22f*c+.10f*n;
                if(edge>=3&&edge<6) value+=.09f;
            }
            if(material==3) value=.58f+.26f*fine+.16f*n+(n>.96f?.10f:0);
            if(material==4) {
                // Wood grain follows a sine wave distorted by a second sine and low-frequency noise.
                float grain=std::sin(x*.49f+3*std::sin(y*6.2831853f/256)+c*2);
                value=.70f+.13f*grain+.14f*c+.06f*n;
                if(x%64<2) value=.34f+.08f*n;
                if(x%64==3) value=.95f;
            }
            if(material==5) value=.79f+.12f*noise(0,y,5)+.07f*c+.02f*n;
            if(material==6) value=.77f+.09f*((x+y)%2)+.08f*c+.05f*n;
            if(material==7) value=.90f+.06f*c+.04f*n;
            if(material==8) value=.67f+.14f*c+.1f*n-((x%8<2&&y%8<2)?.18f:0);
            // Convert reflectance in [0,1] to an 8-bit channel: byte=clamp(value,0,1)*255.
            unsigned char pixel=static_cast<unsigned char>(std::clamp(value,0.f,1.f)*255);
            for(int channel=0;channel<3;++channel) out.put(char(pixel));
        }
        if(!out) return 1;
    }
}
