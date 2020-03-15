#ifndef BABAI_RNG_GUARD
#define BABAI_RNG_GUARD
#include <gsl/gsl_randist.h>
#include <gsl/gsl_rng.h>
#include <ctime>
namespace babai {
class Random {
private:
  // random number generator type
  static const gsl_rng_type *_rnd_rng_type;
  // random number generator
  static gsl_rng *_rnd_rng;
  // check whether random generator is initialized
  static bool _initialized;
public:
  // initialize random generator
  static void init() {
    if (Random::_initialized)
      return;
    // random generator environment setup
    gsl_rng_env_setup();
    // set random generator type
    Random::_rnd_rng_type = gsl_rng_default;
    // set random generator
    Random::_rnd_rng = gsl_rng_alloc(_rnd_rng_type);
    // set seed of random generator
    unsigned long seed = time(0);
    gsl_rng_set(Random::_rnd_rng, seed);
    Random::_initialized = true;
  }
  // access to random generator
  static gsl_rng *generator() { return Random::_rnd_rng; }
};
// initialize random flag
bool Random::_initialized = false;
const gsl_rng_type * Random::_rnd_rng_type = gsl_rng_default;
gsl_rng * Random::_rnd_rng = gsl_rng_alloc(_rnd_rng_type);
};     // namespace babai
#endif /* end of include guard: BABAI_RNG_GUARD */
