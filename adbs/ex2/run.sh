ant \
	&& java -cp dist/attica.jar org.dejave.attica.server.Database --properties dist/attica.properties < share/rewind.sql \
	&& java -cp dist/attica.jar org.dejave.attica.server.Database --properties dist/attica.properties < share/query.sql

rm -f ~/attica/tmp/attica-*.tmp
