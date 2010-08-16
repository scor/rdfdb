<?php



$query = db_select('node', 'n', $options);


////

$query = db_select('users', 'u');

$query
  ->condition('u.uid', 0, '<>')
  ->fields('u', array('uid', 'name', 'status', 'created', 'access'))
  ->range(0, 50);

$result = $query->execute();



//////////
$myselect = db_select('mytable')
  ->fields('mytable')
  ->condition('myfield', 'myvalue');
$alias = $query->join($myselect, 'myalias', 'n.nid = myalias.nid');



$query->fields('n', array('nid', 'title', 'created', 'uid'));


// Force filtering of duplicate records in the result set.
$query->distinct()

$count_alias = $query->addExpression('COUNT(uid)', 'uid_count');
$query->orderBy('title', 'DESC');
$query->groupBy('uid');
$query->range(5, 10);


//// SPARQL lib API functions

// Insert data into a graph
// INSERT DATA { graph_triples }
//  graph_triples :: = TriplesBlock | GRAPH <graph_uri> { TriplesBlock }
// INSERT DATA { GRAPH <http://example/bookStore> { <http://example/book3>  dc:title  "Fundamentals of Compiler Design" } }

rdfdb_insert_data($graph_triples)

// Delete data from a graph
// DELETE DATA { graph_triples }
//  graph_triples :: = TriplesBlock | GRAPH <graph_uri> { TriplesBlock }
rdfdb_delete_data($graph_triples)


// @todo 4.1.3 DELETE/INSERT
// @todo 4.1.4 DELETE
// @todo 4.1.5 INSERT
// @todo 4.1.6 DELETE WHERE

// Load data from remote graph
// LOAD <documentURI> [ INTO GRAPH <uri> ]
rdfdb_load($document, $graph = NULL)

// Remove all triples from a graph
// CLEAR GRAPH ( <uri> | DEFAULT )
rdfdb_clear($graph = NULL)

// Create graph
// CREATE [ SILENT ] GRAPH <uri>
rdfdb_create($graph)

// Drop graph
// DROP [ SILENT ] GRAPH <uri>
rdfdb_drop($graph)

//// other potential candidates for API

check() // Check if the triplestore is up.
set($graph, $data) // create or replace the data in a graph.
add($graph, $data) // add the data in a graph.
delete($graph) // delete graph and its data
count($graph = NULL)  // count how many triples in a graph/endpoint

queryRead($query, $typeoutput) // @see query()
queryReadJSON($query, $typeoutput)
queryReadTabSeparated($query, $typeoutput)
queryUpdate($query, $typeoutput)

