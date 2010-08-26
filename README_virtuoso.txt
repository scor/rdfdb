

INSERT DATA INTO GRAPH <http://mygraph.com> { <http://demo.openlinksw.com/DAV/home/demo_about.rdf> <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://rdfs.org/sioc/ns#Userscor>}



DELETE DATA FROM GRAPH <http://mygraph.com> {
<http://demo.openlinksw.com/DAV/home/demo_about.rdf>
<http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <http://rdfs.org/sioc/ns#Userscor3>.

}



CLEAR GRAPH <http://mygraph.com>

// No way to clear an entire store.


CREATE GRAPH <http://example/bookStore>



This will ignore the graph
select * not from <http://www.openlinksw.com/schemas/virtrdf#> where { {?s ?p ?o}}
but this query does not ignore the graph
select * not from <http://www.openlinksw.com/schemas/virtrdf#> where {graph ?g {?s ?p ?o}}
