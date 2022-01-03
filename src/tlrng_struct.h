#ifndef TLRNG_STRUCT_H
#define TLRNG_STRUCT_H

struct rng_state{
        uint64_t s[4];
        uint8_t gen;
        double z1;
        int has_gauss; /* !=0: gauss contains a gaussian deviate */
        double gauss;
};

#endif
