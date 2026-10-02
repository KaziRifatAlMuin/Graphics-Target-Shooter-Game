#pragma once
#include "SceneObject.h"
#include <array>
#include <cctype>
namespace shooter {
struct UiVertex { float x,y,r,g,b; };
namespace ui {
inline const Vec3 ink{.88f,.92f,.94f}, muted{.53f,.63f,.69f}, teal{.26f,.82f,.71f}, amber{.98f,.70f,.31f};
// Small built-in 5x7 font. No OS fonts, texture downloads, or extra dependencies.
inline std::array<unsigned char,7> glyph(char character) {
    switch (std::toupper(static_cast<unsigned char>(character))) {
    case 'A': return {14,17,17,31,17,17,17}; case 'B': return {30,17,17,30,17,17,30};
    case 'C': return {14,17,16,16,16,17,14}; case 'D': return {30,17,17,17,17,17,30};
    case 'E': return {31,16,16,30,16,16,31}; case 'F': return {31,16,16,30,16,16,16};
    case 'G': return {14,17,16,23,17,17,15}; case 'H': return {17,17,17,31,17,17,17};
    case 'I': return {14,4,4,4,4,4,14}; case 'J': return {7,2,2,2,2,18,12};
    case 'K': return {17,18,20,24,20,18,17}; case 'L': return {16,16,16,16,16,16,31};
    case 'M': return {17,27,21,21,17,17,17}; case 'N': return {17,25,21,19,17,17,17};
    case 'O': return {14,17,17,17,17,17,14}; case 'P': return {30,17,17,30,16,16,16};
    case 'Q': return {14,17,17,17,21,18,13}; case 'R': return {30,17,17,30,20,18,17};
    case 'S': return {15,16,16,14,1,1,30}; case 'T': return {31,4,4,4,4,4,4};
    case 'U': return {17,17,17,17,17,17,14}; case 'V': return {17,17,17,17,17,10,4};
    case 'W': return {17,17,17,21,21,21,10}; case 'X': return {17,17,10,4,10,17,17};
    case 'Y': return {17,17,10,4,4,4,4}; case 'Z': return {31,1,2,4,8,16,31};
    case '0': return {14,17,19,21,25,17,14}; case '1': return {4,12,4,4,4,4,14};
    case '2': return {14,17,1,2,4,8,31}; case '3': return {30,1,1,14,1,1,30};
    case '4': return {2,6,10,18,31,2,2}; case '5': return {31,16,16,30,1,1,30};
    case '6': return {14,16,16,30,17,17,14}; case '7': return {31,1,2,4,8,8,8};
    case '8': return {14,17,17,14,17,17,14}; case '9': return {14,17,17,15,1,1,14};
    case ':': return {0,4,4,0,4,4,0}; case '.': return {0,0,0,0,0,6,6};
    case '-': return {0,0,0,31,0,0,0}; case '/': return {1,1,2,4,8,16,16};
    case '+': return {0,4,4,31,4,4,0}; case '>': return {16,8,4,2,4,8,16};
    case '!': return {4,4,4,4,4,0,4};
    case '\'': return {4,4,8,0,0,0,0}; case '"': return {10,10,0,0,0,0,0};
    case '_': return {0,0,0,0,0,0,31}; case ',': return {0,0,0,0,0,4,8};
    case '(': return {2,4,8,8,8,4,2}; case ')': return {8,4,2,2,2,4,8};
    default: return {};
    }
}
struct Painter {
    std::vector<UiVertex> vertices;
    void rect(float x,float y,float w,float h,Vec3 c) {
        for (auto p:std::array<std::array<float,2>,6>{{{x,y},{x+w,y},{x+w,y+h},{x,y},{x+w,y+h},{x,y+h}}})
            vertices.push_back({p[0],p[1],c.x,c.y,c.z});
    }
    void text(float x,float y,const std::string& value,float scale,Vec3 c) {
        for (char ch:value) {
            const auto rows=glyph(ch);
            for (int row=0; row<7; ++row) for (int col=0; col<5; ++col)
                if (rows[row]&(1<<(4-col))) rect(x+col*scale,y+row*scale,scale,scale,c);
            x+=6*scale;
        }
    }
    void line(float x1,float y1,float x2,float y2,float thickness,Vec3 c) {
        const float dx=x2-x1,dy=y2-y1, length=std::sqrt(dx*dx+dy*dy);
        if (length==0) return;
        const float x=-dy/length*thickness/2, y=dx/length*thickness/2;
        const float points[][2]={{x1+x,y1+y},{x2+x,y2+y},{x2-x,y2-y},{x1+x,y1+y},{x2-x,y2-y},{x1-x,y1-y}};
        for (auto& p:points) vertices.push_back({p[0],p[1],c.x,c.y,c.z});
    }
};
}
}
