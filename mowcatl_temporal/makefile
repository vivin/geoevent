CC 	= g++ -g -O -Wno-deprecated 

OBJECTS = \
   cmdLineIntFace.o \
   conversions.o \
   errorhandler.o \
   event.o \
   moitem.o \
   moitemset.o  \
   mop_ruleitem.o \
   moparallelitem.o \
   main.o \
   documentation.o \
   morule.o \
   moruleset.o \
   mos_ruleitem.o \
   moserialitem.o \
   moserialrule.o \
   moserialruleset.o \
   occur.o \
   stringhelper.o \
   timer.o \
   timestamp.o \
   xmlintface.o \
   mowcatl.o \
   
SOURCES = $(OBJECTS:%.o=%.cpp)

#------------------------------------------------------------------------

%.o:%.cc
	@echo "\nCompiling $<..."
	$(CC) -c -o $@ $<

all: $(OBJECTS)
	$(CC) -o mowcatl $(OBJECTS) -lm -lstdc++

clean:
	rm -f core *.o
	rm -rf mowcatl


