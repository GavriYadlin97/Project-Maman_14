#This MakeFile is a little more advanced.
#If needed, please see GNU make manual at https://www.gnu.org/software/make/manual/make.html


OBJ	 = <.o עם סיומת .c להכניס את השמות של כל קבצי >
CFLAGS = -ansi -Wall -pedantic -Werror

program: $(OBJ)
	gcc $(CFLAGS) $^ -o $@

%.o: %.c
	gcc -c $(CFLAGS) $< -o $@