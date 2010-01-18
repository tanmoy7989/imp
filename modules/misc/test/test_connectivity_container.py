import unittest
import IMP
import IMP.test
import IMP.misc
import IMP.atom
import math

class TunnelTest(IMP.test.TestCase):
    """Tests for tunnel scores"""

    def test_score(self):
        """Test connectivity"""
        IMP.set_log_level(IMP.VERBOSE)
        m= IMP.Model()
        ps= IMP.core.create_xyzr_particles(m, 10, .1)
        for p in ps:
            p.set_coordinates_are_optimized(True)
        lsc=IMP.core.ListSingletonContainer(ps)
        cpc=IMP.misc.ConnectingPairContainer(lsc, .1, False)
        #m.add_restraint(pr)
        m.evaluate(False)
        for pp in cpc.get_particle_pairs():
            print pp
            # for p0 in ps:
            #     for p1 in ps:
            #         if p0 != p1:
            #             d0= IMP.core.XYZR(p0)
            #             d1= IMP.core.XYZR(p1)
            #             if IMP.core.distance(d0, d1) < 0:
            #                 print str(p0)+" " +str(p1)
            #                 self.assert_(cpc.get_contains_particle_pair(ParticlePair(p0, p1))
            #                 or cpc.get_contains_particle_pair(ParticlePair(p1, p0)))
        cg= IMP.core.ConjugateGradients(m)
        ub= IMP.core.HarmonicUpperBound(0, 1)
        sd= IMP.core.DistancePairScore(ub)
        pr= IMP.core.PairsRestraint(sd, cpc)
        m.add_restraint(pr)
        print "added"
        print pr.evaluate(False)
        cg.set_threshold(.00001)
        cg.optimize(1000)
        for pp in cpc.get_particle_pairs():
            pp.show()
            print
        for p0 in ps:
            for p1 in ps:
                if p0 != p1:
                    d0= IMP.core.XYZ(p0.get_particle())
                    d1= IMP.core.XYZ(p1.get_particle())
                    print d0
                    print d1
                    print IMP.ParticlePair(p0.get_particle(), p1.get_particle())
                    self.assert_(IMP.core.distance(d0, d1) <= 0.1)
                    #self.assert_(cpc.get_contains_particle_pair(IMP.ParticlePair(p0.get_particle(), p1.get_particle()))
                                 #or cpc.get_contains_particle_pair(IMP.ParticlePair(p1.get_particle(), p0.get_particle())))



if __name__ == '__main__':
    unittest.main()
