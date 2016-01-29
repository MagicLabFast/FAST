CC = gcc
CFLAGS  = -Wall -g
LIBS = -lpthread -lpcap -lnet
TARGET = launch
RM = rm -f
OBJS = fast_control_plane_api.o nmac_api.o main.o
all:$(OBJS) 
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)
clean:
	$(RM) $(TARGET) $(OBJS)