<?php

/**
 * @file
 * Query code for ARC2 engine.
 */

class RdfdbInsertDataQuery_arc2 extends RdfdbInsertDataQuery {
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

    $query = $this->toString();
    $this->connection->query($query, $this->queryOptions);
  }

  public function toString() {
    // Serializes prefixes in prologue if they are given. Otherwise ARC2 will
    // use the prefixes which were defined during the remote endpoint setup.
    $prologue = $this->preparePrefixes();
    if (!empty($this->graph)) {
      return $prologue . 'INSERT INTO <' . $this->graph . '> { ' . $this->triples . ' }';
    }
    else {
      // ARC2 requires a graph for insertion no matter what.
      return $prologue . 'INSERT INTO <http://example.org/default_graph> { ' . $this->triples . ' }';
    }
  }
}

class RdfdbDeleteDataQuery_arc2 extends RdfdbDeleteDataQuery {
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
    //var_dump($query);
    $this->connection->query($query, $this->queryOptions);
  }

  public function toString() {
    // Serializes prefixes in prologue if they are given. Otherwise ARC2 will
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

class RdfdbClearQuery_arc2 extends RdfdbClearQuery {

  // This method only exist because of the zombie triples bug in ARC2
  // @todo remove once this bug has been fixed.
  public function execute() {
    // If validation fails, simply return NULL.
    // Note that validation routines in preExecute() may throw exceptions instead.
    if (!$this->preExecute()) {
      return NULL;
    }

    // The zombie bug in ARC2 causes the triples to reappear after attempting
    // to delete them. Deletes the graphs one by one as a work around.
    if (empty($this->graph)) {
      $rs = rdfdb_select('DISTINCT ?g')->where('GRAPH ?g { ?s ?p ?o . } ')->execute();
      foreach ($rs['result']['rows'] as $row) {
        rdfdb_clear($row['g'])->execute();
      }
      return;
    }

    $query = $this->toString();
    //var_dump($query);
    return $this->connection->query($query, $this->queryOptions);
  }



  public function toString() {
    // Serializes prefixes in prologue if they are given. Otherwise ARC2 will
    // use the prefixes which were defined during the remote endpoint setup.
    $prologue = $this->preparePrefixes();
    if (!empty($this->graph)) {
      return $prologue . ' DELETE FROM <' . $this->graph . '> { ?s ?p ?o . } WHERE { ?s ?p ?o . }';
    }
    else {
      // This is not reliable and will be overridden until the zombie bug
      // has been fixed in ARC2.
      // @see $this->execute()
      return $prologue . ' DELETE { ?s ?p ?o . } WHERE { ?s ?p ?o . }';
    }
  }
}
