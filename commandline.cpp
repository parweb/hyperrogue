// Hyperbolic Rogue -- commandline options
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

#if CAP_COMMANDLINE
const char *scorefile = "hyperrogue.log";
const char *conffile = "hyperrogue.ini";
string levelfile = "hyperrogue.lev";
string picfile = "hyperrogue.pic";
const char *musicfile = "";
const char *loadlevel = NULL;

bool appears(const string& haystack, const string& needle) {
  return haystack.find(needle) != string::npos;
  }

eLand readland(const string& ss) {
  if(ss == "II") return laCrossroads2;
  if(ss == "III") return laCrossroads3;
  if(ss == "IV") return laCrossroads4;
  if(ss == "V") return laCrossroads5;
  for(int l=0; l<landtypes; l++) if(appears(linf[l].name, ss)) {
    return eLand(l);
    break;
    }
  return laNone;
  }

eItem readItem(const string& ss) {
  for(int i=0; i<ittypes; i++) if(appears(iinf[i].name, ss)) {
    return eItem(i);
    break;
    }
  return itNone;
  }

eMonster readMonster(const string& ss) {
  for(int i=0; i<motypes; i++) if(appears(minf[i].name, ss)) {
    return eMonster(i);
    break;
    }
  return moNone;
  }

void initializeCLI() {
  printf("HyperRogue by Zeno Rogue <zeno@attnam.com>, version " VER "\n");

#ifndef NOLICENSE
  printf("released under GNU General Public License version 2 and thus\n");
  printf("comes with absolutely no warranty; see COPYING for details\n");
#endif

  #ifdef FHS
  static string sbuf, cbuf;
  if(getenv("HOME")) {
    sbuf = getenv("HOME"); sbuf += "/."; sbuf += scorefile;
    cbuf = getenv("HOME"); cbuf += "/."; cbuf += conffile;
    scorefile = sbuf.c_str();
    conffile = cbuf.c_str();
    }
  #endif
  }

namespace arg {
  vector<string> argument;
  int pos;

  void lshift() { pos++; }

  void shift() {
    lshift(); if(pos > isize(argument)) { printf("Missing parameter\n"); exit(1); }
    }
  
  const string& args() { return argument[pos]; }
  const char* argcs() { return args().c_str(); }
  int argi() { return atoi(argcs()); }
  int arghex() { return strtol(argcs(), NULL, 16); }
  ld argf() { return atof(argcs()); }
  bool argis(const string& s) { return args() == s; }
  
  void init(int argc, char **argv) { for(int i=0; i<argc; i++) argument.push_back(argv[i]); shift(); }
 
  void phaseerror(int x) {
    printf("Command line error: cannot read command '%s' from phase %d in phase %d\n", args().c_str(), x, curphase);
    exit(1);
    }

  }

#define CHEAT autocheat = true; cheater++; timerghost = false;

