override CPPFLAGS	+= --std=c++1z
override CPPFLAGS	+= -MMD -MP
override CPPFLAGS	+= -I../source -I../include
override CPPFLAGS	+= $(shell cat ../.cxxflags 2> /dev/null | xargs )

SOURCES	:= $(shell echo *.cpp)
TARGETS	:= $(SOURCES:%.cpp=%.test)
TEMPDIR	:= temp
OBJECTS	:= $(SOURCES:%.cpp=$(TEMPDIR)/%.o)
DEPENDS	:= $(OBJECTS:.o=.d)
RUNS	:= $(OBJECTS:.o=.run)

run: $(RUNS)

build: $(TARGETS)

$(TEMPDIR)/%.run: %.test
	./$<
	@echo "This is a dummy file for make to keep track of test runs" > $@

%.test: $(TEMPDIR)/%.o
	$(CXX) $(LDFLAGS) $< $(LDLIBS) -o $@

$(TEMPDIR)/%.o: %.cpp | $(TEMPDIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

$(TEMPDIR):
	@mkdir $@

cleanrun:
	@rm $(RUNS) 2> /dev/null || true

clean: cleanrun
	@rm $(DEPENDS) 2> /dev/null || true
	@rm $(OBJECTS) 2> /dev/null || true
	@rmdir $(TEMPDIR) 2> /dev/null || true
	@rm $(TARGETS) 2> /dev/null || true
	@echo All clean!

-include $(DEPENDS)

.PRECIOUS : $(OBJECTS) $(TARGETS)
.PHONY : clean cleanrun
