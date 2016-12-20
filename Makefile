.PHONY: clean All

All:
	@echo "----------Building project:[ TVseries - Release ]----------"
	@$(MAKE) -f  "TVseries.mk"
clean:
	@echo "----------Cleaning project:[ TVseries - Release ]----------"
	@$(MAKE) -f  "TVseries.mk" clean
