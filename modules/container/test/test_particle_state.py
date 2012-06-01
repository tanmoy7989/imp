# NOTE: This file is generated from modules/core/tools/container_tempates/test.py
# do not edit.


import IMP
import IMP.test
import IMP.core
import IMP.container
import math




class SingletonTestModifier(IMP.SingletonModifier):
    def __init__(self, k):
        IMP.SingletonModifier.__init__(self)
        self.k=k
    def do_show(self, fh):
        fh.write("Test Particle")
    def apply(self, a0, da=None):
        if a0.has_attribute(self.k):
            pass
        else:
            a0.add_attribute(self.k, 1)
    def get_version_info(self):
        return 1
    def get_input_particles(self, p):
        return [p]
    def get_output_particles(self, p):
        return [p]
    def get_input_containers(self, p):
        return []
    def get_output_containers(self, p):
        return []

class PairTestModifier(IMP.PairModifier):
    def __init__(self, k):
        IMP.PairModifier.__init__(self)
        self.k=k
        self.sm= SingletonTestModifier(k)
    def do_show(self, fh):
        fh.write("Test Particle")
    def apply(self, a0, da=None):
        self.sm.apply(a0[0])
        self.sm.apply(a0[1])
    def get_version_info(self):
        return 1
    def get_input_particles(self, p):
        return [p]
    def get_output_particles(self, p):
        return [p]
    def get_input_containers(self, p):
        return []
    def get_output_containers(self, p):
        return []

def particle_has_attribute(p, k):
    return p.has_attribute(k)

def particle_pair_has_attribute(p, k):
    return p[0].has_attribute(k) and p[1].has_attribute(k)

def particle_triplet_has_attribute(p, k):
    return p[0].has_attribute(k) and p[1].has_attribute(k) and p[2].has_attribute(k)

# This file is generated by the make-container script

class SingletonContainerTest(IMP.test.TestCase):
    """Tests for SingletonContainer related objects"""

    def create_particle(self,m):
        p= IMP.Particle(m)
        p.add_attribute(IMP.FloatKey("thekey"), float(1))
        return p

    def create_particle_pair(self,m):
        p0= IMP.Particle(m)
        p1= IMP.Particle(m)
        d0= IMP.core.XYZ.setup_particle(p0)
        d1= IMP.core.XYZ.setup_particle(p1)
        d0.set_coordinates(IMP.algebra.Vector3D(0,0,1))
        d1.set_coordinates(IMP.algebra.Vector3D(0,0,0))
        return (p0,p1)

    def same(self, a,b):
        return a.get_name() == b.get_name()

    def create_singleton_score_state(self, f, a, t):
        return IMP.core.SingletonConstraint(f, a, t)

    def create_pair_score_state(self, f, a, t):
        return IMP.core.PairConstraint(f, a, t)

    def create_particle_score(self):
        uf= IMP.core.Linear(0,1)
        return IMP.core.AttributeSingletonScore(uf,IMP.FloatKey("thekey"))

    def create_particle_pair_score(self):
        uf= IMP.core.Linear(0,1)
        return IMP.core.DistancePairScore(uf)

    def test_set(self):
        """Testing SingletonsConstraint"""
        # write increment an int field
        # call evaluate and check that it is incremented
        IMP.set_log_level(IMP.VERBOSE)
        print "start"
        m= IMP.Model()
        print "hi"
        c= IMP.container.ListSingletonContainer(m)
        cs=[]
        for i in range(0,30):
            t=self.create_particle(m)
            c.add_particle(t)
            cs.append(t)
        print "dl"
        k= IMP.IntKey("thevalue")
        f= SingletonTestModifier(k)
        print "apply"
        s= IMP.container.SingletonsConstraint(f, None, c)
        print "add"
        m.add_score_state(s)
        m.update()
        for p in cs:
            self.assertTrue(particle_has_attribute(p, k))
        print "done"

    def test_sset(self):
        """Testing SingletonConstraint"""
        # write increment an int field
        # call evaluate and check that it is incremented
        IMP.set_log_level(IMP.VERBOSE)
        print "start"
        m= IMP.Model()
        print "hi"
        t=self.create_particle(m)
        print "dl"
        k= IMP.IntKey("thevalue")
        f= SingletonTestModifier(k)
        print "apply"
        s= self.create_singleton_score_state(f, None, t)
        m.add_score_state(s)
        print "add"
        m.update()
        self.assertTrue(particle_has_attribute(t, k))
        print "done"

if __name__ == '__main__':
    IMP.test.main()
