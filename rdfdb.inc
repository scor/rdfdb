<?php

/**
 * @file
 * Core systems for the RDF database layer.
 *
 * Classes required for basic functioning of the database system should be
 * placed in this file.  All utility functions should also be placed in this
 * file only, as they cannot auto-load the way classes can.
 */

/**
 * Allow the use of different database servers using the same code base.
 *
 * This library provides an RDF store abstraction layer to provide developers
 * with the ability to support multiple database servers easily. The intent of
 * this layer is to preserve the syntax and power of SPARQL 1.1 as much as
 * possible, but also allow developers a way to leverage more complex
 * functionality in a unified way. It also provides a structured interface for
 * dynamically constructing queries when appropriate, and enforcing security
 * checks and similar good practices.
 */

require_once realpath(dirname(__FILE__)) . '/rdfdb.query.inc';


/**
 * Base RDF Database API class.
 *
 * Every database driver implementation must provide a concrete implementation
 * of this class to support special handling for the RDF store in use.
 */
abstract class RdfdbConnection {

  /**
   * The database target this connection is for.
   *
   * We need this information for later auditing and logging.
   *
   * @var string
   */
  protected $target = NULL;

  /**
   * The current database logging object for this connection.
   *
   * @var DatabaseLog
   */
  protected $logger = NULL;

  /**
   * Index of what driver-specific class to use for various operations.
   *
   * @var array
   */
  protected $driverClasses = array();

  /**
   * The name of the Statement class for this connection.
   *
   * @var string
   */
  //protected $statementClass = 'DatabaseStatementBase';

  /**
   * An index used to generate unique temporary graphs.
   *
   * @var integer
   */
  protected $temporaryGraphIndex = 0;

  /**
   * An object representing the remote store.
   *
   * @var integer
   */
  protected $store = NULL;

  /**
   * The connection information for this connection object.
   *
   * @var array
   */
  protected $connectionOptions = array();

  function __construct($dsn, $username, $password, $driver_options = array()) {
  }

  /**
   * Returns the default query options for any given query.
   *
   * A given query can be customized with a number of option flags in an
   * associative array:
   * - target: The database "target" against which to execute a query. Valid
   *   values are "default" or "slave". The system will first try to open a
   *   connection to a database specified with the user-supplied key. If one
   *   is not available, it will silently fall back to the "default" target.
   *   If multiple databases connections are specified with the same target,
   *   one will be selected at random for the duration of the request.
   * - throw_exception: By default, the database system will catch any errors
   *   on a query as an Exception, log it, and then rethrow it so that code
   *   further up the call chain can take an appropriate action. To suppress
   *   that behavior and simply return NULL on failure, set this option to
   *   FALSE.
   *
   * @return
   *   An array of default query options.
   */
  protected function defaultOptions() {
    return array(
      'target' => 'default',
      'throw_exception' => TRUE,
    );
  }

  /**
   * Returns the connection information for this connection object.
   *
   * Note that Rdfdb::getConnectionInfo() is for requesting information
   * about an arbitrary database connection that is defined. This method
   * is for requesting the connection information of this specific
   * open connection object.
   *
   * @return
   *   An array of the connection information. The exact list of
   *   properties is driver-dependent.
   */
  public function getConnectionOptions() {
    return $this->connectionOptions;
  }

  /**
   * Prepares a query string and returns the prepared statement.
   *
   * This method caches prepared statements, reusing them when
   * possible.
   *
   * @param $query
   *   The query string as SQL, with curly-braces surrounding the
   *   table names.
   *
   * @return DatabaseStatementInterface
   *   A PDO prepared statement ready for its execute() method.
   */
  public function prepareQuery($query) {

    // Call PDO::prepare.
    //return parent::prepare($query);
  }

  /**
   * Tells this connection object what its target value is.
   *
   * This is needed for logging and auditing. It's sloppy to do in the
   * constructor because the constructor for child classes has a different
   * signature. We therefore also ensure that this function is only ever
   * called once.
   *
   * @param $target
   *   The target this connection is for. Set to NULL (default) to disable
   *   logging entirely.
   */
  public function setTarget($target = NULL) {
    if (!isset($this->target)) {
      $this->target = $target;
    }
  }

  /**
   * Returns the target this connection is associated with.
   *
   * @return
   *   The target string of this connection.
   */
  public function getTarget() {
    return $this->target;
  }

