//
// Endianess handling, swapping 16bit and 32bit.
//

#ifndef __I_SWAP__
#define __I_SWAP__

// Endianess handling.
// WAD files are stored little endian.

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define SDL_SwapLE16(x) ((unsigned short)((((unsigned short)(x)) >> 8) | ((((unsigned short)(x)) & 0xff) << 8)))
#define SDL_SwapLE32(x) ((unsigned int)(((((unsigned int)(x)) & 0xff000000) >> 24) | \
                                         ((((unsigned int)(x)) & 0x00ff0000) >> 8) | \
                                         ((((unsigned int)(x)) & 0x0000ff00) << 8) | \
                                         ((((unsigned int)(x)) & 0x000000ff) << 24)))
#define SDL_SwapBE16(x) (x)
#define SDL_SwapBE32(x) (x)
#else
#define SDL_SwapLE16(x) (x)
#define SDL_SwapLE32(x) (x)
#define SDL_SwapBE16(x) ((unsigned short)((((unsigned short)(x)) >> 8) | ((((unsigned short)(x)) & 0xff) << 8)))
#define SDL_SwapBE32(x) ((unsigned int)(((((unsigned int)(x)) & 0xff000000) >> 24) | \
                                         ((((unsigned int)(x)) & 0x00ff0000) >> 8) | \
                                         ((((unsigned int)(x)) & 0x0000ff00) << 8) | \
                                         ((((unsigned int)(x)) & 0x000000ff) << 24)))
#endif

// These are deliberately cast to signed values; this is the behaviour
// of the macros in the original source and some code relies on it.

#define SHORT(x)  ((signed short) SDL_SwapLE16(x))
#define LONG(x)   ((signed int) SDL_SwapLE32(x))

#endif
