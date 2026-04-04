#include <math.h>
#include <stdlib.h>
#include <ncurses.h>
#include "gun.h"
#include "defs.h"

int gun_frame = 0;
int gun_timer = 0;
float gun_offset  = 20.0f;
float gun_offset2 = 20.0f;
int prev_gun_idx = -1;  // track gun swaps
Gun guns[GUN_COUNT];

static const char *gun0_art[] = {
    "   ___",
    "  [ X ]",
    "   {T}",
    " _/#0#\\_",
    "[@\\=---\\\\_",
    "[$@\\=----\\\\",
    "[\\$@\\==---\\\\",
    "\\[\\$@\\==---\\\\_",
    " \\[\\$@\\==----\\\\",
    "  \\[\\$@\\===---\\\\",
    "   \\[\\$@\\_=====\\\\__",
    "    \\[\\$@]\\[\\HHHHH\\]__",
    "      Y$@[/==|[@A@@@A@]__",
    "     /[]\\=\\==[\\[########]/",
    "     \\****[\\==|========|/",
    "      \\****[\\_|@||====|Y\\_",
    "       \\_**_/|@|[########]",
    "        \\@@@@@||==__|V^",
    "        /_/Y$||==__V^",
    "       /_/Y$||==__/",
    "      /_/Y$||==__/",
};

static const char *gun0_clr[] = {
    "   FFF",
    "  F G F",
    "   FFF",
    " FFBFBFF",
    "FEFBBBBFFF",
    "FBEFBBBBBFF",
    "FFBEFBBBBBFF",
    "FFFBEFBBBBBFFF",
    " FFFBEFBBBBBBFF",
    "  FFFBEFBBBBBBFF",
    "   FFFBEFFBBBBBFFFF",
    "    FFFBEFFHHHHHHHHHFF",
    "      FBEFFBBFHEGEEEGEHFFF",
    "     FFFFFFBBFFHHHHHHHHHHF",
    "     FDDDDFFBBFFFFFFFFFFF",
    "      FDDDDFFFFEBBBBBBBFFF",
    "       FFDDFFFEBHHHHHHHHHH",
    "        FEEEEEBBBBBBBFF",
    "        FDFFBBBBBBBFF",
    "       FDFFBBBBBBBF",
    "      FDFFBBBBBFFF",
};

static const char *gun1_art[] = {
    "   ___",
    "  [ X ]",
    "   {T}",
    "  /#0#\\",
    " |\\=---\\ ",
    " |@\\=---\\",
    " |@[\\==--\\",
    " |@|$\\_==-\\_",
    " |@\\$[\\@\\==\\@\\",
    "  \\@\\$[\\@\\==\\@\\",
    "   \\@\\$[\\@\\==\\@\\ ",
    "    \\@\\|_V[===\\V",
    "     \\@|@\\\\]___]\\__",
    "      \\|_/\\\\==\\}___}_",
    "         |*\\Y==\\}____}_",
    "       _/**/]L==-\\}_____}",
    "      /V*_/$/@L==-\\}_____}",
    "    _/**/$$/\\$@\\==-\\}_____}",
    "   /V**/$/   \\$@\\==-//_|_\\\\",
    "   |**/$/    /L$@\\==\\\\____\\\\",
    "    \\V$/    /L_\\$@\\==V_____V",
    "           /L__/\\$@\\==\\=====\\",
};

static const char *gun1_clr[] = {
    "   FFF",
    "  F G F",
    "   FFF",
    "  FBFBF",
    " FFBBBBF",
    " FEFBBBBF",
    " FEFFBBBBF",
    " FEFFFFBBBFF",
    " FEFFFFEFBBFEF",
    "  FEFFFFEFBBFEF",
    "   FEFFFFEFBBFEF",
    "    FEFFFFFBBBFF",
    "     FEFEFFFFFFFFFF",
    "      FFFFFFBBFHHHHHF",
    "         FDFFBBFHHHHHHF",
    "       FFDDFFFBBBFHHHHHHH",
    "      FFDFFFFEFBBBFHHHHHHH",
    "    FFDDFFFFFFFEFBBBFHHHHHHH",
    "   FFDDFFF   FFEFBBBFFBBBFF",
    "   FDDFFF    FFFEFBBFFBBBBFF",
    "    FFFF    FFBFFEFBBFBBBBBF",
    "           FFBBFFFEFBBFBBBBBF",
};

