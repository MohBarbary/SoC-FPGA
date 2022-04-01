all:
	arm-linux-gnueabihf-gcc -Dsoc_cv_av -O2 -o main main.c
	arm-linux-gnueabihf-strip -s main
clean:
	rm -rf main