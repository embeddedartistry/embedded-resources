# C Files
%.o : %.c
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q)$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $(BUILDRESULTS)/$@ $<

# C++ Files
%.o : %.cpp
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $(BUILDRESULTS)/$@ $<

# Preprocessed Assembly Files
%.o : %.S
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q)$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $(BUILDRESULTS)/$@ $<

# Assembly Files
%.o : %.s
	$(Q)[ -d "$(BUILDRESULTS)" ] || mkdir -p $(BUILDRESULTS)
	$(Q)$(AS) $(ASFLAGS) -c -o $(BUILDRESULTS)/$@ $<
