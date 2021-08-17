OUT = bin
CFLAGS = -Wall -DDEBUG -g
CXXFLAGS = -Wall -DDEBUG -g

SEARCHPATH += src
vpath %.c $(SEARCHPATH)
vpath %.h $(SEARCHPATH)
vpath %.cpp $(SEARCHPATH)
vpath %.hpp $(SEARCHPATH)

DEPS += defs.h structs.h

_OBJS += draw.o
_OBJS += init.o input.o
_OBJS += main.o
_OBJS += stage.o
_OBJS += util.o
_OBJS += sound.o
_OBJS += text.o
_OBJS += background.o
_OBJS += title.o
_OBJS += db.opp
_OBJS += highscores.opp

OBJS = $(patsubst %,$(OUT)/%,$(_OBJS))

# top-level rule to create the program.
all: $(PROG)

# compiling other source files.
$(OUT)/%.o: %.c %.h $(DEPS)
	@mkdir -p $(OUT)
	$(CXX) $(CFLAGS) $(CXXFLAGS) -c -o $@ $<

$(OUT)/%.opp: %.cpp %.hpp $(DEPS)
	@mkdir -p $(OUT)
	$(CXX) $(CFLAGS) $(CXXFLAGS) -c -o $@ $<
	
# cleaning everything that can be automatically recreated with "make".
clean:
	$(RM) -rf $(OUT) $(PROG)
