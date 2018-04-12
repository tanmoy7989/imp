import utils
import os
import unittest
import sys
if sys.version_info[0] >= 3:
    from io import StringIO
else:
    from io import BytesIO as StringIO
try:
    import urllib.request as urllib2
except ImportError:
    import urllib2

TOPDIR = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
utils.set_search_paths(TOPDIR)
import ihm
import ihm.location
import ihm.representation

class Tests(unittest.TestCase):
    def test_system(self):
        """Test System class"""
        s = ihm.System(title='test system')
        self.assertEqual(s.title, 'test system')
        self.assertEqual(s.id, 'model')

    def test_chem_comp(self):
        """Test ChemComp class"""
        cc1 = ihm.ChemComp(id='GLY', code='G', code_canonical='G')
        self.assertEqual(cc1.id, 'GLY')
        self.assertEqual(cc1.code, 'G')
        self.assertEqual(cc1.code_canonical, 'G')
        self.assertEqual(cc1.type, 'other')
        cc2 = ihm.ChemComp(id='GLY', code='G', code_canonical='G')
        cc3 = ihm.ChemComp(id='G', code='G', code_canonical='G')
        self.assertEqual(cc1, cc2)
        self.assertEqual(hash(cc1), hash(cc2))
        self.assertNotEqual(cc1, cc3)

    def test_peptide_chem_comp(self):
        """Test PeptideChemComp class"""
        cc1 = ihm.PeptideChemComp(id='GLY', code='G', code_canonical='G')
        self.assertEqual(cc1.type, 'Peptide linking')

    def test_l_peptide_chem_comp(self):
        """Test LPeptideChemComp class"""
        cc1 = ihm.LPeptideChemComp(id='MET', code='M', code_canonical='M')
        self.assertEqual(cc1.type, 'L-peptide linking')

    def test_rna_chem_comp(self):
        """Test RNAChemComp class"""
        cc1 = ihm.RNAChemComp(id='G', code='G', code_canonical='G')
        self.assertEqual(cc1.type, 'RNA linking')

    def test_dna_chem_comp(self):
        """Test DNAChemComp class"""
        cc1 = ihm.DNAChemComp(id='DG', code='DG', code_canonical='G')
        self.assertEqual(cc1.type, 'DNA linking')

    def test_l_peptide_alphabet(self):
        """Test LPeptideAlphabet class"""
        a = ihm.LPeptideAlphabet
        self.assertEqual(a._comps['G'].type, 'Peptide linking')

        self.assertEqual(a._comps['M'].id, 'MET')
        self.assertEqual(a._comps['M'].code, 'M')
        self.assertEqual(a._comps['M'].code_canonical, 'M')
        self.assertEqual(a._comps['M'].type, 'L-peptide linking')

        a = ihm.LPeptideAlphabet()
        self.assertTrue('MSE' in a)
        self.assertFalse('DG' in a)
        self.assertEqual(len(a.keys), 22)
        self.assertEqual(len(a.values), 22)
        self.assertEqual(sorted(a.keys)[0], 'A')
        self.assertEqual(len(a.items), 22)
        item0 = sorted(a.items)[0]
        self.assertEqual(item0[0], 'A')
        self.assertEqual(item0[1].id, 'ALA')
        self.assertEqual(a['MSE'].id, 'MSE')
        self.assertEqual(a['MSE'].code, 'MSE')
        self.assertEqual(a['MSE'].code_canonical, 'M')
        self.assertEqual(a['MSE'].type, 'L-peptide linking')
        self.assertEqual(a['UNK'].id, 'UNK')
        self.assertEqual(a['UNK'].code, 'UNK')
        self.assertEqual(a['UNK'].code_canonical, 'X')
        self.assertEqual(a['UNK'].type, 'L-peptide linking')

    def test_rna_alphabet(self):
        """Test RNAAlphabet class"""
        a = ihm.RNAAlphabet
        self.assertEqual(a._comps['A'].id, 'A')
        self.assertEqual(a._comps['A'].code, 'A')
        self.assertEqual(a._comps['A'].code_canonical, 'A')

    def test_dna_alphabet(self):
        """Test DNAAlphabet class"""
        a = ihm.DNAAlphabet
        self.assertEqual(a._comps['DA'].id, 'DA')
        self.assertEqual(a._comps['DA'].code, 'DA')
        self.assertEqual(a._comps['DA'].code_canonical, 'A')

    def test_entity(self):
        """Test Entity class"""
        e1 = ihm.Entity('AHCD', description='foo')
        # Should compare identical if sequences are the same
        e2 = ihm.Entity('AHCD', description='bar')
        e3 = ihm.Entity('AHCDE', description='foo')
        self.assertEqual(e1, e2)
        self.assertNotEqual(e1, e3)
        self.assertEqual(e1.seq_id_range, (1,4))
        self.assertEqual(e3.seq_id_range, (1,5))

    def test_software(self):
        """Test Software class"""
        s1 = ihm.Software(name='foo', version='1.0',
                          classification='1', description='2', location='3')
        s2 = ihm.Software(name='foo', version='2.0',
                          classification='4', description='5', location='6')
        s3 = ihm.Software(name='foo', version='1.0',
                          classification='7', description='8', location='9')
        s4 = ihm.Software(name='bar', version='1.0',
                          classification='a', description='b', location='c')
        # Should compare equal iff name and version both match
        self.assertEqual(s1, s3)
        self.assertEqual(hash(s1), hash(s3))
        self.assertNotEqual(s1, s2)
        self.assertNotEqual(s1, s4)

    def test_citation(self):
        """Test Citation class"""
        s = ihm.Citation(title='Test paper', journal='J Mol Biol',
                         volume=45, page_range=(1,20), year=2016,
                         authors=['Smith A', 'Jones B'],
                         doi='10.2345/S1384107697000225',
                         pmid='1234')
        self.assertEqual(s.title, 'Test paper')

    def _get_from_pubmed_id(self, json_fname):
        def mock_urlopen(url):
            self.assertTrue(url.endswith('&id=29539637'))
            fname = utils.get_input_file_name(TOPDIR, json_fname)
            return open(fname)
        # Need to mock out urllib2 so we don't hit the network (expensive)
        # every time we test
        try:
            orig_urlopen = urllib2.urlopen
            urllib2.urlopen = mock_urlopen
            return ihm.Citation.from_pubmed_id(29539637)
        finally:
            urllib2.urlopen = orig_urlopen

    def test_citation_from_pubmed_id(self):
        """Test Citation.from_pubmed_id()"""
        c = self._get_from_pubmed_id('pubmed_api.json')
        self.assertEqual(c.title,
                'Integrative structure and functional anatomy of a nuclear '
                'pore complex (test of python-ihm lib).')
        self.assertEqual(c.journal, 'Nature')
        self.assertEqual(c.volume, '555')
        self.assertEqual(c.page_range, ['475','482'])
        self.assertEqual(c.year, '2018')
        self.assertEqual(c.pmid, 29539637)
        self.assertEqual(c.doi, '10.1038/nature26003')
        self.assertEqual(len(c.authors), 32)
        self.assertEqual(c.authors[0], 'Kim SJ')

    def test_citation_from_pubmed_id_no_doi(self):
        """Test Citation.from_pubmed_id() with no DOI"""
        c = self._get_from_pubmed_id('pubmed_api_no_doi.json')
        self.assertEqual(c.title,
                'Integrative structure and functional anatomy of a nuclear '
                'pore complex (test of python-ihm lib).')
        self.assertEqual(c.doi, None)

    def test_entity_residue(self):
        """Test Residue derived from an Entity"""
        e = ihm.Entity('AHCDAH')
        r = e.residue(3)
        self.assertEqual(r.entity, e)
        self.assertEqual(r.asym, None)
        self.assertEqual(r.seq_id, 3)

    def test_asym_unit_residue(self):
        """Test Residue derived from an AsymUnit"""
        e = ihm.Entity('AHCDAH')
        a = ihm.AsymUnit(e, auth_seq_id_map=5)
        r = a.residue(3)
        self.assertEqual(r.entity, None)
        self.assertEqual(r.asym, a)
        self.assertEqual(r.seq_id, 3)
        self.assertEqual(r.auth_seq_id, 8)

    def test_entity_range(self):
        """Test EntityRange class"""
        e = ihm.Entity('AHCDAH')
        e._id = 42
        self.assertEqual(e.seq_id_range, (1,6))
        r = e(3,4)
        self.assertEqual(r.seq_id_range, (3,4))
        self.assertEqual(r._id, 42)
        samer = e(3,4)
        otherr = e(2,4)
        self.assertEqual(r, samer)
        self.assertEqual(hash(r), hash(samer))
        self.assertNotEqual(r, otherr)
        self.assertNotEqual(r, e) # entity_range != entity

    def test_asym_range(self):
        """Test AsymUnitRange class"""
        e = ihm.Entity('AHCDAH')
        a = ihm.AsymUnit(e)
        a._id = 42
        self.assertEqual(a.seq_id_range, (1,6))
        r = a(3,4)
        self.assertEqual(r.seq_id_range, (3,4))
        self.assertEqual(r._id, 42)
        self.assertEqual(r.entity, e)
        samer = a(3,4)
        otherr = a(2,4)
        self.assertEqual(r, samer)
        self.assertEqual(hash(r), hash(samer))
        self.assertNotEqual(r, otherr)
        self.assertNotEqual(r, a)      # asym_range != asym
        self.assertNotEqual(r, e(3,4)) # asym_range != entity_range
        self.assertNotEqual(r, e)      # asym_range != entity

    def test_auth_seq_id_offset(self):
        """Test auth_seq_id offset from seq_id"""
        e = ihm.Entity('AHCDAH')
        a = ihm.AsymUnit(e, auth_seq_id_map=5)
        self.assertEqual(a._get_auth_seq_id(1), 6)

    def test_auth_seq_id_dict(self):
        """Test auth_seq_id dict map from seq_id"""
        e = ihm.Entity('AHCDAH')
        a = ihm.AsymUnit(e, auth_seq_id_map={1:0, 2:4})
        self.assertEqual(a._get_auth_seq_id(1), 0)
        self.assertEqual(a._get_auth_seq_id(2), 4)
        self.assertEqual(a._get_auth_seq_id(3), 3)

    def test_auth_seq_id_list(self):
        """Test auth_seq_id list map from seq_id"""
        e = ihm.Entity('AHCDAH')
        a = ihm.AsymUnit(e, auth_seq_id_map=[None, 0, 4])
        self.assertEqual(a._get_auth_seq_id(1), 0)
        self.assertEqual(a._get_auth_seq_id(2), 4)
        self.assertEqual(a._get_auth_seq_id(3), 3)

    def test_assembly(self):
        """Test Assembly class"""
        e1 = ihm.Entity('AHCD')
        e2 = ihm.Entity('AHC')
        a = ihm.Assembly([e1, e2], name='foo', description='bar')
        self.assertEqual(a.name, 'foo')
        self.assertEqual(a.description, 'bar')

    def test_remove_identical(self):
        """Test remove_identical function"""
        x = {}
        y = {}
        all_objs = ihm._remove_identical([x, x, y])
        # Order should be preserved, but only one x should be returned
        self.assertEqual(list(all_objs), [x, y])

    def test_all_model_groups(self):
        """Test _all_model_groups() method"""
        model_group1 = []
        model_group2 = []
        state1 = [model_group1, model_group2]
        state2 = [model_group2, model_group2]
        s = ihm.System()
        s.state_groups.append([state1, state2])
        mg = s._all_model_groups()
        # List may contain duplicates
        self.assertEqual(list(mg), [model_group1, model_group2,
                                    model_group2, model_group2])

    def test_all_models(self):
        """Test _all_models() method"""
        class MockModel(object):
            pass
        model1 = MockModel()
        model2 = MockModel()
        model_group1 = [model1, model2]
        model_group2 = [model1, model1]
        s = ihm.System()
        s.state_groups.append([[model_group1, model_group2]])
        ms = s._all_models()
        models = [model for group, model in ms]
        # duplicates should be filtered within groups, but not between groups
        self.assertEqual(models, [model1, model2, model1])

    def test_all_protocols(self):
        """Test _all_protocols() method"""
        class MockObject(object):
            pass
        model1 = MockObject()
        model2 = MockObject()
        model3 = MockObject()
        model_group1 = [model1, model2, model3]
        s = ihm.System()
        s.state_groups.append([[model_group1]])
        p1 = MockObject()
        p2 = MockObject()
        s.orphan_protocols.append(p1)
        model1.protocol = None
        model2.protocol = p2
        model3.protocol = p1
        # duplicates should be filtered globally
        self.assertEqual(list(s._all_protocols()), [p1, p2])

    def test_all_representations(self):
        """Test _all_representations() method"""
        class MockObject(object):
            pass
        model1 = MockObject()
        model2 = MockObject()
        model3 = MockObject()
        model_group1 = [model1, model2, model3]
        s = ihm.System()
        s.state_groups.append([[model_group1]])
        r1 = MockObject()
        r2 = MockObject()
        s.orphan_representations.append(r1)
        model1.representation = None
        model2.representation = r2
        model3.representation = r1
        # duplicates should be filtered globally
        self.assertEqual(list(s._all_representations()), [r1, r2])

    def test_all_assemblies(self):
        """Test _all_assemblies() method"""
        class MockObject(object):
            pass
        model1 = MockObject()
        model2 = MockObject()
        model_group1 = [model1, model2]
        s = ihm.System()
        s.state_groups.append([[model_group1]])
        asmb1 = MockObject()
        asmb2 = MockObject()
        s.orphan_assemblies.append(asmb1)
        model1.assembly = None
        model1.protocol = None
        model2.assembly = asmb2
        step = MockObject()
        step.assembly = asmb1
        prot = MockObject()
        prot.steps = [step]

        analysis1 = MockObject()
        astep1 = MockObject()
        astep1.assembly = asmb2
        analysis1.steps = [astep1]
        prot.analyses = [analysis1]

        model2.protocol = prot
        rsr1 = MockObject()
        rsr1.assembly = asmb2
        rsr2 = MockObject()
        rsr2.assembly = None
        s.restraints.extend((rsr1, rsr2))
        # duplicates should be present; complete assembly is always first
        self.assertEqual(list(s._all_assemblies()),
                         [s.complete_assembly, asmb1, asmb2, asmb1,
                          asmb2, asmb2])

    def test_all_citations(self):
        """Test _all_citations() method"""
        class MockObject(object):
            pass

        c1 = ihm.Citation(title='Test paper', journal='J Mol Biol',
                          volume=45, page_range=(1,20), year=2016,
                          authors=['Smith A', 'Jones B'],
                          doi='10.2345/S1384107697000225',
                          pmid='1234')
        c2 = ihm.Citation(title='Test paper', journal='J Mol Biol',
                          volume=45, page_range=(1,20), year=2016,
                          authors=['Smith A', 'Jones B'],
                          doi='1.2.3.4',
                          pmid='1234')
        rsr1 = MockObject() # Not a 3dem restraint
        rsr2 = MockObject() # 3dem but with no provided citation
        rsr2.fitting_method_citation_id = None
        rsr3 = MockObject()
        rsr2.fitting_method_citation_id = c1

        s = ihm.System()
        s.restraints.extend((rsr1, rsr2, rsr3))
        s.citations.extend((c2, c2))
        # duplicates should be filtered globally
        self.assertEqual(list(s._all_citations()), [c2, c1])

    def test_all_dataset_groups(self):
        """Test _all_dataset_groups() method"""
        class MockObject(object):
            pass
        dg1 = MockObject()
        dg2 = MockObject()
        s = ihm.System()
        s.orphan_dataset_groups.append(dg1)
        step1 = MockObject()
        step2 = MockObject()
        step3 = MockObject()
        step1.dataset_group = None
        step2.dataset_group = dg2
        step3.dataset_group = dg1
        protocol1 = MockObject()
        protocol1.steps = [step1, step2, step3]
        analysis1 = MockObject()
        astep1 = MockObject()
        astep1.dataset_group = dg2
        analysis1.steps = [astep1]
        protocol1.analyses = [analysis1]
        s.orphan_protocols.append(protocol1)
        # duplicates should not be filtered
        self.assertEqual(list(s._all_dataset_groups()), [dg1, dg2, dg1, dg2])

    def test_all_locations(self):
        """Test _all_locations() method"""
        class MockObject(object):
            pass
        class MockDataset(object):
            parents = []
        loc1 = MockObject()
        loc2 = MockObject()
        loc3 = MockObject()

        s = ihm.System()
        dataset1 = MockDataset()
        dataset2 = MockDataset()
        dataset2.location = None
        dataset3 = MockDataset()
        dataset3.location = loc1

        s.locations.append(loc1)
        s.orphan_datasets.extend((dataset1, dataset2, dataset3))

        ensemble = MockObject()
        ensemble.file = loc2
        density = MockObject()
        density.file = loc1
        ensemble.densities = [density]
        s.ensembles.append(ensemble)

        start_model = MockObject()
        start_model.dataset = None
        template = MockObject()
        template.dataset = None
        template.alignment_file = loc3
        start_model.templates = [template]
        s.orphan_starting_models.append(start_model)

        # duplicates should not be filtered
        self.assertEqual(list(s._all_locations()), [loc1, loc1, loc2,
                                                    loc1, loc3])

    def test_all_datasets(self):
        """Test _all_datasets() method"""
        class MockObject(object):
            pass
        class MockDataset(object):
            parents = []

        s = ihm.System()
        d1 = MockDataset()
        d2 = MockDataset()
        d3 = MockDataset()
        d4 = MockDataset()

        s.orphan_datasets.append(d1)

        dg1 = [d2]
        s.orphan_dataset_groups.append(dg1)

        start_model1 = MockObject()
        start_model1.dataset = None

        start_model2 = MockObject()
        start_model2.dataset = d3

        template = MockObject()
        template.dataset = None
        start_model1.templates = [template]
        start_model2.templates = []
        s.orphan_starting_models.extend((start_model1, start_model2))

        rsr1 = MockObject()
        rsr1.dataset = d4
        d4.parents = [d2]
        d2.parents = [d1]
        d1.parents = d3.parents = []
        s.restraints.append(rsr1)

        # duplicates should not be filtered
        self.assertEqual(list(s._all_datasets()), [d1, d1, d2, d3, d1, d2, d4])

    def test_all_starting_models(self):
        """Test _all_starting_models() method"""
        class MockObject(object):
            pass

        s = ihm.System()
        sm1 = MockObject()
        sm2 = MockObject()
        s.orphan_starting_models.append(sm1)
        rep = ihm.representation.Representation()
        seg1 = ihm.representation.Segment()
        seg1.starting_model = None
        seg2 = ihm.representation.Segment()
        seg2.starting_model = sm2
        seg3 = ihm.representation.Segment()
        seg3.starting_model = sm2
        rep.extend((seg1, seg2, seg3))
        s.orphan_representations.append(rep)
        # duplicates should be filtered
        self.assertEqual(list(s._all_starting_models()), [sm1, sm2])

    def test_update_locations_in_repositories(self):
        """Test update_locations_in_repositories() method"""
        s = ihm.System()
        loc = ihm.location.InputFileLocation(path='foo', repo='bar')
        s.locations.append(loc)
        r = ihm.location.Repository(doi='foo', root='..')
        s.update_locations_in_repositories([r])


if __name__ == '__main__':
    unittest.main()