static const char *gun2_art[] = {
"                       ___                                                             ___                       ",
"                      [ X ]                                                           [ X ]                      ",
"                       {T}                                                             {T}                       ",
"                      /#0#\\                                                           /#0#\\                      ",
"                     /=-/$/                                                           \\$\\-=\\                     ",
"                    /=-/$/                                                             \\$\\-=\\                    ",
"                   /=-/$/                                                               \\$\\-=\\                   ",
"                  /=-/$/                                                                 \\$\\-=\\                  ",
"                 /=-/$/   ___                                                       ___   \\$\\-=\\                 ",
"                /=-/$/   [ X ]                                                     [ X ]   \\$\\-=\\                ",
"           __  /=-/$/     {T}                                                       {T}     \\$\\-=\\  __           ",
"          /@/ /==//@\\    /#0#\\                                                     /#0#\\    /@\\\\==\\ \\@\\          ",
"         /@/ /==//@@/   /=-/$/                                                     \\$\\-=\\   \\@@\\\\==\\ \\@\\         ",
"        /@/_/==//@@/   /=-/$/        _______________________________________        \\$\\-=\\   \\@@\\\\==\\_\\@\\        ",
"       /@/*/==//@@/   /=-/$/        /..__________...,=======,...__________..\\        \\$\\-=\\   \\@@\\\\==\\*\\@\\       ",
"      /@/*/==//@@/   /=-/\\/________/../\\*****___/..//'''''''\\\\..\\___*****/\\..\\________\\/\\==\\   \\@@\\\\==\\*\\@\\      ",
"     |@/*/==//@@/|  /==/@/.._________/**\\____\\====||'[]'''[]'||====/____/**\\_________..\\@\\==\\  |\\@@\\\\==\\*\\@|     ",
"   [/HHHH/]*/@@/*] /==/@/..///\\****\\\\\\**/{__}.\\___\\\\''''^''''//___/.{__}\\**///****/\\\\\\..\\@\\==\\ [*\\@@\\*[/HHHH/]   ",
"  [@A@@A@]*/@@/**[/==/@/..///**\\____\\\\\\/..||..._]===]'/|||\\'[===[_...||..\\///____/**\\\\\\..\\@\\==\\]**\\@@\\*[@A@@A@]  ",
" [######]*/@@/**[/HHH/]..///***/..........||__/-----['|||||']-----\\__||..........\\***\\\\\\..[\\HHH\\]**\\@@\\*[######] ",
"/========\\@@/**/=======\\///***/___________||___/=================\\___||___________\\***\\\\\\/=======\\**\\@@/========\\"
};

