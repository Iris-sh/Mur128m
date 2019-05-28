#ifndef COMMAND_FORMAT_H
#define COMMAND_FORMAT_H

enum FMT_kind
{
    AH, BC, DE, F, G, M
};

struct M_fmt{
    unsigned unused : 2;
    unsigned sf     : 3;
    unsigned ix     : 5;
    unsigned b      : 5;
    unsigned r      : 5;
    unsigned op     : 8;
    unsigned fmt    : 4;
};

struct G_fmt{
    unsigned r4     : 5;
    unsigned r3     : 5;
    unsigned r2     : 5;
    unsigned r1     : 5;
    unsigned op     : 8;
    unsigned fmt    : 4;
};

struct F_fmt{
    unsigned unused : 5;
    unsigned r3     : 5;
    unsigned r2     : 5;
    unsigned r1     : 5;
    unsigned op     : 8;
    unsigned fmt    : 4;
};

struct DE_fmt{
    unsigned imm    : 10;
    unsigned r2     : 5;
    unsigned r1     : 5;
    unsigned op     : 8;
    unsigned fmt    : 4;
};

struct BC_fmt{
    unsigned imm    : 15;
    unsigned r      : 5;
    unsigned op     : 8;
    unsigned fmt    : 4;
};

struct AH_fmt{
    unsigned imm    : 20;
    unsigned op     : 8;
    unsigned fmt    : 4;
};



#endif
