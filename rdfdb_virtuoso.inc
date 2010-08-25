<?php

/**
 * @file
 * RDFDB interface code for Virtuoso servers.
 */

ARC2::getComponent('ARC2_RemoteStore');

class ARC2_RemoteStore_Rdfdb extends ARC2_RemoteStore {

  function query($q, $result_format = '', $src = '', $keep_bnode_ids = 0, $log_query = 0) {
    if ($log_query) $this->logQuery($q);
    ARC2::inc('SPARQLPlusParser');
    //$p = & new ARC2_SPARQLPlusParser($this->a, $this);
    //$p->parse($q, $src);
    //$infos = $p->getQueryInfos();
    $infos['query']['type'] = 'insert';
    $t1 = ARC2::mtime();
    //if (!$errs = $p->getErrors()) {
      $qt = $infos['query']['type'];
      $r = array('query_type' => $qt, 'result' => $this->runQuery($q, $qt, $infos));
    //}
    //else {
    //  $r = array('result' => '');
    //}
    $t2 = ARC2::mtime();
    $r['query_time'] = $t2 - $t1;
    /* query result */
    if ($result_format == 'raw') {
      return $r['result'];
    }
    if ($result_format == 'rows') {
      return $this->v('rows', array(), $r['result']);
    }
    if ($result_format == 'row') {
      if (!isset($r['result']['rows'])) return array();
      return $r['result']['rows'] ? $r['result']['rows'][0] : array();
    }
    return $r;
  }

}

class RdfdbConnection_virtuoso extends RdfdbConnection {

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
    $this->store = new ARC2_RemoteStore($config, $this);

    // Special connection for updates.
    $this->store_update = new ARC2_RemoteStore_Rdfdb($config, $this);
  }

  public function query($query, array $options = array()) {
    $r = $this->store->query($query, $options);
//    var_dump($query);
    if ($e = $this->store->getErrors()) {
      throw new PDOException('Invalid return directive: ' . implode($e));
    }
    else {
      //var_dump($r);
      return $r;
    }
  }

  // @todo push this into the connection class.
  public function update_query($query, array $options = array()) {
    $rdfdb_info = Rdfdb::getConnectionInfo();

    $config = array(
      'remote_store_endpoint' => $rdfdb_info['default']['endpoint'],
      'store_write_key' => $rdfdb_info['default']['apikey'],
    );
    //var_dump($query);
    //$query = 'INSERT DATA INTO GRAPH <http://mygraph.com> {<http://example.com/person/1234#person> <http://xmlns.com/foaf/0.1/name> "John Doe" .}';
    $data = array(
      //'graph' => 'http://example.org/g1',
      'query' => $query,
      //'mime-type' => 'application/x-turtle',
    );
    $s = '';
    foreach($data as $k => $v) {
      $s .= '&' . $k . '=' . urlencode($v);
    }

    $http_options = array(
      'headers' => array('Content-Type' => 'application/x-www-form-urlencoded; charset=utf-8'),
      'method' => 'POST',
      'data' => $s,
    );

    return http_request($rdfdb_info['default']['endpoint'], $http_options);
  }

  public function driver() {
    return 'virtuoso';
  }

  public function databaseType() {
    return 'virtuoso';
  }

}

