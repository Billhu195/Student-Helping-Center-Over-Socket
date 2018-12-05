PORT=50640
CFLAGS = -Wall -g -std=c99 -DPORT=$(PORT)

hcq_server: hcq.o hcq_server.o
	gcc $(CFLAGS) -o $@ $^

helpcentre: helpcentre.o hcq.o 
	gcc $(CFLAGS) -o $@ $^

%.o: %.c hcq.h
	gcc $(CFLAGS) -c $<
	
clean: 
	rm *.o hcq_server
