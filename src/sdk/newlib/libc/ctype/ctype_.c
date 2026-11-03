/*
 * Unit: sdk/newlib/libc/ctype/ctype_
 *
 * Data only (no functions): the <ctype.h> class table, indexed by c + 1.
 */

#define _U 01
#define _L 02
#define _N 04
#define _S 010
#define _P 020
#define _C 040
#define _X 0100
#define _B 0200

const char _ctype_[1 + 256] = {
    0,
    _C,     _C,     _C,     _C,     _C,     _C,     _C,     _C,
    _C,     _C|_S,  _C|_S,  _C|_S,  _C|_S,  _C|_S,  _C,     _C,
    _C,     _C,     _C,     _C,     _C,     _C,     _C,     _C,
    _C,     _C,     _C,     _C,     _C,     _C,     _C,     _C,
    _S|_B,  _P,     _P,     _P,     _P,     _P,     _P,     _P,
    _P,     _P,     _P,     _P,     _P,     _P,     _P,     _P,
    _N,     _N,     _N,     _N,     _N,     _N,     _N,     _N,
    _N,     _N,     _P,     _P,     _P,     _P,     _P,     _P,
    _P,     _U|_X,  _U|_X,  _U|_X,  _U|_X,  _U|_X,  _U|_X,  _U,
    _U,     _U,     _U,     _U,     _U,     _U,     _U,     _U,
    _U,     _U,     _U,     _U,     _U,     _U,     _U,     _U,
    _U,     _U,     _U,     _P,     _P,     _P,     _P,     _P,
    _P,     _L|_X,  _L|_X,  _L|_X,  _L|_X,  _L|_X,  _L|_X,  _L,
    _L,     _L,     _L,     _L,     _L,     _L,     _L,     _L,
    _L,     _L,     _L,     _L,     _L,     _L,     _L,     _L,
    _L,     _L,     _L,     _P,     _P,     _P,     _P,     _C
};
