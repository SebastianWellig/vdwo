#pragma once

#ifdef DEBUG
    // Based on https://stackoverflow.com/a/37264642/11638559
    #define ASSERT(EXPR, MSG) \
        if(!(EXPR)) { \
            std::cerr << "Assert failed:\t" << (MSG) << std::endl \
                      << "Expected:\t" << #EXPR << std::endl \
                      << "Source:\t\t" << __FILE__ << ", in " << __FUNCTION__ << ", line " << __LINE__ \
                      << std::endl; \
            exit(EXIT_FAILURE); \
        };
#else
    #define ASSERT(EXPR, MSG)
#endif

#define PERROR(MSG) \
    std::cerr << "Error:\t" << MSG << std::endl \
              << "Source:\t" << __FILE__ << ", in " << __FUNCTION__ << ", line " << __LINE__ \
              << std::endl; \
    exit(EXIT_FAILURE); \

