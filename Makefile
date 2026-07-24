examples/x11: examples/x11.c glcc.h
	cc $< --debug -I./ -lX11 -lXext -lEGL -o $@
