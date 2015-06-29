CC=gcc
PREFIX=/usr
DOCDIR=${PREFIX}/share/doc/rarcrack

all:
	${CC} -pthread rarcrack.c `xml2-config --libs --cflags` -O3 -o rarcrack

debug:
	${CC} -Wextra -Werror -pthread rarcrack.c `xml2-config --libs --cflags` -O3 -o rarcrack

clean:
	-rm -rf *.o *.xml rarcrack

install:
	install -s rarcrack ${PREFIX}/bin
	-mkdir -p ${DOCDIR}
	chmod 755 ${DOCDIR}
	install -m 644 -t ${DOCDIR} CHANGELOG LICENSE README README.html RELEASE_NOTES

uninstall:
	-rm -rf ${PREFIX}/bin/rarcrack
