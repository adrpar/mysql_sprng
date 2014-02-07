#include <limits.h>

#ifndef _cmrg_h
#define _cmrg_h

#if LONG_MAX > 2147483647L 
#if LONG_MAX > 35184372088831L 
#if LONG_MAX >= 9223372036854775807L 
#define LONG_SPRNG
#define LONG64 long		/* 64 bit long */
#define store_long64 store_long
#define store_long64array store_longarray
#define load_long64 load_long
#define load_long64array load_longarray
#endif
#endif
#endif

#if !defined(LONG_SPRNG) &&  defined(_LONG_LONG)
#define LONG64 long long
#define store_long64 store_longlong
#define store_long64array store_longlongarray
#define load_long64 load_longlong
#define load_long64array load_longlongarray
#endif

extern "C" {
class CMRG : public Sprng
{
 public:
  
  int init_rng (int, int, int, int);
  CMRG();
  ~CMRG();
  CMRG (const CMRG &);
  CMRG & operator= (const CMRG &);

  int get_rn_int ();
  float get_rn_flt();
  double get_rn_dbl ();
  int spawn_rng (int, Sprng ***);
  int get_seed_rng ();
  int free_rng ();
  int pack_rng (char **);
  int unpack_rng (char *);
  int print_rng ();

 private:
  int rng_type;
  char *gentype;
  int stream_number;
  int nstreams;
  int init_seed;
  int parameter;
  int narrays;
  int *array_sizes;
  int **arrays;
  int spawn_offset;
  /*** declare other variables here ***/
  unsigned int prime;
#ifdef LONG64                   /* 64 bit integer types */
  unsigned LONG64 state, multiplier;
  unsigned LONG64 s0, s1, s2, s3, s4;
#else  /* No 64 bit type available, so use array of floats */
  double state[3], multiplier[3];/* simulate 64 bit arithmetic */
  int s0, s1, s2, s3, s4;
#endif
  /*
#if defined(CHECK_POINTERS)
  spawn_rng(int, Sprng ***);
#endif
  */
  inline void advance_state();
  inline void advance_cmrg();
};
}

#endif
