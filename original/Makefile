CC=gcc
PREFIX=/usr
DOCDIR=${PREFIX}/share/doc/rarcrack

all:
	${CC} -pthread rarcrack.c `xml2-config --libs --cflags` -O2 -o rarcrack
clean:
	-rm *.o rarcrack
install:
	install -s rarcrack ${PREFIX}/bin
	-mkdir -p ${DOCDIR}
	chmod 755 ${DOCDIR}
	install -m 644 -t ${DOCDIR} CHANGELOG LICENSE README README.html RELEASE_NOTES
uninstall:
	-rm ${PREFIX}/bin/rarcrack