  /**
   * Associates a logging object with this connection.
   *
   * @param $logger
   *   The logging object we want to use.
   */
  public function setLogger(DatabaseLog $logger) {
    $this->logger = $logger;
  }

  /**
   * Gets the current logging object for this connection.
   *
   * @return DatabaseLog
   *   The current logging object for this connection. If there isn't one,
   *   NULL is returned.
   */
  public function getLogger() {
    return $this->logger;
  }

  /**
   * Executes a query string against the database.
   *
   * This method provides a central handler for the actual execution of every
   * query.
   *
   * @param $query
   *   The query to execute. In most cases this will be a string containing
   *   a SPARQL query with placeholders.
   * @param $args
   *   An array of arguments for the prepared statement. If the prepared
   *   statement uses ? placeholders, this array must be an indexed array.
   *   If it contains named placeholders, it must be an associative array.
   * @param $options
   *   An associative array of options to control how the query is run. See
   *   the documentation for RdfdbConnection::defaultOptions() for details.
   *
   * @return DatabaseStatementInterface
   *   This method will return one of: the executed statement, the number of
   *   rows affected by the query (not the number matched), or the generated
   *   insert IT of the last query, depending on the value of
   *   $options['return']. Typically that value will be set by default or a
   *   query builder and should not be set by a user. If there is an error,
   *   this method will return NULL and may throw an exception if
   *   $options['throw_exception'] is TRUE.
   */
  public function query($query, array $args = array(), $options = array()) {

    // Use default values if not already set.
    $options += $this->defaultOptions();

    try {
      // We allow either a pre-bound statement object or a literal string.
      // In either case, we want to end up with an executed statement object,
      // which we pass to PDOStatement::execute.
      if ($query instanceof DatabaseStatementInterface) {
        $stmt = $query;
        $stmt->execute(NULL, $options);
      }
      else {
        $stmt = $this->prepareQuery($query);
        $stmt->execute($args, $options);
      }

      // Depending on the type of query we may need to return a different value.
      // See RdfdbConnection::defaultOptions() for a description of each
      // value.
      switch ($options['return']) {
        case Rdfdb::RETURN_STATEMENT:
          return $stmt;
        case Rdfdb::RETURN_AFFECTED:
          return $stmt->rowCount();
        case Rdfdb::RETURN_INSERT_ID:
          return $this->lastInsertId();
        case Rdfdb::RETURN_NULL:
          return;
        default:
          throw new PDOException('Invalid return directive: ' . $options['return']);
      }
    }
    catch (PDOException $e) {
      if ($options['throw_exception']) {
        // Add additional debug information.
        if ($query instanceof DatabaseStatementInterface) {
          $e->query_string = $stmt->getQueryString();
        }
        else {
          $e->query_string = $query;
        }
        $e->args = $args;
        throw $e;
      }
      return NULL;
    }
  }

  /**
   * Gets the driver-specific override class if any for the specified class.
   *
   * @param string $class
   *   The class for which we want the potentially driver-specific class.
   * @return string
   *   The name of the class that should be used for this driver.
   */
  public function getDriverClass($class) {
    if (empty($this->driverClasses[$class])) {
      $this->driverClasses[$class] = $class . '_' . $this->driver();
      if (!class_exists($this->driverClasses[$class])) {
        $this->driverClasses[$class] = $class;
      }
    }
    return $this->driverClasses[$class];
  }

  /**
   * Prepares and returns a query object.
   *
   * @param $options
   *   An array of options on the query.
   *
   * @return SelectCustomQuery
   *   A new SelectCustomQuery object.
   */
  public function selectCustom($vars, array $options = array()) {
    $class = $this->getDriverClass('RdfdbSelectCustomQuery');
    return new $class($this, $vars, $options);
  }

  /**
   * Prepares and returns a SELECT query object.
   *
   * @param $options
   *   An array of options on the query.
   *
   * @return SelectQuery
   *   A new SelectQuery object.
   */
  public function select($vars, array $options = array()) {
    $class = $this->getDriverClass('RdfdbSelectQuery');
    return new $class($this, $vars, $options);
  }

  /**
   * Prepares and returns an INSERT DATA query object.
   *
   * @param $options
   *   An array of options on the query.
   *
   * @return InsertQuery
   *   A new InsertQuery object.
   */
  public function insertData($graph, $triples, array $options = array()) {
    $class = $this->getDriverClass('RdfdbInsertDataQuery');
    return new $class($this, $graph, $triples, $options);
  }

