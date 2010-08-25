<?php

/**
 * @file
 * Query code for Virtuoso engine.
 */

class RdfdbInsertDataQuery_virtuoso extends RdfdbInsertDataQuery {
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
    return $this->connection->update_query($query, $this->queryOptions);
  }

  public function toString() {
    // Serializes prefixes in prologue if they are given. Otherwise ARC2 will
    // use the prefixes which were defined during the remote endpoint setup.
    $prologue = $this->preparePrefixes();
    //return 'SELECT * WHERE {  GRAPH ?g { ?s ?p ?o . } } LIMIT 10';
    if (!empty($this->graph)) {
      // ARC2 requires a graph no matter what.
      return $prologue . 'INSERT INTO <' . $this->graph . '> { ' . $this->triples . ' }';
    }
    else {
      return $prologue . 'INSERT INTO <http://example.org/default_graph> { ' . $this->triples . ' }';
    }
  }

  /**
   * Generic preparation and validation for an INSERT query.
   *
   * @return
   *   TRUE if the validation was successful, FALSE if not.
   */
  public function preExecute() {
    return TRUE;
  }
}

class RdfdbDeleteDataQuery_virtuoso extends RdfdbDeleteDataQuery {
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
    return $this->connection->update_query($query, $this->queryOptions);
  }

  public function toString() {
    // Serializes prefixes in prologue if they are given. Otherwise ARC2 will
    // use the prefixes which were defined during the remote endpoint setup.
    $prologue = $this->preparePrefixes();
    //return 'SELECT * WHERE {  GRAPH ?g { ?s ?p ?o . } } LIMIT 10';
    if (!empty($this->graph)) {
      return $prologue . 'DELETE FROM <' . $this->graph . '> { ' . $this->triples . ' }';
    }
    else {
      return $prologue . 'DELETE { ' . $this->triples . ' }';
    }
  }

  /**
   * Generic preparation and validation for an INSERT query.
   *
   * @return
   *   TRUE if the validation was successful, FALSE if not.
   */
  public function preExecute() {
    return TRUE;
  }
}

class RdfdbClearQuery_virtuoso extends RdfdbClearQuery {

  // This method only exist because of the zombie triples bug in ARC2
  // @todo remove once this bug has been fixed.
  public function execute() {
    // If validation fails, simply return NULL.
    // Note that validation routines in preExecute() may throw exceptions instead.
    if (!$this->preExecute()) {
      return NULL;
    }

    // The zombie bug in ARC2 causes the triples to reappear after attempting
    // to clear the whole store with DELETE { ?s ?p ?o . } WHERE { ?s ?p ?o . }
    if (empty($this->graph)) {
      $rs = rdfdb_select('DISTINCT ?g')->where('GRAPH ?g { ?s ?p ?o . } ')->execute();
      foreach ($rs['result']['rows'] as $row) {
        rdfdb_clear($row['g'])->execute();
      }
      return;
    }

    $query = $this->toString();
    //var_dump($query);
    return $this->connection->update_query($query, $this->queryOptions);
  }



  public function toString() {
    // Serializes prefixes in prologue if they are given. Otherwise ARC2 will
    // use the prefixes which were defined during the remote endpoint setup.
    $prologue = $this->preparePrefixes();
    if (!empty($this->graph)) {
      return $prologue . ' CLEAR GRAPH <' . $this->graph . '>';
    }
    else {
      throw new Exception('The graph to clear must be provided');
    }
  }
}
