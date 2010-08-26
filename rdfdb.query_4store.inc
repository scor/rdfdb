<?php

/**
 * @file
 * Query code for 4store engine.
 */

class RdfdbInsertDataQuery_4store extends RdfdbInsertDataQuery {
  /**
   * Executes the insert data query.
   *
   * @return
   *   The number of inserted triples, or FALSE is there were errors.
   */
  public function execute() {
    // If validation fails, simply return NULL.
    // Note that validation routines in preExecute() may throw exceptions instead.
    if (!$this->preExecute()) {
      return NULL;
    }
    // In this case, the query is just the triples in turtle format.
    $query = $this->triples;
    $this->queryOptions['graph'] = $this->graph;
    return $this->connection->query_insert($query, $this->queryOptions);
  }

}

class RdfdbDeleteDataQuery_4store extends RdfdbDeleteDataQuery {
  /**
   * Executes the delete data query.
   *
   * @return
   *   The number of deleted triples, or FALSE is there were errors.
   */
  public function execute() {
    // If validation fails, simply return NULL.
    // Note that validation routines in preExecute() may throw exceptions instead.
    if (!$this->preExecute()) {
      return NULL;
    }

    $query = $this->toString();
    $this->connection->query($query, $this->queryOptions);
  }

  public function toString() {
    // Serializes prefixes in prologue if they are given. Otherwise 4store will
    // use the prefixes which were defined during the remote endpoint setup.
    $prologue = $this->preparePrefixes();
    if (!empty($this->graph)) {
      return $prologue . 'DELETE FROM <' . $this->graph . '> { ' . $this->triples . ' }';
    }
    else {
      return $prologue . 'DELETE { ' . $this->triples . ' }';
    }
  }
}

class RdfdbClearQuery_4store extends RdfdbClearQuery {

  // This method only exist because of the zombie triples bug in 4store
  // @todo remove once this bug has been fixed.
  public function execute() {
    // If validation fails, simply return NULL.
    // Note that validation routines in preExecute() may throw exceptions instead.
    if (!$this->preExecute()) {
      return NULL;
    }

    // Deletes the graphs one by one.
    if (empty($this->graph)) {
      $rs = rdfdb_select('DISTINCT ?g')->where('GRAPH ?g { ?s ?p ?o . } ')->execute();
      foreach ($rs['result']['rows'] as $row) {
        rdfdb_clear($row['g'])->execute();
      }
      return;
    }

    $query = $this->graph;
    return $this->connection->query_clear($query, $this->queryOptions);
  }

}

