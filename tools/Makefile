override CPPFLAGS	+= --std=c++1z
# override CPPFLAGS	+= -MMD -MP
override CPPFLAGS	+= -I../source -I../include
override CPPFLAGS	+= $(shell cat ../.cxxflags 2> /dev/null | xargs )

SOURCES	:= $(shell echo *.cpp)
TARGETS	:= $(SOURCES:%.cpp=%)
DEPENDS	:= $(SOURCES:%.cpp=%.d)

build: $(TARGETS)

clean:
	@rm $(DEPENDS) 2> /dev/null || true
	@rm $(OBJECTS) 2> /dev/null || true
	@rm $(TARGETS) 2> /dev/null || true
	@echo All clean!

-include $(DEPENDS)

.PRECIOUS : $(TARGETS)
.PHONY : clean