  /**
   * Prepares and returns a DELETE DATA query object.
   *
   * @param $options
   *   An array of options on the query.
   *
   * @return DeleteQuery
   *   A new DeleteQuery object.
   */
  public function deleteData($graph, $triples, array $options = array()) {
    $class = $this->getDriverClass('RdfdbDeleteDataQuery');
    return new $class($this, $graph, $triples, $options);
  }

  /**
   * Prepares and returns a CLEAR query object.
   */
  public function clear($graph, array $options = array()) {
    $class = $this->getDriverClass('RdfdbClearQuery');
    return new $class($this, $graph, $options);
  }

  /**
   * Generates a temporary table name.
   *
   * @return
   *   A graph name.
   */
  protected function generateTemporaryGraphName() {
    return "graph_temporary_" . $this->temporaryGraphIndex++;
  }

  /**
   * Returns the type of database driver.
   *
   * This is not necessarily the same as the type of the database itself. For
   * instance, there could be two MySQL drivers, mysql and mysql_mock. This
   * function would return different values for each, but both would return
   * "mysql" for databaseType().
   */
  abstract public function driver();

  /**
   * Returns the type of the database being accessed.
   */
  abstract public function databaseType();
}

/**
 * Primary front-controller for the RDF database system.
 *
 * This class is uninstantiatable and un-extendable. It acts to encapsulate
 * all control and shepherding of database connections into a single location
 * without the use of globals.
 */
abstract class Rdfdb {

  /**
   * Flag to indicate a query call should simply return NULL.
   *
   * This is used for queries that have no reasonable return value anyway, such
   * as INSERT statements to a table without a serial primary key.
   */
  const RETURN_NULL = 0;

  /**
   * Flag to indicate a query call should return the prepared statement.
   */
  const RETURN_STATEMENT = 1;

  /**
   * Flag to indicate a query call should return the number of affected rows.
   */
  const RETURN_AFFECTED = 2;

  /**
   * Flag to indicate a query call should return the "last insert id".
   */
  const RETURN_INSERT_ID = 3;

  /**
   * An nested array of all active connections. It is keyed by database name
   * and target.
   *
   * @var array
   */
  static protected $connections = array();

  /**
   * A processed copy of the database connection information.
   *
   * @var array
   */
  static protected $databaseInfo = NULL;

  /**
   * A list of key/target credentials to simply ignore.
   *
   * @var array
   */
  static protected $ignoreTargets = array();

  /**
   * The key of the currently active database connection.
   *
   * @var string
   */
  static protected $activeKey = 'default';

  /**
   * An array of active query log objects.
   *
   * Every connection has one and only one logger object for all targets and
   * logging keys.
   *
   * array(
   *   '$db_key' => DatabaseLog object.
   * );
   *
   * @var array
   */
  static protected $logs = array();

  /**
   * Starts logging a given logging key on the specified connection.
   *
   * @param $logging_key
   *   The logging key to log.
   * @param $key
   *   The database connection key for which we want to log.
   *
   * @return DatabaseLog
   *   The query log object. Note that the log object does support richer
   *   methods than the few exposed through the Database class, so in some
   *   cases it may be desirable to access it directly.
   *
   * @see DatabaseLog
   */
  final public static function startLog($logging_key, $key = 'default') {
    if (empty(self::$logs[$key])) {
      self::$logs[$key] = new DatabaseLog($key);

      // Every target already active for this connection key needs to have the
      // logging object associated with it.
      if (!empty(self::$connections[$key])) {
        foreach (self::$connections[$key] as $connection) {
          $connection->setLogger(self::$logs[$key]);
        }
      }
    }

    self::$logs[$key]->start($logging_key);
    return self::$logs[$key];
  }

  /**
   * Retrieves the queries logged on for given logging key.
   *
   * This method also ends logging for the specified key. To get the query log
   * to date without ending the logger request the logging object by starting
   * it again (which does nothing to an open log key) and call methods on it as
   * desired.
   *
   * @param $logging_key
   *   The logging key to log.
   * @param $key
   *   The database connection key for which we want to log.
   *
   * @return array
   *   The query log for the specified logging key and connection.
   *
   * @see DatabaseLog
   */
  final public static function getLog($logging_key, $key = 'default') {
    if (empty(self::$logs[$key])) {
      return NULL;
    }
    $queries = self::$logs[$key]->get($logging_key);
    self::$logs[$key]->end($logging_key);
    return $queries;
  }

