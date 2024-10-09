#pragma once
// HACK for musl

struct pollfd
{
    s32   fd;
    s16   events;
    s16   revents;
};

struct in_addr
{
    u32 s_addr;           // 0
};

struct sockaddr_in {
    sa_family_t    sin_family; /* address family: AF_INET */
    in_port_t      sin_port;   /* port in network byte order */
    struct in_addr sin_addr;   /* internet address */
};

struct sockaddr
{
    u8 _0;              // 0
    u8 family;          // 1
    u16 port;           // 2
    struct in_addr address;    // 4
    u8 _8[8];           // 8
};

struct hostent
{
    char*   h_name;
    char**  h_aliases;
    int     h_addrtype;
    int     h_length;
    char**  h_addr_list;
};

struct msghdr
{
    void *msg_name;   /* Address to send to/receive from.  */
    socklen_t msg_namelen;  /* Length of address data.  */

    struct iovec *msg_iov;  /* Vector of data to send/receive into.  */
    size_t msg_iovlen;    /* Number of elements in the vector.  */

    void *msg_control;    /* Ancillary data (eg BSD filedesc passing). */
    size_t msg_controllen;  /* Ancillary data buffer length.
                              !! The type should be socklen_t but the
                              definition of the kernel is incompatible
                              with this.  */

    int msg_flags;    /* Flags on received message.  */
};

struct mmsghdr {
    struct msghdr msg_hdr;  /* Message header */
    unsigned int  msg_len;  /* Number of received bytes for header */
};

/* Structure used for storage of ancillary data object information.  */
struct cmsghdr
{
    size_t cmsg_len;    /* Length of data in cmsg_data plus length
                        of cmsghdr structure.
                        !! The type should be socklen_t but the
                        definition of the kernel is incompatible
                        with this.  */
    int cmsg_level;   /* Originating protocol.  */
    int cmsg_type;    /* Protocol specific type.  */
};

struct addrinfo {
  int ai_flags;
  int ai_family;
  int ai_socktype;
  int ai_protocol;
  socklen_t ai_addrlen;
  struct sockaddr *ai_addr;
  char *ai_canonname;
  struct addrinfo *ai_next;
};
