CC := gcc

CFLAGS := -std=c11 -Wall -Werror -Wextra -Wpedantic
CFLAGS += -Ipresentation/ -Idatalayer/ -Idomain/generation/ -Idomain/entities/ -Idomain/characters/ -Idomain/data_structs/

LDFLAGS := -ljson-c -lncursesw -lcheck -lpthread -lrt -lsubunit -lm
CFLAGS += -I/usr/include/json-c

APP_TARGET := ./build/rogue.exe
APP_DIRECTS := ./presentation ./datalayer ./domain ./app
APP_SOURCES := $(shell find $(APP_DIRECTS) -name '*.c')
APP_OBJECTS := $(patsubst %.c, ./build/%.o, $(notdir $(APP_SOURCES)))
APP_DEPENDS := $(patsubst %.c, ./build/%.d, $(notdir $(APP_SOURCES)))

TEST_TARGET := ./build/unit_tests.exe
TEST_DIRECTS := ./presentation ./datalayer ./domain ./unit_tests
TEST_SOURCES := $(shell find $(TEST_DIRECTS) -name '*.c')
TEST_OBJECTS := $(patsubst %.c, ./build/%.o, $(notdir $(TEST_SOURCES)))
TEST_DEPENDS := $(patsubst %.c, ./build/%.d, $(notdir $(TEST_SOURCES)))

vpath %.c domain/characters:domain/data_structs:domain/generation:presentation:app:datalayer:unit_tests

release: CFLAGS += -g0 -O3
release: $(APP_TARGET)

debug: CFLAGS += -g3 -O0
debug: $(APP_TARGET)

unit_tests: $(TEST_TARGET)

$(APP_TARGET) : $(APP_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(APP_OBJECTS) $(LDFLAGS)

$(TEST_TARGET) : $(TEST_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(TEST_OBJECTS) $(LDFLAGS)

./build/%.o : %.c
	$(CC) $(CFLAGS) -c -MMD $< -o $@

include $(wildcard ./build/*.d)

.PHONY : clean doc

clean:
	rm -rf ./build/*.d
	rm -rf ./build/*.o
	rm -rf ./build/*.exe

doc: ../doc
	doxygen ../doc/config