static const char *gun2_clr[] = {                                                      
"```````````````````````FFF`````````````````````````````````````````````````````````````FFF```````````````````````",
"``````````````````````F`G`F```````````````````````````````````````````````````````````F`G`F``````````````````````",
"```````````````````````FFF`````````````````````````````````````````````````````````````FFF```````````````````````",
"``````````````````````FBFBF```````````````````````````````````````````````````````````FBFBF``````````````````````",
"`````````````````````FBBFBF```````````````````````````````````````````````````````````FBFBBF`````````````````````",
"````````````````````FBBFBF`````````````````````````````````````````````````````````````FBFBBF````````````````````",
"```````````````````FBBFBF```````````````````````````````````````````````````````````````FBFBBF```````````````````",
"``````````````````FBBFBF`````````````````````````````````````````````````````````````````FBFBBF``````````````````",
"`````````````````FBBFBF```FFF```````````````````````````````````````````````````````FFF```FBFBBF`````````````````",
"````````````````FBBFBF```F`G`F`````````````````````````````````````````````````````F`G`F```FBFBBF````````````````",
"```````````FF``FBBFBF`````FFF```````````````````````````````````````````````````````FFF`````FBFBBF``FF```````````",
"``````````FEF`FBBFFEF````FBFBF`````````````````````````````````````````````````````FBFBF````FEFFBBF`FEF``````````",
"`````````FEF`FBBFFEEF```FBBDBF`````````````````````````````````````````````````````FBDBBF```FEEFFBBF`FEF`````````",
"````````FEFFFBBFFEEF```FBBDBF````````FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF````````FBDBBF```FEEFFBBFFFEEF```````",
"```````FEFDFBBFFEEF```FBBDBF````````FMMFFFFFFFFFFMMMSSSSSSSSSMMMFFFFFFFFFFMMF````````FBDBBF```FEEFFBBFDFEF```````",
"``````FEFDFBBFFEEF```FBBDFFFFFFFFFFFMMFFDDDDDFFFFMMSSSSSSSSSSSMMFFFFDDDDDFFMMFFFFFFFFFFFDBBF```FEEFFBBFDFEF``````",
"`````FEFDFBBFFEEFF``FBBDEFMMFFFFFFFFFFDDFFGGFFBBBBSSSGGSSSGGSSSBBBBFFGGFFDDFFFFFFFFFFDDFEDBBF``FFEEFFBBFDFEF`````",
"```HHHHHHHHDFEEFDF`FBBDEFMMFFFFDDDDFFFDDFGGGGMFFFBSSSSSSGSSSSSSBFFFMGGGGFDDFFFDDDDFFFFMMFEDBBF`FDFEEFDHHHHHHHH```",
"``HEGEEGEHDFEEFDDFFBBDEFMMFFFDDFFFFFFFFFMMFFMMMFFBBBSSGGGGGSSBBBFFMMMFFMMFFFFFFFFFDDFFFMMFEDBBFFDDFEEFDHEGEEGEH``",
"`HHHHHHHHDFEEFDDHHHHHHHMMFFFDDDFMMMMMMMMMMFFFFFBBBBBSSGGGGGSSBBBBBFFFFFMMMMMMMMMMFFDDFFFMMHHHHHHHDDFEEFDHHHHHHHH`",
"HHHHHHHHHHEEFDDHHHHHHHHHFFFDDDFMMMMMMMMMMMFFFFFFFFFFFSSSSSSSFFFFFFFFFFFMMMMMMMMMMMMFDDFFFHHHHHHHHHDDFEEHHHHHHHHHH"
};


static int resolve_color(char c) {
    switch (c) {
        case 'M': return GUN_BDR;
        case 'F': return GUN_TRIM;
        case 'B': return GUN_BODY;
        case 'E': return GUN_ACC;
        case 'S': return MUZ_1;
        case 'D': return GUN_DIRT;
        case 'H': return HAND_CLR_S;
        case 'G': return HAND_CLR;
        default:  return CP_ENTITY_Y;
    }
}

void init_guns(void)
{
    guns[0].name = "The P3w";
    guns[0].art = gun0_art;
    guns[0].clr = gun0_clr;
    guns[0].dmg = 10;
    guns[0].cx = 19;
    guns[0].r = -21;
    guns[0].num_art_rows = (int)(sizeof(gun0_art) / sizeof(gun0_art[0]));
    guns[0].num_muzzles = 1;
    guns[0].muzzles[0] = (MuzzlePos){2, 4};

    guns[1].name = "The P3w P3w";
    guns[1].art = gun0_art;
    guns[1].clr = gun0_clr;
    guns[1].dmg = 15;
    guns[1].cx = 19;
    guns[1].r = -21;
    guns[1].num_art_rows = (int)(sizeof(gun0_art) / sizeof(gun0_art[0]));
    guns[1].num_muzzles = 1;
    guns[1].muzzles[0] = (MuzzlePos){2, 4};

    guns[2].name = "The Krok";
    guns[2].art = gun1_art;
    guns[2].clr = gun1_clr;
    guns[2].dmg = 20;
    guns[2].cx = 19;
    guns[2].r = -22;
    guns[2].num_art_rows = (int)(sizeof(gun1_art) / sizeof(gun1_art[0]));
    guns[2].num_muzzles = 1;
    guns[2].muzzles[0] = (MuzzlePos){2, 4};

    guns[3].name = "Akimbo Kroks";
    guns[3].art = gun1_art;
    guns[3].clr = gun1_clr;
    guns[3].dmg = 25;
    guns[3].cx = 19;
    guns[3].r = -22;
    guns[3].num_art_rows = (int)(sizeof(gun1_art) / sizeof(gun1_art[0]));
    guns[3].num_muzzles = 1;
    guns[3].muzzles[0] = (MuzzlePos){2, 4};

    guns[4].name = "The Krab Mech";
    guns[4].art = gun2_art;
    guns[4].clr = gun2_clr;
    guns[4].dmg = 30;
    guns[4].cx = -57;
    guns[4].r = -21;
    guns[4].num_art_rows = (int)(sizeof(gun2_art) / sizeof(gun2_art[0]));
    guns[4].num_muzzles = 4;
    guns[4].muzzles[0] = (MuzzlePos){1, 24};
    guns[4].muzzles[1] = (MuzzlePos){1, 88};
    guns[4].muzzles[2] = (MuzzlePos){9, 27};
    guns[4].muzzles[3] = (MuzzlePos){9, 85};
}

