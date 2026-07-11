#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "doomtype.h"
#include "i_system.h"
#include "net_defs.h"
#include "net_sdl.h"

static boolean NET_SDL_InitClient(void)
{
    return false;
}

static boolean NET_SDL_InitServer(void)
{
    return false;
}

static void NET_SDL_SendPacket(net_addr_t *addr, net_packet_t *packet)
{
}

static boolean NET_SDL_RecvPacket(net_addr_t **addr, net_packet_t **packet)
{
    return false;
}

static void NET_SDL_AddrToString(net_addr_t *addr, char *buffer, int buffer_len)
{
    if (buffer_len > 0)
    {
        buffer[0] = '\0';
    }
}

static void NET_SDL_FreeAddress(net_addr_t *addr)
{
}

static net_addr_t *NET_SDL_ResolveAddress(char *address)
{
    return NULL;
}

net_module_t net_sdl_module =
{
    NET_SDL_InitClient,
    NET_SDL_InitServer,
    NET_SDL_SendPacket,
    NET_SDL_RecvPacket,
    NET_SDL_AddrToString,
    NET_SDL_FreeAddress,
    NET_SDL_ResolveAddress,
};
