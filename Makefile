CC      = gcc
CFLAGS  = -Wall -O -g
LIBS    = -lbluetooth
SRCS    = classic.c nunchuk.c  uinput.c  wiimote.c  wm2jsPlus.c
OBJS    = $(SRCS:.c=.o)
MAIN    = wm2jsPlus


.PHONY: depend clean

all:		$(MAIN)

$(MAIN):	$(OBJS) 
		$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
		$(CC) $(CFLAGS) $(INCLUDES) -c $<

clean:
		$(RM) *.o *~ $(MAIN)

depend: 	$(SRCS)
		makedepend $(INCLUDES) $^

