<?php
require_once('simpletest/autorun.php');
require_once('../rdfdb.inc');
require_once('../arc/ARC2.php');
require_once('../http.inc');

$rdfdb_connection = array(
  'driver' => 'virtuoso',
  'endpoint' => 'http://localhost:18890/sparql',
  'apikey' => '',
);

Rdfdb::addConnectionInfo('default', 'default', $rdfdb_connection);
rdfdb_set_active('defaut');


class TestRdfdb_arc2 extends UnitTestCase {


  function testInsertData() {
    // Start with clean store.
    $this->_reset();

    // Insert some dummy data into 2 graphs.
    $graph1 = 'http://example/testClearGraph1';
    $rs = $this->_insertData($graph1, '<http://example/book1>  <http://ex.org/title>  "Fundamentals of Compiler Design" . <http://example/book1>  <http://ex.org/author>  "Bob" .', 2);

    // Check the content of the store.
    $expected_tsv = '
http://example/book1	http://ex.org/author	Bob
http://example/book1	http://ex.org/title	Fundamentals of Compiler Design';
    $sorted_rs = explode("\n", $this->dumpTSV());
    sort($sorted_rs);
    $sorted_rs = implode("\n", $sorted_rs);
    $this->assertEqual($sorted_rs, $expected_tsv, 'The data has been inserted in the store [%s]');

  }



  function _insertData($graph, $triples, $expected_count = NULL, $options = array()) {
    $count = $this->countQuads();
    $rs = rdfdb_insert_data($graph, $triples)->execute();
    if ($expected_count) {
      // Find out how many triples were inserted via SPARQL querying.
      $inserted1 = $this->countQuads() - $count;
      // @todo get this bug fixed in ARC2, see
      $this->assertEqual($inserted1, $expected_count, "$expected_count triples were inserted according to SPARQL querying [%s]");
   }
  }

  /**
   * Helper functions to reset an entire store during tests. This removes all
   * triples from all graphs in the store.
   */
  function _reset($options = array()) {
    $rs = rdfdb_select('DISTINCT ?g')->where('GRAPH ?g { ?s ?p ?o . } ')->execute();
    foreach ($rs['result']['rows'] as $row) {
      if ($row['g'] != 'http://www.openlinksw.com/schemas/virtrdf#' && $row['g'] != 'http://localhost:18890/DAV') {
        rdfdb_clear($row['g'])->execute();
      }
    }
    $this->assertEqual(0, $this->countQuads(), 'The store has been cleared. [%s]');
  }





  function dumpTSV() {
    $rs = rdfdb_query('SELECT * FROM <http://example/testClearGraph1>  WHERE { {  ?s ?p ?o . } } LIMIT 100')->execute();
    return $this->SparqlToTSV($rs['result']);
  }

  function countQuads() {
    $rs = rdfdb_query('SELECT * FROM <http://example/testClearGraph1>  WHERE { {  ?s ?p ?o . } } LIMIT 100')->execute();
    return count($rs['result']['rows']);
  }

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
        //$hr .= $r ? '' : ($hr ? $delim . $var : $var);
        $val = isset($row[$var]) ? str_replace($delim, $esc_delim, $row[$var]) : '';
        $rr .= $rr ? $delim . $val : $val;
      }
      $r .= $hr . "\n" . $rr;
    }
    return $r;
  }


  /**
   * Generates a random string containing letters and numbers.
   *
   * The letters may be upper or lower case. This method is better for
   * restricted inputs that do not accept certain characters. For example,
   * when testing input fields that require machine readable values (ie without
   * spaces and non-standard characters) this method is best.
   *
   * @param $length
   *   Length of random string to generate which will be appended to $db_prefix.
   * @return
   *   Randomly generated string.
   */
  public static function randomName($length = 8) {
    global $db_prefix;

    $values = array_merge(range(65, 90), range(97, 122), range(48, 57));
    $max = count($values) - 1;
    $str = '';
    for ($i = 0; $i < $length; $i++) {
      $str .= chr($values[mt_rand(0, $max)]);
    }
    return str_replace('simpletest', 's', $db_prefix) . $str;
  }

}

