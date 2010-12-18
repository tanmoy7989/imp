# NOTE: This file is generated from modules/core/tools/container_tempates/test.py
# do not edit.

import IMP
import IMP.test
import IMP.core
import IMP.container
import math


# This file is generated by the make-container script

class PairContainerTest(IMP.test.TestCase):
    """Tests for PairContainer related objects"""

    def create_particle(self,m):
        p= IMP.Particle(m)
        d=IMP.core.XYZ.setup_particle(p)
        d.set_coordinates(IMP.algebra.get_random_vector_in(IMP.algebra.get_unit_bounding_box_3d()))
        p.add_attribute(IMP.FloatKey("thekey"), d.get_x())
        return p

    def create_particle_pair(self,m):
        p0= IMP.Particle(m)
        p1= IMP.Particle(m)
        d0= IMP.core.XYZ.setup_particle(p0)
        d1= IMP.core.XYZ.setup_particle(p1)
        d0.set_coordinates(IMP.algebra.get_random_vector_in(IMP.algebra.get_unit_bounding_box_3d()))
        d1.set_coordinates(IMP.algebra.get_random_vector_in(IMP.algebra.get_unit_bounding_box_3d()))
        return (p0,p1)

    def create_singleton_score(self):
        uf= IMP.core.Linear(0,1)
        return IMP.core.AttributeSingletonScore(uf,IMP.FloatKey("thekey"))

    def create_pair_score(self):
        uf= IMP.core.Linear(0,1)
        return IMP.core.DistancePairScore(uf)

    def create_singleton_restraint(self, ps,p):
        return IMP.core.SingletonRestraint(ps, p)

    def create_pair_restraint(self, ps, p):
        return IMP.core.PairRestraint(ps, (p[0], p[1]))


    def test_restraint(self):
        """Test the PairsRestraint"""
        m= IMP.Model()
        gs=self.create_pair_score()
        c= IMP.container.ListPairContainer(m)
        f=0
        for i in range(0,10):
            p=self.create_particle_pair(m)
            f= f+ gs.evaluate(p, None)
            c.add_particle_pair(p)
            r= IMP.container.PairsRestraint(gs, c)
            r.set_was_used(True)
        m.add_restraint(r)
        self.assertAlmostEqual(m.evaluate(False), f, delta=.1*f)
        p=self.create_particle_pair(m)
        f= f+ gs.evaluate(p,None)
        c.add_particle_pair(p)
        self.assertAlmostEqual(m.evaluate(False), f, delta=.1*f)


    def test_irestraint(self):
        """Test the incremental evaluation of the PairsRestraint"""
        m= IMP.Model()
        m.set_log_level(IMP.TERSE)
        gs=self.create_pair_score()
        c= IMP.container.ListPairContainer(m)
        ps=[]
        ps2=[]
        f=0
        for i in range(0,20):
            p=self.create_particle_pair(m)
            ps.append(p)
            f=f+gs.evaluate(p,None)
        for i in range(0,10):
            p=self.create_particle_pair(m)
            ps2.append(p)
            #f=f+evaluate_particle_pair_score(gs, p)
        c.set_particle_pairs(ps)
        r= IMP.container.PairsRestraint(gs, c)
        m.add_restraint(r)
        m.set_is_incremental(True)
        self.assertAlmostEqual(m.evaluate(False), f, delta=.1*f)
        self.assertAlmostEqual(m.evaluate(False), f, delta=.1*f)
        ps= ps+ps2
        f=0
        for p in ps:
            f=f+ gs.evaluate(p,None)
        c.set_particle_pairs(ps)
        self.assertAlmostEqual(m.evaluate(False), f, delta=.1*f)


    def test_srestraint(self):
        """Test the PairRestraint"""
        m= IMP.Model()
        gs=self.create_pair_score()
        p=self.create_particle_pair(m)
        f= gs.evaluate(p, None)
        r= self.create_pair_restraint(gs, p)
        r.set_was_used(True)
        m.add_restraint(r)
        self.assertAlmostEqual(m.evaluate(False), f, delta=.1*f)

    def test_min_restraint(self):
        """Test the MinimumPairRestraint"""
        m= IMP.Model()
        c= IMP.container.ListPairContainer(m)
        self.assertEqual(c.get_ref_count(), 1)
        for i in range(0,10):
            c.add_particle_pair(self.create_particle_pair(m))
        print c.get_number_of_particle_pairs()
        d= self.create_pair_score()
        self.assertEqual(d.get_ref_count(), 1)
        r= IMP.container.MinimumPairRestraint(d, c)
        self.assertEqual(d.get_ref_count(), 2)
        self.assertEqual(c.get_ref_count(), 2)
        r.set_n(4)
        m.add_restraint(r)
        f= m.evaluate(False)
        print f
        ms= []
        print c.get_number_of_particle_pairs()
        for i in range(0,10):
            ps= c.get_particle_pair(i)
            cm= d.evaluate(ps, None)
            ms.append(cm)
        print ms
        ms.sort()
        mt=0
        for i in range(0, 4):
            mt = mt+ ms[i]
        print mt
        self.assertAlmostEqual(mt, f, delta=.1*f)

    def test_max_restraint(self):
        """Test the MaximumPairRestraint"""
        m= IMP.Model()
        c= IMP.container.ListPairContainer(m)
        self.assertEqual(c.get_ref_count(), 1)
        for i in range(0,10):
            c.add_particle_pair(self.create_particle_pair(m))
        print c.get_number_of_particle_pairs()
        d= self.create_pair_score()
        self.assertEqual(d.get_ref_count(), 1)
        r= IMP.container.MaximumPairRestraint(d, c)
        self.assertEqual(c.get_ref_count(), 2)
        self.assertEqual(d.get_ref_count(), 2)
        r.set_n(4)
        m.add_restraint(r)
        f= m.evaluate(False)
        print f
        ms= []
        print c.get_number_of_particle_pairs()
        for i in range(0,10):
            ps= c.get_particle_pair(i)
            cm= d.evaluate(ps, None)
            ms.append(cm)
        ms.sort()
        print ms
        mt=0
        for i in range(0, 4):
            mt = mt+ ms[-i-1]
        print mt
        self.assertAlmostEqual(mt, f, delta=.1*f)
    def test_max_score(self):
        """Test the MaximumPairScore"""
        m= IMP.Model()
        s= []
        for i in range(0,5):
            s.append(IMP._ConstPairScore(i))
        ps= IMP.container.MaximumPairScore(s, 2)
        p= self.create_particle_pair(m)
        ps.set_was_used(True)
        v= ps.evaluate(p, None)
        self.assertEqual(v, 7)
    def test_min_score(self):
        """Test the MinimumPairScore"""
        m= IMP.Model()
        s= []
        for i in range(0,5):
            s.append(IMP._ConstPairScore(i))
        ps= IMP.container.MinimumPairScore(s, 3)
        p= self.create_particle_pair(m)
        ps.set_was_used(True)
        v= ps.evaluate(p, None)
        self.assertEqual(v, 3)

    def test_set(self):
        """Testing PairContainerSet"""
        IMP.set_log_level(IMP.VERBOSE)
        m= IMP.Model()
        print "hi"
        c= IMP.container.PairContainerSet(m)
        ls=[]
        cs=[]
        for i in range(0,3):
            l= IMP.container.ListPairContainer(m)
            c.add_pair_container(l)
            for j in range(0,3):
                t=self.create_particle_pair(m)
                l.add_particle_pair(t)
                cs.append(t)
        for p in cs:
            self.assertTrue(c.get_contains_particle_pair(p))
        ret=[]
        for i in range(0, len(cs)):
            ret.append(c.get_particle_pair(i))
        ret.sort(cmp)
        #print ret
        cs.sort(cmp)
        #rint cs
        for i in range(0, len(ret)):
            self.assertEqual(cmp(ret[i], cs[i]), 0)
        self.assertEqual(c.get_number_of_particle_pairs(), len(cs))

if __name__ == '__main__':
    IMP.test.main()
