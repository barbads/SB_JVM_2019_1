#ifndef _Types_H_
#define _Types_H_

#include <map>
#include <string>

enum Type { B, C, D, F, I, J, S, Z, L, A };

static const std::map<std::string, Type> TypeMap = std::map<std::string, Type>{
    {"B", B}, {"C", C}, {"D", D}, {"F", F}, {"I", I},
    {"J", J}, {"S", S}, {"Z", Z}, {"L", L}, {"[", A}};

/*B = byte
  C = char
  D = double
  F = float
  I = int
  J = long
  S = short
  Z = bool
  L = class
*/

#endif