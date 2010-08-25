<?php
require_once('simpletest/autorun.php');
require_once('../rdfdb.inc');
require_once('../arc/ARC2.php');

$rdfdb_connection = array(
  'driver' => 'arc2',
  'endpoint' => 'http://localhost/arc2_endpoint.php',
  'apikey' => 'somekey',
);

Rdfdb::addConnectionInfo('default', 'default', $rdfdb_connection);
rdfdb_set_active('defaut');


class TestRdfdb_arc2 extends UnitTestCase {

  function testClearGraph() {
    // Start with clean store.
    $this->_clear();

    // Insert some dummy data into 2 graphs.
    $graph1 = 'http://example/testClearGraph1';
    $rs = $this->_insertData($graph1, '<http://example/book1>  <http://ex.org/title>  "Fundamentals of Compiler Design" . <http://example/book1>  <http://ex.org/author>  "Bob" .', 2);
    $graph2 = 'http://example/testClearGraph2';
    $rs = $this->_insertData($graph2, '<http://example/book2>  <http://ex.org/title>  "Fundamentals of Compiler Design" . <http://example/book2>  <http://ex.org/author>  "Bob" .', 2);

    // Check the content of the store.
    $expected_tsv = '
http://example/testClearGraph1	http://example/book1	http://ex.org/author	Bob
http://example/testClearGraph1	http://example/book1	http://ex.org/title	Fundamentals of Compiler Design
http://example/testClearGraph2	http://example/book2	http://ex.org/author	Bob
http://example/testClearGraph2	http://example/book2	http://ex.org/title	Fundamentals of Compiler Design';
    $sorted_rs = explode("\n", $this->dumpTSV());
    sort($sorted_rs);
    $sorted_rs = implode("\n", $sorted_rs);
    $this->assertEqual($sorted_rs, $expected_tsv, 'The data has been inserted in the store [%s]');

    // Clear one of the graphs and check the content of the store.
    rdfdb_clear('http://example/testClearGraph1')->execute();
    $expected_tsv = '
http://example/testClearGraph2	http://example/book2	http://ex.org/author	Bob
http://example/testClearGraph2	http://example/book2	http://ex.org/title	Fundamentals of Compiler Design';
    $sorted_rs = explode("\n", $this->dumpTSV());
    sort($sorted_rs);
    $sorted_rs = implode("\n", $sorted_rs);
    $this->assertEqual($sorted_rs, $expected_tsv, 'The data has been inserted in the store [%s]');
  }

  function testClearDefaultGraph() {
    // Start with clean store.
    $this->_clear();

    // Insert some dummy data so we can clear the store afterwards.
    $graph = 'http://example/ttestClearDefaultGraph';
    $rs = $this->_insertData($graph, '<http://example/book3>  <http://ex.org/title>  "Fundamentals of Compiler Design"', 1);
    rdfdb_clear()->execute();
    $this->assertEqual(0, $this->countQuads(), 'The store is empty [%s]');

    // Insert some dummy data to test against the ARC2 zombie triples.
    // http://arc.semsol.org/community/arc-dev/archives/2010/08/AANLkTi=yOyYYSAdYzZ9-MXYedKS2_0GUMFi5od6ZgSXW@mail.gmail.com
    $graph1 = 'http://example/testClearGraph1';
    $rs = $this->_insertData($graph1, '<http://example/book1>  <http://ex.org/title>  "Fundamentals of Zombie"');
    $this->assertEqual(1, $this->countQuads(), 'No zombie triple was found after clearing the store. [%s]');
    // Leave the store clean.
    rdfdb_clear()->execute();
  }


  function testInsertData() {
    // Start with clean store.
    $this->_clear();

    // Insert some dummy data into 2 graphs.
    $graph1 = 'http://example/testClearGraph1';
    $rs = $this->_insertData($graph1, '<http://example/book1>  <http://ex.org/title>  "Fundamentals of Compiler Design" . <http://example/book1>  <http://ex.org/author>  "Bob" .', 2);
    $graph2 = 'http://example/testClearGraph2';
    $rs = $this->_insertData($graph2, '<http://example/book2>  <http://ex.org/title>  "Fundamentals of Compiler Design" . <http://example/book2>  <http://ex.org/author>  "Bob" .', 2);

    // Check the content of the store.
    $expected_tsv = '
http://example/testClearGraph1	http://example/book1	http://ex.org/author	Bob
http://example/testClearGraph1	http://example/book1	http://ex.org/title	Fundamentals of Compiler Design
http://example/testClearGraph2	http://example/book2	http://ex.org/author	Bob
http://example/testClearGraph2	http://example/book2	http://ex.org/title	Fundamentals of Compiler Design';
    $sorted_rs = explode("\n", $this->dumpTSV());
    sort($sorted_rs);
    $sorted_rs = implode("\n", $sorted_rs);
    $this->assertEqual($sorted_rs, $expected_tsv, 'The data has been inserted in the store [%s]');

    // Insert some more dummy data into the same graphs.
    $graph1 = 'http://example/testClearGraph1';
    $rs = $this->_insertData($graph1, '<http://example/book11>  <http://ex.org/title>  "Fundamentals of Programming" . <http://example/book11>  <http://ex.org/author>  "Henry" .', 2);
    $graph2 = 'http://example/testClearGraph2';
    $rs = $this->_insertData($graph1, '<http://example/book22>  <http://ex.org/title>  "Fundamentals of Programming" . <http://example/book22>  <http://ex.org/author>  "Charles" .', 2);

    // Check the content of the store.
    $expected_tsv = '
http://example/testClearGraph1	http://example/book1	http://ex.org/author	Bob
http://example/testClearGraph1	http://example/book1	http://ex.org/title	Fundamentals of Compiler Design
http://example/testClearGraph1	http://example/book11	http://ex.org/author	Henry
http://example/testClearGraph1	http://example/book11	http://ex.org/title	Fundamentals of Programming
http://example/testClearGraph1	http://example/book22	http://ex.org/author	Charles
http://example/testClearGraph1	http://example/book22	http://ex.org/title	Fundamentals of Programming
http://example/testClearGraph2	http://example/book2	http://ex.org/author	Bob
http://example/testClearGraph2	http://example/book2	http://ex.org/title	Fundamentals of Compiler Design';
    $sorted_rs = explode("\n", $this->dumpTSV());
    sort($sorted_rs);
    $sorted_rs = implode("\n", $sorted_rs);
    $this->assertEqual($sorted_rs, $expected_tsv, 'The data has been inserted in the store [%s]');
  }



  function _insertData($graph, $triples, $expected_count = NULL, $options = array()) {
    // Warning: @todo this test will fail if the data already exist.
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
   * Helper functions to clear an entire store during tests.
   */
  function _clear($options = array()) {
    rdfdb_clear()->execute();
    $this->assertEqual(0, $this->countQuads(), 'The store has been cleared. [%s]');
  }

  function dumpTSV() {
    $rs = rdfdb_select('?g ?s ?p ?o')->where('GRAPH ?g { ?s ?p ?o . } ')->execute();
    return $this->SparqlToTSV($rs['result']);
  }

  function countQuads() {
    $rs = rdfdb_select('?g ?s ?p ?o')->where('GRAPH ?g { ?s ?p ?o . } ')->execute();
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

