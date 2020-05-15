CC = gcc
CFLAGS = -g -Wall -I/opt/vc/include -Iinclude
LDFLAGS = -g -Wall -L/opt/vc/lib -lbrcmGLESv2 -lbrcmEGL -lbcm_host -lm

all: app

%.o: %.c global.h input.h models.h
	$(CC) $(CFLAGS) -o $@ -c $<

app: main.o shaders.o textures.o input.o video.o util.o models.o
	$(CC) $(LDFLAGS) $^ -o $@
