/* Stellarium Web Engine - Copyright (c) 2018 - Noctua Software Ltd
 *
 * This program is licensed under the terms of the GNU AGPL v3, or
 * alternatively under a commercial licence.
 *
 * The terms of the AGPL v3 license can be found in the main directory of this
 * repository.
 */

#include "bayer.h"

#include "uthash.h"
#include "utils/utils.h"

#include <assert.h>

static const unsigned char DATA[7131];

static const char *GREEK[][3] = {
    {"α", "alf", "Alpha"},
    {"β", "bet", "Beta"},
    {"γ", "gam", "Gamma"},
    {"δ", "del", "Delta"},
    {"ε", "eps", "Epsilon"},
    {"ζ", "zet", "Zeta"},
    {"η", "eta", "Eta"},
    {"θ", "tet", "Theta"},
    {"ι", "iot", "Iota"},
    {"κ", "kap", "Kappa"},
    {"λ", "lam", "Lambda"},
    {"μ", "mu" , "Mu"},
    {"ν", "nu" , "Nu"},
    {"ξ", "xi" , "Xi"},
    {"ο", "omi", "Omicron"},
    {"π", "pi" , "Pi"},
    {"ρ", "rho", "Rho"},
    {"σ", "sig", "Sigma"},
    {"τ", "tau", "Tau"},
    {"υ", "ups", "Upsilon"},
    {"φ", "phi", "Phi"},
    {"χ", "chi", "Chi"},
    {"ψ", "psi", "Psi"},
    {"ω", "ome", "Omega"}
};

static const char *CSTS[] = {
    "Aql", "And", "Scl", "Ara", "Lib", "Cet", "Ari", "Sct", "Pyx", "Boo",
    "Cae", "Cha", "Cnc", "Cap", "Car", "Cas", "Cen", "Cep", "Com", "CVn",
    "Aur", "Col", "Cir", "Crt", "CrA", "CrB", "Crv", "Cru", "Cyg", "Del",
    "Dor", "Dra", "Nor", "Eri", "Sge", "For", "Gem", "Cam", "CMa", "UMa",
    "Gru", "Her", "Hor", "Hya", "Hyi", "Ind", "Lac", "Mon", "Lep", "Leo",
    "Lup", "Lyn", "Lyr", "Ant", "Mic", "Mus", "Oct", "Aps", "Oph", "Ori",
    "Pav", "Peg", "Pic", "Per", "Equ", "CMi", "LMi", "Vul", "UMi", "Phe",
    "Psc", "PsA", "Vol", "Pup", "Ret", "Sgr", "Sco", "Ser", "Sex", "Men",
    "Tau", "Tel", "Tuc", "Tri", "TrA", "Aqr", "Vir", "Vel"};

typedef struct {
    UT_hash_handle  hh;

    int             hd;
    uint8_t         cst;
    uint8_t         bayer;
    uint8_t         bayer_n;
} entry_t;

static entry_t *g_data = NULL;        // Global array.
static entry_t *g_entries = NULL;     // Global hash.

static void bayer_init(void)
{
    struct {int hd; uint8_t cst, bayer, bayer_n; char pad;} *data;
    int size, nb, i;

    memcpy(&size, DATA, 4);
    data = malloc(size);
    z_uncompress(data, size, DATA + 4, ARRAY_SIZE(DATA) - 4);

    nb = size / sizeof(*data);
    g_data = calloc(nb + 1, sizeof(*g_data));
    for (i = 0; i < nb; i++) {
        assert(data[i].hd);
        g_data[i].hd = data[i].hd;
        g_data[i].cst = data[i].cst;
        g_data[i].bayer = data[i].bayer;
        g_data[i].bayer_n = data[i].bayer_n;
        HASH_ADD_INT(g_entries, hd, &g_data[i]);
    }
    free(data);
}

bool bayer_get(int hd, char cons[4], int *bayer, int *n)
{
    entry_t *e;
    if (!g_entries) bayer_init();
    HASH_FIND_INT(g_entries, &hd, e);
    if (!e) {
        *bayer = 0;
        return false;
    }
    if (cons)
        memcpy(cons, CSTS[(e->cst - 1)], 4);
    *bayer = e->bayer;
    if (n) *n = e->bayer_n;
    return true;
}

bool bayer_iter(int i, int *hd, char cons[4], int *bayer, int *n,
                const char *greek[3])
{
    const entry_t *e;
    if (!g_data) bayer_init();
    e = &g_data[i];
    if (!e->hd) return false;
    *hd = e->hd;
    memcpy(cons, CSTS[(e->cst - 1)], 4);
    *bayer = e->bayer;
    *n = e->bayer_n;
    greek[0] = GREEK[e->bayer - 1][0];
    greek[1] = GREEK[e->bayer - 1][1];
    greek[2] = GREEK[e->bayer - 1][2];
    return true;
}

// Binary data of the form:
//
// hd       4 bytes
// cst      1 byte (1-indexed)
// bayer    1 byte (1-indexed)
// bayer_n  1 byte (for double stars)
// padding  1 byte
//
// Generated from The BSC5P database
// http://heasarc.gsfc.nasa.gov/W3Browse/star-catalog/bsc5p.html
// using tools/makebayer.py

