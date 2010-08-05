The PHP SPARQL Objects (PSO) library defines a lightweight, consistent interface for accessing RDF stores in PHP using the SPARQL query and update language. Each SPARQL endpoint driver that implements the PSO interface can expose store-specific features as regular extension functions. Note that in order to use this interface you must use a store-specific PSO driver to access a SPARQL endpoint. PSO provides a data-access abstraction layer, which means that, regardless of which store you're using, you use the same functions to issue queries and fetch data.

unified, object-oriented API for accessing different RDF stores via SPARQL.

PSI: PHP SPARQL Interface

Resources:
SPARQL portal http://www.w3.org/standards/techs/sparql
http://www.w3.org/TR/sparql11-query/
http://www.w3.org/TR/sparql11-update/
http://www.w3.org/TR/sparql11-protocol/
http://www.w3.org/TR/sparql11-http-rdf-update/


4store SPARQL endpoints for testing:
http://dbtune.org/classical/sparql/
http://cima.ng-london.org.uk/raphael/sparql/


Install 4store on debian squeeze (testing)
# Dependencies
sudo apt-get install build-essential libpcre3-dev librasqal2-dev libraptor1-dev libglib2.0-dev ncurses-dev libreadline-dev
# 4store
sudo apt-get install git-core
git clone http://github.com/garlik/4store.git
cd 4store
make
make install
make test
