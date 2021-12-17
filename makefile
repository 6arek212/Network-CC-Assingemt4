.PHONY : clean all

all : sender reciver


sender : sender.o 
	gcc -o sender sender.o 


reciver : measure.o 
	gcc -o reciver measure.o 



sender.o : sender.c 
	gcc -c sender.c 


measure.o : measure.c 
	gcc -c measure.c 



clean : 
	rm -rf *.o prog reciver