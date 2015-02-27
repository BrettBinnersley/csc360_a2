#
#MakeFile
#
#Brett Binnersley, V00776751
#Csc 360, Assignment #1
#
# To use: make
#

all: makePQS

clean: cleanItems

makePQS:
	gcc pqs.c SimpleList.c customer.c clerk.c -o pqs

cleanItems:
	-rm -rf *.o *.exe
