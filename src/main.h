#ifndef MAIN_H
#define MAIN_H

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <math.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/uio.h>

#include "ssl.h"
#include "aprintf.h"
#include "stats.h"
#include "units.h"
#include "zmalloc.h"

#include <dirent.h>
#include <sys/stat.h>


struct config;

static void *thread_main(void *);
static int connect_socket(thread *, connection *);
static int reconnect_socket(thread *, connection *);

static int calibrate(aeEventLoop *, long long, void *);
static int sample_rate(aeEventLoop *, long long, void *);
static int delayed_initial_connect(aeEventLoop *, long long, void *);
static int check_timeouts(aeEventLoop *, long long, void *);

static void socket_connected(aeEventLoop *, int, void *, int);
static void socket_writeable(aeEventLoop *, int, void *, int);
static void socket_readable(aeEventLoop *, int, void *, int);

static int response_complete(http_parser *);
static int header_field(http_parser *, const char *, size_t);
static int header_value(http_parser *, const char *, size_t);
static int response_body(http_parser *, const char *, size_t);

static uint64_t time_us();

static int parse_args(struct config *, char **, struct http_parser_url *, char **, int, char **);
static char *copy_url_part(char *, struct http_parser_url *, enum http_parser_url_fields);
static void print_stats_header();
static void print_stats(char *, stats *, char *(*)(long double));
//static void print_stats_latency(stats *);
static void print_hdr_latency(struct hdr_histogram*, const char*);

// AMD extras =====================================================================

typedef struct topoCore_s {
    struct topoCore_s *p_nodeListNext;
    struct topoCore_s *p_chipletListNext;
    int coreId;               // system global
    int dieId;
    int chipletId;
    int cpuCnt;                // number of CPUs in a Core
    int cpus[2];               // OS CPU numbers 
} topoCore_t;

typedef struct topoChiplet_s {
    struct topoChiplet_s *p_next;
    int chipletId;             // only assume node scope
    int coreCnt;               // number of Cores in a chiplet
    topoCore_t *p_cores;       // pointer to an array of Cores 
} topoChiplet_t;

typedef struct topoNode_s {
    struct topoNode_s *p_nodeListNext; 
    int nodeId;                 // system global
    int coreCnt;
    struct topoCore_s *p_cores;
    //second phase
    struct topoNode_s *p_socketListNext; 
    int chipletCnt;             // number of chiplets in a node
    topoChiplet_t *p_chiplets;     // pointer to an array of chiplets 
} topoNode_t;

typedef struct topoSocket_s {
    struct topoSocket_s *p_next;
    int socketId;
    int nodeCnt;                // number of nodes in a socket
    topoNode_t *p_nodes;        // pointer to an array of nodes 
} topoSocket_t;




int topoSocketsCnt = 0;             //TODO is this needed
topoSocket_t *p_topoSockets = NULL;
int topoNodesCnt = 0;               //TODO is this needed
topoNode_t *p_topoNodes = NULL;


static int topology_init();
static void topologyDump(void);
static topoSocket_t * topologyGetSocket(int);
static void topologySocketAddNode(topoSocket_t *p_socket, topoNode_t *p_node);
static topoChiplet_t * topologyGetChiplet(topoNode_t *, int);
static void topologyChipletAddCore(topoChiplet_t *p_chiplet, topoCore_t *p_core);

#endif /* MAIN_H */
