#ifdef USE_MATTSON_RNG
#define RNG_MULTIPLIER 1366
#define RNG_INCREMENT 150889
#define RNG_MOD 714025
#else
#define RNG_MULTIPLIER 1140671485
#define RNG_INCREMENT 12820163
#define RNG_MOD 16777216
#endif

int pr_random(void)
{
    static int state = 0;

    return (state = (RNG_MULTIPLIER * state + RNG_INCREMENT) % RNG_MOD);
}

double pr_random_f(double range)
{
    return ((double) pr_random() / (double) RNG_MOD) * range;
}
