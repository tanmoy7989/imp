import IMP
import IMP.test
import IMP.algebra as alg
import IMP.em2d
import IMP.em2d.imp_general.io as io
import IMP.EMageFit.utility as utility
import IMP.EMageFit.solutions_io as solutions_io
import sys
import os
import itertools
import IMP.EMageFit.sampling


class TestClusterSolutions(IMP.test.ApplicationTestCase):

    def test_generating_clusters(self):
        """
            Test that the solutions generated by the clustering a the same
            as those obtainedduring the benchmark for the paper
        """
        emagefit_cluster = \
            self.import_python_application('emagefit_cluster')
        fn_config = self.get_input_file_name("config.py")
        fn_database = self.get_input_file_name("domino_solutions.db")
        fn_db_clusters = "clusters.db"
        # modify the names of the PDB files to include the proper name for
        # testing
        exp = utility.get_experiment_params(fn_config)
        for i in range(len(exp.fn_pdbs)):
            exp.fn_pdbs[i] = self.get_input_file_name(exp.fn_pdbs[i])
        n_solutions = 30
        orderby = "em2d"
        max_rmsd = 10
        tc = emagefit_cluster.AlignmentClustering(exp)
        tc.cluster(fn_database, n_solutions, orderby, max_rmsd)
        tc.store_clusters(fn_db_clusters, "clusters")

        # retrieve the largest cluster
        solutions_stored = '9|10|11|12|13|14|15|16|17|18|19|20|21|22|23|24|25|26|27|28|29'
        solutions_stored = map(int, solutions_stored.split("|"))
        db_clusters = solutions_io.ResultsDB()
        db_clusters.connect(fn_db_clusters)
        cl_record = db_clusters.get_nth_largest_cluster(1)
        elements = map(int, cl_record.elements.split("|"))
        for i, j in zip(solutions_stored, elements):
            self.assertEqual(i, j)
        db_clusters.close()
        os.remove(fn_db_clusters)

if __name__ == '__main__':
    IMP.test.main()