TEAM = '01'

submit:
	@$(if $(ID),,echo 'usage: make ID=1'; exit -1)
	cd src && $(MAKE) clean
	mkdir $(TEAM)
	mkdir pintos
	cp -r src pintos
	mv pintos $(TEAM)
	cp docs/proj$(ID).pdf $(TEAM)/document_$(TEAM).pdf
	tar -czvf os_prj$(ID)_$(TEAM).tar.gz ./$(TEAM)
	rm -rf $(TEAM)
	rm -rf pintos

clean:
	rm -f os_prj*_$(TEAM).tar.gz
	rm -rf $(TEAM)
	rm -rf pintos
