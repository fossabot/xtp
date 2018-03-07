/*
 *            Copyright 2009-2018 The VOTCA Development Team
 *                       (http://www.votca.org)
 *
 *      Licensed under the Apache License, Version 2.0 (the "License")
 *
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __VOTCA_XTP_FRAGMENT_H
#define	__VOTCA_XTP_FRAGMENT_H

#include <vector>
#include <string>

#include <votca/tools/vec.h>
#include <votca/tools/matrix.h>

#include <votca/xtp/atom.h>
#include <fstream>



namespace votca { namespace xtp {

class Topology;
class Molecule;
class Segment;
class PolarSite;
class APolarSite;    

/**
    \brief Rigid fragment. One conjugated segment contains several rigid fragments.

 * Apart from the position and orientation it has a pointer to a conjugated segment 
 * it belongs to as well as positions of atoms which belong to it of two types those 
 * which are generated by MD and those obtained by substitution of rigid copies
 */

class Fragment {
public:

     Fragment(int id, std::string name) : _id(id), _name(name), _symmetry(-1) { }
     Fragment(Fragment *stencil);
    ~Fragment();
    
    void Rotate(votca::tools::matrix spin, votca::tools::vec refPos);    // rotates w.r.t. center of map
    void TranslateBy(const votca::tools::vec &shift);
    void RotTransQM2MD();

    inline void setTopology(Topology *container) { _top = container; }
    inline void setMolecule(Molecule *container) { _mol = container; }
    inline void setSegment(Segment *container)   { _seg = container; }
    void        AddAtom( Atom* atom );
    void        AddPolarSite(PolarSite *pole);
    void        AddAPolarSite(APolarSite *pole);

    Topology            *getTopology() { return _top; }
    Molecule            *getMolecule() { return _mol; }
    Segment             *getSegment()  { return _seg; }
    std::vector< Atom* >     &Atoms() { return _atoms; }
    std::vector<PolarSite*>  &PolarSites() { return _polarSites; }
    std::vector<APolarSite*> &APolarSites() { return _apolarSites; }

    const int    &getId() const { return _id; }
    const std::string &getName() const { return _name; }

    void         Rigidify(bool Auto = 0);
    void         setSymmetry(int sym) { _symmetry = sym; }
    const int   &getSymmetry() { return _symmetry; }
    void         setTrihedron(std::vector<int> trihedron) { _trihedron = trihedron; }
    const std::vector< int > &getTrihedron() { return _trihedron; }


    void          calcPos(std::string tag = "MD");
    void          setPos(votca::tools::vec pos) { _CoMD = pos; }
    const votca::tools::vec    &getPos() const { return _CoMD; }
    const votca::tools::vec    &getCoMD() { return _CoMD; }
    const votca::tools::vec    &getCoQM() { return _CoQM; }
    const votca::tools::vec    &getCoQM0() { return _CoQM0; }
    const votca::tools::matrix &getRotQM2MD() { return _rotateQM2MD; }
    const votca::tools::vec    &getTransQM2MD() { return _translateQM2MD; }
    
    
    

private:

    Segment     *_seg;

    std::vector < Atom* > _atoms;
    std::vector <PolarSite*> _polarSites;
    std::vector <APolarSite*> _apolarSites;
    std::vector< double > _weights;

    int         _id;
    std::string      _name;
    Topology    *_top;
    Molecule    *_mol;
    int              _symmetry;

    votca::tools::matrix      _rotateQM2MD;       // Set via ::Rigidify()
    votca::tools::vec         _CoQM;              // Center of map (QM)
    votca::tools::vec         _CoMD;              // Center of map (MD)
    std::vector< int >    _trihedron;
    votca::tools::vec         _CoQM0;             // Center of map (QM) original (for IZindo)
    votca::tools::vec         _translateQM2MD;    // Set via ::Rigidify()


};

}}

#endif	/* __VOTCA_XTP_FRAGMENT_H */

