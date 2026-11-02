/*
 * Unit: sdk/newlib/libm/common/s_lib_ver
 *
 * Data only (no functions): the libm error-handling mode, built with
 * _XOPEN_MODE.
 */

enum __fdlibm_version {
    __fdlibm_ieee = -1,
    __fdlibm_svid,
    __fdlibm_xopen,
    __fdlibm_posix
};

const enum __fdlibm_version __fdlib_version = __fdlibm_xopen;
