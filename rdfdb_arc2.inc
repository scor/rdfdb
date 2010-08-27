<?php

/**
 * @file
 * Code for connection to ARC2 servers.
 */

class RdfdbConnection_arc2 extends RdfdbConnection {

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
      //var_dump($this);
      //var_dump($e);
      throw new PDOException('Invalid return directive: ' . implode($e));
    }
    else {
      //var_dump($r);
      return $r;
    }
  }

  public function driver() {
    return 'arc2';
  }

  public function databaseType() {
    return 'arc2';
  }

}