void draw_muzzle_flash(int cx, int cy) {
    for (int dy = -4; dy <= 4; dy++) {
        for (int dx = -7; dx <= 7; dx++) {
            float dist = sqrtf((dx*0.55f)*(dx*0.55f) + (float)(dy*dy));
            char ch; int pair;
            if      (dist < 2.0f)                          continue;
            else if (dist < 3.0f) { ch = 'O'; pair = MUZ_1; }
            else if (dist < 3.8f) { ch = 'o'; pair = MUZ_1; }
            else if (dist < 4.3f) { ch = '*'; pair = MUZ_2; }
            else if (dist < 4.8f && (rand()%3 != 0))
                                    { ch = '.'; pair = MUZ_3; }
            else continue;
            if (rand()%3 == 0) ch = ' ';
            attron(COLOR_PAIR(pair) | A_BOLD);
            mvaddch(cy + dy, cx + dx, ch);
            attroff(COLOR_PAIR(pair) | A_BOLD);
        }
    }
}

void draw_gun(int gun_idx, int rows, int cols)
{
    if (gun_idx < 0 || gun_idx >= GUN_COUNT) return;

    Gun *gun = &guns[gun_idx];
    const char **art = gun->art;
    const char **clr = gun->clr;
    int num_rows = gun->num_art_rows;
    int is_dual = (gun_idx % 2 == 1);
    
    // detect gun swap and reset animation
    if (gun_idx != prev_gun_idx) {
        if(!is_dual) {
            gun_offset  = 20.0f;
        }
        gun_offset2 = 20.0f;
        prev_gun_idx = gun_idx;
    }

    // slide animation
    float target2 = is_dual ? 0.0f : 20.0f;
    gun_offset  += (0.0f    - gun_offset)  * 0.18f;
    gun_offset2 += (target2 - gun_offset2) * 0.18f;

    int kick      = (gun_frame == 1) ? 1 : 0;
    int base      = rows + kick;
    int cx        = cols/2 + gun->cx;
    int mirror_cx = cols/2 - gun->cx;
    int r         = base + gun->r + (int)gun_offset;
    int r2        = base + gun->r + (int)gun_offset2;

    // muzzle flash
    if (gun_frame == 1) {
        // draw_muzzle_flash(cx +4, r + 2);
        for (int i = 0; i < gun->num_muzzles; i++) {
            draw_muzzle_flash(cx + gun->muzzles[i].col, r + gun->muzzles[i].row);

            // dual gun or mech
            if (is_dual || gun_idx == 4) {
                draw_muzzle_flash(mirror_cx - gun->muzzles[i].col, r2 + gun->muzzles[i].row);
            }
        }
    }

    // draw right gun
    for (int row = 0; row < num_rows; row++) {
        const char *a = art[row];
        const char *c = clr[row];
        int ci = 0;
        for (int col = 0; a[col] != '\0'; col++) {
            if (a[col] == ' ') { ci++; continue; }
            int pair = resolve_color(c[ci]);
            attron(COLOR_PAIR(pair) | A_BOLD);
            mvaddch(r + row, cx + col, a[col]);
            attroff(COLOR_PAIR(pair) | A_BOLD);
            ci++;
        }
    }

    // draw left gun
    if (is_dual) {
        for (int row = 0; row < num_rows; row++) {
            const char *a = art[row];
            const char *c = clr[row];
            int art_len = 0;
            while (a[art_len] != '\0') art_len++;
            int ci = 0;
            for (int col = 0; col < art_len; col++) {
                if (a[col] == ' ') { ci++; continue; }
                int pair = resolve_color(c[ci]);
                int draw_col = mirror_cx - col;
                attron(COLOR_PAIR(pair) | A_BOLD);
                mvaddch(r2 + row, draw_col, a[col]);
                attroff(COLOR_PAIR(pair) | A_BOLD);
                ci++;
            }
        }
    }
}