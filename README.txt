The PHP SPARQL Objects (PSO) library defines a lightweight, consistent interface for accessing RDF stores in PHP using the SPARQL query and update language. Each SPARQL endpoint driver that implements the PSO interface can expose store-specific features as regular extension functions. Note that in order to use this interface you must use a store-specific PSO driver to access a SPARQL endpoint. PSO provides a data-access abstraction layer, which means that, regardless of which store you're using, you use the same functions to issue queries and fetch data.

The XXX Database API provides a standard, vendor-agnostic abstraction layer for accessing RDF stores. The API is designed to preserve the syntax and power of SPARQL 1.1 as much as possible, but also:

    * to support multiple database servers easily;
    * to provide a structured interface for the dynamic construction of queries;
    * to enforce security checks and other good practices;
    * to provide modules with a clean interface for intercepting and modifying a site's queries. 


PSI: PHP SPARQL Interface

unified, object-oriented API for accessing different RDF stores via SPARQL.


== Drivers ==

Because different RDF stores require different sorts of interaction, the RDF database layer requires a driver for each database type.


== Connections ==

http://drupal.org/node/310070

A connection is an object of class DatabaseConnection, which inherits from the PDO class. Every database to which Drupal connects has a single connection object associated with it. That connection object must be subclassed for each individual driver.

== Queries ==

A query is an SPARQL statement that will be sent to a database connection. There are six types of queries supported by the database system: Static, Dynamic, Insert, Update, Delete, and Merge. Queries use object-oriented query builders. A "query object" refers to an instance of a query builder for one of the various query types.

== Connection key ==

A connection key is a unique identifier for a given RDF database connection. The connection key must be unique for a given site, and there must always be a connection of "default" that will be the primary Drupal database. Note that unlike the core database layer, we do not yet make use of the target key, so it should be left as default for now. Each driver may have different configuration depending on what is appropriate for it.

$databases['rdfdb1']['default'] = array(
  'driver' => 'arc2',
  'database' => 'drupaldb1',
  'username' => 'username',
  'password' => 'secret',
  'host' => 'dbserver1',
);
$databases['rdfdb2']['default'] = array(
  'driver' => '4store',
  'file' => 'files/extradb.sqlite',
);


== Resources ==

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

sudo apt-get install curl

