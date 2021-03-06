// Hyperbolic Rogue
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

// basic utility functions

namespace hr {

#if CAP_TIMEOFDAY
long long getms() {
  struct timeval tval;
  gettimeofday(&tval, NULL);
  return tval.tv_sec * 1000000 + tval.tv_usec;
  }

#if !CAP_SDL
int SDL_GetTicks() {
  return getms() / 1000;
  }
#endif
#endif

long double sqr(long double x) { return x*x; }
string its(int i) { char buf[64]; sprintf(buf, "%d", i); return buf; }
string fts(float x) { char buf[64]; sprintf(buf, "%4.2f", x); return buf; }
string fts3(float x) { char buf[64]; sprintf(buf, "%5.3f", x); return buf; }
string fts4(float x) { char buf[64]; sprintf(buf, "%6.4f", x); return buf; }
string ftsg(float x) { char buf[64]; sprintf(buf, "%4.2g", x); return buf; }

string ftssmart(ld x) {
  if(x == int(x)) return its(int(x));
  if(abs(x) > 1) return fts4(x);
  char buf[64]; sprintf(buf, "%.10e", (float) x); 
  return buf;
  }

/*
string fts_smartdisplay(ld x, int maxdisplay) {
  string rv;
  if(x > 1e9 || x < -1e9) retrun fts(x);
  if(x<0) { rv = "-"; x = -x; }
  int i = int(x);
  rv += its(i);
  x -= i;
  bool nonzero = i;
  if(x == 0) return rv;
  if(x < 1e-9 && nonzero) return rv;
  rv += ".";
  while(maxdisplay > 0) {
    x *= 10;
    rv += '0' + int(x);
    if(int(x)) nonzero = true;
    x -= int(x);
    if(x == 0) return rv;
    if(x < 1e-9 && nonzero) return rv;
    maxdisplay--;
    }
  } */

string cts(char c) { char buf[8]; buf[0] = c; buf[1] = 0; return buf; }
string llts(long long i) {
    // sprintf does not work on Windows IIRC
    if(i < 0) return "-" + llts(-i);
    if(i < 10) return its((int) i);
    return llts(i/10) + its(i%10);
}
string itsh(int i) {static char buf[16]; sprintf(buf, "%03X", i); return buf; }
string itsh2(int i) {static char buf[16]; sprintf(buf, "%02X", i); return buf; }
string itsh8(int i) {static char buf[16]; sprintf(buf, "%08X", i); return buf; }

int gcd(int i, int j) {
  return i ? gcd(j%i, i) : j;
  }

int gmod(int i, int j) {
  i %= j; if(i<0) i += j;
  return i;
  }

ld frac(ld x) {
  x -= int(x);
  if(x < 0) x++;
  return x;
  }

// debug utilities

#if CAP_PROFILING

#define FRAMES 64
#define CATS 16

long long proftable[16][FRAMES];
int pframeid;

void profile_frame() { 
  pframeid++; pframeid %=  FRAMES;
  for(int t=0; t<16; t++) proftable[t][pframeid] = 0;
  }

void profile_start(int t) { proftable[t][pframeid] -= getms(); }
void profile_stop(int t) { proftable[t][pframeid] += getms(); }

void profile_info() {
  for(int t=0; t<16; t++) {
    sort(proftable[t], proftable[t]+FRAMES);
    if(proftable[t][FRAMES-1] == 0) continue;
    long long sum = 0;
    for(int f=0; f<FRAMES; f++) sum += proftable[t][f];
    printf("Category %d: avg = %Ld, %Ld..%Ld..%Ld..%Ld..%Ld\n",
      t, sum / FRAMES, proftable[t][0], proftable[t][16], proftable[t][32],
      proftable[t][48], proftable[t][63]);
    }
  }

#else

#define profile_frame()
#define profile_start(t)
#define profile_stop(t)
#define profile_info()
#endif

int whateveri, whateveri2;

purehookset hooks_tests;
}
