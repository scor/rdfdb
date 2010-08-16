<?php

/**
 * @file
 * Non-specific Database query code. Used by all engines.
 */

/**
 * Interface for a query that can be manipulated via an alter hook.
 */
interface RdfdbQueryAlterableInterface {

  /**
   * Adds a tag to a query.
   *
   * Tags are strings that identify a query. A query may have any number of
   * tags. Tags are used to mark a query so that alter hooks may decide if they
   * wish to take action. Tags should be all lower-case and contain only letters,
   * numbers, and underscore, and start with a letter. That is, they should
   * follow the same rules as PHP identifiers in general.
   *
   * @param $tag
   *   The tag to add.
   * @return QueryAlterableInterface
   *   The called object.
   */
  public function addTag($tag);

  /**
   * Determines if a given query has a given tag.
   *
   * @param $tag
   *   The tag to check.
   * @return
   *   TRUE if this query has been marked with this tag, FALSE otherwise.
   */
  public function hasTag($tag);

  /**
   * Determines if a given query has all specified tags.
   *
   * @param $tags
   *   A variable number of arguments, one for each tag to check.
   * @return
   *   TRUE if this query has been marked with all specified tags, FALSE otherwise.
   */
  public function hasAllTags();

  /**
   * Determines if a given query has any specified tag.
   *
   * @param $tags
   *   A variable number of arguments, one for each tag to check.
   * @return
   *   TRUE if this query has been marked with at least one of the specified
   *   tags, FALSE otherwise.
   */
  public function hasAnyTag();

  /**
   * Adds additional metadata to the query.
   *
   * Often, a query may need to provide additional contextual data to alter
   * hooks. Alter hooks may then use that information to decide if and how
   * to take action.
   *
   * @param $key
   *   The unique identifier for this piece of metadata. Must be a string that
   *   follows the same rules as any other PHP identifier.
   * @param $object
   *   The additional data to add to the query. May be any valid PHP variable.
   * @return QueryAlterableInterface
   *   The called object.
   */
  public function addMetaData($key, $object);

  /**
   * Retrieves a given piece of metadata.
   *
   * @param $key
   *   The unique identifier for the piece of metadata to retrieve.
   * @return
   *   The previously attached metadata object, or NULL if one doesn't exist.
   */
  public function getMetaData($key);
}

/**
 * Base class for the query builders.
 *
 * All query builders inherit from a common base class.
 */
abstract class RdfdbQuery implements RdfdbQueryAlterableInterface {

  /**
   * The connection object on which to run this query.
   *
   * @var DatabaseConnection
   */
  protected $connection;

  /**
   * The query options to pass on to the connection object.
   *
   * @var array
   */
  protected $queryOptions;

  public function __construct(RdfdbConnection $connection, $options) {
    $this->connection = $connection;
    $this->queryOptions = $options;
    //var_dump($options);
  }

  /**
   * Run the query against the database.
   */
  abstract protected function execute();

  public function preparePrefixes() {
    $prefixes = '';
    if (!empty($this->queryOptions['namespaces'])) {
      foreach ($this->queryOptions['namespaces'] as $p => $ns) {
        $prefixes .= 'PREFIX ' . $p . ': <' . $ns . ">\n";
      }
    }
    return $prefixes;
  }

  /* Implementations of QueryAlterableInterface. */

  public function addTag($tag) {
    $this->query->addTag($tag);
    return $this;
  }

  public function hasTag($tag) {
    return $this->query->hasTag($tag);
  }

  public function hasAllTags() {
    return call_user_func_array(array($this->query, 'hasAllTags', func_get_args()));
  }

  public function hasAnyTag() {
    return call_user_func_array(array($this->query, 'hasAnyTags', func_get_args()));
  }

  public function addMetaData($key, $object) {
    $this->query->addMetaData($key, $object);
    return $this;
  }

  public function getMetaData($key) {
    return $this->query->getMetaData($key);
  }

}

/**
 * General class for an abstracted SELECT query.
 */
class RdfdbSelectQuery extends RdfdbQuery {

  /**
   * The variables of the SPARQL query.
   *
   * @var array
   */
  protected $vars;

  /**
   * The Group Graph Patterns of the SPARQL query.
   *
   * @var array
   */
  protected $ggps = array();

  public function __construct($connection, $vars, array $options = array()) {
    parent::__construct($connection, $options);
    //var_dump($this);
    $this->vars = $vars;
  }

  public function where($snippet, $args = array()) {
    // Add a Group Graph Pattern to the list of ggps.
    $this->ggps[] = $snippet;
    return $this;
  }

  /**
   * Executes the select query.
   *
   * @return
   *   The last insert ID of the query, if one exists. If the query
   *   was given multiple sets of values to insert, the return value is
   *   undefined. If the query is flagged "delayed", then the insert ID
   *   won't be created until later when the query actually runs so the
   *   return value is also undefined. If no fields are specified, this
   *   method will do nothing and return NULL. That makes it safe to use
   *   in multi-insert loops.
   */
  public function execute() {
    // If validation fails, simply return NULL.
    // Note that validation routines in preExecute() may throw exceptions instead.
    if (!$this->preExecute()) {
      return NULL;
    }

    $query = $this->toString();
    //var_dump($query);
    return $this->connection->query($query, $this->queryOptions);
  }