static const unsigned char DATA[7131] __attribute__((aligned(4))) = {
    224,48,0,0, // uncompressed size
    120,218,45,218,7,152,149,197,213,192,241,185,239,221,5,105,203,178,
    52,233,176,82,150,165,8,139,2,75,93,64,58,172,212,21,11,137,13,21,
    21,53,42,138,37,42,130,45,246,134,21,225,179,96,23,133,196,26,49,81,
    44,24,91,98,239,93,99,20,140,189,69,253,126,231,122,159,71,158,191,
    103,102,78,157,51,111,187,123,64,46,165,204,191,59,253,43,207,82,218,
    138,249,166,185,244,95,156,84,154,210,137,198,106,243,249,244,92,22,
    227,89,122,63,184,93,74,199,228,83,26,227,223,55,193,118,41,37,107,
    51,227,39,6,43,82,250,18,27,53,73,105,64,163,148,230,250,119,134,127,
    89,171,148,46,13,185,60,165,45,152,55,127,32,157,1,108,190,136,147,
    248,124,41,216,52,165,63,154,203,55,78,233,5,255,95,222,44,165,10,
    255,38,53,203,165,93,112,174,128,39,22,152,165,221,177,220,154,7,10,
    114,62,85,53,71,54,62,106,30,235,179,212,189,133,121,190,250,98,198,
    239,107,65,177,245,43,195,146,148,78,193,114,126,71,183,68,255,191,
    43,78,162,123,78,144,252,90,140,211,123,23,243,252,255,128,115,229,
    209,221,88,35,113,15,197,73,226,156,135,229,230,246,138,241,182,185,
    116,84,121,172,207,210,42,107,51,254,27,169,73,185,117,157,131,98,
    24,132,147,249,63,22,51,242,211,21,161,151,165,103,113,128,92,134,
    181,102,151,238,164,54,252,241,123,36,150,183,201,165,138,182,168,
    230,173,131,109,178,52,186,109,232,229,211,20,204,196,61,187,109,172,
    207,210,57,33,243,183,71,187,152,47,73,139,49,47,207,27,219,163,188,
    62,194,201,124,85,110,79,198,106,156,172,46,131,112,146,127,195,59,
    144,219,231,82,109,145,119,99,57,251,143,224,36,121,247,233,24,227,
    89,58,188,227,111,49,158,133,115,237,217,165,216,136,223,171,48,19,
    195,83,65,235,223,139,117,234,240,93,65,47,159,6,119,98,207,62,173,
    199,201,98,123,56,40,231,85,157,145,222,157,65,235,239,42,242,162,
    46,244,213,233,208,174,252,216,239,239,176,220,186,206,221,204,243,
    223,178,59,63,242,234,215,195,184,220,234,176,17,251,123,132,172,206,
    173,43,173,83,207,223,99,38,191,251,113,162,122,44,220,129,93,245,
    220,128,147,229,55,180,39,138,255,34,156,68,239,111,88,46,190,125,
    123,211,227,103,3,230,197,249,16,78,182,199,11,250,24,55,191,22,43,
    217,123,51,100,243,231,84,153,23,111,155,190,98,17,103,95,204,91,255,
    20,54,50,223,164,218,188,126,169,199,188,186,31,130,3,90,231,210,117,
    253,204,179,183,123,127,148,247,101,88,110,126,35,206,99,255,115,108,
    108,35,183,21,152,165,38,3,204,203,167,15,78,150,255,188,1,97,39,75,
    71,99,99,117,95,143,147,212,227,147,160,56,78,27,104,220,62,253,31,
    86,242,243,104,200,250,109,11,14,160,240,229,142,252,202,179,253,32,
    50,127,211,131,250,99,21,246,20,255,53,56,217,248,154,34,239,193,121,
    226,251,12,39,177,247,13,102,226,251,182,192,44,189,61,152,158,58,
    76,172,225,199,190,45,15,210,91,60,132,158,248,190,218,137,172,222,
    253,118,230,183,44,151,206,197,158,246,97,213,80,243,234,255,113,208,
    254,140,30,38,14,7,245,112,108,108,95,47,193,74,249,60,91,24,207,82,
    205,112,250,234,249,175,90,250,252,28,56,194,58,245,88,139,3,216,249,
    2,203,229,221,113,164,117,250,96,57,246,147,247,5,56,192,250,151,98,
    188,44,75,205,70,177,75,158,128,61,217,191,123,244,111,103,247,192,
    49,100,231,240,142,160,122,247,24,107,92,94,27,177,103,147,92,234,
    81,23,243,89,26,132,141,141,255,9,7,200,227,202,24,111,146,165,91,
    177,159,60,31,195,70,226,249,44,168,110,183,142,179,222,186,245,88,
    41,190,95,177,78,126,219,143,183,222,254,79,195,70,242,159,131,3,172,
    63,7,27,203,243,170,152,183,254,190,152,183,47,143,5,163,127,39,176,
    163,143,246,222,197,60,189,125,177,142,223,101,216,83,225,207,192,
    198,177,143,187,252,118,109,254,58,198,237,195,224,137,104,223,198,
    21,152,165,197,216,88,222,203,67,214,23,107,39,134,221,44,221,80,144,
    243,233,94,236,39,207,151,176,78,63,127,56,201,184,62,107,57,153,94,
    171,44,205,42,48,159,150,226,0,235,46,194,74,113,124,138,117,252,239,
    54,197,188,124,22,20,121,27,54,178,31,155,176,114,187,92,122,168,192,
    44,149,76,101,87,156,123,99,101,171,92,218,128,253,196,187,25,27,233,
    163,215,99,158,254,86,172,147,111,147,105,177,46,75,149,193,214,249,
    116,36,214,217,239,235,10,227,249,116,236,116,250,234,116,53,214,177,
    115,123,80,29,159,192,1,234,222,102,6,217,254,63,17,180,207,211,103,
    138,207,126,85,215,243,195,223,221,216,56,226,217,149,61,92,53,75,
    28,113,191,156,21,254,74,82,155,217,230,213,191,43,78,23,224,176,217,
    49,159,165,201,5,57,75,171,11,243,89,90,135,117,236,117,159,195,47,
    255,167,224,108,251,122,244,92,212,183,235,177,103,187,92,26,52,207,
    58,251,48,105,94,200,89,90,81,96,62,125,49,159,61,113,246,104,224,
    87,254,119,98,157,250,63,16,114,235,210,84,186,187,117,246,229,2,172,
    19,255,57,123,32,251,15,96,79,242,166,144,237,203,63,177,146,188,120,
    207,148,122,199,189,124,47,178,253,169,199,217,246,235,126,236,201,
    255,147,49,222,186,81,106,178,144,95,117,155,180,48,228,198,105,37,
    246,84,223,250,223,179,167,30,127,195,1,236,52,219,59,230,183,75,247,
    98,157,248,102,237,131,242,250,59,86,154,63,122,95,246,237,247,198,
    125,99,93,147,84,191,31,187,226,184,8,103,139,171,122,127,235,205,
    47,198,233,214,159,18,84,167,102,139,204,203,179,230,0,220,62,151,
    158,57,144,126,203,92,250,26,251,209,111,116,16,61,251,219,125,177,
    121,156,191,56,230,179,180,27,246,83,239,27,98,124,251,44,45,59,152,
    61,242,149,216,141,253,79,113,54,254,23,43,219,148,164,111,15,254,
    237,94,90,115,136,117,226,255,22,103,187,190,53,57,148,172,79,6,227,
    236,146,92,186,181,192,44,189,136,149,242,235,186,132,172,31,170,11,
    204,210,136,96,73,62,45,11,186,174,62,136,211,213,117,115,200,234,
    124,225,97,200,254,197,65,125,191,166,192,44,45,57,92,205,228,179,
    229,8,57,227,54,156,173,206,171,255,16,241,229,210,95,113,150,58,61,
    143,179,229,81,119,164,113,242,33,65,207,5,199,97,55,227,125,142,50,
    95,161,143,10,204,210,132,163,81,95,108,68,151,179,84,181,20,237,231,
    39,56,86,189,243,199,208,87,183,178,99,233,235,175,11,176,153,125,
    189,4,103,197,117,25,123,211,251,43,142,242,32,210,122,89,208,253,
    166,192,146,244,121,208,126,156,118,220,111,207,22,239,227,88,251,
    246,65,145,165,199,199,186,210,52,182,192,92,90,135,109,141,63,24,
    178,125,90,115,2,153,94,251,19,99,190,81,90,135,189,197,247,6,182,
    85,167,93,254,136,226,249,1,43,245,193,128,147,228,67,255,94,156,165,
    174,199,156,108,30,63,198,177,174,119,99,79,17,191,235,90,125,129,
    89,58,22,7,179,83,177,60,230,245,5,86,178,191,13,187,177,91,122,42,
    59,228,65,88,25,253,183,130,61,121,55,93,73,143,159,58,28,101,31,142,
    8,217,248,234,160,58,109,69,143,125,169,215,105,230,233,159,136,179,
    244,195,33,167,27,103,231,113,28,165,238,115,206,72,169,157,115,184,
    59,118,211,71,143,99,91,246,210,153,236,88,183,99,208,62,166,179,196,
    39,158,35,177,173,243,55,247,79,244,229,117,120,208,243,216,57,65,
    253,122,51,206,230,239,127,56,86,28,61,207,182,94,223,174,56,59,214,
    101,169,205,57,198,249,249,28,7,91,183,240,92,126,249,105,125,158,
    117,250,249,52,28,165,191,78,47,114,109,80,159,126,131,237,212,105,
    220,249,244,226,185,28,71,73,240,231,32,253,95,138,188,250,2,212,183,
    171,139,188,166,200,53,69,222,85,96,150,54,6,213,111,187,11,145,221,
    170,160,231,221,57,56,91,158,31,97,183,120,158,190,136,223,22,185,
    180,23,142,146,247,145,65,253,112,108,97,60,75,135,93,76,182,254,240,
    34,87,224,44,117,184,14,93,46,211,43,151,200,75,125,127,184,84,220,
    198,159,92,101,158,223,116,153,113,245,111,138,237,236,91,59,28,76,
    255,40,28,165,110,221,47,55,47,239,61,131,234,127,0,118,83,143,75,
    112,172,184,54,226,44,231,245,168,43,204,219,215,205,56,216,124,238,
    74,182,204,239,135,99,35,14,28,37,144,121,87,25,87,159,227,112,148,
    184,78,197,89,228,235,177,155,252,247,187,154,29,250,79,224,96,251,
    250,95,108,199,238,119,49,206,222,142,171,145,191,225,216,78,29,150,
    133,108,31,7,94,67,230,231,160,53,72,239,3,28,37,159,31,112,150,60,
    86,175,13,255,158,187,174,101,215,250,14,215,145,229,243,187,235,173,
    119,222,238,9,26,31,123,67,200,89,234,178,78,60,206,73,29,142,226,
    255,47,55,154,19,215,87,216,139,253,110,55,241,203,238,126,216,141,
    157,119,112,16,255,15,221,28,122,158,63,111,101,199,252,8,156,37,207,
    35,111,119,205,81,215,77,216,43,226,194,182,250,245,241,59,140,203,
    235,53,108,199,254,187,56,72,29,110,191,147,63,118,123,222,229,217,
    129,254,71,216,139,188,104,3,251,226,93,141,131,232,93,83,228,154,
    34,191,193,62,234,217,243,207,177,46,75,223,99,31,207,143,205,239,
    97,47,174,251,247,132,156,165,255,221,19,254,243,41,119,47,187,226,
    174,196,62,250,107,210,189,49,158,165,69,5,89,31,222,23,204,167,161,
    247,123,230,138,251,207,3,49,95,146,182,252,149,158,56,171,55,133,
    92,154,246,193,94,234,116,110,65,110,148,90,63,68,79,220,71,252,61,
    228,198,105,225,35,228,168,23,246,82,231,206,155,99,124,187,52,14,
    199,90,119,58,78,211,135,211,30,85,47,117,122,239,209,152,111,146,
    246,120,140,158,245,15,6,21,226,215,199,209,254,215,60,129,242,89,
    18,212,199,139,182,164,52,21,143,251,7,89,92,173,158,230,199,249,56,
    247,25,178,62,217,138,189,226,125,233,217,176,35,255,160,188,223,126,
    158,63,246,103,190,72,86,255,135,176,151,56,46,124,89,61,201,195,95,
    99,215,117,178,182,192,44,77,127,221,58,118,95,125,195,58,117,110,
    255,150,152,197,115,37,246,178,254,67,236,67,191,253,187,214,147,127,
    255,190,113,113,109,195,62,250,230,87,28,47,190,61,63,68,251,213,239,
    35,52,62,30,123,185,222,237,21,178,184,38,126,194,174,184,63,193,241,
    238,167,207,254,39,152,165,138,79,173,83,175,214,69,62,250,105,140,
    231,211,193,159,145,157,211,11,182,202,135,253,187,183,145,157,247,
    173,159,155,143,247,163,47,228,131,123,125,105,156,221,179,190,66,
    251,49,224,107,20,199,134,111,80,30,99,191,19,119,92,119,112,26,253,
    23,190,71,251,250,241,15,236,24,191,240,71,235,92,71,251,164,92,106,
    233,58,185,1,91,120,222,168,203,5,179,116,83,150,75,189,140,255,36,
    230,105,214,143,44,53,222,60,151,174,110,20,204,210,35,206,212,52,
    121,54,117,13,28,100,254,30,108,225,126,117,177,103,131,5,234,124,
    87,129,89,250,63,108,65,103,109,145,107,10,100,223,205,121,106,60,
    23,232,105,237,153,154,120,246,109,193,223,9,65,251,113,186,107,241,
    84,107,175,44,48,75,127,193,22,254,189,234,95,75,122,219,10,114,150,
    62,47,114,186,107,146,240,210,22,172,146,239,229,206,228,212,198,113,
    175,53,174,47,30,195,169,252,44,238,96,189,231,139,107,130,198,247,
    239,136,214,173,12,122,174,216,210,153,190,115,121,73,151,96,150,6,
    118,205,165,254,246,255,215,160,122,183,247,208,209,196,250,124,119,
    235,197,113,11,54,145,127,187,30,200,254,75,232,152,165,42,47,203,
    253,173,187,1,167,242,243,36,182,96,103,215,29,172,19,231,151,216,
    66,63,125,85,228,241,30,190,251,179,247,20,46,176,110,185,166,232,
    175,190,151,123,24,105,206,206,173,14,97,147,120,143,199,22,158,119,
    94,8,170,213,3,125,131,89,106,84,109,189,56,14,171,142,121,251,131,
    77,34,238,144,157,183,222,253,130,89,218,31,171,248,123,33,100,113,
    180,232,31,235,243,169,19,86,241,215,114,32,234,167,222,216,194,117,
    226,223,59,146,249,187,77,211,245,119,94,222,196,169,244,166,13,142,
    241,44,93,133,45,156,147,207,130,244,58,214,24,143,231,47,108,161,
    223,158,198,38,198,135,13,145,151,125,157,182,19,89,158,223,99,11,
    117,94,50,148,93,245,121,120,104,200,89,122,115,152,51,26,223,211,
    134,219,103,245,108,51,2,197,187,4,169,164,255,140,164,111,253,143,
    110,58,53,236,182,26,77,142,235,36,46,32,159,230,34,36,149,244,48,
    214,136,119,166,151,144,254,222,87,62,194,161,252,174,28,103,220,250,
    203,177,138,157,103,113,168,190,124,174,200,215,99,62,190,157,57,108,
    11,244,115,217,248,208,207,210,236,241,49,159,165,227,177,185,120,
    174,157,100,28,171,39,35,191,123,99,141,190,107,54,197,124,92,247,
    176,134,254,249,138,53,148,159,87,177,134,255,198,51,248,117,190,87,
    96,75,92,89,228,87,49,238,60,94,50,147,190,186,111,198,254,250,244,
    87,172,137,247,222,250,92,218,213,254,47,240,48,92,99,254,38,92,96,
    253,162,57,236,235,143,51,176,198,185,127,110,126,232,101,233,190,
    6,178,241,93,119,163,23,223,129,208,86,164,235,176,185,131,242,198,
    2,235,212,161,219,238,33,103,233,239,123,154,23,247,131,88,101,252,
    123,108,169,126,235,246,162,31,239,229,11,205,59,215,231,22,152,165,
    107,126,151,43,124,139,172,219,135,29,188,25,39,216,175,47,113,104,
    124,115,221,143,158,254,219,11,135,226,145,33,155,255,243,254,244,
    245,217,191,15,160,231,220,14,61,40,152,79,23,98,115,249,29,112,8,
    191,241,157,245,80,20,127,237,18,121,198,55,207,195,204,187,22,45,
    197,254,242,191,180,32,103,233,164,35,196,33,206,91,142,228,199,126,
    84,29,157,75,142,122,186,31,171,248,219,178,20,201,251,28,203,175,
    235,203,137,88,203,238,147,216,223,181,98,196,178,96,150,198,29,111,
    157,253,250,249,4,250,244,86,156,136,228,9,127,164,39,254,125,130,
    226,106,122,50,198,119,19,172,210,39,247,20,249,82,80,92,39,157,66,
    207,250,131,150,91,231,122,113,13,182,178,47,235,79,53,174,174,47,
    96,141,56,127,191,130,172,174,103,7,229,253,217,74,212,71,117,167,
    153,119,126,30,60,61,151,186,91,127,195,25,244,157,223,117,5,102,105,
    216,153,226,229,239,147,179,200,230,59,252,137,158,184,134,99,141,
    62,106,126,182,186,240,215,3,119,83,151,122,220,158,191,97,231,208,
    211,159,119,159,71,182,207,159,225,110,236,236,114,62,138,123,22,86,
    169,207,98,28,206,238,189,23,160,186,127,135,53,113,93,187,208,58,
    220,254,98,118,240,88,172,18,247,115,151,176,167,158,219,86,241,43,
    159,229,151,89,199,255,219,151,231,82,103,247,132,179,174,8,102,233,
    49,108,46,191,214,87,154,23,199,79,216,153,222,235,87,201,35,190,23,
    97,39,251,244,233,213,200,206,63,87,27,215,39,63,174,177,46,238,191,
    107,249,139,235,34,118,226,119,159,255,19,159,117,107,80,251,166,143,
    177,179,188,126,186,150,253,248,94,125,157,117,228,187,177,179,117,
    3,175,183,222,249,221,177,192,92,122,101,157,113,247,186,87,11,204,
    210,194,27,115,169,53,187,143,96,43,251,246,246,77,244,249,59,240,
    102,243,198,255,88,228,123,56,28,187,220,98,158,159,129,183,230,146,
    227,150,38,96,39,156,142,221,229,185,255,109,214,197,247,27,108,213,
    58,158,23,197,165,190,239,220,65,166,223,111,61,25,251,23,57,61,40,
    223,173,119,242,99,191,186,221,69,159,189,214,27,200,120,252,70,125,
    27,207,71,127,33,179,255,76,208,62,189,17,116,158,95,47,48,75,101,
    119,179,163,111,107,177,42,190,59,133,44,143,206,247,136,83,190,93,
    10,204,210,238,247,177,111,124,218,253,230,173,187,22,91,185,23,159,
    252,215,96,150,254,139,195,245,213,135,15,218,111,246,174,220,100,
    93,220,199,208,229,52,253,163,200,118,127,67,241,189,141,195,213,99,
    220,35,214,169,223,63,176,191,188,254,188,153,61,113,183,121,204,188,
    27,223,126,143,7,179,116,39,86,197,247,239,34,55,160,75,83,58,233,
    73,121,202,111,206,211,100,227,127,125,150,190,248,166,188,152,75,
    77,213,241,43,108,165,95,127,194,42,254,74,94,34,199,123,42,54,213,
    255,235,95,37,211,235,248,26,217,252,249,175,147,157,215,77,65,215,
    211,195,223,12,102,233,79,56,66,62,85,111,241,195,238,92,116,27,77,
    251,191,131,246,233,43,236,31,191,113,188,75,182,238,58,116,171,79,
    191,251,128,30,187,167,127,72,79,156,217,199,252,120,78,201,23,152,
    165,253,112,132,120,55,224,110,242,254,23,214,198,251,64,204,211,255,
    228,223,104,252,63,65,251,55,244,147,92,26,66,190,2,119,35,151,253,
    7,229,209,3,91,169,251,105,91,205,43,244,69,5,102,105,243,54,122,241,
    125,248,115,235,228,117,233,23,198,213,251,174,47,141,235,159,138,
    175,80,221,63,192,86,226,188,241,107,178,249,221,191,177,206,252,161,
    223,26,215,7,75,10,204,210,169,223,229,82,251,248,78,247,189,121,28,
    138,110,181,105,21,54,117,127,190,172,192,44,189,21,84,215,183,139,
    124,235,7,254,213,253,207,63,145,157,231,205,65,121,63,90,228,175,
    63,231,82,169,249,61,126,145,63,59,223,225,144,248,141,233,87,227,
    252,119,40,48,75,95,162,215,140,52,47,101,169,169,235,209,29,88,203,
    223,245,230,218,71,191,97,105,188,247,100,198,227,187,180,123,207,
    68,251,84,159,143,241,44,189,134,67,200,7,185,23,180,178,31,167,59,
    227,165,234,221,215,187,91,251,232,15,215,158,166,244,22,4,173,219,
    27,75,213,227,120,28,98,127,79,40,242,25,61,217,84,189,207,179,168,
    84,127,77,104,102,188,89,124,171,67,27,55,167,192,44,45,14,138,247,
    64,190,75,245,243,176,150,193,44,77,17,179,215,169,116,66,80,254,207,
    7,227,186,137,243,237,71,189,230,106,170,175,170,61,51,53,181,110,
    40,14,81,159,49,88,106,221,193,122,114,136,241,53,65,15,102,107,11,
    204,210,203,88,170,110,111,163,215,188,244,81,140,123,230,29,238,222,
    57,164,52,126,123,201,82,71,243,175,132,236,223,135,238,101,67,244,
    207,94,156,151,198,243,53,118,84,135,75,138,92,234,230,86,171,15,110,
    197,17,242,90,143,77,93,63,238,44,48,75,207,224,68,126,126,197,122,
    125,87,222,145,29,215,203,110,216,93,159,254,5,253,151,174,232,20,
    227,249,244,133,139,83,83,241,126,211,57,228,146,212,173,75,232,101,
    169,26,231,243,127,90,151,152,207,210,195,56,132,221,159,177,163,125,
    29,212,53,252,103,233,10,44,53,126,3,214,203,127,93,145,191,160,71,
    242,52,172,155,121,249,140,239,22,118,243,233,88,28,98,63,47,196,142,
    234,247,106,97,188,52,45,234,110,157,115,156,235,97,94,67,31,135,77,
    227,124,224,68,254,142,170,180,94,30,71,23,232,57,19,75,245,203,191,
    43,99,125,150,126,170,12,59,141,210,201,59,96,124,175,14,218,175,39,
    130,173,27,167,229,61,173,143,58,98,71,241,254,136,245,250,165,139,
    151,46,143,102,233,54,236,168,143,238,11,185,245,118,233,31,189,81,
    157,158,10,234,163,21,125,80,30,55,120,40,169,231,119,75,80,252,95,
    227,16,126,142,232,75,150,215,69,216,81,254,143,227,124,235,222,143,
    113,117,156,80,77,102,239,38,172,183,127,205,250,89,23,191,29,99,105,
    60,7,224,68,125,121,76,200,242,237,229,38,93,205,110,175,1,236,59,
    63,189,11,204,210,76,156,73,191,249,64,118,228,49,10,59,210,95,25,
    178,122,221,130,51,227,186,183,163,113,126,203,112,72,188,127,224,
    204,120,142,27,148,165,30,250,254,254,160,241,11,7,91,199,222,67,65,
    117,107,231,97,161,158,254,163,56,211,121,126,172,200,137,14,105,15,
    250,245,88,173,14,187,227,76,239,152,123,20,152,165,171,177,187,184,
    215,225,16,108,216,201,184,7,185,189,119,10,127,89,122,167,32,59,127,
    67,233,203,251,160,34,239,28,198,175,125,255,39,246,144,255,75,56,
    194,115,226,103,5,102,105,43,86,203,235,155,152,183,126,161,155,234,
    204,248,94,84,228,153,56,95,92,215,134,108,63,55,212,90,199,255,39,
    56,83,253,86,140,200,210,72,243,111,99,15,207,191,247,141,228,207,
    245,227,254,34,63,29,101,60,190,23,6,93,139,106,70,27,151,223,129,
    56,50,158,155,199,176,227,124,190,25,172,136,247,18,241,136,247,7,
    172,39,239,89,71,142,239,255,56,95,252,175,227,8,242,155,216,81,221,
    190,198,145,226,234,53,142,236,156,13,43,208,251,46,206,87,239,55,
    113,164,250,191,85,100,126,188,56,226,247,8,156,104,127,46,192,248,
    237,251,21,28,41,159,243,61,164,143,20,223,117,216,221,252,47,232,
    21,35,213,184,168,142,84,167,101,168,197,210,113,33,203,235,74,156,
    175,30,215,134,204,206,192,73,236,235,219,25,147,197,105,221,38,156,
    41,159,147,166,208,147,207,141,56,191,113,46,253,140,35,229,115,249,
    12,227,88,54,211,56,63,47,98,53,191,255,116,56,74,240,148,89,244,197,
    59,103,54,186,14,221,85,96,150,110,155,131,206,107,217,220,96,150,
    122,207,99,47,238,167,88,18,223,235,29,134,146,210,92,58,190,193,184,
    125,253,108,183,144,179,116,217,2,121,179,247,28,86,99,135,133,242,
    20,111,199,34,191,193,145,252,78,248,29,187,120,212,222,198,227,189,
    121,31,235,221,151,14,47,144,173,125,209,190,119,197,17,252,46,13,
    90,119,66,140,235,239,7,113,164,125,221,84,228,187,251,163,253,74,
    139,236,171,190,58,18,75,226,58,130,35,213,111,212,1,100,245,26,19,
    180,191,173,14,196,120,206,57,136,61,251,54,183,200,29,23,91,175,142,
    231,97,73,124,63,61,88,156,246,235,117,212,214,105,227,161,214,217,
    151,103,112,38,62,189,68,252,236,126,129,37,244,74,14,71,241,245,199,
    153,252,238,124,120,252,141,72,46,77,41,48,75,83,209,43,116,90,121,
    132,56,227,123,16,142,140,223,147,254,64,143,189,241,71,209,19,215,
    34,172,151,215,9,56,90,157,54,98,137,117,63,98,181,188,174,60,154,
    61,253,48,101,41,125,254,31,58,134,158,235,66,233,50,243,209,31,56,
    82,158,203,150,197,120,150,158,199,238,174,203,47,20,89,114,50,242,
    127,63,86,199,119,187,83,172,87,183,43,176,58,254,46,5,187,199,119,
    148,229,100,227,215,97,189,250,47,57,213,253,75,31,151,174,8,59,174,
    27,56,82,255,173,46,242,240,149,89,154,225,156,174,197,18,118,239,
    196,209,234,209,242,180,24,207,210,9,167,147,197,127,210,153,236,202,
    111,61,206,209,39,163,207,210,55,234,221,225,28,235,196,223,255,60,
    126,156,135,135,113,134,252,47,190,144,158,184,26,93,68,142,191,183,
    184,88,60,198,39,227,206,234,53,230,18,227,236,108,198,237,226,61,
    228,82,118,227,123,239,42,243,236,245,185,140,28,207,179,56,67,157,
    127,196,209,241,183,59,151,203,131,159,155,131,242,26,126,5,125,121,
    191,139,115,108,120,167,43,131,222,79,174,202,82,7,249,28,29,116,223,
    62,170,192,44,109,92,109,125,124,23,188,134,189,248,110,178,150,63,
    242,83,215,26,23,231,39,216,129,191,127,94,71,22,223,17,215,91,199,
    206,215,56,131,223,15,110,48,31,191,135,173,163,231,60,181,44,178,
    79,129,89,170,42,114,86,208,121,156,93,96,150,46,198,14,206,249,102,
    244,8,149,122,221,24,114,150,78,196,209,252,189,124,147,184,213,229,
    135,155,173,215,127,77,111,9,102,105,96,208,250,3,112,180,249,147,
    110,21,135,249,134,219,130,89,218,45,24,191,199,98,247,56,87,56,90,
    159,141,191,157,158,122,172,194,57,236,127,80,144,179,212,238,14,251,
    160,0,237,11,204,210,100,156,225,126,119,6,110,103,127,214,20,228,
    44,61,138,29,228,223,233,174,44,229,112,103,220,89,157,207,193,238,
    206,215,47,216,65,253,186,110,48,30,191,83,225,12,114,235,63,243,231,
    92,238,142,51,196,51,254,47,214,209,155,80,228,52,172,21,239,78,119,
    155,215,127,163,48,71,111,175,96,212,29,59,216,135,153,247,162,125,
    185,57,24,223,139,239,227,71,31,238,133,51,244,231,184,7,228,73,127,
    219,38,178,235,253,35,15,153,143,223,249,254,46,62,250,83,31,38,219,
    183,149,152,115,95,236,188,57,75,93,212,111,16,206,137,239,63,155,
    99,220,243,27,206,112,95,157,250,40,182,114,61,11,202,239,216,160,
    251,236,185,133,241,44,45,121,12,197,243,14,118,231,119,159,39,200,
    237,114,233,130,2,243,169,231,150,172,240,55,96,71,96,142,255,33,79,
    162,126,253,26,231,232,151,111,49,254,70,237,252,167,178,180,11,59,
    219,176,139,243,127,208,51,200,255,193,5,122,239,15,58,255,99,159,
    165,39,223,134,231,216,137,239,57,255,66,241,239,27,52,62,252,121,
    254,212,241,96,220,193,254,12,121,129,94,188,119,97,206,248,233,47,
    146,245,235,53,184,3,127,7,188,132,234,184,231,203,230,157,155,245,
    175,176,31,127,175,130,57,245,27,247,42,202,251,198,160,125,202,94,
    163,47,143,205,184,67,60,167,191,110,92,126,239,225,14,246,235,199,
    55,200,236,246,123,19,237,243,37,216,133,221,14,111,145,213,247,95,
    56,67,156,47,99,247,248,157,246,109,251,133,171,48,231,186,240,10,
    206,208,167,123,191,67,142,231,52,28,237,126,54,252,93,178,184,95,
    126,55,228,44,181,126,143,93,246,150,4,197,185,30,103,232,223,77,5,
    102,233,140,247,197,19,215,189,143,173,143,191,159,195,156,253,121,
    17,231,240,115,193,191,205,211,127,227,19,113,232,203,57,159,145,213,
    233,41,204,97,147,173,89,42,211,255,199,20,152,165,155,63,55,30,239,
    125,95,144,37,120,54,86,168,251,198,130,156,165,147,190,68,245,189,
    252,43,84,191,151,131,226,223,225,107,241,184,206,180,252,38,152,165,
    149,56,90,125,6,124,43,110,241,205,248,222,58,245,59,249,7,100,191,
    237,143,168,110,237,138,252,224,39,235,244,233,23,56,140,221,126,255,
    139,191,229,203,165,115,177,66,156,155,254,23,243,89,122,15,187,170,
    227,129,63,163,188,142,195,209,236,61,143,181,234,247,126,129,158,
    119,126,113,237,224,127,30,118,141,247,251,95,195,94,150,110,12,218,
    231,219,208,43,67,250,91,208,62,13,77,249,84,166,110,75,177,43,127,
    23,7,229,125,115,140,183,137,191,21,36,179,247,0,142,142,235,87,150,
    79,3,249,127,23,187,24,159,159,55,175,254,121,55,235,209,21,241,219,
    49,189,246,241,219,122,62,13,107,18,127,51,97,222,125,102,70,129,89,
    90,132,93,228,241,7,108,192,181,56,48,126,111,247,142,84,209,56,126,
    51,52,239,188,44,192,97,236,95,143,3,217,89,222,152,204,255,121,88,
    166,127,250,187,248,123,4,79,187,97,131,115,114,104,179,124,170,181,
    254,105,236,162,142,13,205,243,105,28,251,45,91,208,179,15,239,7,249,
    185,189,140,158,245,67,90,178,19,207,117,56,12,203,202,81,30,181,88,
    166,14,91,176,139,62,122,167,21,89,127,95,85,145,47,252,109,232,13,
    216,192,254,114,239,88,227,226,123,87,91,235,216,221,193,181,96,28,
    253,89,88,22,127,239,136,93,212,245,217,24,23,215,10,47,165,195,196,
    243,31,28,200,254,190,219,155,143,126,10,198,117,21,135,109,23,239,
    145,234,96,253,9,88,22,223,205,59,209,151,231,184,78,49,159,165,5,
    56,58,254,62,5,7,198,247,49,77,86,198,255,231,221,242,169,175,120,
    39,120,24,240,168,151,222,68,175,204,233,132,74,246,245,201,58,172,
    141,191,191,115,104,202,196,241,56,54,88,191,93,47,50,59,107,177,86,
    221,151,246,38,219,255,89,125,232,149,187,143,224,20,113,220,143,99,
    34,254,24,215,111,159,22,232,90,91,101,220,250,5,88,161,174,79,97,
    89,220,183,131,113,61,236,75,95,190,103,225,64,245,187,26,43,196,241,
    94,223,176,159,165,134,254,214,209,219,15,13,165,182,3,172,115,94,
    198,75,110,32,253,35,130,206,215,49,131,216,177,79,79,225,64,113,228,
    106,196,207,222,113,53,49,159,165,237,189,52,247,141,191,253,197,49,
    177,255,216,16,223,201,176,66,158,87,133,44,143,238,59,89,103,252,
    16,28,19,125,133,83,248,191,40,104,31,78,114,179,155,98,223,54,224,
    24,249,127,25,20,127,167,161,33,59,99,154,169,34,254,30,51,24,223,
    157,177,86,191,127,139,83,226,247,209,225,100,241,93,128,125,213,41,
    171,13,102,233,76,172,48,254,16,54,176,247,10,206,21,199,92,47,49,
    181,225,215,67,96,131,125,234,58,74,62,226,95,136,13,234,113,44,246,
    21,199,245,5,102,169,124,76,62,237,24,215,115,108,80,167,69,56,87,
    157,94,29,155,47,252,109,115,171,58,116,255,31,137,13,158,67,70,21,
    168,111,10,227,89,26,164,105,167,136,127,4,86,208,155,130,13,241,119,
    157,49,206,238,85,49,238,126,248,48,206,101,111,232,120,254,196,217,
    123,66,140,123,55,193,6,241,110,11,218,143,137,187,176,171,97,79,47,
    48,75,7,77,52,174,31,222,152,196,158,124,199,76,150,31,187,103,227,
    24,126,111,66,175,234,233,95,33,219,191,186,41,100,121,159,135,99,
    216,155,58,149,28,191,71,78,163,103,223,246,198,49,248,135,160,250,
    190,132,13,173,115,105,43,246,101,255,157,233,33,103,105,194,12,243,
    246,241,122,172,176,175,229,51,249,87,191,113,56,70,126,227,177,129,
    254,81,216,151,254,82,28,200,239,16,47,247,13,241,61,9,199,144,31,
    192,41,228,239,234,99,93,62,157,184,43,89,254,109,102,147,197,63,125,
    78,62,101,226,254,27,78,81,255,29,230,230,147,87,187,52,16,251,202,
    187,6,189,194,166,7,131,244,142,154,103,220,126,159,26,84,183,74,155,
    90,65,111,32,246,21,231,174,11,248,119,189,168,221,29,61,239,111,68,
    170,233,222,144,93,119,222,198,201,113,159,219,35,230,179,52,0,251,
    90,191,20,107,245,233,153,152,143,245,133,249,124,106,190,39,251,226,
    91,141,99,204,127,177,16,249,173,253,29,61,227,87,224,228,248,253,
    241,247,40,174,9,251,208,23,111,221,190,249,52,73,60,87,237,39,63,
    117,122,51,24,241,237,111,60,206,47,54,184,79,173,197,201,198,55,99,
    69,60,127,99,94,221,63,198,140,221,179,23,153,167,255,232,162,88,239,
    57,243,64,227,206,199,205,56,201,62,60,136,229,226,90,118,16,189,168,
    251,98,121,184,63,108,127,176,56,229,85,118,136,121,251,112,240,161,
    49,238,220,224,220,248,174,130,99,216,169,94,194,142,250,141,194,114,
    246,22,227,100,247,131,59,112,174,126,24,127,152,121,246,143,195,90,
    113,191,133,121,251,246,255,22,65,235,3,
};
