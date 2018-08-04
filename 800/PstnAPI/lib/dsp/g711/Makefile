all : dtmf
	./dtmf

dtmf:g711.o as_dtmf_gen.o
	gcc $^ -o dtmf -lm 

g771.o:g711.c

as_dtmf_gen.o:as_dtmf_gen.c

clean:
	rm *.o
	rm dtmf
