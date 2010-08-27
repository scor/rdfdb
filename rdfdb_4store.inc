<?php

/**
 * @file
 * Code for connection to 4store servers.
 */

class RdfdbConnection_4store extends RdfdbConnection {

  public function __construct(array $connection_options = array()) {
    // Gets connection info and instanciate an ARC2 remote SPARQL store.
    $rdfdb_info = Rdfdb::getConnectionInfo();
    $config = array(
      'remote_store_endpoint' => $rdfdb_info['default']['endpoint'],
      'store_write_key' => $rdfdb_info['default']['apikey'],
    );
    if (!empty($rdfdb_info['default']['namespaces'])) {
      $config['ns'] = $rdfdb_info['default']['namespaces'];
    }
    $this->store = ARC2::getRemoteStore($config);
  }

  public function query($query, array $options = array()) {
    $r = $this->store->query($query, $options);
    if ($e = $this->store->getErrors()) {
      throw new PDOException('Invalid return directive: ' . implode($e));
    }
    else {
      return $r;
    }
  }

  // For update queries we need to send custom HTTP header so we cannot use
  // ARC2 Remote Store class. @todo use ARC2 HTTP client instead of http.inc
  public function query_insert($query, array $options = array()) {
    $rdfdb_info = Rdfdb::getConnectionInfo();
    // 4store RESTful API accepts data at the /data/ endpoint.
    $data_endpoint = substr_replace($rdfdb_info['default']['endpoint'], 'data', -7, -1);
    $config = array(
      'remote_store_endpoint' => $data_endpoint,
      'store_write_key' => $rdfdb_info['default']['apikey'],
    );
    // In this case the query is just the triples to be inserted.
    $data = array(
      'graph' => $options['graph'],
      'data' => $query,
      'mime-type' => 'application/x-turtle',
    );
    $s = '';
    foreach($data as $k => $v) {
      $s .= $k . '=' . urlencode($v) . '&';
    }

    $http_options = array(
      'headers' => array('Content-Type' => 'application/x-www-form-urlencoded; charset=utf-8'),
      'method' => 'POST',
      'data' => $s,
    );
    return http_request($data_endpoint, $http_options);
  }

  // For update queries we need to send custom HTTP header so we cannot use
  // ARC2 Remote Store class. @todo use ARC2 HTTP client instead of http.inc
  public function query_clear($query, array $options = array()) {
    $rdfdb_info = Rdfdb::getConnectionInfo();
    // 4store RESTful API accepts data at the /data/ endpoint.
    $data_endpoint = substr_replace($rdfdb_info['default']['endpoint'], 'data', -7, -1);
    $config = array(
      'remote_store_endpoint' => $data_endpoint,
      'store_write_key' => $rdfdb_info['default']['apikey'],
    );
    // In this case the query is the graph to be deleted.
    $graph = $query;
    $http_options = array(
      'method' => 'DELETE',
    );
    return http_request($rdfdb_info['default']['endpoint'] . $graph, $http_options);
  }

  public function driver() {
    return '4store';
  }

  public function databaseType() {
    return '4store';
  }

}

