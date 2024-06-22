SELECT ?x ?y ?z WHERE { 
	?z <data/univ-bench.owl#subOrganizationOf> ?y . 
	?z <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <data/univ-bench.owl#Department> . 
	?x <data/univ-bench.owl#memberOf> ?z . 
	?x <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <data/univ-bench.owl#GraduateStudent> . 
	?x <data/univ-bench.owl#undergraduateDegreeFrom> ?y . 
	?y <http://www.w3.org/1999/02/22-rdf-syntax-ns#type> <data/univ-bench.owl#University> . 
}
