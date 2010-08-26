The RDF Database (RDFDB) library defines a lightweight, consistent interface for accessing RDF stores in PHP using the SPARQL query and update language. Each RDF store driver that implements the RDFDB interface can expose store-specific features as regular extension functions. Note that in order to use this interface you must use a store-specific RDFDB driver to access a SPARQL endpoint. RDFDB provides a data-access abstraction layer, which means that, regardless of which store you're using, you use the same functions to issue queries.

The RDF Database library provides a standard, vendor-agnostic abstraction layer for accessing RDF stores. The API is designed to preserve the syntax and power of SPARQL 1.1 as much as possible, but also:

    * to support multiple RDF database servers easily;
    * to provide a structured interface for the dynamic construction of queries;
    * to enforce security checks and other good practices;
    * to provide developers with a clean interface for intercepting and modifying a site's queries.


== Drivers ==

Because different RDF stores require different sorts of interaction, the RDF database layer requires a driver for each database type. RDFDB currently include drivers for ARC2, Virtuoso and 4store.

== Connections ==

A connection is an object of class RdfdbConnection. Every RDF database to which your application connects has a single connection object associated with it. That connection object must be subclassed for each individual driver.

== Queries ==

A query is an SPARQL statement that will be sent to a database connection. Queries use object-oriented query builders. A "query object" refers to an instance of a query builder for one of the various query types. Only the SPARQL queries of the following types are supported: SELECT, INSERT DATA, DELETE DATA, CLEAR.

== Connection key ==

A connection key is a unique identifier for a given RDF database connection. The connection key must be unique for a given site, and there must always be a connection of "default" that will be the primary database. Note that at the moment, we do not yet make use of the target key, so it should be left as default for now. Each driver may have different configuration depending on what is appropriate for it.

$databases['rdfdb1']['default'] = array(
  'driver' => 'arc2',
  'endpoint' => 'http://localhost/arc2_endpoint.php',
  'apikey' => 'somekey',
);
$databases['rdfdb2']['default'] = array(
  'driver' => '4store',
  'endpoint' => 'http://localhost:8080/sparql/',
);


== Resources ==

SPARQL portal
http://www.w3.org/standards/techs/sparql

SPARQL 1.1
http://www.w3.org/TR/sparql11-query/
http://www.w3.org/TR/sparql11-update/
http://www.w3.org/TR/sparql11-protocol/
http://www.w3.org/TR/sparql11-http-rdf-update/


