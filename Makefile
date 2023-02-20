
CF = clang-format-12 -i

.PHONY: format check-format check-arg

check-arg:
	@if test "$(DIR)" = "" ; then \
		echo DIR not set; \
		echo Use:;\
		echo make cmd DIR=...;\
		echo ;\
		exit 1; \
	fi

format: check-arg
	@echo format SRC
	$(CF) $(DIR)/*.[ch]

check-format: check-arg
	@echo "\n\e[32m*** Check format of source code ***\e[0m\n"
	@utils/checkpatch.pl --no-tree -f $(DIR)/*.[ch]
