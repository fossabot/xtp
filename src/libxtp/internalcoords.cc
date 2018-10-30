#include<votca/xtp/internalcoords.h>
#include<boost/graph/connected_components.hpp>
#include<iostream>
#include<limits>
#include<algorithm>
#include<cmath>
#include<random>
#include<votca/xtp/CoordContainer.h>

namespace votca { namespace xtp {

const std::vector<std::string> electroNegElements {"N", "O", "F", "P", "S", "Cl"};
tools::Elements elements;

template < typename T>
inline bool VectorContains (const T& item, const std::vector<T>& vec){
    return (std::find(vec.begin(), vec.end(), item) != vec.end());
}

inline bool IsElectronegative(const std::string& type){
    return VectorContains(type, electroNegElements);
}

inline std::tuple<int, int, double> ClosestAtoms(const std::vector<int>& comp1,
                                                 const std::vector<int>& comp2,
                                                 const std::vector<QMAtom*>& qmm){

    double min = std::numeric_limits<double>::infinity();
    std::pair<int, int> closest = std::make_pair(-1, -1);

    for (const auto& i : comp1){
        auto posI = qmm[i]->getPos();
        for (const auto& j: comp2){
            auto posJ = qmm[j]->getPos();

            const double dist = abs(posI - posJ);

            if (dist < min){
                closest = std::make_pair(i, j);
                min = dist;
            }
        }
    }
    return std::make_tuple(closest.first, closest.second, min);
}


void  InternalCoords::ConnectBonds(){
    const double threshFactor = 1.3;
    const double auxThreshFactor = 2.5;

    int _numAtoms = _qmMolecule.size();

    for (int i = 0; i < _numAtoms; ++i){
        auto atomI = _qmMolecule[i];

        const double iCovRad = elements.getCovRad(atomI->getType(), "bohr");
        const tools::vec iPos = atomI->getPos();
        for (int j = i+1; j < _numAtoms; ++j){
            auto atomJ = _qmMolecule[j];

            const double jCovRad = elements.getCovRad(atomJ->getType(), "bohr");
            const tools::vec jPos = atomJ->getPos();

            double thresh = (iCovRad + jCovRad);

            double dist = abs(iPos - jPos);

            if (dist < threshFactor*thresh){

                boost::add_edge(i, j, _bondGraph);
                _numBonds += 1;
                _vector.emplace_back(dist);
                _bonds[{i,j}] = dist;
            } else if (_withAuxiliary && dist < auxThreshFactor*thresh){
                boost::add_edge(i, j, _bondGraph);
                _numAuxBonds += 1;
                _auxBonds[{i,j}]=dist;
                _vector.emplace_back(dist);

                _bonds[{i,j}] = dist;
            }
        }
    }
}


void InternalCoords::ConnectMolecules(){
    // This part of the algorithm is a bit involved...
    // First we find the number of connected components
    // These are not necessarily real molecules in our case. But it is a
    // starting point for a topology...
    // Rather, they are atoms that are within covalent range of each other
    // times a factor.
    // Since this code is for calculating internal coordinates, we must only
    // have a single connected component in our graph.
    // This probably has some name in Graph theory, but I don't know it.

    // this will contain the component index that an atom belongs to.
    std::vector<int> idxInComponent(boost::num_vertices(_bondGraph));

    int numComponents = boost::connected_components(_bondGraph,
                                                    idxInComponent.data());
    _possibleNumMols = numComponents;

    if (numComponents > 1){
        std::vector<std::vector<int>> components(_possibleNumMols);

        for (int atomIdx = 0; atomIdx < _numAtoms; ++atomIdx){
            int componentIdx = idxInComponent[atomIdx];
            components[componentIdx].push_back(atomIdx);
        }


        // Now connect the closest two atoms in each component with a bond
        for (int compI = 0; compI < numComponents; ++compI){
            std::vector<int> compIAtoms = components[compI];

            for (int compJ = compI + 1; compJ < numComponents; ++compJ){
                std::vector<int> compJAtoms = components[compJ];

                auto closest = ClosestAtoms(compIAtoms, compJAtoms,
                                            _qmMolecule);


                const int i = std::get<0>(closest);
                const int j = std::get<1>(closest);
                const double dist = std::get<2>(closest);

                boost::add_edge(i, j, _bondGraph);
                _numInterMolBonds += 1;
                _vector.emplace_back(dist);
                _bonds[{i, j}] = dist;


                if (_withAuxiliary){
                    // auxiliary interfragment bonds if needed

                    // the paper says the threshold for this can be
                    // 2.0 ang or less than 1.3 times the minimum
                    // interfragment distance. I'll go with the 1.3
                    // factor...

                    double thresholdDist = 1.3*dist;

                    // Now connect atoms within that threshold belonging
                    // to different components
                    for (const auto& iAtom : compIAtoms){
                        for (const auto& jAtom : compJAtoms){
                            const double dist = abs(_qmMolecule[iAtom]->getPos() -
                                                    _qmMolecule[jAtom]->getPos());
                            if (dist <= thresholdDist){
                                boost::add_edge(iAtom, jAtom, _bondGraph);
                                _numInterMolBonds += 1;
                                _auxBonds[{iAtom, jAtom}]=dist;
                                _vector.emplace_back(dist);
                                _bonds[{iAtom, jAtom}] = dist;

                            }
                        }
                    }
                }
            }
        }
    }

    numComponents = boost::connected_components(_bondGraph,
                                                idxInComponent.data());

    if (numComponents != 1){
        throw std::runtime_error("Failed to create a single connected component");
    }
}


void InternalCoords::ConnectHBonds(){
    // we can proceed to calculate the hydrogen bonds
    // Assume we have this situation:
    //                H-.-.-.-B
    //               /
    //              A
    // Where A,B are some elements, A is bonded to H, and we are checking
    // if B and H can form a hydrogen bond.
    // Then:
    // 1. A, B must be electronegative
    // 2. The distance between H, B must be greater than 0.9 times the sum of their
    // (H and B) covalent radii and less than the sum of their VdW radii
    // 3. The angle AHB must be > 90 deg

    double hVdWRad = tools::conv::ang2bohr*elements.getVdWChelpG("H");
    for (int i = 0; i<_numAtoms; ++i){
        // first, for each H atom...
        if (_qmMolecule[i]->getType() == "H"){
            const int HAtomInd = i;
            BglGraph::adjacency_iterator it, it_end;
            boost::tie(it, it_end) = boost::adjacent_vertices(HAtomInd, _bondGraph);

            const tools::vec HAtomPos = _qmMolecule[i]->getPos();

            for (;it != it_end; ++it){
                // if a BONDED neighbour is electronegative
                if ( IsElectronegative( _qmMolecule[*it]->getType() ) ){
                    const int neighAInd = *it;
                    const tools::vec neighAPos = _qmMolecule[neighAInd]->getPos();

                    // for each neighbour within range
                    for (int neighBInd = 0; neighBInd < _numAtoms; ++neighBInd){

                        if (neighBInd == HAtomInd || neighBInd == neighAInd ||
                            // if the neighbour is electronegative
                            !IsElectronegative(_qmMolecule[neighBInd]->getType()))
                            continue;

                        const tools::vec neighBPos = _qmMolecule[neighBInd]->getPos();

                        const double dist = abs(HAtomPos - neighBPos);

                        double lowerBound = 0.9*(elements.getCovRad(_qmMolecule[neighBInd]->getType(), "bohr") + elements.getCovRad("H", "bohr"));


                        double upperBound = tools::conv::ang2bohr*(elements.getVdWChelpG(_qmMolecule[neighBInd]->getType()) + elements.getVdWChelpG("H"));
                        // And it is within range
                        if (lowerBound < dist && dist < upperBound){

                            const tools::vec HABond = _qmMolecule[neighAInd]->getPos() - HAtomPos;
                            const tools::vec HBBond = _qmMolecule[neighBInd]->getPos() - HAtomPos;
                            const double cosTheta = HABond*HBBond;
                            // And the angle is greater than 90
                            // ie cos(angle) < 0
                            if (cosTheta < 0){
                                boost::add_edge(HAtomInd, neighBInd, _bondGraph);
                                _numHBonds+=1;
                                _vector.emplace_back(dist);
                                _bonds[{HAtomInd, neighBInd}] = dist;
                            }
                        }
                    }
                }
            }
        }
    }
}

void InternalCoords::CalculateAnglesDihedrals(){

    // Consider a system that looks like this:
    //           A      D
    //            \    /
    //             B--C

    // That would mean we have two angles: ABC, BCD.
    // And one dihedral ABCD, which is the angle formed by the planes
    // ^    ^      ^    ^
    // BA X BC and CB X CD (X is cross-product), named plane1, plane2
    // respectively. In this case a normal is enough to define the planes
    // orientations.
    // If any of the bonds AB, BC, CD are auxiliary, then they don't
    // contribute to an angle or a dihedral. ie if BC is auxiliary, then no
    // angles or dihedrals. If either AB or CD is auxiliary, then only one angle
    // is created: BCD, or ABC respectively.

    double tol = 1e-6;

    for (int atomAIdx = 0; atomAIdx < _numAtoms; ++atomAIdx){
        BglGraph::adjacency_iterator itA, itA_end;
        boost::tie(itA, itA_end) = boost::adjacent_vertices(atomAIdx, _bondGraph);

        const tools::vec atomAPos = _qmMolecule[atomAIdx]->getPos();

        for (; itA!=itA_end; ++itA){
            const int atomBIdx = *itA;
            if (_auxBonds.Contains({atomAIdx, atomBIdx})) continue;

            const tools::vec atomBPos = _qmMolecule[atomBIdx]->getPos();
            const tools::vec BAVec = (atomAPos - atomBPos).normalize();

            BglGraph::adjacency_iterator itB, itB_end;
            boost::tie(itB, itB_end) = boost::adjacent_vertices(atomBIdx, _bondGraph);

            for (; itB != itB_end; ++itB){
                const int atomCIdx = *itB;
                if (_auxBonds.Contains({atomBIdx, atomCIdx}) ||
                    atomCIdx == atomAIdx) continue;

                const tools::vec atomCPos = _qmMolecule[atomCIdx]->getPos();

                auto index = AngleIdx{atomAIdx, atomBIdx, atomCIdx};

                const tools::vec BCVec = (atomCPos - atomBPos).normalize();

                double cosABC = BAVec*BCVec;

                if (!_angles.Contains(index)){
                    // If the 175<angle<180, then a special orthogonal angle
                    // should be added, but I don't know how to do that...
                    _angles[index] = std::acos(cosABC);
                    _vector.emplace_back(std::acos(cosABC));
                }


                // Now add proper dihedrals
                BglGraph::adjacency_iterator itC, itC_end;
                boost::tie(itC, itC_end) = boost::adjacent_vertices(atomCIdx, _bondGraph);
                tools::vec normPlaneA = BAVec^BCVec;

                for (; itC != itC_end; ++itC){
                    const int atomDIdx = *itC;
                    if (_auxBonds.Contains({atomCIdx, atomDIdx}) ||
                        atomDIdx == atomBIdx ||
                        atomDIdx == atomAIdx) continue;


                    const tools::vec atomDPos = _qmMolecule[atomDIdx]->getPos();
                    auto index = DihedralIdx{atomAIdx, atomBIdx, atomCIdx, atomDIdx};

                    if (!_dihedrals.Contains(index)){
                        const tools::vec CBVec = -BCVec;
                        const tools::vec CDVec = (atomDPos - atomCPos).normalize();

                        const double cosBCD = BCVec*CDVec;

                        // ABC and BCD must not be 180 degrees
                        // abs(cosABC + 1) < tol
                        if (std::abs(-1 - cosABC) > tol && std::abs(-1 - cosBCD) > tol){

                            tools::vec normPlaneB = (CBVec^CDVec);

                            const double cosPhi = normPlaneA*normPlaneB;

                            _dihedrals[index] = std::acos(cosPhi);
                            _vector.emplace_back(std::acos(cosPhi));
                        }
                    }
                }
            }
        }
    }

    _numAngles = _angles.size();
    _numDihedrals = _dihedrals.size();
    if (_numAtoms > 3 && _numDihedrals == 0){
        // Need to do something here...
        // arbitrarily choose 4 atoms and check if they form a
        // valid dihedral

        std::random_device rd;
        std::mt19937 g(rd());
        std::uniform_int_distribution<int> dist(0, _numAtoms-1);

        auto RandomSelector = [&]() -> DihedralIdx {
            DihedralIdx idx = {-1,-1,-1,-1};
            for (int i = 0; i<4; ++i){
                while (idx[i] == -1){
                    int coord = dist(g);
                    if (!IdxContains(coord, idx))
                        idx[i] = coord;
                }
            }
            std::sort(idx.begin(), idx.end());
            return idx;
        };

        std::function<int(int)> factorial =  [&](int num) -> int {
            if (num < 2) return 1;
            return num*factorial(num-1);
        };

        int checkCount = 0;
        int checkCountMax = factorial(_numAtoms)/(factorial(4));
        do {
            DihedralIdx ind = RandomSelector();
            std::cout << "(" << ind[0] << " " << ind[1] << " " << ind[2] << " "
                      << ind[3] << ")"<< std::endl;
            bool okay = true;
            for (int i =0; i<4; ++i){
                for (int j = i+1; i<4; ++i){
                    if (_auxBonds.Contains({ind[i], ind[j]})){
                        okay = false;
                        break;
                    }
                }
            }
            if (!okay || _dihedrals.Contains(ind)){
                ++checkCount;
                continue;
            }

            do {
                const int atomAIdx = ind[0];
                const int atomBIdx = ind[1];
                const int atomCIdx = ind[2];
                const int atomDIdx = ind[3];
                // std::cout << "(" << ind[0] << " " << ind[1] << " " << ind[2] << " "
                //           << ind[3] << ")"<< std::endl;

                const tools::vec BAVec = (_qmMolecule[atomAIdx]->getPos() - _qmMolecule[atomBIdx]->getPos()).normalize();
                const tools::vec BCVec = (_qmMolecule[atomCIdx]->getPos() - _qmMolecule[atomBIdx]->getPos()).normalize();

                const tools::vec CBVec = -BCVec;
                const tools::vec CDVec = (_qmMolecule[atomDIdx]->getPos() - _qmMolecule[atomCIdx]->getPos()).normalize();

                const double cosABC = BAVec*BCVec;
                const double cosBCD = CBVec*CDVec;


                if(abs(-1 - cosABC) > tol && abs(-1 - cosBCD) > tol){
                    const tools::vec normPlaneA = BAVec^BCVec;
                    const tools::vec normPlaneB = CBVec^CDVec;
                    const double cosPhi = normPlaneA*normPlaneB;
                    auto index = DihedralIdx{atomAIdx, atomBIdx, atomCIdx, atomDIdx};

                    if (!_dihedrals.Contains(index)){
                        std::cout << "(" << ind[0] << " " << ind[1] << " " << ind[2] << " "
                                  << ind[3] << ")"<< std::endl;
                        _dihedrals[index] = std::acos(cosPhi);
                        _vector.emplace_back(std::acos(cosPhi));
                        _numDihedrals += 1;
                    }
                }
            } while (std::next_permutation(ind.begin(), ind.end()));
            ++checkCount;
        } while (checkCount < checkCountMax);
    }
}


inline int delta(const int& i, const int& j){
    if (i==j) return 1;
    return 0;
}

inline int zeta(const int& a, const int& m, const int& n){
    return (delta(a, m) - delta(a, n));
}


inline tools::vec GetPerpTo(const tools::vec& u, const tools::vec& v){
    double tol = 1e-6;
    if (std::abs(1-abs(u))>tol || std::abs(1-abs(v))>tol)
        throw std::runtime_error("GetPerpTo only accepts normalized vectors.");

    tools::vec x(1, -1, 1);
    tools::vec y(-1, 1, 1);
    tools::vec ret;

    auto AreParallel = [] (const tools::vec& a, const tools::vec& b) -> bool{
        return ( 1 - std::abs(a*b) < 1e-3 );
    };

    if (!AreParallel(u,v))
        ret = u^v;
    else if(!AreParallel(u, x) && !AreParallel(v, x))
        ret = u^x;
    else
        ret = u^y;

    ret = ret.normalize();

    return ret;
}


void InternalCoords::PopulateWilsonMatrix(){
    _wilsonBMatrix = Eigen::MatrixXd::Zero(_vector.size(),
                                           _cartCoords().size());

    int numBonds = _numBonds+_numInterMolBonds+_numHBonds+_numAuxBonds;
    for (int b = 0; b < numBonds; ++b){
        BondIdx bondIdx = _bonds._indices[b];
        double bondLen = _bonds[bondIdx];

        int atIdxM, atIdxN;

        atIdxM = bondIdx[0];
        atIdxN = bondIdx[1];


        tools::vec bondVec = (  _qmMolecule[atIdxM]->getPos() -
                                _qmMolecule[atIdxN]->getPos()   ).normalize();

        auto writeBondElem = [&](int a) -> void{
            double z_amn = zeta(a, atIdxM, atIdxN);
            a *= 3;
            _wilsonBMatrix(b, a+0) = z_amn * bondVec.x();
            _wilsonBMatrix(b, a+1) = z_amn * bondVec.y();
            _wilsonBMatrix(b, a+2) = z_amn * bondVec.z();
        };

        writeBondElem(atIdxM);
        writeBondElem(atIdxN);
    }


    for (int a = 0; a < _numAngles; ++a){
        int idx = a + numBonds;
        AngleIdx angleIdx = _angles._indices[a];

        double angle = _angles[angleIdx];

        int atIdxM, atIdxN, atIdxO;

        // Consider a system that looks like this:
        //           A
        //            \
        //             B--C
        // The labelling below is:
        //           M
        //            \
        //             O--N
        // to be in accordance with the paper. Our data structure
        // stores it as A,B,C. We convert it to M, O, N.

        atIdxM = angleIdx[0];
        atIdxO = angleIdx[1];
        atIdxN = angleIdx[2];

        tools::vec u = (_qmMolecule[atIdxM]->getPos() -
                        _qmMolecule[atIdxO]->getPos());

        double lu = abs(u);
        u = u.normalize();

        tools::vec v = (_qmMolecule[atIdxN]->getPos() -
                        _qmMolecule[atIdxO]->getPos());

        double lv = abs(v);
        v = v.normalize();

        tools::vec w = GetPerpTo(u, v); // w = u^v
                                        // UNLESS u is parallel to v

        tools::vec uw = (u^w)/lu;

        tools::vec wv = (w^v)/lv;

        auto writeAngleElem = [&] (int a) -> void{
            tools::vec t = zeta(a, atIdxM, atIdxO)*uw +
                           zeta(a, atIdxN, atIdxO)*wv;
            a *= 3;
            _wilsonBMatrix(idx, a+0) = t.x();
            _wilsonBMatrix(idx, a+1) = t.y();
            _wilsonBMatrix(idx, a+2) = t.z();
        };

        writeAngleElem(atIdxM);
        writeAngleElem(atIdxO);
        writeAngleElem(atIdxN);
    }


    for (int d = 0; d<_numDihedrals; ++d){
        int idx = d+numBonds+_numAngles;
        DihedralIdx index = _dihedrals._indices[d];

        int atIdxM, atIdxN, atIdxO, atIdxP;

        // Consider a system that looks like this:
        //           A      D
        //            \    /
        //             B--C
        // The labelling below is:
        //           M      N
        //            \    /
        //             O--P
        //
        // I don't know why. I think this labeling makes things
        // harder...  We follow the paper, and it is easier to just
        // use their notation.  Our data structure stores them in
        // A,B,C,D order so we adjust it

        atIdxM = index[0];
        atIdxO = index[1];
        atIdxP = index[2];
        atIdxN = index[3];

        tools::vec u = _qmMolecule[atIdxM]->getPos() -
                       _qmMolecule[atIdxO]->getPos();

        tools::vec v = _qmMolecule[atIdxN]->getPos() -
                       _qmMolecule[atIdxP]->getPos();

        tools::vec w = _qmMolecule[atIdxP]->getPos() -
                       _qmMolecule[atIdxO]->getPos();

        double lu = abs(u);
        double lv = abs(v);
        double lw = abs(w);

        u = u.normalize();
        v = v.normalize();
        w = w.normalize();


        double cosPhiU =  u*v;
        double cosPhiV = -v*w;

        double sinPhiU = sqrt(1 - cosPhiU*cosPhiU);
        double sinPhiV = sqrt(1 - cosPhiV*cosPhiV);

        double su2 = sinPhiU*sinPhiU;
        double sv2 = sinPhiV*sinPhiV;

        tools::vec uw = u^w;
        tools::vec vw = v^w;


        auto writeDihedralElem = [&](int a) -> void{
            tools::vec t1 = ( zeta(a, atIdxM, atIdxO)/(lu*su2) )*uw;
            tools::vec t2 = ( zeta(a, atIdxP, atIdxN)/(lv*sv2) )*vw;
            tools::vec t3 = zeta(a, atIdxO, atIdxP)*(uw*cosPhiU/(lw*su2) - vw*cosPhiV/lw*sv2);
            a *= 3;
            _wilsonBMatrix(idx, a+0) = t1.x() - t2.x() - t3.x();
            _wilsonBMatrix(idx, a+1) = t1.y() - t2.y() - t3.y();
            _wilsonBMatrix(idx, a+2) = t1.z() - t2.z() - t3.z();

        };

        writeDihedralElem(atIdxM);
        writeDihedralElem(atIdxN);
        writeDihedralElem(atIdxO);
        writeDihedralElem(atIdxP);
    }
}

InternalCoords::InternalCoords(const Orbitals& orb):
    InternalCoords(orb, true){};

InternalCoords::InternalCoords(const Orbitals& orb, const bool withAux):
    CoordBase(INTERNAL, orb), _withAuxiliary(withAux),
    _numBonds(0), _numInterMolBonds(0), _numHBonds(0),
    _numAngles(0), _numDihedrals(0), _numAuxBonds(0), _bondGraph(_numAtoms),
    _cartCoords(orb)
{
    // This code implements the algorithm described in
    // https://doi.org/10.1063/1.1515483

    // covalent bonds
    ConnectBonds();

    // Intermolecule bonds
    ConnectMolecules();

    // Hydrogen bonds
    ConnectHBonds();

    CalculateAnglesDihedrals();

    _coords = Eigen::Map<Eigen::VectorXd>(_vector.data(), _vector.size());

    PopulateWilsonMatrix();
}

void InternalCoords::Increment(Eigen::VectorXd dx){
    CoordBase::Increment(dx);
}

int InternalCoords::getPossibleNumMols(){
    return _possibleNumMols;
}

int InternalCoords::getNumBonds(){
    return _numBonds;
}

int InternalCoords::getNumHBonds(){
    return _numHBonds;
}

int InternalCoords::getNumAngles(){
    return _numAngles;
}

int InternalCoords::getNumAuxBonds(){
    return _numAuxBonds;
}

int InternalCoords::getNumDihedrals(){
    return _numDihedrals;
}

Eigen::MatrixXd InternalCoords::getWilsonBMatrix(){
    return _wilsonBMatrix;
}

std::ostream& operator<<(std::ostream& s, const InternalCoords& ic){
    s << "Bonds " << std::endl;
    s << ic._bonds;
    s << "AuxBonds" << std::endl;
    s << ic._auxBonds;
    s << "Angles " << std::endl;
    s << ic._angles;
    s << "Dihedrals" << std::endl;
    s << ic._dihedrals;
    return s;
}

} // xtp
} // votca