<?php

echo "test script";

include 'rdfdb.inc';
//require 'http.inc';
include 'arc/ARC2.php';
include 'http.inc';


global $rdfdb_connections;

$rdfdb_connections['rdfdb1']['default'] = array(
  'driver' => 'arc2',
  'endpoint' => 'http://localhost/d7git3/sparql',
  'apikey' => 'somekey',
//  'namespaces' => array ('dc' => 'http://purl.org/dc/terms/'),
);
$rdfdb_connections['rdfdb2']['default'] = array(
  'driver' => '4store',
  'endpoint' => 'http://localhost:8080/data/',
  'apikey' => '',
);
$rdfdb_connections['rdfdb3']['default'] = array(
  'driver' => 'virtuoso',
  'endpoint' => 'http://localhost:18890/sparql',
  'apikey' => '',
);

Rdfdb::addConnectionInfo('default', 'default', $rdfdb_connections['rdfdb3']['default']);
rdfdb_set_active('defaut');


//var_dump(rdfdb_insert_data('http://example/bookStore', '<http://example/book4>  <http://ex.org/title>  "Fundamentals of Compiler Desig4n"')->execute());


//var_dump(Rdfdb::getConnectionInfo('default'));

//echo realpath(dirname(__FILE__));

// Insert data into a graph
// INSERT DATA { graph_triples }
//  graph_triples :: = TriplesBlock | GRAPH <graph_uri> { TriplesBlock }
// INSERT DATA { GRAPH <http://example/bookStore> { <http://example/book3>  dc:title  "Fundamentals of Compiler Design" } }
// ARC2 INSERT INTO <http://example.com/> { <#foo> <bar> "baz" .} 

//var_dump(rdfdb_insert_data('http://example/bookStore', '<http://example/book3>  <http://ex.org/title>  "Fundamentals of Compiler Design"')->execute());
rdfdb_insert_data('http://example/bookStore', '<http://example/book3>  <http://ex.org/title>  "Fundamentals of Compiler Design"')->execute();
rdfdb_insert_data('http://example/bookStore', '<http://example/book4>  <http://ex.org/title>  "Fundamentals of Compiler Design4"')->execute();

// An array containing prefix and namespaces as name/value pairs. 
$options = array ('namespaces' => array ('dc' => 'http://purl.org/dc/terms/'));

//var_dump(rdfdb_insert_data('http://example/bookStore', '<http://example/book3>  dc:title  "Fundamentals of Compiler Design with ns34"', $options)->execute());


///////////////////////////////////////////////////
///////////////////////////////////////////////////
// Delete data from a graph
// DELETE DATA { graph_triples }
//  graph_triples :: = TriplesBlock | GRAPH <graph_uri> { TriplesBlock }
//rdfdb_delete_data($graph_triples)

// DELETE FROM <http://example/bookStore>

// DELETE { ?s ?p ?o . } WHERE { ?s ?p ?o . }

//var_dump(rdfdb_delete_data(NULL, '<http://example/book3>  <http://ex.org/title>  "Fundamentals of Compiler Design"')->execute());

//var_dump(rdfdb_delete_data('http://example/bookStore', '<http://example/book4>  <http://ex.org/title>  "Fundamentals of Compiler Design"')->execute());


//rdfdb_select('?s ?p ?o', $options)->where('?s ?p ?o')->execute();

$rs = rdfdb_clear('http://example/bookStore', $options)->execute();
var_dump($rs);


//rdfdb_clear()->execute();
//return;

//echo SparqlToTSV($rs['result']);
//ARC2::getComponent('ARC2_StoreEndpoint');
//echo SparqlToTSV($rs['result']['rows'], $rs['result']['variables']);

//$rs = rdfdb_clear('http://example.org/g2', $options)->execute();
//var_dump($rs);


  // Dummy function exporting SPARQL results as TSV plain text, making it easier
  // to do RDF comparisons for unit testing purposes. 
  function SparqlToTSV($results) {
    $rows = $results['rows'];
    $vars = $results['variables'];
    $r = '';
    $delim = "\t";
    $esc_delim = "\\t";
    foreach ($rows as $row) {
      $hr = '';
      $rr = '';
      foreach ($vars as $var) {
        $hr .= $r ? '' : ($hr ? $delim . $var : $var);
        $val = isset($row[$var]) ? str_replace($delim, $esc_delim, $row[$var]) : '';
        $rr .= $rr ? $delim . $val : $val;
      }
      $r .= $hr . "\n" . $rr;
    }
    return $r;
  }
