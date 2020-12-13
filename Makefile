CFLAGS = -Wall -g
OBJDIR = obj/
OBJss = main.o ui.o draw.o app.o mouse.o
OBJs = $(addprefix $(OBJDIR),$(OBJss))


all:app

app:$(OBJs)
	gcc $(CFLAGS) $^ -o $@ -lpthread -lm -lmxml
$(OBJDIR)%.o:%.c
	gcc -c $(CFLAGS) $^ -o $@ -fgnu89-inline 

.PHONY:clean
clean:
	rm -rf $(OBJDIR)*.o app

