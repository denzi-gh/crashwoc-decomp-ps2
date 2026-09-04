/*
 * Unit: sdk/gcc/dp-bit
 *
 * Functions:
 *   0x0026ea50 __pack_d
 *   0x0026eb80 __unpack_d
 *   0x0026ec20 _fpadd_parts
 *   0x0026ee60 dpadd
 *   0x0026eeb8 dpsub
 *   0x0026ef20 dpmul
 *   0x0026f1c8 dpdiv
 *   0x0026f330 __fpcmp_parts_d
 *   0x0026f448 dpcmp
 *   0x0026f498 litodp
 *   0x0026f550 dptoli
 *   0x0026f5e8 dptoul
 *   0x0026f688 __negdf2
 *   0x0026f6c0 __make_dp
 *   0x0026f6f0 dptofp
 */

typedef float DFtype __attribute__ ((mode (DF)));
typedef DFtype FLO_type;

typedef union {
    ulong fraction;
} FLO_union_type__bits;

typedef union {
    long value;
    ulong value_raw;
    uint words[2];
    FLO_union_type__bits bits;
} FLO_union_type;

typedef enum {
    CLASS_SNAN,
    CLASS_QNAN,
    CLASS_ZERO,
    CLASS_NUMBER,
    CLASS_INFINITY
} fp_class_type;

typedef union {
    ulong ll;
    uint l[2];
} fp_number_type__fraction;

typedef struct {            /* size: 0x18 */
    fp_class_type class;    /* 0x00 */
    uint sign;                   /* 0x04 */
    int normal_exp;              /* 0x08 */
    uint pad;                    /* 0x0c */
    fp_number_type__fraction fraction;        /* 0x10 */
} fp_number_type;


extern FLO_type __pack_d (fp_number_type * src);                                    /* extern */
extern void __unpack_d ( FLO_type * src,  fp_number_type * dst);                            /* extern */
extern  fp_number_type * _fpadd_parts ( fp_number_type * a,  fp_number_type * b,  fp_number_type * tmp);                     /* extern */

 FLO_type dpadd( FLO_type arg_a, FLO_type arg_b) {
   fp_number_type *src;
   FLO_union_type FVar1;
   fp_number_type fStack_90;
   fp_number_type fStack_70;
   fp_number_type fStack_50;
   FLO_type local_30;
   FLO_type local_28;
  
  local_30 = arg_a;
  local_28 = arg_b;
  __unpack_d(&local_30,&fStack_90);
  __unpack_d(&local_28,&fStack_70);
  src = _fpadd_parts(&fStack_90,&fStack_70,&fStack_50);
  return __pack_d(src);
}

FLO_type litodp(int arg_a)
{
    FLO_union_type packed;
    fp_number_type in;
    
    in.sign = arg_a < 0;
    in.class = CLASS_NUMBER;
    
    if (arg_a == 0) {
        in.class = CLASS_ZERO;
    } else {
        in.normal_exp = 60;
        if (in.sign != 0) {
            if (arg_a == 0x80000000) {
                return -2147483648.0;
            }
            in.fraction.ll = -arg_a;
        } else {
            in.fraction.ll = arg_a;
        }
        while (in.fraction.ll < 0x1000000000000000) {
            in.fraction.ll <<= 1;
            in.normal_exp--;
        }
    }
    return __pack_d(&in);
}