int arg::readCommon() {

// first phase options

  if(argis("-c")) { PHASE(1); shift(); conffile = argcs(); }
  else if(argis("-s")) { PHASE(1); shift(); scorefile = argcs(); }
  else if(argis("-m")) { PHASE(1); shift(); musicfile = argcs(); }
  else if(argis("-nogui")) { PHASE(1); noGUI = true; }
#ifndef EMSCRIPTEN
  else if(argis("-font")) { PHASE(1); shift(); fontpath = args(); }
#endif
#if CAP_SDLAUDIO
  else if(argis("-se")) { PHASE(1); shift(); wheresounds = args(); }
#endif

// change the configuration from the command line
  else if(argis("-svol")) { PHASEFROM(2); shift(); effvolume = argi(); }
  else if(argis("-back")) {
    PHASEFROM(2); shift(); backcolor = arghex();
    }
  else if(argis("-borders")) {
    PHASEFROM(2); shift(); bordcolor = arghex();
    }
  else if(argis("-fore")) {
    PHASEFROM(2); shift(); forecolor = arghex();
    }
  else if(argis("-aa")) { PHASEFROM(2); shift(); vid.antialias = argi(); }
  else if(argis("-lw")) { PHASEFROM(2); shift(); vid.linewidth = argf(); }
  else if(argis("-wm")) { PHASEFROM(2); shift(); vid.wallmode = argi(); }
  else if(argis("-mm")) { PHASEFROM(2); shift(); vid.monmode = argi(); }

  else if(argis("-wsh")) { shift(); patterns::whichShape = args()[0]; }
  else if(argis("-noshadow")) { noshadow = true; }

// non-configurable options
  else if(argis("-vsync_off")) {
    vsync_off = true;
    if(curphase == 3) setvideomode();
    }
  else if(argis("-noplayer")) 
    mapeditor::drawplayer = !mapeditor::drawplayer;
  else if(argis("-nofps")) {
    nofps = true;
    }
  else if(argis("-nohud")) {
    nohud = true;
    }
  else if(argis("-nomenu")) {
    nomenukey = true;
    }
  else if(argis("-nohelp")) {
    nohelp = true;
    }
  else if(argis("-dont_face_pc")) {
    dont_face_pc = true;
    }
  else if(argis("-rch")) {    
    PHASEFROM(2); CHEAT reptilecheat = true;
    }

// cheats
  else if(argis("-WT")) {
    PHASE(3);
    shift(); 
    activateSafety(readland(args()));
    CHEAT;
    }
  else if(argis("-W2")) {
    shift(); cheatdest = readland(args()); CHEAT;
    showstartmenu = false;
    }
  else if(argis("-I")) {
    PHASE(3) CHEAT
    shift(); eItem i = readItem(args());
    shift(); items[i] = argi(); 
    }
  else if(argis("-IP")) {
    PHASE(3) CHEAT
    shift(); eItem i = readItem(args());
    shift(); int q = argi();
    placeItems(q, i);
    }
  else if(argis("-SM")) {
    PHASEFROM(2);
    shift(); stereo::mode = stereo::eStereo(argi());
    }
#if CAP_INV
  else if(argis("-IU")) {
    PHASE(3) CHEAT
    shift(); eItem i = readItem(args());
    shift(); inv::usedup[i] += argi();
    inv::compute();
    }
  else if(argis("-IX")) {
    PHASE(3) CHEAT
    shift(); eItem i = readItem(args());
    shift(); inv::extra_orbs[i] += argi();
    inv::compute();
    }
#endif
  else if(argis("-ambush")) {
    // make all ambushes use the given number of dogs
    // example: hyper -W Hunt -IP Shield 1 -ambush 60
    PHASE(3) CHEAT
    shift(); ambushval = argi();
    }
  else if(argis("-M")) {
    PHASE(3) CHEAT
    shift(); eMonster m = readMonster(args());
    shift(); int q = argi();
    printf("m = %s q = %d\n", dnameof(m), q);
    restoreGolems(q, m, 7);
    }
  else if(argis("-MK")) {
    PHASE(3) CHEAT
    shift(); eMonster m = readMonster(args());
    shift(); kills[m] += argi();
    }

// mode changes:

#define TOGGLE(x, param, act) \
else if(args()[0] == '-' && args()[1] == x && !args()[2]) { showstartmenu = false; act; } \
else if(args()[0] == '-' && args()[1] == x && args()[2] == '1') { showstartmenu = false; if(!param) act; } \
else if(args()[0] == '-' && args()[1] == x && args()[2] == '0') { showstartmenu = false; if(param) act; }

  TOGGLE('o', vid.usingGL, switchGL())
  TOGGLE('C', chaosmode, stop_game_and_switch_mode(rg::chaos))
  TOGGLE('7', nonbitrunc, stop_game_and_switch_mode(rg::bitrunc))
  TOGGLE('f', vid.full, switchFullscreen())
  TOGGLE('T', tactic::on, stop_game_and_switch_mode(rg::tactic))
  TOGGLE('S', shmup::on, stop_game_and_switch_mode(rg::shmup))
  TOGGLE('H', hardcore, switchHardcore())
  TOGGLE('R', randomPatternsMode, stop_game_and_switch_mode(rg::randpattern))
  TOGGLE('i', inv::on, stop_game_and_switch_mode(rg::inv))
  
  else if(argis("-peace")) {
    peace::otherpuzzles = true;
    stop_game_and_switch_mode(peace::on ? 0 : rg::peace);
    }
  else if(argis("-pmem")) {
    peace::otherpuzzles = false;
    stop_game_and_switch_mode(peace::on ? 0 : rg::peace);
    }
  else if(argis("-geo")) { 
    shift(); targetgeometry = (eGeometry) argi();
    stop_game_and_switch_mode(rg::geometry);
    }
  else if(argis("-gp")) {
    PHASEFROM(2);
    stop_game_and_switch_mode(rg::nothing);
    shift(); gp::param.first = argi();
    shift(); gp::param.second = argi();
    stop_game_and_switch_mode(rg::gp);
    }
// 'do something'
  else if(argis("-W")) {
    PHASEFROM(2);
    shift(); 
    firstland0 = firstland = specialland = readland(args());
    stop_game_and_switch_mode(rg::nothing);
    showstartmenu = false;
    }
  else if(argis("-canvas")) {
    PHASEFROM(2);
    firstland = specialland = laCanvas;
    shift();
    if(args() == "i") canvas_invisible = !canvas_invisible;
    else if(args().size() == 1) patterns::whichCanvas = args()[0];
    else patterns::canvasback = arghex();
    stop_game_and_switch_mode(rg::nothing);
    }
  else if(argis("-pattern")) {
    PHASEFROM(2);
    shift();
    const char *c = argcs();
    using namespace patterns;
    subpattern_flags = 0;
    whichPattern = 0;
    while(*c) { 
      if(*c >= '0' && *c <= '9') subpattern_flags ^= 1 << (*c - '0'); 
      else if(*c == '@') subpattern_flags ^= 1 << 10; 
      else if(*c == '-') subpattern_flags ^= 1 << 11; 
      else if(*c == '~') subpattern_flags ^= 1 << 12; 
      else whichPattern = *c;
      c++; 
      }
    }

// non-categorized:
  else if(argis("-pal")) {
    PHASEFROM(2); CHEAT;
    shift(); int id = argi();
    shift(); linepatterns::patterns[id].color |= argi();
    }
  else if(argis("-palrgba")) {
    PHASEFROM(2); CHEAT;
    shift(); int id = argi();
    shift(); linepatterns::patterns[id].color = arghex();
    }
  else if(argis("-qs")) {
    CHEAT;
    shift(); currfp.qpaths.push_back(args());
    }
  else if(argis("-fix")) {
    PHASE(1);
    fixseed = true; autocheat = true;
    }
  else if(argis("-fixx")) {
    PHASE(1);
    fixseed = true; autocheat = true;
    shift(); startseed = argi();
    }
  else if(argis("-steplimit")) {
    fixseed = true; autocheat = true;
    shift(); steplimit = argi();
    }
  else if(argis("-qpar")) { 
    int p;
    shift(); sscanf(argcs(), "%d,%d,%d", 
      &p, &quotientspace::rvadd, &quotientspace::rvdir
      );
    autocheat = true;
    currfp.init(p); 
    }
  else if(argis("-test")) 
    callhooks(hooks_tests);
  else if(argis("-qpar2")) {
    stop_game_and_switch_mode(rg::nothing);
    int a, b;
    shift(); sscanf(argcs(), "%d,%d", &a, &b);
    using namespace fieldpattern;
    current_extra = a;
    fgeomextras[current_extra].current_prime_id = b;
    enableFieldChange();
    if(geometry != gFieldQuotient) {
      targetgeometry = gFieldQuotient; stop_game_and_switch_mode(rg::geometry);
      }
    }
  else if(argis("-tpar")) { 
    torusconfig::torus_mode = torusconfig::tmSingle;
    shift(); sscanf(argcs(), "%d,%d,%d", 
      &torusconfig::qty, 
      &torusconfig::dx,
      &torusconfig::dy
      );
    }
  else if(argis("-tparx")) {
    shift(); 
    sscanf(argcs(), "%d,%d,%d", 
      (int*) &torusconfig::torus_mode,
      &torusconfig::sdx,
      &torusconfig::sdy
      );
    if(torusconfig::torus_mode == torusconfig::tmSingle)
      torusconfig::qty = torusconfig::sdx,
      torusconfig::dy = torusconfig::sdy;
    torusconfig::activate();
    }
  else if(argis("-cs")) {
    shift(); CHEAT
    fieldpattern::matrix M = currfp.strtomatrix(args());
    fieldpattern::subpathid = currfp.matcode[M];
    fieldpattern::subpathorder = currfp.order(M);
    }
  else if(argis("-csp")) {
    CHEAT
    currfp.findsubpath();
    }
  else if(argis("-quantum")) {
    CHEAT;
    quantum = true;
    }
  else if(argis("-P")) { 
    PHASE(2); shift(); 
    vid.scfg.players = argi();
    stop_game_and_switch_mode(rg::nothing);
    }
  else if(argis("-PM")) { 
    PHASEFROM(2); shift(); pmodel = eModel(argi());
    }
  else if(argis("-offline")) {
    PHASE(1);
    offlineMode = true;
    }
  else if(argis("-debugf")) {
    debugfile = fopen("hyperrogue-debug.txt", "w");
    shift(); debugflags = argi();
    }
  else if(argis("-debuge")) {
    debugfile = stderr;
    shift(); debugflags = argi();
    }
  else if(argis("-each")) {
    PHASEFROM(2); start_game();
    shift(); int q = argi(); autocheat = true;
    for(int i=0; i<ittypes; i++)
      if(itemclass(eItem(i)) == IC_TREASURE)
        items[i] = q;
    }
  else if(argis("-ch")) { autocheat = true; }
  else if(argis("-zoom")) { 
    PHASEFROM(2); shift(); vid.scale = argf();
    }
  else if(argis("-alpha")) { 
    PHASEFROM(2); shift(); vid.alpha = argf();
    }
  else if(argis("-Y")) { 
    yendor::on = true;
    shift(); yendor::challenge = argi();
    }
  else if(argis("-r")) { 
    PHASEFROM(2);
    shift(); 
    int clWidth=0, clHeight=0, clFont=0;
    sscanf(argcs(), "%dx%dx%d", &clWidth, &clHeight, &clFont);
    if(clWidth) vid.xres = clWidth;
    if(clHeight) vid.yres = clHeight;
    if(clFont) vid.fsize = clFont;
    }    
  else if(argis("--run")) {
    PHASE(3); 
    start_game();
    mainloop(); quitmainloop = false;
    }
  else if(argis("--msg")) {
    shift(); addMessage(args());
    printf("%s\n", args().c_str());
    }
  else if(argis("--msg0")) {
    clearMessages();
    }
#if CAP_TOUR
  else if(argis("--tour")) {
    PHASEFROM(2); start_game(); tour::start();
    }
  else if(argis("--presentation")) {
    PHASEFROM(2); tour::texts = false;
    start_game(); tour::start();
    }
#endif
  else if(argis("--draw")) {
    PHASE(3); drawscreen();
    }
  else if(argis("--exit")) {
    PHASE(3); printf("Success.\n");
    exit(0);
    }
  else if(argis("-gencells")) {
    PHASEFROM(2); shift(); start_game();
    printf("Generating %d cells...\n", argi());
    celllister cl(cwt.c, 50, argi(), NULL);
    printf("Cells generated: %d\n", isize(cl.lst));
    for(int i=0; i<isize(cl.lst); i++)
      setdist(cl.lst[i], 7, NULL);
    }
  else if(argis("-sr")) {    
    PHASEFROM(2);
    shift(); sightrange_bonus = argi();
    }
  else if(argis("-srx")) {    
    PHASEFROM(2); CHEAT;
    shift(); sightrange_bonus = genrange_bonus = gamerange_bonus = argi();
    }
  else if(argis("-els")) {
    shift(); conformal::extra_line_steps = argf();
    }
  else if(argis("-we")) {    
    PHASEFROM(2);
    shift(); whatever = argf(); resetGeometry();
    }
  else if(argis("-wei")) {    
    PHASEFROM(2);
    shift(); whateveri = argf(); resetGeometry();
    }
  else if(argis("-wei2")) {
    PHASEFROM(2);
    shift(); whateveri2 = argf(); resetGeometry();
    }
  else if(argis("-bright")) {    
    bright = true;
    }
#if CAP_SDL
  else if(argis("-pngshot")) {
    PHASE(3); shift(); start_game();
    printf("saving PNG screenshot to %s\n", argcs());
    saveHighQualityShot(argcs());
    }
#endif
  else if(argis("-svgsize")) {
    shift(); sscanf(argcs(), "%d/%d", &svg::svgsize, &svg::divby);
    }
  else if(argis("-svgfont")) {
    shift(); svg::font = args();
    // note: use '-svgfont latex' to produce text output as: \myfont{size}{text}
    // (this is helpful with Inkscape's PDF+TeX output feature; define \myfont yourself)
    }
  else if(argis("-pngsize")) {
    shift(); pngres = argi();
    }
  else if(argis("-pngformat")) {
    shift(); pngformat = argi();
    }
  else if(argis("-svggamma")) {
    shift(); svg::gamma = argf();
    }
  else if(argis("-svgshot")) {
    PHASE(3); shift(); start_game();
    printf("saving SVG screenshot to %s\n", argcs());
    svg::render(argcs());
    }

// other
#if CAP_EDIT
  else if(argis("-lev")) { shift(); levelfile = args(); }
  else if(argis("-pic")) { shift(); picfile = args(); }
  else if(argis("-load")) { PHASE(3); shift(); mapstream::loadMap(args()); }
  else if(argis("-picload")) { PHASE(3); shift(); mapeditor::loadPicFile(args()); }
#endif
// graphical options
  else if(argis("-noscr")) {
    PHASE(3);
    popScreenAll();
    showstartmenu = false;
    }

  else if(argis("-W3")) {
    shift(); top_land = readland(args()); CHEAT;
    showstartmenu = false;
    }
  else if(argis("-top")) {
    PHASE(3); View = View * spin(-M_PI/2);
    }

// informational
  else if(argis("--version") || argis("-v")) {
    printf("HyperRogue version " VER "\n");
    exit(0);
    }
  else if(argis("-fi")) {
    fieldpattern::info();
    exit(0);
    } 
  else if(argis("-L")) {
    printf("Treasures:\n");
    for(int i=1; i<ittypes; i++) 
      if(itemclass(eItem(i)) == IC_TREASURE)
        printf("    %s\n", iinf[i].name);
    printf("\n");
    printf("Orbs:\n");
    for(int i=1; i<ittypes; i++) 
      if(itemclass(eItem(i)) == IC_ORB)
        printf("    %s\n", iinf[i].name);
    printf("\n");
    printf("Other items:\n");
    for(int i=1; i<ittypes; i++) 
      if(itemclass(eItem(i)) == IC_OTHER)
        printf("    %s\n", iinf[i].name);
    printf("\n");
    printf("Monsters:\n");
    for(int i=1; i<motypes; i++) 
      printf("    %s\n", minf[i].name);
    printf("\n");
    printf("Lands:\n");
    for(int i=1; i<landtypes; i++) 
      printf("    %s\n", linf[i].name);
    printf("\n");
    printf("Walls:\n");
    for(int i=0; i<walltypes; i++) 
      printf("    %s\n", winf[i].name);
    printf("\n");
    exit(0);
    }
  else if(argis("--help") || argis("-h")) {
    printf("Press F1 while playing to get ingame options.\n\n");
    printf("HyperRogue accepts the following command line options:\n");
    printf("  -c FILE        - use the specified configuration file\n");
    printf("  -s FILE        - use the specified highscore file\n");
    printf("  -m FILE        - use the specified soundtrack (music)\n");
    printf("  -se DIR        - the directory containing sound effects\n");
    printf("  -lev FILE      - use the specified filename for the map editor (without loading)\n");
    printf("  -load FILE     - use the specified filename for the map editor\n");
    printf("  -canvas COLOR  - set background color or pattern code for the canvas\n");
    printf("  --version, -v  - show the version number\n");
    printf("  --help, -h     - show the commandline options\n");
    printf("  -f*            - toggle fullscreen mode\n");
    printf("  -wm n, -mm n   - start in the given wallmode or monmode\n");
    printf("  -r WxHxF       - use the given resolution and font size\n");
    printf("  -o*            - toggle the OpenGL mode\n");
    printf("  -W LAND        - start in the given land (cheat)\n");
    printf("  -W2 LAND       - make the given land easy to find (also turns on autocheat)\n");
    printf("  -ch            - auto-enable cheat mode\n");
    printf("  -geo n         - switch geometry (1=Euclidean, 2=spherical, 3=elliptic, 4/5=quotient)\n");
    printf("  -qs <desc>     - fieldpattern: quotient by the given <desc> (must be followed by qpar)\n");
    printf("  -qpar <prime>  - fieldpattern: use the given prime instead of 43\n");
    printf("  -cs <desc>     - fieldpattern: set subpath to the given <desc> (cannot be followed by qpar)\n");
    printf("  -csp           - fieldpattern: find the subpath of order <prime> (cannot be followed by qpar)\n");
    printf("  -S*            - toggle Shmup\n");
    printf("  -P n           - switch Shmup number of players (n=1..7)\n");
    printf("  -PM            - switch the model index\n");
    printf("  -H*            - toggle Hardcore\n");
    printf("  -T*            - toggle Tactical\n");
    printf("  -7*            - toggle heptagonal mode\n");
    printf("  -C*            - toggle Chaos mode\n");
    printf("  -R*            - toggle Random Pattern\n");
    printf("  -Y id          - enable Yendor, level id\n");
    printf("  -D             - disable all the special game modes\n");
    printf("  -L             - list of features\n");
    printf("  -debugf 7      - output debugging information to hyperrogue-debug.txt\n");
    printf("  -debuge 7      - output debugging information to stderr\n");
    printf("  -offline       - don't connect to Steam (for Steam versions)\n");
    printf("  -I ITEM n      - start with n of ITEM (activates cheat and disables ghosts)\n");
    printf("  -fix           - fix the seed\n");
    printf("Toggles: -o0 disables, -o1 enables, -o switches");
    printf("Not all options are documented, see hyper.cpp");
    exit(0);
    }
  else if(ca::readArg()) ;
  else return 1;
  return 0;
  }

purehookset hooks_config;

hookset<int()> *hooks_args;

namespace arg {
  int curphase;

  auto ah = addHook(hooks_args, 0, readCommon);
  
  void read(int phase) { 
    curphase = phase;
    callhooks(hooks_config);
    while(pos < isize(argument)) {
      for(auto& h: *hooks_args) {
        int r = h.second(); if(r == 2) return; if(r == 0) { lshift(); goto cont; }
        }
      printf("Unknown option: %s\n", argcs());
      exit(3);
      cont: ;
      }
    }
  }
#endif

}
