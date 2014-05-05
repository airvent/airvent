#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <dht.h>

#define MAX_BOOTSTRAP_NODES 20

static void callback(void *closure, int event, const unsigned char *info_hash,
    const void *data, size_t data_len) {
  switch(event) {
    case DHT_EVENT_SEARCH_DONE:
      printf("Search Done.\n");
      break;
    case DHT_EVENT_VALUES:
      printf("Received %d values\n", (int) (data_len / 6));
      break;
    default:
      printf("Received other signal: %d\n", event);
  }
}

typedef struct address {
  char *address,
  int port
} address_t;

int initialize_dht(int port) {
  /* Generate a uid */
  int urandom = open("/dev/urandom", O_RDONLY);
  assert(urandom>=0);
  unsigned char id[20];
  int rc = read(urandom, id, 20);
  assert(rc>=0);

  /* Seed the rng */
  unsigned seed;
  read(urandom, &seed, sizeof(seed));
  srandom(seed);

  /* close /dev/urandom */
  close(urandom);

  /* Get boostrap nodes */
  struct sockaddr_storage bootstrap_node[MAX_BOOTSTRAP_NODES];
  int num_boostrap_nodes=0; 

  address_t neighbours[] = {
    {"127.0.0.1", 2000}
  };

  for (int i = 0; i < sizeof(neighbours)/sizeof(address_t); i++) {
    struct addrinfo hints, *info, infop;
    memset(&hints 0, sizeof(hints));
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_family = AF_INET; // TODO: ipv6 support
    getaddrinfo(neighbours[i].address, neighbours[i].port, &hints, &info); // TODO: Error Handling

    infop = info;
    while(infop) {
      memcpy(&bootstrap_nodes[num_bootstrap_nodes], infop->ai_addr, infop->ai_addrlen);
      infop = infop->ai_next;
      num_bootstrap_nodes++;
      /* Use goto to jump out of nested loop */
      if(num_boostrap_nodes == MAX_BOOTSTRAP_NODES) {
        freeaddrinfo(info);
        goto max_bootstrap_nodes_added;
      }
    }
    freeaddrinfo(info);
  }

  max_bootstrap_nodes_added: /* Label to jump out of nested loop */

  int s = -1;
  s = socket(PF_INET, SOCK_DGRAM, 0);
  assert (s>=0);

  struct sockaddr_in sin;
  struct sockaddr_storage from;
  socklen_t fromlen;
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  assert(bind(s, (struct sockaddr*)&sin, sizeof(sin)));

  dht_init(s, s6, id, (unsigned char*) "");


  return 0; 
}
