CFLAGS = -std=c11
CFLAGS += -Wall
CFLAGS += -g
CFLAGS += -D_POSIX_C_SOURCE=199309L		#used to define CLOCK_MONOTONIC when running performance test mode
#CFLAGS += -DPERF_TEST					#remove the "#" in front of the CFLAGS to run in performance test mode !
LDLIBS = -lcrypto
LDLIBS += -lcheck
LDLIBS += -lm
LDLIBS += -lrt
LDLIBS += -pthread
LDLIBS += -lsubunit


all: pps-launch-server pps-client-put pps-client-get test-hashtable pps-list-nodes pps-dump-node pps-client-cat pps-client-substr pps-client-find

pps-launch-server: pps-launch-server.o system.o error.o client.o node.o util.o hashtable.o network.o node_list.o args.o ring.o

pps-client-put: pps-client-put.o network.o client.o error.o node.o util.o hashtable.o system.o node_list.o args.o ring.o

pps-client-get: pps-client-get.o network.o client.o error.o node.o util.o hashtable.o system.o node_list.o args.o ring.o

pps-list-nodes: pps-list-nodes.o network.o client.o error.o node.o util.o hashtable.o system.o node_list.o args.o ring.o

pps-dump-node: pps-dump-node.o error.o node_list.o node.o util.o client.o hashtable.o system.o args.o ring.o

pps-client-cat: pps-client-cat.o network.o client.o error.o node.o util.o hashtable.o system.o node_list.o args.o ring.o

pps-client-substr: pps-client-substr.o network.o client.o error.o node.o util.o hashtable.o system.o node_list.o args.o ring.o

pps-client-find: pps-client-find.o network.o client.o error.o node.o util.o hashtable.o system.o node_list.o args.o ring.o

test-hashtable: test-hashtable.o error.o hashtable.o

args.o: args.c args.h

client.o: client.c client.h error.h node.h util.h system.h node_list.h \
 config.h args.h ring.h hashtable.h
 
error.o: error.c

hashtable.o: hashtable.c hashtable.h error.h util.h

hastableTest.o: hastableTest.c tests.h error.h hashtable.h

log-packets.o: log-packets.c

network.o: network.c network.h client.h error.h node.h util.h system.h \
 node_list.h config.h args.h ring.h hashtable.h
 
node.o: node.c node.h error.h util.h system.h

node_list.o: node_list.c node_list.h node.h error.h util.h system.h \
 config.h
 
pps-client-cat.o: pps-client-cat.c network.h client.h error.h node.h \
 util.h system.h node_list.h config.h args.h ring.h hashtable.h
 
pps-client-find.o: pps-client-find.c network.h client.h error.h node.h \
 util.h system.h node_list.h config.h args.h ring.h hashtable.h
 
pps-client-get.o: pps-client-get.c system.h error.h client.h node.h \
 util.h node_list.h config.h args.h ring.h hashtable.h network.h
 
pps-client-put.o: pps-client-put.c network.h client.h error.h node.h \
 util.h system.h node_list.h config.h args.h ring.h hashtable.h
 
pps-client-substr.o: pps-client-substr.c network.h client.h error.h \
 node.h util.h system.h node_list.h config.h args.h ring.h hashtable.h
 
pps-dump-node.o: pps-dump-node.c error.h system.h node_list.h node.h \
 util.h config.h client.h args.h ring.h hashtable.h
 
pps-launch-server.o: pps-launch-server.c system.h error.h client.h node.h \
 util.h node_list.h config.h args.h ring.h hashtable.h network.h
 
pps-list-nodes.o: pps-list-nodes.c error.h system.h node_list.h node.h \
 util.h config.h client.h args.h ring.h hashtable.h
 
ring.o: ring.c ring.h error.h hashtable.h node_list.h node.h util.h \
 system.h config.h
 
system.o: system.c error.h system.h

test-hashtable.o: test-hashtable.c tests.h error.h hashtable.h

util.o: util.c
