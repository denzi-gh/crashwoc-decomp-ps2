/*
 * Unit: sdk/newlib/libc/locale/locale
 *
 * Functions:
 *   0x00276f78 _setlocale_r
 *   0x00277000 _localeconv_r
 *   0x00277010 setlocale
 *   0x00277040 localeconv
 */

struct _reent {
    int _errno;
    void *_stdin;
    void *_stdout;
    void *_stderr;
    int _inc;
    char _emergency[25];
    int _current_category;
    const char *_current_locale;
};

struct lconv {
    char *decimal_point;
    char *thousands_sep;
    char *grouping;
    char *int_curr_symbol;
    char *currency_symbol;
    char *mon_decimal_point;
    char *mon_thousands_sep;
    char *mon_grouping;
    char *positive_sign;
    char *negative_sign;
    char int_frac_digits;
    char frac_digits;
    char p_cs_precedes;
    char p_sep_by_space;
    char n_cs_precedes;
    char n_sep_by_space;
    char p_sign_posn;
    char n_sign_posn;
};

extern struct _reent *_impure_ptr[];
#define _REENT (_impure_ptr[0])

extern struct lconv D_0062C408;
#define lconv D_0062C408

extern int strcmp(const char *s1, const char *s2);


char *_setlocale_r(struct _reent *p, int category, const char *locale)
{
    if (locale) {
        if (strcmp(locale, "C") && strcmp(locale, "")) {
            return 0;
        }
        p->_current_category = category;
        p->_current_locale = locale;
    }
    return "C";
}


struct lconv *_localeconv_r(struct _reent *data)
{
    return &lconv;
}


char *setlocale(int category, const char *locale)
{
    return _setlocale_r(_REENT, category, locale);
}


struct lconv *localeconv(void)
{
    return _localeconv_r(_REENT);
}
