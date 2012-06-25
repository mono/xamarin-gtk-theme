xmldir = $(datadir)/gtk-engines

xml_DATA = xamarin.xml
xml_in_files = $(xml_DATA:.xml=.xml.in)
xml_in_in_files = $(xml_DATA:.xml=.xml.in.in)

EXTRA_DIST = $(xml_in_in_files)

@INTLTOOL_XML_RULE@

CLEANFILES = $(xml_DATA) $(xml_in_files)