  /**
   * Gets the connection object for the specified database key and target.
   *
   * @param $target
   *   The database target name.
   * @param $key
   *   The database connection key. Defaults to NULL which means the active key.
   * @param $type
   *   The database connection type. Some driver implementation might need to
   *   use a custom HTTP client in the case of updates for example. The default
   *   uses the ARC2 RemoteStore class which works for most SELECT queries.
   *
   * @return RdfdbConnection
   *   The corresponding connection object.
   */
  final public static function getConnection($target = 'default', $key = NULL, $type = 'default') {
    if (!isset($key)) {
      // By default, we want the active connection, set in setActiveConnection.
      $key = self::$activeKey;
    }
    // If the requested target does not exist, or if it is ignored, we fall back
    // to the default target. The target is typically either "default" or
    // "slave", indicating to use a slave server if one is available. If
    // it's not available, then the default/master server is the correct server
    // to use.
    if (!empty(self::$ignoreTargets[$key][$target]) || !isset(self::$databaseInfo[$key][$target])) {
      $target = 'default';
    }

    if (!isset(self::$connections[$key][$target])) {
      // If necessary, a new connection is opened.
      self::$connections[$key][$target] = self::openConnection($key, $target);
    }
    return self::$connections[$key][$target];
  }

  /**
   * Determines if there is an active connection.
   *
   * Note that this method will return FALSE if no connection has been
   * established yet, even if one could be.
   *
   * @return
   *   TRUE if there is at least one database connection established, FALSE
   *   otherwise.
   */
  final public static function isActiveConnection() {
    return !empty(self::$activeKey) && !empty(self::$connections) && !empty(self::$connections[self::$activeKey]);
  }

  /**
   * Sets the active connection to the specified key.
   *
   * @return
   *   The previous database connection key.
   */
  final public static function setActiveConnection($key = 'default') {
    if (empty(self::$databaseInfo)) {
      self::parseConnectionInfo();
    }

    if (!empty(self::$databaseInfo[$key])) {
      $old_key = self::$activeKey;
      self::$activeKey = $key;
      return $old_key;
    }
  }

  /**
   * Process the configuration file for database information.
   */
  final public static function parseConnectionInfo() {
    global $databases;

    $database_info = is_array($databases) ? $databases : array();
    foreach ($database_info as $index => $info) {
      foreach ($database_info[$index] as $target => $value) {
        // If there is no "driver" property, then we assume it's an array of
        // possible connections for this target. Pick one at random. That allows
        //  us to have, for example, multiple slave servers.
        if (empty($value['driver'])) {
          $database_info[$index][$target] = $database_info[$index][$target][mt_rand(0, count($database_info[$index][$target]) - 1)];
        }
      }
    }

    if (!is_array(self::$databaseInfo)) {
      self::$databaseInfo = $database_info;
    }

    // Merge the new $database_info into the existing.
    // array_merge_recursive() cannot be used, as it would make multiple
    // database, user, and password keys in the same database array.
    else {
      foreach ($database_info as $database_key => $database_values) {
        foreach ($database_values as $target => $target_values) {
          self::$databaseInfo[$database_key][$target] = $target_values;
        }
      }
    }
  }

  /**
   * Adds database connection information for a given key/target.
   *
   * This method allows the addition of new connection credentials at runtime.
   * Under normal circumstances the preferred way to specify database
   * credentials is via settings.php. However, this method allows them to be
   * added at arbitrary times, such as during unit tests, when connecting to
   * admin-defined third party databases, etc.
   *
   * If the given key/target pair already exists, this method will be ignored.
   *
   * @param $key
   *   The database key.
   * @param $target
   *   The database target name.
   * @param $info
   *   The database connection information, as it would be defined in
   *   settings.php. Note that the structure of this array will depend on the
   *   database driver it is connecting to.
   */
  public static function addConnectionInfo($key, $target, $info) {
    if (empty(self::$databaseInfo[$key][$target])) {
      self::$databaseInfo[$key][$target] = $info;
      // Includes driver files.
      require_once realpath(dirname(__FILE__)) . '/rdfdb.query_' . $info['driver'] . '.inc';
    }
  }

