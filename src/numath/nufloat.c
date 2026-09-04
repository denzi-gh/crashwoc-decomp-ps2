/*
 * Unit: numath/nufloat
 *
 * Functions:
 *   0x0010b348 NuFabs
 *   0x0010b368 NuFnabs
 *   0x0010b388 NuFneg
 *   0x0010b3a8 NuFsign
 *   0x0010b3d0 NuFres
 *   0x0010b418 NuEquiv
 *   0x0010b458 NuEquivTollerance
 */

double pow(double x, double y);
extern float fetol; //D_00645750 //0.0099999998f

float NuFabs(float f) {
    int i = *(int *)&f;
    i &= 0x7fffffff;
    return *(float *)&i;
}

float NuFnabs(float f) {
    int temp = *(int *)&f;
    temp |= 0x80000000;
    return *(float *)&temp;
}

float NuFneg(float v) {
    int temp = *(int *)&v;
    temp ^= 0x80000000;
    return *(float *)&temp;
}

float NuFsign(float f)
{
    int temp = *(int *)&f;

    if (temp >= 0) {
        return 1.0f;
    }
    return -1.0f;
}

float NuFres(float f) {
    uint temp = *(uint *)&f;

    return (float)pow(
        2.0,
        (double)(int)(((temp >> 0x17) & 0xff) - 0x7f)
    );
}

int NuEquiv(float f1, float f2)
{
    int temp;
    int rv;
    float result;

    result = f1 - f2;
    temp = *(int *)&result;
    temp &= 0x7fffffff;
    diff = *(float *)&temp;

    rv = (result < fetol) ? 1 : 0;
    return rv;
}

void NuEquivTollerance(float f) {
  fetol = f;
}