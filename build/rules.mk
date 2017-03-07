# NOTE: $(@F) gets the file portion of the target
# This means buildresults is flat.
# https://www.gnu.org/software/make/manual/html_node/Automatic-Variables.html

# C Files
%.o : %.c
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q)$(CC) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) -c -o $(BUILDRESULTS)/$(@F) $<

# C++ Files
%.o : %.cpp
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q)$(CXX) $(INCLUDES) $(CPPFLAGS) $(CXXFLAGS) -c -o $(BUILDRESULTS)/$(@F) $<

# Preprocessed Assembly Files
%.o : %.S
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q)$(CC) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) -c -o $(BUILDRESULTS)/$(@F) $<

# Assembly Files
%.o : %.s
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q)$(AS) $(ASFLAGS) -c -o $(BUILDRESULTS)/$(@F) $<