  /**
   * Gets information on the specified database connection.
   *
   * @param $connection
   *   The connection key for which we want information.
   */
  final public static function getConnectionInfo($key = 'default') {
    if (empty(self::$databaseInfo)) {
      self::parseConnectionInfo();
    }

    if (!empty(self::$databaseInfo[$key])) {
      return self::$databaseInfo[$key];
    }
  }

  /**
   * Rename a connection and its corresponding connection information.
   *
   * @param $old_key
   *   The old connection key.
   * @param $new_key
   *   The new connection key.
   * @return
   *   TRUE in case of success, FALSE otherwise.
   */
  final public static function renameConnection($old_key, $new_key) {
    if (empty(self::$databaseInfo)) {
      self::parseConnectionInfo();
    }

    if (!empty(self::$databaseInfo[$old_key]) && empty(self::$databaseInfo[$new_key])) {
      // Migrate the database connection information.
      self::$databaseInfo[$new_key] = self::$databaseInfo[$old_key];
      unset(self::$databaseInfo[$old_key]);

      // Migrate over the RdfdbConnection object if it exists.
      if (isset(self::$connections[$old_key])) {
        self::$connections[$new_key] = self::$connections[$old_key];
        unset(self::$connections[$old_key]);
      }

      return TRUE;
    }
    else {
      return FALSE;
    }
  }

  /**
   * Remove a connection and its corresponding connection information.
   *
   * @param $key
   *   The connection key.
   * @return
   *   TRUE in case of success, FALSE otherwise.
   */
  final public static function removeConnection($key) {
    if (isset(self::$databaseInfo[$key])) {
      unset(self::$databaseInfo[$key]);
      unset(self::$connections[$key]);
      return TRUE;
    }
    else {
      return FALSE;
    }
  }

  /**
   * Opens a connection to the server specified by the given key and target.
   *
   * @param $key
   *   The database connection key, as specified in settings.php. The default is
   *   "default".
   * @param $target
   *   The database target to open.
   */
  final protected static function openConnection($key, $target) {
    // If the requested database does not exist then it is an unrecoverable
    // error.
    if (!isset(self::$databaseInfo[$key])) {
      throw new RdfdbConnectionNotDefinedException('The specified database connection is not defined: ' . $key);
    }

    if (!$driver = self::$databaseInfo[$key][$target]['driver']) {
      throw new RdfdbDriverNotSpecifiedException('Driver not specified for this database connection: ' . $key);
    }

    $driver_class = 'RdfdbConnection_' . $driver;
    require_once 'rdfdb_' . $driver . '.inc';
    $new_connection = new $driver_class(self::$databaseInfo[$key][$target]);
    $new_connection->setTarget($target);

    // If we have any active logging objects for this connection key, we need
    // to associate them with the connection we just opened.
    if (!empty(self::$logs[$key])) {
      $new_connection->setLogger(self::$logs[$key]);
    }

    return $new_connection;
  }

  /**
   * Closes a connection to the server specified by the given key and target.
   *
   * @param $target
   *   The database target name.  Defaults to NULL meaning that all target
   *   connections will be closed.
   * @param $key
   *   The database connection key. Defaults to NULL which means the active key.
   */
  public static function closeConnection($target = NULL, $key = NULL) {
    // Gets the active connection by default.
    if (!isset($key)) {
      $key = self::$activeKey;
    }
    // To close the connection, we need to unset the static variable.
    if (isset($target)) {
      unset(self::$connections[$key][$target]);
    }
    else {
      unset(self::$connections[$key]);
    }
  }

  /**
   * Instructs the system to temporarily ignore a given key/target.
   *
   * At times we need to temporarily disable slave queries. To do so, call this
   * method with the database key and the target to disable. That database key
   * will then always fall back to 'default' for that key, even if it's defined.
   *
   * @param $key
   *   The database connection key.
   * @param $target
   *   The target of the specified key to ignore.
   */
  public static function ignoreTarget($key, $target) {
    self::$ignoreTargets[$key][$target] = TRUE;
  }

}

/**
 * Exception thrown if an undefined database connection is requested.
 */
class RdfdbConnectionNotDefinedException extends Exception {}

/**
 * Exception thrown if no driver is specified for a database connection.
 */
class RdfdbDriverNotSpecifiedException extends Exception {}


/**
 * The following utility functions are simply convenience wrappers.
 *
 * They should not have any database-specific code in them.
 */

