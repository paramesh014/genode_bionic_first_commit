BIONIC_PORT_DIR := $(call select_from_ports,bionic)
BIONIC_DIR      := $(BIONIC_PORT_DIR)/src/lib/bionic
LOCAL_PATH := $(BIONIC_DIR)/libm


libm_common_src_files := \
    $(LOCAL_PATH)/digittoint.c  \
# TODO: this comes from from upstream's libc, not libm, but it's an
# implementation detail that should have hidden visibility, so it needs
# to be in whatever library the math code is in.
libm_common_src_files += \
    $(LOCAL_PATH)/significandl.c \
    $(LOCAL_PATH)/sincos.c \
    
libm_common_src_files += \
    $(LOCAL_PATH)/fake_long_double.c \
    $(LOCAL_PATH)/signbit.c \
    
libm_common_src_files += \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/bsdsrc/b_exp.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/bsdsrc/b_log.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/bsdsrc/b_tgamma.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_acos.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_acosf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_acosh.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_acoshf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_asin.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_asinf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_atan2.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_atan2f.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_atanh.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_atanhf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_cosh.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_coshf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_expf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_fmod.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_fmodf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_gamma.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_gammaf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_gammaf_r.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_gamma_r.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_hypot.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_hypotf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_j0.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_j0f.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_j1.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_j1f.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_jn.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_jnf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_lgamma.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_lgammaf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_lgammaf_r.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_lgamma_r.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_log10.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_log10f.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_log2.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_log2f.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_log.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_logf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_pow.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_powf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_remainder.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_remainderf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_rem_pio2.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_rem_pio2f.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_scalb.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_scalbf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_sinh.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_sinhf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_sqrt.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_sqrtf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/imprecise.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/k_cos.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/k_cosf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/k_exp.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/k_expf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/k_rem_pio2.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/k_sin.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/k_sinf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/k_tan.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/k_tanf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_asinh.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_asinhf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_atan.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_atanf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_carg.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_cargf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_cbrt.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_cbrtf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_ccosh.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_ccoshf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_ceil.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_ceilf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_cexp.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_cexpf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_cimag.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_cimagf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_conj.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_conjf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_copysign.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_copysignf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_cos.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_cosf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_cproj.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_cprojf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_creal.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_crealf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_csinh.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_csinhf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_csqrt.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_csqrtf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_ctanh.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_ctanhf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_erf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_erff.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_exp2.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_exp2f.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_expm1.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_expm1f.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_fabs.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_fabsf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_fdim.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_finite.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_finitef.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_floor.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_floorf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_fma.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_fmaf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_fmax.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_fmaxf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_fmin.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_fminf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_frexp.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_frexpf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_ilogb.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_ilogbf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_llrint.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_llrintf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_llround.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_llroundf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_log1p.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_log1pf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_logb.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_logbf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_lrint.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_lrintf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_lround.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_lroundf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_modf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_modff.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_nan.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_nearbyint.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_nextafter.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_nextafterf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_remquo.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_remquof.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_rint.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_rintf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_round.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_roundf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_scalbln.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_scalbn.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_scalbnf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_signgam.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_significand.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_significandf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_sin.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_sinf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_tan.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_tanf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_tanh.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_tanhf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_tgammaf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_trunc.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_truncf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/w_cabs.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/w_cabsf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/w_drem.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/w_dremf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_exp.c \

libm_ld_src_files := \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_acosl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_acoshl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_asinl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_atan2l.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_atanhl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_fmodl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_hypotl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_remainderl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/e_sqrtl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_asinhl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_atanl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_cbrtl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_ceill.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_copysignl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_cosl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_fabsl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_floorl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_fmal.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_fmaxl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_fminl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_modfl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_frexpl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_ilogbl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_llrintl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_llroundl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_logbl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_lrintl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_lroundl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_nextafterl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_nexttoward.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_nexttowardf.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_remquol.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_rintl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_roundl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_scalbnl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_sinl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_tanl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/s_truncl.c \

libm_ld_src_files += \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/ld128/invtrig.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/ld128/k_cosl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/ld128/k_sinl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/ld128/k_tanl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/ld128/s_exp2l.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/ld128/s_expl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/ld128/s_logl.c \
    $(LOCAL_PATH)/upstream-freebsd/lib/msun/ld128/s_nanl.c \
    
# TODO: re-enable i387/e_sqrtf.S for x86, and maybe others.

libm_common_cflags := \
    -DFLT_EVAL_METHOD=0 \
    -std=c99 \
    -include $(LOCAL_PATH)/freebsd-compat.h \
    -Wno-missing-braces \
    -Wno-parentheses \
    -Wno-sign-compare \
    -Wno-uninitialized \
    -Wno-unknown-pragmas \
    -fvisibility=hidden \

# Workaround the GCC "(long)fn -> lfn" optimization bug which will result in
# self recursions for lrint, lrintf, and lrintl.
# BUG: 14225968
libm_common_cflags += -fno-builtin-rint -fno-builtin-rintf -fno-builtin-rintl

libm_common_includes := $(LOCAL_PATH)/upstream-freebsd/lib/msun/src/

libm_ld_includes := $(LOCAL_PATH)/upstream-freebsd/lib/msun/ld128/

