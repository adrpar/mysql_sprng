#include <limits.h>

#ifndef _lcg_h
#define _lcg_h

#if LONG_MAX > 2147483647L	/* 32 bit integer */
#if LONG_MAX > 35184372088831L	/* 46 bit integer */
#if LONG_MAX >= 9223372036854775807L /* 64 bit integer */

#define LONG_SPRNG
#define LONG64 long
#define store_long64 store_long
#define store_long64array store_longarray
#define load_long64 load_long
#define load_long64array load_longarray

#define INIT_SEED 0x2bc68cfe166dL
#define MSB_SET 0x3ff0000000000000L
#define LSB48 0xffffffffffffL
#define AN1 0xdadf0ac00001L
#define AN2 0xfefd7a400001L
#define AN3 0x6417b5c00001L
#define AN4 0xcf9f72c00001L
#define AN5 0xbdf07b400001L
#define AN6 0xf33747c00001L
#define AN7 0xcbe632c00001L
#define PMULT1 0xa42c22700000L
#define PMULT2 0xfa858cb00000L
#define PMULT3 0xd0c4ef00000L
#define PMULT4 0xc3cc8e300000L
#define PMULT5 0x11bdbe700000L
#define PMULT6 0xb0f0e9f00000L
#define PMULT7 0x6407de700000L
#define MULT1 0x2875a2e7b175L	/* 44485709377909  */
#define MULT2 0x5deece66dL	/* 1575931494      */
#define MULT3 0x3eac44605265L	/* 68909602460261  */
#define MULT4 0x275b38eb4bbdL	/* 4327250451645   */
#define MULT5 0x1ee1429cc9f5L	/* 33952834046453  */
#define MULT6 0x739a9cb08605L	/* 127107890972165 */
#define MULT7 0x3228d7cc25f5L	/* 55151000561141  */
#endif
#endif
#endif

#if !defined(LONG_SPRNG) &&  defined(_LONG_LONG)
#define LONG64 long long
#define store_long64 store_longlong
#define store_long64array store_longlongarray
#define load_long64 load_longlong
#define load_long64array load_longlongarray

#define INIT_SEED 0x2bc68cfe166dLL
#define MSB_SET 0x3ff0000000000000LL
#define LSB48 0xffffffffffffLL
#define AN1 0xdadf0ac00001LL
#define AN2 0xfefd7a400001LL
#define AN3 0x6417b5c00001LL
#define AN4 0xcf9f72c00001LL
#define AN5 0xbdf07b400001LL
#define AN6 0xf33747c00001LL
#define AN7 0xcbe632c00001LL
#define PMULT1 0xa42c22700000LL
#define PMULT2 0xfa858cb00000LL
#define PMULT3 0xd0c4ef00000LL
#define PMULT4 0x11bdbe700000LL
#define PMULT5 0xc3cc8e300000LL
#define PMULT6 0xb0f0e9f00000LL
#define PMULT7 0x6407de700000LL
#define MULT1 0x2875a2e7b175LL
#define MULT2 0x5deece66dLL
#define MULT3 0x3eac44605265LL
#define MULT4 0x1ee1429cc9f5LL
#define MULT5 0x275b38eb4bbdLL
#define MULT6 0x739a9cb08605LL
#define MULT7 0x3228d7cc25f5LL
#endif

extern "C" {
class LCG : public Sprng
{
 public:

  LCG();
  int init_rng(int, int, int, int);
  ~LCG();
  LCG (const LCG &);
  LCG & operator= (const LCG &);

  int get_rn_int ();
  float get_rn_flt ();
  double get_rn_dbl ();
  int spawn_rng (int nspawned, Sprng ***newgens);
  int get_seed_rng ();
  int free_rng ();
  int pack_rng (char **buffer);
  int unpack_rng (char *packed);
  int print_rng ();

 private:

  int rng_type;
#ifdef LONG64
  unsigned LONG64 seed;
  int init_seed;
  int prime;
  int prime_position;
  int prime_next;
  char *gentype;
  int parameter;
  unsigned LONG64 multiplier;

  inline void multiply();

#else
  int seed[2];
  int init_seed;
  int prime;
  int prime_position;
  int prime_next;
  char *gentype;
  int parameter;
  int *multiplier;

  inline void multiply(int *, int *, int *);
  
#endif

  void advance_seed();
};
}

#endif