  public function toString() {
    $prologue = $this->preparePrefixes();
    if (count($this->ggps) > 1) {
      $where = '{ ' . implode(" } \n { ", $this->ggps) . ' }';
    }
    else {
      $where = $this->ggps[0];
    }
    return $prologue . "\n" . 'SELECT ' . $this->vars . ' WHERE { ' . $where . ' } ';
  }

  public function preparePrefixes() {
//  var_dump($this);
    $prefixes = '';
    if (!empty($this->queryOptions['namespaces'])) {
      foreach ($this->queryOptions['namespaces'] as $p => $ns) {
        $prefixes .= 'PREFIX ' . $p . ': <' . $ns . ">\n";
      }
    }
    return $prefixes;
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

/**
 * General class for an abstracted INSERT DATA operation.
 */
class RdfdbInsertDataQuery extends RdfdbQuery {

  /**
   * The graph in which to insert the data.
   *
   * @var string
   */
  protected $graph;

  /**
   * The triples to be inserted.
   *
   * @var string
   */
  protected $triples;

  public function __construct($connection, $graph, $triples, array $options = array()) {
    parent::__construct($connection, $options);
    $this->graph = $graph;
    $this->triples = $triples;
  }

  /**
   * Executes the insert data query.
   *
   * @return
   *   The last insert ID of the query, if one exists. If the query
   *   was given multiple sets of values to insert, the return value is
   *   undefined. If the query is flagged "delayed", then the insert ID
   *   won't be created until later when the query actually runs so the
   *   return value is also undefined. If no fields are specified, this
   *   method will do nothing and return NULL. That makes it safe to use
   *   in multi-insert loops.
   */
  public function execute() {
    // If validation fails, simply return NULL.
    // Note that validation routines in preExecute() may throw exceptions instead.
    if (!$this->preExecute()) {
      return NULL;
    }

    $query = $this->toString();
    echo 'ss';
    //return;
//    return $this->connection->runQuery($query, $this->queryOptions);
    return $this->connection->query($query, $this->queryOptions);
//    return $this->toString();
  }

  public function toString() {
    if (!empty($this->graph)) {
      return 'INSERT DATA { GRAPH <' . $this->graph . '> { ' . $this->triples . ' } }';
    }
    else {
      return 'INSERT DATA { ' . $this->triples . ' }';
    }
  }

  public function preparePrefixes() {
    $prefixes = '';
    if (!empty($this->queryOptions['namespaces'])) {
      foreach ($this->queryOptions['namespaces'] as $p => $ns) {
        $prefixes .= 'PREFIX ' . $p . ': <' . $ns . ">\n";
      }
    }
    return $prefixes;
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


/**
 * General class for an abstracted INSERT DATA operation.
 */
class RdfdbDeleteDataQuery extends RdfdbQuery {

  /**
   * The graph in which to delete the data.
   *
   * @var string
   */
  protected $graph;

  /**
   * The triples to be deleteed.
   *
   * @var string
   */
  protected $triples;

  public function __construct($connection, $graph, $triples, array $options = array()) {
    parent::__construct($connection, $options);
    $this->graph = $graph;
    $this->triples = $triples;
  }

  /**
   * Executes the delete data query.
   *
   * @return
   *   The last delete ID of the query, if one exists. If the query
   *   was given multiple sets of values to delete, the return value is
   *   undefined. If the query is flagged "delayed", then the delete ID
   *   won't be created until later when the query actually runs so the
   *   return value is also undefined. If no fields are specified, this
   *   method will do nothing and return NULL. That makes it safe to use
   *   in multi-delete loops.
   */
  public function execute() {
    // If validation fails, simply return NULL.
    // Note that validation routines in preExecute() may throw exceptions instead.
    if (!$this->preExecute()) {
      return NULL;
    }

    return $this->toString();

  }

  public function toString() {
    if (!empty($this->graph)) {
      return 'DELETE DATA { GRAPH <' . $this->graph . '> { ' . $this->triples . ' } }';
    }
    else {
      return 'DELETE DATA { ' . $this->triples . ' }';
    }
  }

  public function preparePrefixes() {
    $prefixes = '';
    if (!empty($this->queryOptions['namespaces'])) {
      foreach ($this->queryOptions['namespaces'] as $p => $ns) {
        $prefixes .= 'PREFIX ' . $p . ': <' . $ns . ">\n";
      }
    }
    return $prefixes;
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


/**
 * General class for an abstracted CLEAR operation.
 */
class RdfdbClearQuery extends RdfdbQuery {

  /**
   * The graph clear.
   *
   * @var string
   */
  protected $graph;

  public function __construct($connection, $graph, array $options = array()) {
    parent::__construct($connection, $options);
    $this->graph = $graph;
  }

  /**
   * Executes the clear data query.
   *
   * @return
   *   The last delete ID of the query, if one exists. If the query
   *   was given multiple sets of values to delete, the return value is
   *   undefined. If the query is flagged "delayed", then the delete ID
   *   won't be created until later when the query actually runs so the
   *   return value is also undefined. If no fields are specified, this
   *   method will do nothing and return NULL. That makes it safe to use
   *   in multi-delete loops.
   */
  public function execute() {
    // If validation fails, simply return NULL.
    // Note that validation routines in preExecute() may throw exceptions instead.
    if (!$this->preExecute()) {
      return NULL;
    }

    $query = $this->toString();
    var_dump($query);
    return $this->connection->query($query, $this->queryOptions);
  }

  public function toString() {
    if (!empty($this->graph)) {
      return 'CLEAR GRAPH <' . $this->graph . '>';
    }
    else {
      return 'CLEAR GRAPH DEFAULT';
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