/**
 * Returns a new SelectQuery object for the active database.
 *
 * @param $vars
 *   The variables of the SPARQL query as a string.
 * @param $options
 *   An array of options to control how the query operates.
 *
 * @return RdfdbSelectQuery
 *   A new RdfdbSelectQuery object for this connection.
 */
function rdfdb_select($vars, array $options = array()) {
  if (empty($options['target'])) {
    $options['target'] = 'default';
  }
  //var_dump($options);
  return Rdfdb::getConnection($options['target'])->select($vars, $options);
}

/**
 * Returns a new InsertQuery object for the active database.
 *
 * @param $graph
 *   The graph into which to insert.
 * @param $triples
 *   Triples to be inserted.
 *   @todo allow for ARC2 document to be seralized later in toString().
 * @param $options
 *   An array of options to control how the query operates.
 *
 * @return RdfdbInsertDataQuery
 *   A new RdfdbInsertDataQuery object for this connection.
 */
function rdfdb_insert_data($graph = NULL, $triples, array $options = array()) {
  if (empty($options['target']) || $options['target'] == 'slave') {
    $options['target'] = 'default';
  }
  return Rdfdb::getConnection($options['target'], NULL, 'update')->insertData($graph, $triples, $options);
}

/**
 * Returns a new DeleteQuery object for the active database.
 *
 * @param $graph
 *   The graph into which to delete.
 * @param $triples
 *   Triples to be deleted.
 *   @todo allow for ARC2 document to be seralized later in toString().
 * @param $options
 *   An array of options to control how the query operates.
 *
 * @return RdfdbDeleteDataQuery
 *   A new RdfdbDeleteDataQuery object for this connection.
 */
function rdfdb_delete_data($graph = NULL, $triples, array $options = array()) {
  if (empty($options['target']) || $options['target'] == 'slave') {
    $options['target'] = 'default';
  }
  return Rdfdb::getConnection($options['target'], NULL, 'update')->deleteData($graph, $triples, $options);
}

/**
 * Returns a new ClearQuery object for the active database.
 *
 * @param $graph
 *   The graph to clear.
 * @param $options
 *   An array of options to control how the query operates.
 *
 * @return ClearQuery
 *   A new ClearQuery object for this connection.
 */
function rdfdb_clear($graph = NULL, array $options = array()) {
  if (empty($options['target']) || $options['target'] == 'slave') {
    $options['target'] = 'default';
  }
  return Rdfdb::getConnection($options['target'], NULL, 'update')->Clear($graph, $options);
}

/**
 * Returns a new SelectCustomQuery object for the active database.
 *
 * This function should only be used for SELECT, CONSTRUCT, ASK or DESCRIBE
 * queries. The query will be sent as is to the SPARQL endpoint.
 *
 * @param $query
 *   The SPARQL query.
 * @param $options
 *   An array of options to control how the query operates.
 *
 * @return SelectCustomQuery
 *   A new SelectCustomQuery object for this connection.
 */
function rdfdb_query($query, array $options = array()) {
  if (empty($options['target'])) {
    $options['target'] = 'default';
  }
  //var_dump($options);
  return Rdfdb::getConnection($options['target'])->selectCustom($query, $options);
}


/**
 * Sets a new active database.
 *
 * @param $key
 *   The key in the $databases array to set as the default database.
 *
 * @return
 *   The key of the formerly active database.
 */
function rdfdb_set_active($key = 'default') {
  return Rdfdb::setActiveConnection($key);
}

/**
 * Retrieves the name of the currently active database driver.
 *
 * @return
 *   The name of the currently active database driver.
 */
function rdfdb_driver() {
  return Rdfdb::getConnection()->driver();
}

/**
 * Closes the active database connection.
 *
 * @param $options
 *   An array of options to control which connection is closed. Only the target
 *   key has any meaning in this case.
 */
function rdfdb_close(array $options = array()) {
  if (empty($options['target'])) {
    $options['target'] = NULL;
  }
  Rdfdb::closeConnection($options['target']);
}

/**
 * Sets a session variable specifying the lag time for ignoring a slave server.
 */
function rdfdb_ignore_slave() {
  $connection_info = Rdfdb::getConnectionInfo();
  // Only set ignore_slave_server if there are slave servers being used, which
  // is assumed if there are more than one.
  if (count($connection_info) > 1) {
    $duration = variable_get('maximum_replication_lag', 300);
    // Set session variable with amount of time to delay before using slave.
    $_SESSION['ignore_slave_server'] = REQUEST_TIME + $duration;
  }
}
