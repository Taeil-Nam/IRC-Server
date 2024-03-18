SUBDIRS = $(filter-out %.dylib, $(wildcard lib/*/))

all :
	$(foreach subdir, $(SUBDIRS), make all -C $(subdir);)

clean :
	$(foreach subdir, $(SUBDIRS), make clean -C $(subdir);)

fclean :
	$(foreach subdir, $(SUBDIRS), make fclean -C $(subdir);)

re :
	$(foreach subdir, $(SUBDIRS), make re -C $(subdir);)
