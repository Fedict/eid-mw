if PLANTUML
%.png: %.puml
	@PLANTUML@ -o $(abs_builddir) $<

%.svg: %.puml
	@PLANTUML@ -o $(abs_builddir) -tsvg $<

dist_noinst_DATA = be-statemach.png be-statemach-docs.png
endif
EXTRA_DIST = be-statemach.puml be-statemach-docs.puml
