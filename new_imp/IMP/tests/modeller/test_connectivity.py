import modeller, unittest
import modeller.optimizers
import os
import IMP.modeller_intf
import IMP.test, IMP

# Class to test connectivity restraints
class test_connectivity(IMP.test.IMPTestCase):
    """test connectivity restraints"""

    def setUp(self):
        """set up Modeller with connectivity restraints """
        modeller.log.level(0,0,0,0,0)

        self.env = modeller.environ()
        self.env.io.atom_files_directory = '../data/'
        self.env.edat.dynamic_sphere = False
        self.env.libs.topology.read(file='$(LIB)/top_heav.lib')
        self.env.libs.parameters.read(file='$(LIB)/par.lib')

        self.imp_model = IMP.Model()
        self.particles = []
        self.restraint_sets = []
        self.rsrs = []

        self.t = self.env.edat.energy_terms
        self.t.append(IMP.modeller_intf.IMP_Restraints(self.imp_model, self.particles))

        self.modeller_model = IMP.modeller_intf.Create_Particles(12, self.env, self.imp_model, self.particles)
        p1 = self.particles[0]
        p1.add_float("radius", 1.0, False)
        p1.add_int("protein", 1)
        p1.add_int("id", 1)

        p1 = self.particles[1]
        p1.add_float("radius", 1.0, False)
        p1.add_int("protein", 1)
        p1.add_int("id", 2)

        p1 = self.particles[2]
        p1.add_float("radius", 1.0, False)
        p1.add_int("protein", 1)
        p1.add_int("id", 3)

        p1 = self.particles[3]
        p1.add_float("radius", 1.5, False)
        p1.add_int("protein", 2)
        p1.add_int("id", 4)

        p1 = self.particles[4]
        p1.add_float("radius", 1.5, False)
        p1.add_int("protein", 2)
        p1.add_int("id", 5)

        p1 = self.particles[5]
        p1.add_float("radius", 1.5, False)
        p1.add_int("protein", 2)
        p1.add_int("id", 6)

        p1 = self.particles[6]
        p1.add_float("radius", 1.5, False)
        p1.add_int("protein", 2)
        p1.add_int("id", 7)

        p1 = self.particles[7]
        p1.add_float("radius", 2.0, False)
        p1.add_int("protein", 3)
        p1.add_int("id", 8)

        p1 = self.particles[8]
        p1.add_float("radius", 2.0, False)
        p1.add_int("protein", 3)
        p1.add_int("id", 9)

        p1 = self.particles[9]
        p1.add_float("radius", 2.0, False)
        p1.add_int("protein", 3)
        p1.add_int("id", 10)

        p1 = self.particles[10]
        p1.add_float("radius", 2.0, False)
        p1.add_int("protein", 3)
        p1.add_int("id", 11)

        p1 = self.particles[11]
        p1.add_float("radius", 2.0, False)
        p1.add_int("protein", 3)
        p1.add_int("id", 12)

        self.atmsel = modeller.selection(self.modeller_model)

        self.opt = modeller.optimizers.conjugate_gradients()


    # connectivity
    def test_connectivity(self):
        """ all particles in a single protein should be connected, and all proteins
        should be connected, either directly or indirectly through other proteins """
        self.atmsel.randomize_xyz(deviation=100.0)

        rs = IMP.RestraintSet("connect")
        self.restraint_sets.append(rs)
        self.imp_model.add_restraint_set(rs)

        # add connectivity restraints

        particle_indexes = IMP.vectori()
        rsrs = []

        score_func_params_ub = IMP.BasicScoreFuncParams("harmonic_upper_bound", 0.0, 0.1)

        # set up exclusion volumes
        IMP.modeller_intf.Set_Up_Exclusion_Volumes(self.imp_model, self.particles, "radius", rsrs)

        # connect 3 proteins together
        particle_indexes.clear()
        for i in range(12):
            particle_indexes.push_back(i)
        rsrs.append(IMP.ConnectivityRestraint(self.imp_model, particle_indexes, "protein", "radius", score_func_params_ub))

        # connect particles in protein1 together
        particle_indexes.clear()
        for i in range(3):
            particle_indexes.push_back(i)
        rsrs.append(IMP.ConnectivityRestraint(self.imp_model, particle_indexes, "id", "radius", score_func_params_ub))

        # connect particles in protein2 together
        particle_indexes.clear()
        for i in range(3, 7):
            particle_indexes.push_back(i)
        rsrs.append(IMP.ConnectivityRestraint(self.imp_model, particle_indexes, "id", "radius", score_func_params_ub))

        # connect particles in protein3 together
        particle_indexes.clear()
        for i in range(7, 12):
            particle_indexes.push_back(i)
        rsrs.append(IMP.ConnectivityRestraint(self.imp_model, particle_indexes, "id", "radius", score_func_params_ub))

        # add restraints
        for i in range(len(rsrs)):
            rs.add_restraint(rsrs[i])

        self.atmsel.randomize_xyz(deviation=100.0)
        new_mdl = self.opt.optimize (self.atmsel, max_iterations=55, actions=None)
        self.modeller_model.write (file='out_connectivity.pdb', model_format='PDB')

        coords = self.LoadCoordinates('out_connectivity.pdb')

        # min distances
        for i in range(len(coords)):
            for j in range(i+1,len(coords)):
                self.assert_(self.TestMinDistance(coords[i], coords[j], self.particles[i].get_float("radius") + self.particles[j].get_float("radius") - 0.05), "min distance for any pair condition")

        # max distances
        d12 = 10000000
        d13 = 10000000
        d23 = 10000000
        for i in range(len(coords)):
            for j in range(i+1,len(coords)):
                t1 = self.particles[i].get_int("protein")
                t2 = self.particles[j].get_int("protein")
                d = self.Distance(coords[i], coords[j]) - self.particles[i].get_float("radius") - self.particles[j].get_float("radius")
                if t1 == 1 and t2 == 2:
                    if d < d12:
                        d12 = d
                if t1 == 1 and t2 == 3:
                    if d < d13:
                        d13 = d
                if t1 == 2 and t2 == 3:
                    if d < d23:
                        d23 = d

        sum = 0;
        max_dist = 0.05
        if d12 < max_dist:
            sum = sum + 1
        if d13 < max_dist:
            sum = sum + 1
        if d23 < max_dist:
            sum = sum + 1
        self.assert_(sum >= 2, "min spanning tree for three particle types")
        os.unlink('out_connectivity.pdb')

if __name__ == '__main__':
    unittest.main()
