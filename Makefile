#This MakeFile is a little more advanced.
#If needed, please see GNU make manual at https://www.gnu.org/software/make/manual/make.html


OBJ	 = main.o funcLib.o assmbler.o preAsm.o #<.o עם סיומת .c להכניס את השמות של כל קבצי >
CFLAGS = -ansi -Wall -pedantic -Werror -g -ggdb3

program: $(OBJ)
	gcc $(CFLAGS) $^ -o $@

%.o: %.c
	gcc -c $(CFLAGS) $< -o $@